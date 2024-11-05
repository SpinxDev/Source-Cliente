// PhysDemoView.cpp : implementation of the CPhysDemoView class
//

#include "Global.h"
#include "PhysDemo.h"
#include "PhysDemoDoc.h"
#include "PhysDemoView.h"
#include "Render.h"
#include "Game.h"
#include "PhysObjectMan.h"
#include "PhysStaticObject.h"
#include "PhysTerrain.h"
#include "PhysModelObject.h"

#include "DlgPhysXTrnGen.h"

#include "APhysX.h"

#include <ALog.h>
#include <A3DDevice.h>
#include <A3DCursor.h>
#include <A3DCamera.h>
#include <A3DFuncs.h>
#include <A3DViewport.h>
#include <CommonFileName.h>

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoView

IMPLEMENT_DYNCREATE(CPhysDemoView, CView)

BEGIN_MESSAGE_MAP(CPhysDemoView, CView)
	//{{AFX_MSG_MAP(CPhysDemoView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_EDIT_ADDSTATICOBJ, OnEditAddstaticobj)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_EDIT_PHYSXTRNGEN, OnEditPhysxtrngen)
	ON_COMMAND(ID_EDIT_ADDTERRAIN, OnEditAddterrain)
	ON_COMMAND(ID_EDIT_ADD2STICK, OnEditAdd2stick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoView construction/destruction

CPhysDemoView::CPhysDemoView()
{
	m_bRightPressed	= false;
	m_ptMouseLock.x = 0;
	m_ptMouseLock.y = 0;
	m_ptMouseLast.x = 0;
	m_ptMouseLast.y = 0;

	m_dwRenderStart	= 0;
	m_dwLastRender	= 0;
}

CPhysDemoView::~CPhysDemoView()
{
}

BOOL CPhysDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoView drawing

void CPhysDemoView::OnDraw(CDC* pDC)
{
	CPhysDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoView diagnostics

#ifdef _DEBUG
void CPhysDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CPhysDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPhysDemoDoc* CPhysDemoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPhysDemoDoc)));
	return (CPhysDemoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoView message handlers

void CPhysDemoView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if (!cx || !cy)
		return;
	
	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;

	g_Render.ResizeDevice(cx, cy);
}

void CPhysDemoView::OnInitialUpdate() 
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
bool CPhysDemoView::Tick(DWORD dwTime)
{
	if (!g_Game.IsTerrainInit())
		return true;

	//	Restore mouse to original position
	if (m_bRightPressed)
	{
		::SetCursorPos(m_ptMouseLock.x, m_ptMouseLock.y);
		m_ptMouseLast = m_ptMouseLock;
	}

	//	Move camera
	MoveCamera(dwTime);

	//	Do logic tick here.
	g_Game.Tick(dwTime);

	return true;
}

//	Render scene
bool CPhysDemoView::Render()
{
	m_dwRenderStart = a_GetTime();

	if (!g_Render.BeginRender())
		return false;

	//	Clear viewport
	g_Render.ClearViewport();

	//	Do render
	g_Game.Render();

	//	Draw prompts
	DrawPrompts();

	g_Render.EndRender();

	m_dwLastRender = a_GetTime() - m_dwRenderStart;

	return true;
}

//	Move camera
void CPhysDemoView::MoveCamera(DWORD dwTime)
{
	A3DCamera* pA3DCamera = g_Render.GetCamera();

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

	float fSpeed = 20.0f;
	if (GetAsyncKeyState(VK_SHIFT))
		fSpeed *= 2.5f;

	vDelta = a3d_Normalize(vDelta) * (fSpeed * (float)dwTime * 0.001f);
	
	pA3DCamera->Move(vDelta);

	//	Adjust camera position so that it won't move into some wrong place
	A3DVECTOR3 vPos = pA3DCamera->GetPos();
	CPhysTerrain* pTerrain = g_Game.GetTerrain();
	if (pTerrain)
	{
		float fHeight = pTerrain->GetPosHeight(vPos);
		a_ClampFloor(vPos.y, fHeight + 1.0f);
	}

	pA3DCamera->SetPos(vPos);
}

