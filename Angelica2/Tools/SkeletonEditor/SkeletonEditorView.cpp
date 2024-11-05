// SkeletonEditorView.cpp : implementation of the CSkeletonEditorView class
//

#include "Global.h"
#include "SkeletonEditor.h"
#include "SkeletonEditorView.h"
#include "SkeletonEditorDoc.h"

#include "Render.h"
#include "Model.h"
#include "A3D.h"
#include "AFI.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorView

IMPLEMENT_DYNCREATE(CSkeletonEditorView, CView)

BEGIN_MESSAGE_MAP(CSkeletonEditorView, CView)
	//{{AFX_MSG_MAP(CSkeletonEditorView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorView construction/destruction

CSkeletonEditorView::CSkeletonEditorView()
{
	m_pCamera		= NULL;
	m_pViewport		= NULL;
	m_bInitialized	= false;
	m_bLeftPressed	= false;
	m_bRightPressed	= false;
	m_bDoSelect		= false;
	m_bAddSelect	= false;
}

CSkeletonEditorView::~CSkeletonEditorView()
{
}

BOOL CSkeletonEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorView drawing

void CSkeletonEditorView::OnDraw(CDC* pDC)
{
	CSkeletonEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorView printing

BOOL CSkeletonEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSkeletonEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSkeletonEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorView diagnostics

#ifdef _DEBUG
void CSkeletonEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CSkeletonEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSkeletonEditorDoc* CSkeletonEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSkeletonEditorDoc)));
	return (CSkeletonEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorView message handlers

void CSkeletonEditorView::OnInitialUpdate() 
{
	if (!m_bInitialized)
	{
		//	Initialize engine...
		m_bInitialized = true;

		//	Create render
		if (!g_Render.Init(AfxGetInstanceHandle(), m_hWnd))
		{
			g_Log.Log("CSkeletonEditorView::OnInitialUpdate, Failed to create render");
			return;
		}

		//	Create camera
		if (!(m_pCamera = new A3DCamera()))
		{
			g_Log.Log("CSkeletonEditorView::OnInitialUpdate: Failed to create camera object!");
			return;
		}

		if (!m_pCamera->Init(g_Render.GetA3DDevice(), DEG2RAD(56.0f), 0.1f, 2000.0f))
		{
			g_Log.Log(0, "CSkeletonEditorView::OnInitialUpdate: Failed to initialize camera.");
			return;
		}
							    
		m_pCamera->SetPos(g_vOrigin - g_vAxisZ * 10.0f);
		m_pCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);

		//	Create main Viewport
		RECT rc;
		GetClientRect(&rc);

		if (!g_Render.GetA3DDevice()->CreateViewport(&m_pViewport, 0, 0, rc.right, rc.bottom,
												0.0f, 1.0f, true, true, 0xff404040))
		{
			g_Log.Log(0, "CSkeletonEditorView::OnInitialUpdate: Failed to create viewport.");
			return;
		}

		m_pViewport->SetCamera(m_pCamera);
	}
	
	CView::OnInitialUpdate();
}

void CSkeletonEditorView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;

	if (!m_bInitialized)
		return;

	g_Render.ResizeDevice(cx, cy);

	ASSERT(m_pViewport && m_pCamera);

	A3DVIEWPORTPARAM Params;
	Params.X		= 0;
	Params.Y		= 0;
	Params.Width	= cx;
	Params.Height	= cy;
	Params.MinZ		= 0.0f;
	Params.MaxZ		= 1.0f;

	m_pViewport->SetParam(&Params);

	//	Adjust camera
	float fRatio = (float)cx / cy;
	m_pCamera->SetProjectionParam(m_pCamera->GetFOV(), 0.1f, 2000.0f, fRatio);
}

void CSkeletonEditorView::OnDestroy() 
{
	if (m_pCamera)
	{
		m_pCamera->Release();
		delete m_pCamera;
		m_pCamera = NULL;
	}

	if (m_pViewport)
	{
		m_pViewport->Release();
		delete m_pViewport;
		m_pViewport = NULL;
	}

	CView::OnDestroy();
}

//	Frame move
bool CSkeletonEditorView::FrameMove(DWORD dwTime)
{
	//	Move camera
	MoveCamera(dwTime);

	CSkeletonEditorDoc* pDoc = GetDocument();
	pDoc->FrameMove(dwTime);

	//	Tick animation
	static DWORD dwLastTime = a_GetTime();
	DWORD dwCurTime = a_GetTime();
	if (dwCurTime >= dwLastTime + 30)
	{
		g_Render.GetA3DEngine()->TickAnimation();
		dwLastTime = dwCurTime;
	}

	return true;
}

