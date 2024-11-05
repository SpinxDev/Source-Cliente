// Terrain2ViewView.cpp : implementation of the CTerrain2ViewView class
//

#include "Global.h"
#include "Terrain2View.h"
#include "Terrain2ViewDoc.h"
#include "Terrain2ViewView.h"
#include "Render.h"
#include "Settings.h"
#include "A3DTerrain2Wrapper.h"

#include <ALog.h>
#include <A3DViewport.h>
#include <A3DCamera.h>
#include <A3DFuncs.h>
#include <A3DEngine.h>
#include <A3DDevice.h>
#include <A3DHorizonCull.h>
#include <A3DTerrain2CullHC.h>

#define TIME_TICKANIMATION	30		//	ms

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewView

IMPLEMENT_DYNCREATE(CTerrain2ViewView, CView)

BEGIN_MESSAGE_MAP(CTerrain2ViewView, CView)
	//{{AFX_MSG_MAP(CTerrain2ViewView)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewView construction/destruction

CTerrain2ViewView::CTerrain2ViewView()
{
	// TODO: add construction code here
	m_bRightPressed	= false;
}

CTerrain2ViewView::~CTerrain2ViewView()
{
}

BOOL CTerrain2ViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewView drawing

void CTerrain2ViewView::OnDraw(CDC* pDC)
{
	CTerrain2ViewDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewView diagnostics

#ifdef _DEBUG
void CTerrain2ViewView::AssertValid() const
{
	CView::AssertValid();
}

void CTerrain2ViewView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTerrain2ViewDoc* CTerrain2ViewView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTerrain2ViewDoc)));
	return (CTerrain2ViewDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewView message handlers

void CTerrain2ViewView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if (!cx || !cy)
		return;
	
	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;

	g_Render.ResizeDevice(cx, cy);
}

void CTerrain2ViewView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	if (!g_Render.EngineIsReady())
	{
		//	Create render
		if (!g_Render.Init(AfxGetInstanceHandle(), m_hWnd, false))
		{
			a_LogOutput(1, "CSkeletonEditorView::OnInitialUpdate, Failed to create render");
			return;
		}
	}
}

//	Logic tick
bool CTerrain2ViewView::Tick(DWORD dwTime)
{
	CTerrain2ViewDoc* pDoc = GetDocument();
	A3DCamera* pCamera = g_Render.GetCamera();

	//	Move camera
	MoveCamera(dwTime);

	//	Tick terrain
	if (pDoc->m_pA3DTerrain)
		pDoc->m_pA3DTerrain->Update(dwTime, pCamera, pCamera->GetPos());

	//	Get block info
	GetBlockInfo(g_Render.GetViewport());

	//	A3DEngine::TickAnimation trigger animation of many objects.
	//	For example: A3DSky objects, GFX objects etc.
	static DWORD dwAnimTime = 0;
	if ((dwAnimTime += dwTime) >= TIME_TICKANIMATION)
	{
		dwAnimTime -= TIME_TICKANIMATION;
		g_Render.GetA3DEngine()->TickAnimation();

		//	Update ear position so that all 3D sounds' positions are correct
		pCamera->UpdateEar();
	}

	return true;
}

//	Render scene
bool CTerrain2ViewView::Render()
{
	if (!g_Render.BeginRender())
		return false;

	CTerrain2ViewDoc* pDoc = GetDocument();
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	CSettings& s = glb_GetApp()->GetSettings();

	A3DViewport* pA3DViewport = g_Render.GetViewport();
	A3DCamera* pA3DCamera = g_Render.GetCamera();
	pA3DViewport->Active();

	//	Clear viewport
	DWORD dwCol = glb_GetApp()->GetSettings().GetBackColor();
	pA3DDevice->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, dwCol, 1.0f, 0);

	pA3DDevice->SetFillMode(s.GetWireFrame() ? A3DFILL_WIREFRAME : A3DFILL_SOLID);

	//	Render terrain
	if (pDoc->m_pA3DTerrain)
	{
		pDoc->m_pA3DTerrain->Render(pA3DViewport);

		if (s.GetBlockQuadShow())
		{
			int r=-1, c=-1;
			if (s.GetBlockQuadShow() & CSettings::BQS_PICKED)
			{
				r = m_ptPickedBlk.y;
				c = m_ptPickedBlk.x;
			}
			
			pDoc->m_pA3DTerrain->RenderBlockQuads(pA3DViewport, r, c);
		}

		if (s.GetHoriLineShow())
			pDoc->m_pA3DTerrain->RenderHorizonCull(pA3DViewport);
	}

	//	test code