//	Render prompts
void CPhysDemoView::DrawPrompts()
{
	HWND hWnd = g_Render.GetWndHandler();

	RECT rcClient;
	::GetClientRect(hWnd, &rcClient);

	int x = rcClient.left + 10;
	int y = rcClient.bottom - 24;
	DWORD dwColor = 0xff00ff00;
	AString str;

	//	Camera position
	A3DVECTOR3 vPos = g_Render.GetCamera()->GetPos();
	str.Format("Camera: %.3f, %.3f, %.3f", vPos.x, vPos.y, vPos.z);
	g_Render.TextOut(x, y, str, dwColor);
	y -= 16;

	//	Physical state
	int iState = g_Game.GetPhysState();
	if (iState == CPhysObject::PHY_STATE_PURESIMULATE)
		str = "Pure simulation";
	else if (iState == CPhysObject::PHY_STATE_ANIMATION)
		str = "Pure animation";
	else if (iState = CPhysObject::PHY_STATE_PARTSIMULATE)
		str = "Partial simulation";

	g_Render.TextOut(x, y, str, dwColor);
	y -= 16;

	//	Render time of last frame
	str.Format("Render time: %d", m_dwLastRender);
	g_Render.TextOut(x, y, str, dwColor);
	y -= 16;
}

//	Get the terrain position pointed by cursor
bool CPhysDemoView::GetCurTerrainPos(int x, int y, A3DVECTOR3& vOutPos)
{
	A3DVECTOR3 vDest = A3DVECTOR3((float)x, (float)y, 1.0f);
	if (!g_Render.GetViewport()->InvTransform(vDest, vDest))
		return false;
	
	A3DVECTOR3 vStart = g_Render.GetCamera()->GetPos();
	A3DVECTOR3 vDelta = vDest - vStart;
	RAYTRACERT TraceRt;

	CPhysTerrain* pTerrain = g_Game.GetTerrain();
	if (!pTerrain)
		return false;

	RAYTRACERT rt;
	if (!pTerrain->RayTrace(vStart, vDelta, 1.0f, &rt))
		return false;

	vOutPos = rt.vHitPos;
	return true;
}

void CPhysDemoView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPhysObject* pObject = g_Game.GetPickedObject();
	if (pObject)
	{
		//	Set up picked object
		A3DVECTOR3 vHitPos;
		if (GetCurTerrainPos(point.x, point.y, vHitPos))
			g_Game.SetupPickedObject(vHitPos, g_vAxisZ, g_vAxisY);
	}

	CView::OnLButtonDown(nFlags, point);
}

void CPhysDemoView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CView::OnLButtonUp(nFlags, point);
}

void CPhysDemoView::OnMouseMove(UINT nFlags, CPoint point) 
{
	POINT pt = {point.x, point.y};
	ClientToScreen(&pt);

	if (m_bRightPressed)
	{
		int dx = pt.x - m_ptMouseLast.x;
		int dy = pt.y - m_ptMouseLast.y;

		//	Rotate camera
		float f1 = 0.03f;
		A3DCamera* pCamera = g_Render.GetCamera();

		if (dx)
		{
			float fYaw = dx * f1;
			pCamera->DegDelta(fYaw);
		}
		
		if (dy)
		{
			float fPitch = -dy * f1;
			pCamera->PitchDelta(fPitch);
		}

		m_ptMouseLast = pt;
	}
	else
	{
		CPhysObject* pObject = g_Game.GetPickedObject();
		if (pObject)
		{
			A3DVECTOR3 vHitPos;
			if (GetCurTerrainPos(point.x, point.y, vHitPos))
			{
				pObject->SetPos(vHitPos);
			}
		}
	}

	CView::OnMouseMove(nFlags, point);
}

