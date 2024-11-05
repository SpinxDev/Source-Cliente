// RenderWnd.cpp : implementation file
//

#include "stdafx.h"
#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
#include "Shlwapi.h"
#include "RenderWnd.h"
#include "A3DGFXExMan.h"
#include "EC_Model.h"

#include "RenderableObject.h"
#include "Renderable2DObject.h"
#include "Renderable3DObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern char g_szAppPath[];


#define NUM_2D_EXT 5
#define NUM_3D_EXT 2

const TCHAR *sz2DFileExt[] = 
{
		_T("bmp"),
		_T("jpg"),
		_T("dds"),
		_T("png"),
		_T("tga")
};

const TCHAR*sz3DFileExt[] = 
{
	//		"mox",
		_T("ecm"),
		_T("gfx")
};

bool Aux_Is2DFile(CString ext)
{
	ext.MakeLower();
	
	for( int i = 0; i < NUM_2D_EXT; ++i)
		if(sz2DFileExt[i] == ext) 
			return true;
		
		return false;
};

bool Aux_Is3DFile(CString ext)
{
	ext.MakeLower();
	
	for( int i = 0; i < NUM_3D_EXT; ++i)
		if(sz3DFileExt[i] == ext)
			return true;
		
		return false;
};

CRenderableObject* LoadObject(CString& pathName)
{
	pathName.MakeLower();
	
	int pos = pathName.ReverseFind('.');
	if(pos==-1) return NULL;
	
	int len = pathName.GetLength();
	
	CString ext = pathName.Right(len - pos - 1);
	
	if(Aux_Is2DFile(ext))
	{
		CRenderable2DObject* pObj = new CRenderable2DObject;
		ASSERT(pObj);
		if(!pObj->Load(pathName))
			return NULL;
		return pObj;
		
	}else if(Aux_Is3DFile(ext))
	{
		CRenderable3DObject* pObj = new CRenderable3DObject;
		ASSERT(pObj);
		if(!pObj->Load(pathName))
			return NULL;
		return pObj;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CRenderWnd

CRenderWnd::CRenderWnd()
{
	m_dwBuffer= NULL;

#ifdef _ANGELICA22
	m_pMemSurface = NULL;
#endif
	m_pRenderableObject = NULL;
	
	m_iWidth = 640;
	m_iHeight = 480;

	m_pA3DEngine = NULL;
	m_pA3DDevice = NULL;
	m_pA3DCamera = NULL;
	m_pA3DViewport = NULL;
	m_pRenderTarget = NULL;
	m_pA3DOrthoCamera = 0;
	
	m_bOrtho = false;

	m_LbuttonDown = FALSE;
}

CRenderWnd::~CRenderWnd()
{
	BlankWnd();
	ReleaseEngine();
}

bool CRenderWnd::InitRenderWnd()
{
	bool flag = InitEngine();

	//Timer
	if (flag)
		SetTimer(1000,33,NULL);

	return flag;
}

bool CRenderWnd::InitEngine()
{
	m_pA3DEngine = CRender::GetInstance()->GetA3DEngine();
	m_pA3DDevice = CRender::GetInstance()->GetA3DDevice();
	
	RECT rcClient;
	GetClientRect(&rcClient);
	m_iWidth = rcClient.right;
	m_iHeight = rcClient.bottom;

	m_dwBuffer = new DWORD[m_iWidth * m_iHeight];
	memset(m_dwBuffer, 0, sizeof(DWORD) * m_iWidth * m_iHeight);

#if defined(_ANGELICA2)

#elif defined(_ANGELICA21)
    m_pMemSurface = m_pA3DDevice->CreateSystemMemSurface(m_iWidth, m_iHeight, A3DFMT_A8R8G8B8);
    if (!m_pMemSurface)
        return false;
#else

	m_pMemSurface = m_pA3DDevice->CreateSystemMemSurface(m_iWidth, m_iHeight, A3DFMT_A8R8G8B8);
	//if (FAILED(m_pA3DDevice->GetD3DDevice()->CreateOffscreenPlainSurface(m_iWidth, m_iHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pMemSurface, NULL)))
	if(m_pMemSurface == NULL)
		return false;
#endif
	m_pRenderTarget = CreateRenderTarget(m_pA3DDevice, m_iWidth, m_iHeight, true, true);
	if (!m_pRenderTarget)
		return false;

	//	Create A3D viewport
	if (!CRender::GetInstance()->GetA3DDevice()->CreateViewport(&m_pA3DViewport, 0, 0, 
		rcClient.right, rcClient.bottom, 0.0f, 1.0f, true, true, 0xff808080))
		return false;
	
	//	Create camera
	if (!(m_pA3DCamera = new A3DCamera))
		return false;
	
	float fRatio = (float)m_iWidth / m_iHeight;
	if (!m_pA3DCamera->Init(CRender::GetInstance()->GetA3DDevice(), DEG2RAD(56), 0.1f, 3000.0f,fRatio))
		return false;
	
	m_pA3DCamera->SetPos(A3D::g_vOrigin);
	m_pA3DCamera->SetDirAndUp(-A3D::g_vAxisZ, A3D::g_vAxisY);
	
	// ortho
	
	if (!(m_pA3DOrthoCamera = new A3DOrthoCamera))
		return false;
	
	if (!m_pA3DOrthoCamera->Init(CRender::GetInstance()->GetA3DDevice(), -100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f))
		return false;
	
	m_pA3DOrthoCamera->SetPos(A3D::g_vOrigin + A3DVECTOR3(0,2,0));
	m_pA3DOrthoCamera->SetDirAndUp(-A3D::g_vAxisY, A3D::g_vAxisZ);
	m_pA3DOrthoCamera->SetProjectionParam(-100, 100, -100, 100, 30000, -30000);

	return true;
}

void CRenderWnd::ReleaseEngine()
{

	A3DRELEASE(m_pRenderTarget);
	delete [] m_dwBuffer;
	 m_dwBuffer= NULL;

#if defined _ANGELICA22
	if (m_pMemSurface)
	{
		m_pMemSurface->Release();
		m_pMemSurface = NULL;
	}
#endif

	if(m_pA3DCamera)
	{
		m_pA3DCamera->Release();
		delete m_pA3DCamera;
		m_pA3DCamera = NULL;
	}
	if(m_pA3DViewport)
	{
		m_pA3DViewport->Release();
		delete m_pA3DViewport;
		m_pA3DViewport = 0; 
	}
	
	if(m_pA3DOrthoCamera)
	{
		m_pA3DOrthoCamera->Release();
		delete m_pA3DOrthoCamera;
		m_pA3DOrthoCamera = 0; 
	}
}


void CRenderWnd::Tick()
{
	static DWORD dwLastTime = 0;
	DWORD curTime = timeGetTime();
	DWORD delta = curTime - dwLastTime;
	
	if(m_pRenderableObject)
	{
		if(delta > 0 ) 
		{
			m_pRenderableObject->Tick(delta);

			dwLastTime = curTime;
		}
	}	
	AfxGetGFXExMan()->Tick(delta);
}

void CRenderWnd::Render()
{
	if(m_pA3DCamera==NULL || m_pA3DViewport==NULL) return;
	
	CRect rc;
	GetClientRect( &rc);
	if(rc.Width() < 10 || rc.Height() < 10) return;
	
	//AfxGetSkillGfxEventMan()->Render();
	if (!CRender::GetInstance()->BeginRender(false))
		return;

#ifdef _ANGELICA2
	m_pA3DDevice->SetDefaultRestoreToRenderTarget();
#endif
	ApplyRenderTarget(m_pA3DDevice, m_pRenderTarget);

	//REMDER
	if(m_bOrtho) m_pA3DViewport->SetCamera(m_pA3DOrthoCamera);
	else m_pA3DViewport->SetCamera(m_pA3DCamera);
	
	m_pA3DViewport->Active();
	m_pA3DViewport->ClearDevice();
	A3DEngine* pA3DEngine = CRender::GetInstance()->GetA3DEngine();
	A3DDevice* pA3DDevice = CRender::GetInstance()->GetA3DDevice();
	
	pA3DDevice->Clear(D3DCLEAR_TARGET,0xFF404040,0.0f,0);
	
	//	Set render state
	if(m_pRenderableObject)
	{
		if(m_pRenderableObject->GetObjectType() != RENDERABLE_OBJECT_2D) 
			pA3DDevice->SetAlphaTestEnable(true);
		else 
			pA3DDevice->SetAlphaTestEnable(false);
		
		pA3DDevice->SetAlphaBlendEnable(false);
		pA3DDevice->SetAlphaFunction(A3DCMP_GREATEREQUAL);
		pA3DDevice->SetAlphaRef(84);
		m_pRenderableObject->Render(m_pA3DViewport);
	}
	pA3DEngine->GetA3DLitModelRender()->SetDNFactor(1);
	pA3DEngine->GetA3DLitModelRender()->Render(m_pA3DViewport, false);
	pA3DEngine->GetA3DLitModelRender()->ResetRenderInfo(false);
	AfxGetGFXExMan()->RenderAllSkinModels(m_pA3DViewport);
#if defined _ANGELICA21 || defined _ANGELICA2
	A3DSkinRender* pSkinRender = pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
#else
	A3DSkinRenderBase* pSkinRender = pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
#endif
	pSkinRender->Render(m_pA3DViewport, false);
	pSkinRender->RenderAlpha(m_pA3DViewport);
	
	pA3DDevice->SetAlphaTestEnable(false);
	pA3DDevice->SetAlphaBlendEnable(true);
	pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	
	pA3DEngine->GetA3DLitModelRender()->RenderAlpha(m_pA3DViewport);
	pA3DEngine->GetA3DLitModelRender()->ResetRenderInfo(true);
	
	AfxGetGFXExMan()->RenderAllGfx(m_pA3DViewport);

	RestoreRenderTarget(m_pA3DDevice, m_pRenderTarget);

	CRender::GetInstance()->EndRender(false);
	
	if (m_pRenderTarget)
	{
#ifdef _ANGELICA2
		IDirect3DSurface8* pTargetSurface = m_pRenderTarget->GetTargetSurface();

		D3DLOCKED_RECT rcLocked;
		HRESULT hr = pTargetSurface->LockRect(&rcLocked, NULL, D3DLOCK_READONLY);
		if (FAILED(hr))
			return;

		BYTE* pBytes = (BYTE*)rcLocked.pBits;
		DWORD* pDst = m_dwBuffer;
		for (int iRowIdx = 0; iRowIdx < m_iHeight; ++iRowIdx)
		{
			memcpy(pDst, pBytes, m_iWidth * sizeof(DWORD));
			pDst += m_iWidth;
			pBytes += rcLocked.Pitch;
		}

		pTargetSurface->UnlockRect();
		Invalidate();
#elif defined _ANGELICA22 || defined _ANGELICA21
		IDirect3DSurface9* pTargetSurface = m_pRenderTarget->GetTargetSurface()->m_pD3DSurface;
#endif

#ifdef _ANGELICA2
#else
        m_pA3DDevice->GetRenderTargetData(pTargetSurface, m_pMemSurface);
		D3DLOCKED_RECT rcLocked;
		HRESULT hr = m_pMemSurface->LockRect(&rcLocked, NULL, D3DLOCK_READONLY);
		if (FAILED(hr))
			return;

		BYTE* pBytes = (BYTE*)rcLocked.pBits;
		DWORD* pDst = m_dwBuffer;
		for (int iRowIdx = 0; iRowIdx < m_iHeight; ++iRowIdx)
		{
			memcpy(pDst, pBytes, m_iWidth * sizeof(DWORD));
			pDst += m_iWidth;
			pBytes += rcLocked.Pitch;
		}

		m_pMemSurface->UnlockRect();
		Invalidate();
#endif
	}
}

void CRenderWnd::BlankWnd()
{
	if (m_pRenderableObject)
	{
		m_pRenderableObject->Release();
		delete m_pRenderableObject;
		m_pRenderableObject = NULL;
	}
}

bool CRenderWnd::SetRenderableObject(CString& filepath)
{
	CString inpath = filepath;
	inpath.MakeLower();
	
	if(m_pRenderableObject)
	{
		m_pRenderableObject->Release();
		delete m_pRenderableObject;
		m_pRenderableObject = NULL;
	}
	m_sizeinfo.Empty();
	
	m_pRenderableObject = LoadObject(filepath);
	
	if(m_pRenderableObject)
	{
		//2D
		if(m_pRenderableObject->GetObjectType() == RENDERABLE_OBJECT_2D)
		{
			CRenderable2DObject *p2DObj = (CRenderable2DObject*)m_pRenderableObject;
			
			//get image infomation
			int nWidth = p2DObj->GetWidth();
			int nHeight = p2DObj->GetHeight();
			m_sizeinfo.Format(_T("W*H: %d*%d"), nWidth, nHeight);
			
			float sx = m_iWidth/(float)nWidth;
			float sy = m_iHeight/(float)nHeight;
			
			if(sx < sy) sy = sx;
			p2DObj->ResetScale();
			p2DObj->ScaleObject(sy);
			nWidth = p2DObj->GetWidth();
			nHeight = p2DObj->GetHeight();
			
			//set image position in window;
			int x, y;
			if (nWidth < m_iWidth)
				x = (m_iWidth - nWidth)/2;
			else
				x = 0;
			if (nHeight < m_iHeight)
				y = (m_iHeight - nHeight)/2;
			else
				y = 0;
			
			p2DObj->SetPos(x, y);			
		}
		else if(m_pRenderableObject->GetObjectType() == RENDERABLE_OBJECT_3D)
		{
			CRenderable3DObject* p3DObj = (CRenderable3DObject*)m_pRenderableObject;

			p3DObj->GetAABB(&m_ObjectAABB);

			A3DVECTOR3 objpos = p3DObj->GetPos();
			m_LookAt = ( objpos.x, objpos.y + (m_ObjectAABB.Maxs.y + m_ObjectAABB.Mins.y)/2, objpos.z);
			
			float fRadius = 0.0f;
			if(m_ObjectAABB.Extents.x > fRadius) 
				fRadius = m_ObjectAABB.Extents.x;
			if(m_ObjectAABB.Extents.y > fRadius) 
				fRadius = m_ObjectAABB.Extents.y;
			if(m_ObjectAABB.Extents.z > fRadius) 
				fRadius = m_ObjectAABB.Extents.z;
			
			fRadius = fRadius*3;
			a_ClampFloor(fRadius, 2.f);
			m_CameraPos = A3DVECTOR3(m_LookAt.x, m_LookAt.y, m_LookAt.z + fRadius);

			A3DVECTOR3 vDir = m_LookAt - m_CameraPos;
			m_OrigDis = vDir.Magnitude();	//record origin distance from look-at point to camera positon
			m_CameraDir = a3d_Normalize(vDir);
			m_CameraUp = A3D::g_vAxisY;

			m_pA3DCamera->SetPos(m_CameraPos);
			m_pA3DCamera->SetDirAndUp(vDir, m_CameraUp);

		}
		
	}
	else
		return false;
	
	return true;
}

BEGIN_MESSAGE_MAP(CRenderWnd, CStatic)
	//{{AFX_MSG_MAP(CRenderWnd)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenderWnd message handlers

BOOL CRenderWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}


void CRenderWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	//Render();
	Tick();
	Render();
	
	CStatic::OnTimer(nIDEvent);
}

void CRenderWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	SetCapture();
	m_LbuttonDown = TRUE;
	m_OldMousePos = point;
	CStatic::OnLButtonDown(nFlags, point);
}

void CRenderWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_pRenderableObject || m_pRenderableObject->GetObjectType() != RENDERABLE_OBJECT_3D || m_LbuttonDown == FALSE) 
		return;

	int detx = point.x - m_OldMousePos.x;
	int dety = point.y - m_OldMousePos.y;
	m_OldMousePos = point;


	float xfRad = detx/62.8f;
	float yfRad = -dety/62.8f;

	//xfRad
	A3DVECTOR3 camerapos = a3d_RotatePosAroundLine(m_CameraPos, m_LookAt, m_CameraUp, xfRad);
	m_CameraPos = camerapos;
	m_CameraDir = a3d_Normalize(m_LookAt - m_CameraPos);

	m_pA3DCamera->SetPos(m_CameraPos);
	m_pA3DCamera->SetDirAndUp(m_CameraDir, m_CameraUp);

	//yfRad	
	A3DVECTOR3 rotateline;
	rotateline.CrossProduct(m_CameraDir, m_CameraUp);
	rotateline = a3d_Normalize(rotateline);
	
	camerapos = a3d_RotatePosAroundLine(m_CameraPos, m_LookAt, rotateline, yfRad);
	m_CameraPos = camerapos;
	m_CameraDir = a3d_Normalize(m_LookAt - m_CameraPos);
	
	A3DVECTOR3 cameraup;
	cameraup = a3d_RotateVecAroundLine(m_CameraUp, m_LookAt, rotateline, yfRad);
	m_CameraUp = a3d_Normalize(cameraup);

	m_pA3DCamera->SetPos(m_CameraPos);
	m_pA3DCamera->SetDirAndUp(m_CameraDir, m_CameraUp);


	CStatic::OnMouseMove(nFlags, point);
}

void CRenderWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		ReleaseCapture();
		m_LbuttonDown = FALSE;
	}

	CStatic::OnLButtonUp(nFlags, point);
}

BOOL CRenderWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

	if( m_LbuttonDown == TRUE || !m_pRenderableObject || m_pRenderableObject->GetObjectType() != RENDERABLE_OBJECT_3D)
		return CStatic::OnMouseWheel(nFlags, zDelta, pt);

	float detdis = zDelta;
	
	A3DVECTOR3 dir = m_LookAt - m_CameraPos;
	
	float olddis = dir.Magnitude();
	detdis = m_OrigDis*zDelta/6000;
	float newdis = olddis - detdis;

	if ( newdis < 1.0f )
		newdis = 1.0f;

	m_CameraPos = m_LookAt - dir*newdis/olddis;
	
	m_pA3DCamera->SetPos(m_CameraPos);

	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

void CRenderWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	CDC memDc;
	memDc.CreateCompatibleDC(&dc);

	CBitmap bm;
	bm.CreateCompatibleBitmap(&dc, m_iWidth, m_iHeight);

	memDc.SelectObject(&bm);

	bm.SetBitmapBits(m_iHeight * m_iWidth * 4, m_dwBuffer);

	dc.BitBlt(0, 0, m_iWidth, m_iHeight, &memDc, 0, 0, SRCCOPY);
}

#endif
