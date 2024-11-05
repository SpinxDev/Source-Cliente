/*
 * FILE: WndOrthogonal.cpp
 *
 * DESCRIPTION: Orthogonal viewport window base class
 *
 * CREATED BY: Duyuxin, 2003/7/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "MainFrm.h"
#include "Utility.h"
#include "WndOrthogonal.h"
#include "OrthoCamera.h"
#include "OrthoViewport.h"
#include "OrthoGrid.h"
#include "Render.h"
#include "SectorEditorDoc.h"
#include "SectorEditorView.h"
#include "SceneFrame.h"
#include "OptBase.h"
#include "Sector.h"
#include "Portal.h"
#include "Bound.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define MIN_AREASIZE	0.5f
#define MAX_AREASIZE	2000.0f

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement COrthogonalWnd
//
///////////////////////////////////////////////////////////////////////////

COrthogonalWnd::COrthogonalWnd()
{
	m_pCamera	= NULL;
	m_pViewport	= NULL;
	m_pGrid		= NULL;
}

//	Initialize viewport
bool COrthogonalWnd::Init(CRender* pRender, int x, int y, int iWid, int iHei)
{
	if (!CViewportWnd::Init(pRender, x, y, iWid, iHei))
		return false;

	if (!(m_pCamera = new COrthoCamera))
	{
		g_Log.Log("OrthogonalWnd::Init, Failed to new camera object");
		return false;
	}

	A3DVECTOR3 vMins(-100.0f, -100.0f, -WORLD_BOUND);
	A3DVECTOR3 vMaxs(100.0f, 100.0f, WORLD_BOUND);

	if (!(m_pCamera->Init(pRender->GetA3DDevice(), vMins, vMaxs)))
	{
		g_Log.Log("OrthogonalWnd::Init, Failed to initialize camera object");
		return false;
	}

	if (!(m_pViewport = new COrthoViewport))
	{
		g_Log.Log("OrthogonalWnd::Init, Failed to new viewport object");
		return false;
	}

	A3DVIEWPORTPARAM Params;
	Params.X		= x;
	Params.Y		= y;
	Params.Width	= iWid;
	Params.Height	= iHei;
	Params.MinZ		= 0.0f;
	Params.MaxZ		= 1.0f;

	if (!(m_pViewport->Init(pRender->GetA3DDevice(), &Params, true, false, g_aColTable[COL_VIEWPORTBG])))
	{
		g_Log.Log("OrthogonalWnd::Init, Failed to initialize viewport object");
		return false;
	}

	m_pViewport->SetCamera(m_pCamera);

	//	We only create grid object here, COrthoGrid::Init() should be called in
	//	inherited classes !!
	if (!(m_pGrid = new COrthoGrid))
	{
		g_Log.Log("OrthogonalWnd::Init, Failed to new COrthoGrid");
		return false;
	}

	return true;
}

//	Release viewport
void COrthogonalWnd::Release()
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

	if (m_pGrid)
	{
		m_pGrid->Release();
		delete m_pGrid;
		m_pGrid = NULL;
	}

	CViewportWnd::Release();
}

//	Reset window.
bool COrthogonalWnd::Reset()
{
	if (m_pCamera)
		m_pCamera->SetPos(g_vOrigin);

	if (m_pGrid)
		m_pGrid->Resize(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f);

	return true;
}

//	Move and resize viewport
bool COrthogonalWnd::Move(int x, int y, int iWid, int iHei)
{
	CViewportWnd::Move(x, y, iWid, iHei);

	ASSERT(m_pViewport && m_pCamera);

	A3DVIEWPORTPARAM Params;
	Params.X		= x;
	Params.Y		= y;
	Params.Width	= iWid;
	Params.Height	= iHei;
	Params.MinZ		= 0.0f;
	Params.MaxZ		= 1.0f;

	m_pViewport->SetParam(&Params);

	//	Adjust camera
	float fRatio = (float)iHei / iWid;
	A3DVECTOR3 vMaxs = m_pCamera->GetProjectRangeMaxs();
	vMaxs.y = vMaxs.x * fRatio;
	A3DVECTOR3 vMins = -vMaxs;
	m_pCamera->SetProjectRange(vMins, vMaxs);

	//	Adjust grid
	AdjustGrid();

	return true;
}

//	Render viewport
bool COrthogonalWnd::Render()
{
	if (!m_bRender)
		return true;

	ASSERT(m_pViewport && m_pCamera);

	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();

	m_pViewport->Active();
	m_pViewport->ClearDevice();

	//	Set render state
	m_pRender->SetWireRenderStates(true);

	//	Render grid
	if (m_pGrid && g_Configs.bShowGrid)
		m_pGrid->Render();

	//	Render scene
	CFrameScene* pScene = pDoc->GetFrameScene();
	if (pScene && g_Configs.bShowScene)
		pScene->Render();

	//	Flush wire collector
	m_pRender->FlushWireCollector();

	//	Render current opeation
	if (pDoc->GetCurOperation())
		pDoc->GetCurOperation()->Render(this);

	//	Restore render state
	m_pRender->SetWireRenderStates(false);

	//	Draw coordinates
	DrawCoordinates();

	//	Draw grid size
	DrawGridSize(m_pGrid->GetGridSize());

	//	Draw eye position and direction
	DrawEyePosAndDir();

	//	Flush text
	m_pRender->FlushPreText();

	if (m_bActive)
		DrawActiveFrame();

	return true;
}

//	Adjust grid
void COrthogonalWnd::AdjustGrid()
{
	if (!m_pGrid)
		return;

	A3DVECTOR3 vMins = m_pCamera->GetProjectRangeMins();
	A3DVECTOR3 vMaxs = m_pCamera->GetProjectRangeMaxs();
	float fGridSize, fMax = max2(vMaxs.x, vMaxs.y);

	if (fMax > 500.0f)
		fGridSize = 100.0f;
	else if (fMax > 50.0f)
		fGridSize = 10.0f;
	else if (fMax > 5.0f)
		fGridSize = 1.0f;
	else if (fMax > 1.0f)
		fGridSize = 0.1f;
	else
		fGridSize = 0.05f;

	m_pGrid->Resize(vMaxs.x, vMaxs.y, fGridSize);
}

//	Move grid
void COrthogonalWnd::MoveGrid(float fOffH, float fOffV)
{
	if (!m_pGrid)
		return;

	m_pGrid->Move(fOffH, fOffV);
}

//	Frame move
bool COrthogonalWnd::FrameMove(DWORD dwTime)
{
	//	Move camera
	MoveCamera(dwTime);
	//	Zoom
	Zoom(dwTime);

	return true;
} 

//	Move camera
void COrthogonalWnd::MoveCamera(DWORD dwTime)
{
	//	One operation is being done, forbid to maximum viewport
	COperation* pOpt = AUX_GetMainFrame()->GetDocument()->GetCurOperation();
	if (pOpt && pOpt->FixViewport())
		return;

	A3DVECTOR3 vMaxs = m_pCamera->GetProjectRangeMaxs();

	float fSpeed = 4.0f;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		fSpeed = 12.0f;
	else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		fSpeed = 0.4f;
	
	fSpeed *= (float)dwTime * 0.001f;
	float fOffH=0.0f, fOffV=0.0f;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		fOffV = vMaxs.y * fSpeed;
		m_pCamera->MoveUp(fOffV);
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		fOffV = -vMaxs.y * fSpeed;
		m_pCamera->MoveUp(fOffV);
	}
	
	if (GetAsyncKeyState('A') & 0x8000)
	{
		fOffH = -vMaxs.x * fSpeed;
		m_pCamera->MoveRight(fOffH);
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		fOffH = vMaxs.x * fSpeed;
		m_pCamera->MoveRight(fOffH);
	}

	if (fOffH || fOffV)
		MoveGrid(fOffH, fOffV);
}

//	Zoom
void COrthogonalWnd::Zoom(DWORD dwTime)
{
	//	One operation is being done, forbid to maximum viewport
	COperation* pOpt = AUX_GetMainFrame()->GetDocument()->GetCurOperation();
	if (pOpt && pOpt->FixViewport())
		return;

	float fSpeed = 0.0f;
	if (GetAsyncKeyState(VK_HOME) & 0x8000)
		fSpeed = 1.0f;
	else if (GetAsyncKeyState(VK_END) & 0x8000)
		fSpeed = -1.0f;

	if (!fSpeed)
		return;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		fSpeed *= 0.8f;
	else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		fSpeed *= 0.1f;
	else
		fSpeed *= 0.5f;

	fSpeed *= (float)dwTime * 0.001f;

	Zoom(1.0f - fSpeed);
}

//	Zoom viewport
void COrthogonalWnd::Zoom(float fMultiply)
{
	A3DVECTOR3 vMaxs = m_pCamera->GetProjectRangeMaxs();
	float fRatio = vMaxs.x / vMaxs.y;

	vMaxs.x *= fMultiply;
	vMaxs.y *= fMultiply;

	float t = min2(vMaxs.x, vMaxs.y);
	if (t < MIN_AREASIZE)
	{
		if (vMaxs.x < vMaxs.y)
		{
			vMaxs.x = MIN_AREASIZE;
			vMaxs.y = MIN_AREASIZE / fRatio;
		}
		else
		{
			vMaxs.x = MIN_AREASIZE * fRatio;
			vMaxs.y = MIN_AREASIZE;
		}
	}

	t = max2(vMaxs.x, vMaxs.y);
	if (t >MAX_AREASIZE)
	{
		if (vMaxs.x > vMaxs.y)
		{
			vMaxs.x = MAX_AREASIZE;
			vMaxs.y = MAX_AREASIZE / fRatio;
		}
		else
		{
			vMaxs.x = MAX_AREASIZE * fRatio;
			vMaxs.y = MAX_AREASIZE;
		}
	}

	A3DVECTOR3 vMins = -vMaxs;
	m_pCamera->SetProjectRange(vMins, vMaxs);

	//	Adjust grid
	AdjustGrid();
}

void COrthogonalWnd::Zoom(bool bMagnify, float l, float r, float b, float t)
{
	if (l == r || b == r)
		return;

	float ncx = (l + r) * 0.5f;
	float ncy = (b + t) * 0.5f;
	
	float fScale, fWid;
	fWid = m_pCamera->GetProjectRangeMaxs().x - m_pCamera->GetProjectRangeMins().x;

	if (bMagnify)
		fScale = (float)(fabs(r - l) / fWid);
	else
		fScale = (float)(fWid / fabs(r - l));

	//	Move camera and grid to new position
	Zoom(fScale);
	SetCameraPos(ncx, ncy);

	//	Project range may not be same as expected, so do not use l and b
	//	parameters directly, re-calculate them here.
	l = ncx - m_pCamera->GetProjectRangeMaxs().x;
	b = ncy - m_pCamera->GetProjectRangeMaxs().y;

	m_pGrid->MoveTo(l, b);
}

//	Draw coordinates
void COrthogonalWnd::DrawCoordinates()
{
	A3DPOINT2 ptOrigin(m_x+10, m_y+m_iHeight-12);
	DWORD dwCol = g_aColTable[COL_COORDINATES];

	g_pA3DGDI->Draw2DLine(ptOrigin, A3DPOINT2(ptOrigin.x, ptOrigin.y - 15), dwCol);
	g_pA3DGDI->Draw2DLine(ptOrigin, A3DPOINT2(ptOrigin.x + 15, ptOrigin.y), dwCol);

	DrawCoordinatesChar(ptOrigin.x-2, ptOrigin.y-34, ptOrigin.x+22, ptOrigin.y-12);
}

//	Draw grid size
void COrthogonalWnd::DrawGridSize(float fSize)
{
	int iSizeText = PSTR_GRID;
	if (fSize == 100.0f)
		iSizeText = PSTR_100;
	else if (fSize == 10.0f)
		iSizeText = PSTR_10;
	else if (fSize == 1.0f)
		iSizeText = PSTR_1;
	else if (fSize == 0.1f)
		iSizeText = PSTR_01;
	else if (fSize == 0.05f)
		iSizeText = PSTR_005;

	m_pRender->TextOut(PSTR_GRID, m_x+m_iWidth-75, m_y+m_iHeight-20, g_aColTable[COL_COORDINATES]);
	m_pRender->TextOut(iSizeText, m_x+m_iWidth-40, m_y+m_iHeight-20, g_aColTable[COL_COORDINATES]);
}

//	Mouse action handler
bool COrthogonalWnd::OnMouseMove(int x, int y, DWORD dwFlags)
{
	//	Update mouse's position on status bar
	if (PtInViewport(x, y))
	{
		A3DVECTOR3 vPos((float)x, (float)y, 0.5f);
		InvTransform(vPos, vPos);
		AUX_GetMainFrame()->UpdateMousePos(&vPos);
	}
	else
		AUX_GetMainFrame()->UpdateMousePos(NULL);

	//	Operation
	COperation* pOpt = AUX_GetMainFrame()->GetDocument()->GetCurOperation();
	if (pOpt && pOpt->HandleMouseMsg())
	{
		pOpt->OnMouseMove(x, y, dwFlags);
		return true;
	}

	return CViewportWnd::OnMouseMove(x, y, dwFlags);
}

bool COrthogonalWnd::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();

	//	Operation
	COperation* pOpt = pDoc->GetCurOperation();
	if (pOpt && pOpt->HandleMouseMsg())
	{
		pOpt->OnLButtonDown(x, y, dwFlags);
		return true;
	}

	//	Select object
	int iSide;
	CEditObject* pObject = PickPortal(x, y, &iSide);
	if (pObject)
		pDoc->SelectEditObject(pObject);
	else if ((pObject = PickSector(x, y, &iSide)))
		pDoc->SelectEditObject(pObject);

	return CViewportWnd::OnLButtonDown(x, y, dwFlags);
}

bool COrthogonalWnd::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();

	//	Operation
	COperation* pOpt = pDoc->GetCurOperation();
	if (pOpt && pOpt->HandleMouseMsg())
	{
		pOpt->OnLButtonUp(x, y, dwFlags);
		return true;
	}

	return CViewportWnd::OnLButtonUp(x, y, dwFlags);
}

bool COrthogonalWnd::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	//	Operation
	COperation* pOpt = AUX_GetMainFrame()->GetDocument()->GetCurOperation();
	if (pOpt && pOpt->HandleMouseMsg())
	{
		pOpt->OnRButtonDown(x, y, dwFlags);
		return true;
	}

	return CViewportWnd::OnRButtonDown(x, y, dwFlags);
}

/*	Pick sector

	Return sector object's address.

	x, y: pick position in viewport.
	piSide (out): side which is picked up
*/
CSector* COrthogonalWnd::PickSector(int x, int y, int* piSide)
{
	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();
	CSector* pSector = pDoc->GetFirstSector();

	while (pSector)
	{
		int iSide = ClickSector(x, y, pSector);
		if (iSide != SIDE_NONE)
		{
			*piSide = iSide;
			return pSector;
		}

		pSector = pDoc->GetNextSector();
	}

	if (piSide)
		*piSide = SIDE_NONE;

	return NULL;
}