void CPhysDemoView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bRightPressed)
		return;

	m_bRightPressed	= true;
	m_ptMouseLock.x	= point.x;
	m_ptMouseLock.y = point.y;
	ClientToScreen(&m_ptMouseLock);

	m_ptMouseLast = m_ptMouseLock;

	SetCapture();
	ShowCursor(false);
	
	CView::OnRButtonDown(nFlags, point);
}

void CPhysDemoView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bRightPressed)
		return;

	m_bRightPressed = false;

	ShowCursor(true);
	ReleaseCapture();
	
	CView::OnRButtonUp(nFlags, point);
}

BOOL CPhysDemoView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();

	if (pA3DDevice && pA3DDevice->GetShowCursor())
	{
		if (nHitTest != HTCLIENT)
			return FALSE;
		else
		{
			A3DCursor* pCursor = pA3DDevice->GetCursor();
			if (pCursor)
				pCursor->OnSysSetCursor();
		}
	}
	else
		SetCursor(NULL);

	return TRUE;
//	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CPhysDemoView::OnEditAddstaticobj() 
{
	//	Let user select a mox file
	char szFilter[] = "MOX 文件(*.mox)|*.mox||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.mox", NULL, dwFlags, szFilter, NULL);
	FileDia.m_ofn.lpstrInitialDir = g_szWorkDir;
	
	if (FileDia.DoModal() != IDOK)
		return;

	//	Create static object
	CPhysStaticObject* pObject = new CPhysStaticObject;

	if (!pObject->Load(FileDia.GetPathName()))
	{
		MessageBox("加载数据失败!");
		return;
	}

	g_Game.SetPickedObject(pObject);
}

void CPhysDemoView::OnEditAdd2stick() 
{
	//	Create static object
	CPhysModelObject* pObject = new CPhysModelObject;

	if (!pObject->Init(CPhysModelObject::TYPE_2STICKS))
	{
		MessageBox("加载数据失败!");
		return;
	}

	g_Game.SetPickedObject(pObject);
}

void CPhysDemoView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPhysDemoView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_F1:

		DropDummySphere(20.0f);
		break;

	case VK_F2:
	
		g_Game.SetPhysState(CPhysObject::PHY_STATE_PURESIMULATE);
		break;
	
	case VK_F3:
	
		g_Game.SetPhysState(CPhysObject::PHY_STATE_ANIMATION);
		break;

	case VK_F4:
	
		g_Game.SetPhysState(CPhysObject::PHY_STATE_PARTSIMULATE);
		break;
	}
	
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

//	Drop a dummy sphere with specified height
void CPhysDemoView::DropDummySphere(float fHeight)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	A3DVECTOR3 vHitPos;
	if (GetCurTerrainPos(pt.x, pt.y, vHitPos))
	{
		vHitPos.y += fHeight;
		APhysXScene* pPhysXScene = g_Game.GetPhysXScene();
		pPhysXScene->CreateDummySphereActor(APhysXConverter::A2N_Vector3(vHitPos), 1.0f);
	}
}

void CPhysDemoView::OnEditPhysxtrngen() 
{
	CDlgPhysXTrnGen dlg;
	dlg.DoModal();
}

void CPhysDemoView::OnEditAddterrain() 
{
	CCommonFileName FileDlg(g_Render.GetA3DEngine(), 0);
	FileDlg.SetFileExtension("trn2");
	if (FileDlg.DoModal() != IDOK)
		return;

	AString strFile = FileDlg.GetRelativeFileName();

	if (!g_Game.LoadTerrain(strFile))
	{
		MessageBox("加载地形数据失败！");
		return;
	}

	//	Load PhysX terrain
	af_ChangeFileExt(strFile, "_ptb.cfg");
	if (af_IsFileExist(strFile))
	{
		if (!g_Game.LoadPhysXTerrain(strFile))
			MessageBox("加载 PhysX 地形数据失败！");
	}
}



