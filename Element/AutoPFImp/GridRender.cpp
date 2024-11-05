// GridRender.cpp: implementation of the CGridRender class.
//
//////////////////////////////////////////////////////////////////////

#include "Global.h"

#include "GridRender.h"
#include "Render.h"
#include "MainFrm.h"
#include "FormIsland.h"

using namespace AutoMove;


//////////////////////////////////////////////////////////////////////////
//local var
static int l_img_width = 1;
static int l_img_length = 1;

//////////////////////////////////////////////////////////////////////////
//local func
void   l_SetMapColor(BYTE * pData, int x, int y, BYTE r, BYTE g, BYTE b)
{
	if (x < 0 || x >= l_img_width || y < 0 || y >= l_img_length)
	{
		return;
	}
	//B G R
	int index = y * l_img_width* 4 + x*4;
	pData[index] = b;
	pData[index +1] = g;
	pData[index +2] = r;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define  GRID_NORMAL_COLOR 0xff00ff00
#define  GRID_UNREACH_COLOR 0xffff0000

#define  VIEW_RANGE  3000.0f
//#define OBS_MAP_WIDTH 1024
#define OBS_MAP_DIVIDE 32
#define  MOVE_SPEED   50.0f

CGridRender::CGridRender()
{
	m_pA3DCamera = NULL;
	m_pSprite = NULL;
	m_pData = NULL;
	m_bRDrag = false;
	m_Op = OP_NONE;
	m_pLayerMap = NULL;
	m_bEndAddWayEdge = false;
	
	m_iWidth = 800;
	m_iHeight = 600;

}

CGridRender::~CGridRender()
{
	Release();
}

bool CGridRender::Init(int mapW, int mapL)
{
	//	Create camera
	if (!(m_pA3DCamera = new A3DOrthoCamera))
	{
		g_Log.Log("CGridRender::Init: Failed to create camera object!");
		return false;
	}
	if (!m_pA3DCamera->Init(g_Render.GetA3DDevice(), -100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f))
	{
		g_Log.Log(0, "CGridRender::Init: Failed to initialize camera.");
		return false;
	}
	m_pA3DCamera->SetPos(g_vOrigin);

	m_pA3DCamera->SetDirAndUp(-g_vAxisY, g_vAxisZ);
	m_pA3DCamera->SetProjectionParam(-690, 690, -517, 517, -VIEW_RANGE, VIEW_RANGE);
	
	g_Render.GetA3DViewport()->SetCamera(m_pA3DCamera);


	m_pSprite = new A2DSprite;
	m_pSprite->Release();

	A3DRECT rc[OBS_MAP_DIVIDE*OBS_MAP_DIVIDE];
	int img_width, img_length;
	img_width = mapW;
	img_length = mapL;
	l_img_width = mapW;
	l_img_length = mapL;
	
	int nWidth  = img_width/OBS_MAP_DIVIDE;
	int nHeight = img_length/OBS_MAP_DIVIDE;
	for(int i = 0; i < OBS_MAP_DIVIDE; i++)
		for( int j = 0; j < OBS_MAP_DIVIDE; j++)
			rc[i*OBS_MAP_DIVIDE + j] = A3DRECT(nWidth*j,i*nHeight,nWidth*(j+1),nHeight*(i+1));
	m_pSprite->InitWithoutSurface(g_Render.GetA3DDevice(),img_width,img_length,A3DFMT_X8R8G8B8,OBS_MAP_DIVIDE*OBS_MAP_DIVIDE,rc);
	m_pSprite->SetColor(A3DCOLORRGBA(255,255,255,120));

	m_pData = new BYTE[img_width*img_length*4];
	memset(m_pData,255,img_width*img_length*4);

	m_bEndAddWayEdge = false;
	return true;
}


void CGridRender::Release()
{
	A3DRELEASE(m_pA3DCamera);
	if(m_pSprite) 
	{
		m_pSprite->Release();
		delete m_pSprite;
		m_pSprite = NULL;
	}
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}
}

void CGridRender::Resize(int cx, int cy)
{
	float fRange = m_pA3DCamera->GetRight();
	float fTop = fRange * cy / cx;
	m_pA3DCamera->SetProjectionParam(-fRange, fRange, -fTop, fTop, -VIEW_RANGE, VIEW_RANGE);
	m_iHeight = cy;
	m_iWidth  = cx;
}