/*	Check whick side of specified sector is clicked

	Return side identify.

	x, y: pick position in viewport.
	pSector: sector will be checked
*/
int COrthogonalWnd::ClickSector(int x, int y, CSector* pSector)
{
	//	Skip invisible sectors
	if (!pSector->IsVisible())
		return SIDE_NONE;

	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();
	A3DVECTOR3 vMins, vMaxs;
	int l, t, r, b;

	vMins = pSector->GetMins();
	vMaxs = pSector->GetMaxs();

	Transform(vMins, vMins);
	Transform(vMaxs, vMaxs);

	l = (int)vMins.x;
	r = (int)vMaxs.x;
	b = (int)vMins.y;
	t = (int)vMaxs.y;

	//	Test left border
	if (x >= l && x <= l+PICK_EPSILON && y >= t && y <= b)
		return SIDE_LEFT;

	//	Test top border
	if (x >= l && x <= r && y >= t && y <= t+PICK_EPSILON)
		return SIDE_TOP;

	//	Test right border
	if (x >= r-PICK_EPSILON && x <= r && y >= t && y <= b)
		return SIDE_RIGHT;

	//	Test bottom border
	if (x >= l && x <= r && y >= b-PICK_EPSILON && y <= b)
		return SIDE_BOTTOM;

	return SIDE_NONE;
}