/*	g_SkinModel.Render(pA3DViewport);

	g_Render.GetA3DDevice()->SetSpecularEnable(true);
	g_Render.GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender()->Render(pA3DViewport);
	g_Render.GetA3DDevice()->SetSpecularEnable(false);
*/	
	//	Draw prompt info
	if (s.GetPromptShow())
		DrawPromptInfo(pA3DViewport);

	g_Render.EndRender();

	return true;
}

//	Move camera
void CTerrain2ViewView::MoveCamera(DWORD dwTime)
{
	A3DCamera* pA3DCamera = g_Render.GetCamera();
	CTerrain2ViewDoc* pDoc = GetDocument();

	A3DVECTOR3 vDirH = pA3DCamera->GetDirH();
	A3DVECTOR3 vRightH = pA3DCamera->GetRightH();
	A3DVECTOR3 vDelta(0.0f);

	if (GetAsyncKeyState('W') & 0x8000)
		vDelta = vDirH;
	else if (GetAsyncKeyState('S') & 0x8000)
		vDelta = -vDirH;
	
	if (GetAsyncKeyState('A') & 0x8000)
		vDelta = vDelta - vRightH;
	else if (GetAsyncKeyState('D') & 0x8000)
		vDelta = vDelta + vRightH;
	
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		vDelta = vDelta + g_vAxisY;
	else if (GetAsyncKeyState('Z') & 0x8000)
		vDelta = vDelta - g_vAxisY;

	float fSpeed = glb_GetApp()->GetSettings().GetCameraSpeed();
	vDelta = a3d_Normalize(vDelta) * (fSpeed * (float)dwTime * 0.001f);
	
	pA3DCamera->Move(vDelta);

	//	Adjust camera position so that it won't move into some wrong place
	A3DVECTOR3 vPos = pA3DCamera->GetPos();
	if (pDoc->m_pA3DTerrain)
	{
		float fHeight = pDoc->m_pA3DTerrain->GetPosHeight(vPos);
		a_ClampFloor(vPos.y, fHeight + 1.0f);
	}

	pA3DCamera->SetPos(vPos);
}

//	Get block info
void CTerrain2ViewView::GetBlockInfo(A3DViewport* pViewport)
{
	m_ptCameraBlk.x = -1;
	m_ptCameraBlk.y = -1;
	m_ptPickedBlk.x = -1;
	m_ptPickedBlk.y = -1;

	A3DTerrain2Wrapper* pTerrain = GetDocument()->m_pA3DTerrain;
	if (!pTerrain)
		return;

	const ARectF& rcTerrain = pTerrain->GetTerrainArea();
	A3DVECTOR3 vEyePos = pViewport->GetCamera()->GetPos();

	//	Find the block camera is in
	float fInvBlockSize = 1.0f / pTerrain->GetBlockSize();
	m_ptCameraBlk.x = (int)((vEyePos.x - rcTerrain.left) * fInvBlockSize);
	m_ptCameraBlk.y = (int)(-(vEyePos.z - rcTerrain.top) * fInvBlockSize);
	
	//	Find the block picked by cursor
	POINT ptCursor;
	::GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	A3DVECTOR3 vEnd((float)ptCursor.x, (float)ptCursor.y, 1.0f);
	pViewport->InvTransform(vEnd, vEnd);

	RAYTRACERT rt;
	if (pTerrain->RayTrace(vEyePos, vEnd-vEyePos, 1.0f, &rt))
	{
		m_ptPickedBlk.x = (int)((rt.vHitPos.x - rcTerrain.left) * fInvBlockSize);
		m_ptPickedBlk.y = (int)(-(rt.vHitPos.z - rcTerrain.top) * fInvBlockSize);
	}
}