void CGridRender::Render()
{
	if (!m_pLayerMap)
	{
		return;
	}
	g_Render.GetA3DViewport()->SetCamera(m_pA3DCamera);

	int img_width, img_length;

	m_pLayerMap->GetRMap()->GetImageSize(img_width, img_length);

	ARectF rc(-img_width*0.5f, img_length*0.5f,  img_width*0.5f, -img_length*0.5f);
	float fScaleX,fScaleY;
	A3DVECTOR3 vLeftUp,vRightUp,vLeftBottom,vRightBottom;
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.left,0,rc.top), vLeftUp);
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.right,0,rc.top), vRightUp);
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.left,0,rc.bottom), vLeftBottom);
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.right,0,rc.bottom), vRightBottom);

	float w,h;
	w = vRightUp.x - vLeftUp.x + 1;
	h = vLeftBottom.y - vLeftUp.y + 1;
			

	m_pSprite->UpdateTextures(m_pData,img_width*4,A3DFMT_X8R8G8B8);
	//m_pSprite->UpdateTextures(m_pData,OBS_MAP_WIDTH*3,A3DFMT_R8G8B8);

	float		x, y;
	int			nX, nY;

	float		wc = w / OBS_MAP_DIVIDE;
	float		hc = h / OBS_MAP_DIVIDE;

	y = vLeftUp.y;
	for(int i=0; i<OBS_MAP_DIVIDE; i++)
	{
		x = vLeftUp.x;
		nY = (int) y;
		for(int j=0; j<OBS_MAP_DIVIDE; j++)
		{
			nX = (int) x;
			m_pSprite->SetPosition(nX, nY);
			fScaleX = (float)(int(x + wc) - nX) / (img_width /OBS_MAP_DIVIDE);
			fScaleY = (float)(int(y + hc) - nY) / (img_length /OBS_MAP_DIVIDE);
			m_pSprite->SetScaleX(fScaleX);
			m_pSprite->SetScaleY(fScaleY);
			m_pSprite->SetCurrentItem(i * OBS_MAP_DIVIDE + j);
			m_pSprite->DrawToBack();	
			x += wc;
		}
		y += hc;
	}

	if (m_bEndAddWayEdge)
	{
		g_pA3DGDI->Draw2DLine(m_ptWayEdgeStart, m_ptWayEdgeEnd, A3DCOLORRGB(0,255,255));
	}

	if (m_pLayerMap->GetIslandList())
	{
		m_pLayerMap->GetIslandList()->Render();
	}
}
void CGridRender::UpdateMap()
{
	if (!m_pLayerMap)
	{
		return;
	}
	int x, y;
	for (y = 0; y <l_img_length; y++)
	{
		for (x = 0; x <l_img_width; x++)
		{
			
			if (m_pLayerMap->GetRMap()->GetPixel(x, y))
			{
				l_SetMapColor(m_pData, x, y, g_Color[COL_PASSABLE*3], g_Color[COL_PASSABLE*3+1], g_Color[COL_PASSABLE*3+2]);
			}
			else
			{
				l_SetMapColor(m_pData, x, y, g_Color[COL_BLOCK*3], g_Color[COL_BLOCK*3+1], g_Color[COL_BLOCK*3+2]);
			}
		}
	}
	
	if (g_iLayerStart == m_iLayer)
	{
		l_SetMapColor(m_pData, g_ptStart.x, g_ptStart.y, g_Color[COL_START*3], g_Color[COL_START*3+1], g_Color[COL_START*3+2]);
	}

	if (g_iLayerGoal == m_iLayer)
	{
		l_SetMapColor(m_pData, g_ptGoal.x, g_ptGoal.y, g_Color[COL_END*3], g_Color[COL_END*3+1], g_Color[COL_END*3+2]);
	}

	int i;
	for (i =0; i < (int)g_InitPath.size(); ++i)
	{
		const PathNode& pathNode = g_InitPath[i];
		if (pathNode.layer == m_iLayer)
		{
			l_SetMapColor(m_pData, (int)pathNode.ptMap.x, (int)pathNode.ptMap.y, g_Color[COL_RAW_PATH0*3], g_Color[COL_RAW_PATH0*3+1], g_Color[COL_RAW_PATH0*3+2]);
		}
		else
		{
			l_SetMapColor(m_pData, (int)pathNode.ptMap.x, (int)pathNode.ptMap.y, g_Color[COL_RAW_PATH1*3], g_Color[COL_RAW_PATH1*3+1], g_Color[COL_RAW_PATH1*3+2]);
		}

	}
}
void CGridRender::Tick(DWORD dwTickTime)
{
	_FrameMove(dwTickTime);
}


void CGridRender::_MoveCameraRight(bool bRight, float fDeltaTime)
{
	A3DVECTOR3 vDist = m_pA3DCamera->A3DCameraBase::GetRight() * (MOVE_SPEED * fDeltaTime);

	if (bRight)
		m_pA3DCamera->Move(vDist);
	else
		m_pA3DCamera->Move(-vDist);
}

void CGridRender::_MoveCameraUp(bool bUp, float fDeltaTime)
{
	A3DVECTOR3 vDist = m_pA3DCamera->GetUp() * (MOVE_SPEED * fDeltaTime);

	if (bUp)
		m_pA3DCamera->Move(vDist);
	else
		m_pA3DCamera->Move(-vDist);
}