/*	Pick portal

	Return portal object's address.

	x, y: pick position in viewport.
	piSide (out): side which is picked up
*/
CPortal* COrthogonalWnd::PickPortal(int x, int y, int* piSide)
{
	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();
	CPortal* pPortal = pDoc->GetFirstPortal();

	while (pPortal)
	{
		int iSide = ClickPortal(x, y, pPortal);
		if (iSide != SIDE_NONE)
		{
			*piSide = iSide;
			return pPortal;
		}

		pPortal = pDoc->GetNextPortal();
	}

	return NULL;
}

/*	Check whick side of specified portal is clicked

	Return side identify.

	x, y: pick position in viewport.
	pSector: portal will be checked
*/
int COrthogonalWnd::ClickPortal(int x, int y, CPortal* pPortal)
{
	//	Skip invisible portal
	if (!pPortal->IsVisible())
		return SIDE_NONE;

	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();
	A3DVECTOR3 aVerts[4];
	int i, l, t, r, b;

	pPortal->GetVertices(aVerts);

	l = t = 999999;
	r = b = -999999;

	for (i=0; i < 4; i++)
	{
		Transform(aVerts[i], aVerts[i]);

		l = min2(l, (int)aVerts[i].x);
		t = min2(t, (int)aVerts[i].y);
		r = max2(r, (int)aVerts[i].x);
		b = max2(b, (int)aVerts[i].y);
	}

	if (l != r)
	{
		int m = (t == b) ? (r - l) / 2 : PICK_EPSILON;
		int n = (t == b) ? PICK_EPSILON : 0;

		//	Test left border
		if (x >= l-PICK_EPSILON && x <= l+m && y >= t-n && y <= b+n)
			return SIDE_LEFT;

		//	Test right border
		if (x >= r-m && x <= r+PICK_EPSILON && y >= t-n && y <= b+n)
			return SIDE_RIGHT;
	}

	if (t != b)
	{
		int m = (l == r) ? (b - t) / 2 : PICK_EPSILON;
		int n = (l == r) ? PICK_EPSILON : 0;

		//	Test top border
		if (x >= l-n && x <= r+n && y >= t-PICK_EPSILON && y <= t+m)
			return SIDE_TOP;

		//	Test bottom border
		if (x >= l-n && x <= r+n && y >= b-m && y <= b+PICK_EPSILON)
			return SIDE_BOTTOM;
	}

	return SIDE_NONE;
}