//	Render 
bool CSkeletonEditorView::Render()
{
	if (!g_Render.BeginRender())
		return false;

	m_pViewport->Active();
	m_pViewport->ClearDevice();

	CSkeletonEditorDoc* pDoc = GetDocument();
	pDoc->Render(m_pViewport);

//	g_Render.FlushCollectors();
	g_Render.GetA3DDevice()->SetSpecularEnable(true);
	g_Render.GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender()->Render(m_pViewport);
	g_Render.GetA3DDevice()->SetSpecularEnable(false);

	g_Render.GetA3DEngine()->RenderGFX(m_pViewport, 0xffffffff);

	//	Draw selected rectangle
	if (m_bDoSelect && g_Configs.bSkeletonOnly)
	{
		DWORD dwCol = m_bAddSelect ? g_aColTable[COL_SELRECT] : g_aColTable[COL_UNSELRECT];
		g_Render.Draw2DRect(m_rcSelected, dwCol, true);
	}

	//	Draw ground grid
	if (g_Configs.bShowGround)
		DrawGroundGrid();

	//	Print prompt
	DrawPrompt();

	g_Render.EndRender();

	return true;
}

//	Print prompt
void CSkeletonEditorView::DrawPrompt()
{
	static char szMsg[256];

	//	Print camera position
	A3DVECTOR3 v = m_pCamera->GetPos();
	sprintf(szMsg, "Camera: %.2f, %.2f, %.2f", v.x, v.y, v.z);
	g_Render.TextOut(10, 10, szMsg, 0xffffff00);
	
	//	Print bound box size
	if (g_Configs.bShowBoundBox)
	{
		CModel* pModel = GetDocument()->GetModel();
		if (pModel && pModel->GetA3DSkinModel())
		{
			const A3DAABB& aabb = pModel->GetA3DSkinModel()->GetModelAABB();
			sprintf(szMsg, "AABB ext: %.3f, %.3f, %.3f", aabb.Extents.x, aabb.Extents.y, aabb.Extents.z);
			g_Render.TextOut(10, 26, szMsg, 0xffffff00);
		}
	}

/*	if (AUX_GetApp()->GetTickTime())
	{
		sprintf(szMsg, "Ticktime = %d", AUX_GetApp()->GetTickTime());
		g_Render.TextOut(10, 30, szMsg, 0xffffff00);
	}
*/
}

//	Draw ground grid
void CSkeletonEditorView::DrawGroundGrid()
{
	static bool bInit = false;
	static WORD aIdx1[6];

	if (!bInit)
	{
		aIdx1[0] = 0;
		aIdx1[1] = 1;
		aIdx1[2] = 2;
		aIdx1[3] = 0;
		aIdx1[4] = 2;
		aIdx1[5] = 3;

		bInit = true;
	}

	A3DVECTOR3 aVerts1[4];
	A3DVECTOR3 aVerts2[4];

	float fLen = 5.0f;

	CModel* pModel = GetDocument()->GetModel();
	if (pModel && pModel->GetA3DSkinModel())
		fLen = pModel->GetModelRadius() * 1.2f;

	float fLen2 = fLen * 0.5f;

	//	xz plane
	aVerts1[0].Set(-fLen, 0.0f, -fLen);
	aVerts1[1].Set(fLen, 0.0f, -fLen);
	aVerts1[2].Set(fLen, 0.0f, fLen);
	aVerts1[3].Set(-fLen, 0.0f, fLen);

	//	axises
	aVerts2[0].Set(-fLen2, 0.0f, 0.0f);
	aVerts2[1].Set(0.0f, 0.0f, -fLen2);
	aVerts2[2].Set(fLen2, 0.0f, 0.0f);
	aVerts2[3].Set(0.0f, 0.0f, fLen2);

	//	Render
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	A3DWireCollector* pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	A3DFlatCollector* pFlatCollector = g_Render.GetA3DEngine()->GetA3DFlatCollector();

	pA3DDevice->SetWorldMatrix(A3DMATRIX4(A3DMATRIX4::IDENTITY));
	pA3DDevice->SetFaceCull(A3DCULL_NONE);
//	pA3DDevice->SetZWriteEnable(false);

	pFlatCollector->AddRenderData_3D(aVerts1, 4, aIdx1, 6, A3DCOLORRGBA(255, 255, 255, 64));

	pWireCollector->Add3DLine(aVerts1[0], aVerts1[1], A3DCOLORRGBA(255, 255, 255, 255));
	pWireCollector->Add3DLine(aVerts1[1], aVerts1[2], A3DCOLORRGBA(255, 255, 255, 255));
	pWireCollector->Add3DLine(aVerts1[2], aVerts1[3], A3DCOLORRGBA(255, 255, 255, 255));
	pWireCollector->Add3DLine(aVerts1[3], aVerts1[0], A3DCOLORRGBA(255, 255, 255, 255));

	pWireCollector->Add3DLine(aVerts2[0], aVerts2[2], A3DCOLORRGBA(255, 255, 255, 255));
	pWireCollector->Add3DLine(aVerts2[1], aVerts2[3], A3DCOLORRGBA(255, 255, 255, 255));

	pWireCollector->Flush();
	pFlatCollector->Flush();

	pA3DDevice->SetFaceCull(A3DCULL_CCW);
//	pA3DDevice->SetZWriteEnable(true);
}