bool CGridRender::_FrameMove(DWORD dwDeltaTime)
{
	float fDeltaTime = dwDeltaTime * 0.001f;
	a_ClampRoof(fDeltaTime, 0.1f);
	if (GetAsyncKeyState('A') & 0x8000)
		_MoveCameraRight(false, fDeltaTime);
	else if (GetAsyncKeyState('D') & 0x8000)
		_MoveCameraRight(true, fDeltaTime);
	if (GetAsyncKeyState('W') & 0x8000)
		_MoveCameraUp(true, fDeltaTime);
	else if (GetAsyncKeyState('S') & 0x8000)
		_MoveCameraUp(false, fDeltaTime);

	return true;
}

bool CGridRender::OnMouseMove(int x, int y, DWORD dwFlags)
{
	
	if (m_bRDrag)
	{
		float fDelta = -(y - m_ptLastMou.y) * 10.0f;
		float fRange = m_pA3DCamera->GetRight() + fDelta;
		a_ClampFloor(fRange, 10.0f);

		float fTop = fRange * m_iHeight / m_iWidth;
		//	Top view
		m_pA3DCamera->SetProjectionParam(-fRange, fRange, -fTop, fTop, -VIEW_RANGE, VIEW_RANGE);
		m_ptLastMou.x = x;
		m_ptLastMou.y = y;
	}
	if (m_Op == OP_ISLAND_ADDBORDER)
	{
		m_ptWayEdgeEnd.x = x;
		m_ptWayEdgeEnd.y = y;
	}

	return true;
}

bool CGridRender::OnRButtonDown(int x, int y, DWORD dwFlags)
{

	m_bRDrag = true;
	m_ptLastMou.x = x;
	m_ptLastMou.y = y;
	
	HWND hWnd = AUX_GetMainFrame()->GetView()->GetSafeHwnd();
	::SetCapture(hWnd);
	return true;
}

bool CGridRender::OnRButtonUp(int x, int y, DWORD dwFlags)
{
	if (m_bRDrag)
	{
		m_bRDrag = false;
		::ReleaseCapture();
	}
	if (m_Op == OP_ISLAND_ADDBORDER){
		m_bEndAddWayEdge = false;
	}
	return true;
}

bool CGridRender::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_Op != OP_SETSTART && m_Op != OP_SETGOAL 
		&& m_Op != OP_ISLAND_ADDBORDER
		&& m_Op != OP_ISLAND_DELBORDER
		&& m_Op != OP_ISLAND_SELECT
		&& m_Op != OP_ISLAND_SETPASSTHROUGH)
	{
		return false;
	}

	A3DPOINT2 ptMap;
	TransScr2Map(A3DPOINT2(x, y), ptMap);

	
	switch( m_Op)
	{
	case OP_SETSTART:
		g_ptStart = ptMap;
		g_iLayerStart = m_iLayer;
		break;
	case OP_SETGOAL:
		g_ptGoal = ptMap;
		g_iLayerGoal = m_iLayer;
		break;
	case OP_ISLAND_ADDBORDER:
		{
			if (m_bEndAddWayEdge)
			{
				//add border line
				A3DPOINT2 ptMapStart;
				TransScr2Map(m_ptWayEdgeStart, ptMapStart);
				CFormIsland * pFormIsland = (CFormIsland *)AUX_GetMainFrame()->GetTabCtrlBar()->GetView(RUNTIME_CLASS(CFormIsland));
				bool bStillInvalid = true;
				if (pFormIsland->AddBorderLine(ptMapStart, ptMap, bStillInvalid)){
					m_bEndAddWayEdge = bStillInvalid;
					if (!bStillInvalid){
						//	当前孤岛已完整，为避免误操作，停止向当前孤岛添加边界
						pFormIsland->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_ISLAND_ADDBORDER, BN_CLICKED));
					}
				}
			}else{
				m_bEndAddWayEdge = !m_bEndAddWayEdge;
			}
			m_ptWayEdgeStart.x = x;
			m_ptWayEdgeStart.y = y;
		}
		break;
	case OP_ISLAND_DELBORDER:
		{
			CFormIsland * pFormIsland = (CFormIsland *)AUX_GetMainFrame()->GetTabCtrlBar()->GetView(RUNTIME_CLASS(CFormIsland));
 			pFormIsland->DelBorderPt(ptMap);
		}
		break;
	case OP_ISLAND_SELECT:
		{
			CFormIsland * pFormIsland = (CFormIsland *)AUX_GetMainFrame()->GetTabCtrlBar()->GetView(RUNTIME_CLASS(CFormIsland));
 			pFormIsland->SelectIsland(ptMap);
		}
		break;
	case OP_ISLAND_SETPASSTHROUGH:
		{
			CFormIsland * pFormIsland = (CFormIsland *)AUX_GetMainFrame()->GetTabCtrlBar()->GetView(RUNTIME_CLASS(CFormIsland));
 			pFormIsland->SetPassThrough(ptMap);
		}
		break;
	default:
		assert(0);
		return false;
	}

	UpdateMap();
	
	return true;
}

bool CGridRender::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	return true;
}