//	Transform
void COrthogonalWnd::Transform(A3DVECTOR3& vIn, A3DVECTOR3& vOut)
{
	m_pViewport->Transform(vIn, vOut);
	vOut.x -= (float)m_x;
	vOut.y -= (float)m_y;
}

//	Inverse transform
void COrthogonalWnd::InvTransform(A3DVECTOR3& vIn, A3DVECTOR3& vOut)
{
	vIn.x += (float)m_x;
	vIn.y += (float)m_y;
	m_pViewport->InvTransform(vIn, vOut);
}

//	Draw Eye's position and direction
void COrthogonalWnd::DrawEyePosAndDir()
{
	A3DVECTOR3 vPos, vDir, v1, v2;
	AUX_GetMainFrame()->GetView()->GetEyePosAndDir(&vPos, &vDir);

	//	Draw eye postion
	m_pViewport->Transform(vPos, v1);

	A3DRECT rc;

	rc.left		= (int)(v1.x - 5 + 0.5f);
	rc.right	= (int)(v1.x + 5 + 0.5f);
	rc.top		= (int)(v1.y - 5 + 0.5f);
	rc.bottom	= (int)(v1.y + 5 + 0.5f);

	g_pA3DGDI->Draw2DRectangle(rc, g_aColTable[COL_EYEPOS]);

	//	Draw eye diretion
	m_pViewport->Transform(vPos + (vDir * 300.0f), v2);
	v1.z = v2.z = 0.0f;
	v2 = v1 + Normalize(v2 - v1) * 20.0f;

	g_pA3DGDI->Draw2DLine(A3DPOINT2((long)v1.x, (long)v1.y), A3DPOINT2((long)v2.x, (long)v2.y), g_aColTable[COL_EYEPOS]);
}