//	Draw block info
void CTerrain2ViewView::DrawPromptInfo(A3DViewport* pViewport)
{
	A3DTerrain2Wrapper* pTerrain = GetDocument()->m_pA3DTerrain;
	if (!pTerrain)
		return;

	char szMsg[256];
	int x=10, y=10;

	//	Block infos
	sprintf(szMsg, "Stand on block: [r:%d, c:%d]", m_ptCameraBlk.y, m_ptCameraBlk.x);
	g_Render.TextOut(x, y, szMsg, 0xff00ff00);
	y += 16;
	sprintf(szMsg, "Point to block: [r:%d, c:%d]", m_ptPickedBlk.y, m_ptPickedBlk.x);
	g_Render.TextOut(x, y, szMsg, 0xff00ff00);
	y += 16;
	sprintf(szMsg, "Visible block num: %d", pTerrain->GetRenderedBlockNum());
	g_Render.TextOut(x, y, szMsg, 0xff00ff00);
	y += 16;

	//	Horizonal line infos
	A3DTerrain2CullHC* pTrnCull = (A3DTerrain2CullHC*)pTerrain->GetTerrainCuller();
	if (pTrnCull)
	{
		A3DHorizonCull* pHC = pTrnCull->GetHorzonCull();
		if (pHC)
		{
			sprintf(szMsg, "Inserted/Tested/Drawed line num: %d/%d/%d", pHC->GetInsertLineNum(), 
				pHC->GetTestLineNum(), pHC->GetDrawLineNum());
			g_Render.TextOut(x, y, szMsg, 0xff00ff00);
			y += 16;
		}
	}

	//	Camera infos
	A3DVECTOR3 v = pViewport->GetCamera()->GetPos();
	sprintf(szMsg, "Camera pos: %.3f, %.3f, %.3f", v.x, v.y, v.z);
	g_Render.TextOut(x, y, szMsg, 0xff00ff00);
	y += 16;
	v = pViewport->GetCamera()->GetDir();
	sprintf(szMsg, "Camera dir: %.3f, %.3f, %.3f", v.x, v.y, v.z);
	g_Render.TextOut(x, y, szMsg, 0xff00ff00);
	y += 16;
}

void CTerrain2ViewView::OnMouseMove(UINT nFlags, CPoint point) 
{
	POINT pt = {point.x, point.y};
	ClientToScreen(&pt);

	if (m_bRightPressed)
	{
		//	Rotate camera
		float f1 = 0.1f;
		A3DCamera* pCamera = g_Render.GetCamera();

		if (pt.x - m_ptMouseLast.x)
		{
			float fYaw = (pt.x - m_ptMouseLast.x) * f1;
			pCamera->DegDelta(fYaw);
		}
		
		if (pt.y - m_ptMouseLast.y)
		{
			float fPitch = -(pt.y - m_ptMouseLast.y) * f1;
			pCamera->PitchDelta(fPitch);
		}
	}

	m_ptMouseLast = pt;
	
	CView::OnMouseMove(nFlags, point);
}

void CTerrain2ViewView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_bRightPressed	= true;
	m_ptMouseLast.x	= point.x;
	m_ptMouseLast.y = point.y;
	
	ClientToScreen(&m_ptMouseLast);
	SetCapture();
	
	CView::OnRButtonDown(nFlags, point);
}

void CTerrain2ViewView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bRightPressed)
		return;

	m_bRightPressed = false;
	ReleaseCapture();
	
	CView::OnRButtonUp(nFlags, point);
}