//	Move camera
void CSkeletonEditorView::MoveCamera(DWORD dwTime)
{
	A3DVECTOR3 vDirH	= m_pCamera->GetDirH();
	A3DVECTOR3 vRightH	= m_pCamera->GetRightH();
	A3DVECTOR3 vDelta(0.0f);

	if (GetAsyncKeyState('W') & 0x8000)
		vDelta = vDirH;
	else if (GetAsyncKeyState('S') & 0x8000)
		vDelta = -vDirH;
	
	if (GetAsyncKeyState('A') & 0x8000)
		vDelta = vDelta - vRightH;
	else if (GetAsyncKeyState('D') & 0x8000)
		vDelta = vDelta + vRightH;
	
	if (GetAsyncKeyState('Q') & 0x8000)
		vDelta = vDelta + g_vAxisY;
	else if (GetAsyncKeyState('Z') & 0x8000)
		vDelta = vDelta - g_vAxisY;

	float fSpeed = (float)(0.5 * pow(2.0, (double)g_Configs.iCameraMoveSpeed));
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		fSpeed *= 0.5f;
	else if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		fSpeed *= 2.0f;

	vDelta = Normalize(vDelta) * (fSpeed * (float)dwTime * 0.001f);
	
	m_pCamera->Move(vDelta);
}

//	Set camera position basing on model's position and size
void CSkeletonEditorView::SetCameraPos(const A3DVECTOR3& vModelCenter, float fModelRadius)
{
	m_pCamera->SetPos(vModelCenter - g_vAxisZ * (fModelRadius * 4.0f));
	m_pCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);
}

void CSkeletonEditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bRightPressed)
		return;

	m_bLeftPressed = true;

	if ((nFlags & MK_CONTROL) || (nFlags & MK_SHIFT))	//	Select bones
	{
		SetRect(&m_rcSelected, point.x, point.y, point.x, point.y);

		m_ptSelStart = point;
		m_bDoSelect	 = true;
		m_bAddSelect = (nFlags & MK_CONTROL) ? true : false;
	}
	else
	{
		m_ptMouseLast = point;
		ClientToScreen(&m_ptMouseLast);
	}

	SetCapture();

	CView::OnLButtonDown(nFlags, point);
}

void CSkeletonEditorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bLeftPressed)
	{
		CView::OnLButtonUp(nFlags, point);
		return;
	}

	m_bLeftPressed = false;
	ReleaseCapture();

	if (m_bDoSelect)
	{
		m_bDoSelect = false;

		CModel* pModel = GetDocument()->GetModel();
		if (pModel)
			pModel->SelectBone(m_rcSelected, m_pViewport, m_bAddSelect);
	}
	
	CView::OnLButtonUp(nFlags, point);
}

void CSkeletonEditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bLeftPressed && !m_bRightPressed)
	{
		CView::OnMouseMove(nFlags, point);
		return;
	}

	if (m_bDoSelect)	//	Draw rectangle
	{
		if (m_ptSelStart.x < point.x)
		{
			m_rcSelected.left	= m_ptSelStart.x;
			m_rcSelected.right	= point.x;
		}
		else
		{
			m_rcSelected.left	= point.x;
			m_rcSelected.right	= m_ptSelStart.x;
		}

		if (m_ptSelStart.y < point.y)
		{
			m_rcSelected.top	= m_ptSelStart.y;
			m_rcSelected.bottom	= point.y;
		}
		else
		{
			m_rcSelected.top	= point.y;
			m_rcSelected.bottom = m_ptSelStart.y;
		}
	}
	else
	{
		POINT pt = point;
		ClientToScreen(&pt);
		
		CModel* pModel = GetDocument()->GetModel();
		if (pModel)
		{
			float f1 = 0.025f * (g_Configs.iCameraRotSpeed + 1);
			float f2 = 0.25f * (g_Configs.iMouseDragSpeed + 1);

			if (pt.x - m_ptMouseLast.x)
			{
				if (m_bLeftPressed)
				{
					float fYaw = (pt.x - m_ptMouseLast.x) * f1;
					m_pCamera->DegDelta(fYaw);
				}
				else
				{
					float fDelta = -(pt.x - m_ptMouseLast.x) * f2;
					pModel->AddRotateY(fDelta);
				}
			}
			
			if (pt.y - m_ptMouseLast.y)
			{
				if (m_bLeftPressed)
				{
					float fPitch = -(pt.y - m_ptMouseLast.y) * f1;
					m_pCamera->PitchDelta(fPitch);
				}
				else
				{
					float fDelta = -(pt.y - m_ptMouseLast.y) * f2;
					pModel->AddRotateX(fDelta);
				}
			}
		}

		m_ptMouseLast = pt;
	}

	CView::OnMouseMove(nFlags, point);
}

void CSkeletonEditorView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bLeftPressed)
		return;

	m_bRightPressed	= true;
	m_ptMouseLast	= point;
	
	ClientToScreen(&m_ptMouseLast);
	SetCapture();

	CView::OnRButtonDown(nFlags, point);
}

void CSkeletonEditorView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bRightPressed)
	{
		CView::OnRButtonUp(nFlags, point);
		return;
	}

	m_bRightPressed = false;
	ReleaseCapture();

	CView::OnRButtonUp(nFlags, point);
}


