// AFilePreviewWnd.cpp : implementation file
//

#include "stdafx.h"

#include "AFileDialog.h"
#include "AFilePreviewWnd.h"

#include "Render.h"
#include "ARenderItem.h"
#include "AFilePreview.h"
#include "SyncWrapper.h"

#include "GfxRenderItem.h"
#include "A3DGFXEx.h"
#include "PlatformIndependFuncs.h"

#ifdef _ANGELICA31
#include <A3DAdditionalView.h>

#include "ECMRenderItem.h"
#include "SMDRenderItem.h"
#include "UMDRenderItem.h"
#include "UMDSRenderItem.h"

#endif

#ifdef _ANGELICA31

typedef A3DRenderTarget::RTFMT RTFMT_t;

#elif defined _ANGELICA2 || defined(_ANGELICA22) || defined(_ANGELICA21)

typedef A3DDEVFMT RTFMT_t;

#endif

extern  CCriticalSection global_cs;


LRESULT CALLBACK PreviewWndMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!GetPreviewWnd())
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_MOVE:
		GetPreviewWnd()->OnMove((int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_SIZE:
		GetPreviewWnd()->OnSize(wParam, (int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	case WM_SIZING:
		GetPreviewWnd()->OnSizing(wParam,(LPRECT)lParam);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;


}

// AFilePreviewWnd


DWORD AFilePreviewWnd::s_dwPreviewWndSize = 320;

IMPLEMENT_DYNAMIC(AFilePreviewWnd, CWnd)

AFilePreviewWnd::AFilePreviewWnd()
: m_pA3DDevice(NULL)
//, m_pAdditionalView(NULL)
, m_pA3DCamera(NULL)
, m_pA3DViewport(NULL)
, m_pA3DGfxEngine(NULL)
, m_pFilePreviewObject(NULL)
, m_dwTickCount(0)
, m_bIsWShow(false)
, m_pA3DRenderTarget(NULL)
{

}

AFilePreviewWnd::~AFilePreviewWnd()
{
	Release();
}

bool AFilePreviewWnd::Init(A3DDevice* pA3DDevice, A3DGfxEngine* pA3DGfxEngine)
{
	if (!pA3DDevice || !GetSafeHwnd())
		return false;

#ifdef _ANGELICA31
	if ( !pA3DGfxEngine )
		return false;
#endif

	Release();

	m_pA3DDevice = pA3DDevice;
	m_pA3DGfxEngine = pA3DGfxEngine;

	m_pA3DCamera = new A3DCamera;
	if (!m_pA3DCamera->Init(pA3DDevice))
		return false;

	m_pA3DDevice->GetA3DEngine()->SetActiveCamera(m_pA3DCamera);

	CRect rcClient;
	GetClientRect(&rcClient);
	if (rcClient.Width() != PREVIEW_SIZE || rcClient.Height() != PREVIEW_SIZE)
		return false;

#ifdef _ANGELICA31
	
	m_pA3DViewport = new A3DViewport;
	if (!m_pA3DViewport->Init(pA3DDevice, 0, 0, rcClient.Width(), rcClient.Height(), 0.0f, 1.0f, true, true, A3DCOLORRGB(50, 50, 50)))
		return false;

#elif defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
	if (!m_pA3DDevice->CreateViewport(&m_pA3DViewport, 0, 0, rcClient.Width(), rcClient.Height(),
		0.0f, 1.0f, true, true, A3DCOLORRGB(50, 50, 50)))
	{
		return false;
	}
#endif
	m_pA3DViewport->SetCamera(m_pA3DCamera);
	
	/***rendertarget**/
	m_pA3DRenderTarget = CreateRenderTarget(m_pA3DDevice, rcClient.Width(), rcClient.Height(), true, false);
	if (!m_pA3DRenderTarget)
		return false;


	m_pFilePreviewObject = new AFilePreview();
	if (!m_pFilePreviewObject->Init(pA3DDevice, pA3DGfxEngine))
		return false;

	return true;
}

void AFilePreviewWnd::Release()
{
	SyncWrapper<CCriticalSection> locker(m_cs);

	A3DEngine* pA3DEngine = CRender::GetInstance()->GetA3DEngine();
	if (pA3DEngine && (pA3DEngine->GetActiveViewport() == m_pA3DViewport
			|| pA3DEngine->GetActiveCamera() == m_pA3DCamera))
	{
		pA3DEngine->SetActiveViewport(NULL);
		pA3DEngine->SetActiveCamera(NULL);
	}

	A3DRELEASE(m_pFilePreviewObject);
	A3DRELEASE(m_pA3DViewport);
	A3DRELEASE(m_pA3DCamera);
	//additionalview
	//A3DRELEASE(m_pAdditionalView);
	
	//rendertarget
	A3DRELEASE(m_pA3DRenderTarget);
	
	m_pA3DDevice = NULL;
	m_pA3DGfxEngine = NULL;
	m_strLastPreviewFile.Empty();
	m_pListeners.RemoveAll();
}

void AFilePreviewWnd::AddListener(Listener* pListener)
{
	if (!pListener)
		return;

	SyncWrapper<CCriticalSection> locker(m_cs);
	m_pListeners.AddTail(pListener);
}

void AFilePreviewWnd::RemoveListener(Listener* pListener)
{
	if (!pListener)
		return;

	SyncWrapper<CCriticalSection> locker(m_cs);
	ALISTPOSITION pos = m_pListeners.GetHeadPosition();
	while (pos)
	{
		if (m_pListeners.GetAt(pos) == pListener)
		{
			m_pListeners.RemoveAt(pos);
			break;
		}
	}
}

bool AFilePreviewWnd::SetPreviewFile(const TCHAR* szFile, bool bIsAutoUpdateWindowShow)
{
	if (!m_pA3DDevice || !m_pA3DCamera || !m_pA3DViewport /*||  !m_pAdditionalView*/)
		return false;

	SyncWrapper<CCriticalSection> locker(m_cs);

	m_pFilePreviewObject->Play(szFile);
	if (m_pFilePreviewObject->IsPlaying())
	{
		if (bIsAutoUpdateWindowShow)
		{
			ShowWindow(SW_SHOW);
			SetWShow(true);
		}
	}
	else
	{
		ShowWindow(SW_HIDE);
		SetWShow(false);
	}

	return true;
}

void AFilePreviewWnd::Tick(DWORD dwTickDelta)
{
	m_dwTickCount += dwTickDelta;

	if (!m_pA3DCamera)
		return;

	SyncWrapper<CCriticalSection> glock(global_cs);//zjy
	SyncWrapper<CCriticalSection> locker(m_cs);

	ASSERT( m_pFilePreviewObject );


	m_pFilePreviewObject->Tick(dwTickDelta);

	A3DAABB aabb = m_pFilePreviewObject->GetItemAABB();

	float fHeight = aabb.Maxs.y - aabb.Mins.y;
	a_ClampFloor(fHeight, 1.f);

	A3DVECTOR3 vExt(aabb.Maxs.x - aabb.Mins.x, 0, aabb.Maxs.z - aabb.Mins.z);
	float fRadius = vExt.Magnitude() * 2;
	a_ClampFloor(fRadius, 2.f);


	float fRad = DEG2RAD(m_dwTickCount / 100);

	A3DVECTOR3 vLookAt(0, fHeight / 2, 0);
	vLookAt += m_pFilePreviewObject->GetItemPos();
	A3DVECTOR3 vPos(cos(fRad) * fRadius, fHeight / 2.0f + 2.0f, sin(fRad) * fRadius);
	
	A3DVECTOR3 vDir (vLookAt - vPos);
	m_pA3DCamera->SetPos(vPos);
	m_pA3DCamera->SetDirAndUp(vDir, A3DVECTOR3(0.0f, 1.0f, 0.0f));
}

void AFilePreviewWnd::Render()
{
#ifdef _ANGELICA31
	if (!m_pA3DCamera || !m_pA3DViewport || !m_pA3DGfxEngine || !IsWindowVisible())
		return;
#elif defined _ANGELICA2 || defined(_ANGELICA22)|| defined(_ANGELICA21)
	if (!m_pA3DCamera || !m_pA3DViewport || !IsWindowVisible())
		return;
#endif

	SyncWrapper<CCriticalSection> glock(global_cs);//zjy

	A3DEngine* pA3DEngine = CRender::GetInstance()->GetA3DEngine();
	if (!pA3DEngine)
		return;

	if (!pA3DEngine->BeginRender())
		return;
	
	ApplyRenderTarget(m_pA3DDevice, m_pA3DRenderTarget);

	m_pA3DViewport->Active();
	m_pA3DViewport->ClearDevice();

	SyncWrapper<CCriticalSection> locker(m_cs);

	m_pFilePreviewObject->Render(m_pA3DViewport);

	RestoreRenderTarget(m_pA3DDevice, m_pA3DRenderTarget);

	ALISTPOSITION pos = m_pListeners.GetHeadPosition();
	while (pos)
	{
		Listener* pListener = m_pListeners.GetNext(pos);

#ifdef _ANGELICA2
		pListener->AfterRender(m_pA3DRenderTarget->GetTargetSurface());
#elif defined _ANGELICA31
		pListener->AfterRender(m_pA3DRenderTarget->GetTargetSurface()->m_pD3DSurface);
#endif
	}


	m_pA3DRenderTarget->PresentToBack(0,0);

	if (!pA3DEngine->EndRender())
	{
		a_LogOutput(1, "EndRender call failed.");
	}
	pA3DEngine->Present();
}

BEGIN_MESSAGE_MAP(AFilePreviewWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCPAINT()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_SIZING()
END_MESSAGE_MAP()



// AFilePreviewWnd message handlers


void AFilePreviewWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL AFilePreviewWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	CRect rcRequireClient(0, 0, PREVIEW_SIZE, PREVIEW_SIZE);
	AdjustWindowRect(&rcRequireClient, cs.style, cs.hMenu != NULL);
	cs.cx = rcRequireClient.Width();
	cs.cy = rcRequireClient.Height();

	return TRUE;
}

void AFilePreviewWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnLButtonDown(nFlags, point);
}


void AFilePreviewWnd::OnNcPaint()
{
	// TODO: Add your message handler code here
	// Do not call CWnd::OnNcPaint() for painting messages

	int a = 0;

	CRect rcWindow, rcClient;
	GetWindowRect(&rcWindow);
	GetClientRect(&rcClient);
	ClientToScreen(&rcClient);

	CRect rcTitle;
	rcTitle.left = rcWindow.left;
	rcTitle.right = rcWindow.right;
	rcTitle.top = rcWindow.top;
	rcTitle.bottom = rcClient.top;

	CRect rcLeft;
	rcLeft.left = rcWindow.left;
	rcLeft.top = rcTitle.bottom;
	rcLeft.right = rcClient.left;
	rcLeft.bottom = rcWindow.bottom;

	CRect rcRight;
	rcRight.left = rcClient.right;
	rcRight.top = rcTitle.bottom;
	rcRight.right = rcWindow.right;
	rcRight.bottom = rcWindow.bottom;

	CRect rcBottom;
	rcBottom.left = rcClient.left;
	rcBottom.top = rcClient.bottom;
	rcBottom.right = rcClient.right;
	rcBottom.bottom = rcWindow.bottom;

	CDC* pDC = GetWindowDC();
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	CBrush blackBrush(RGB(0, 0, 0));
	CBrush whiteBrush(RGB(200, 200, 200));
	CBitmap memBack, memLeft, memRight, memBottom;
	memBack.CreateCompatibleBitmap(&memDC, rcWindow.Width(), rcWindow.Height());
	memDC.SelectObject(&memBack);
	
	CBrush* pOldBrush = memDC.SelectObject(&blackBrush);
	memDC.FillRect(&rcWindow, &blackBrush);
	memDC.SelectObject(pOldBrush);

	pDC->BitBlt(0, 0, rcTitle.Width(), rcTitle.Height(), &memDC, 0, 0, SRCCOPY);
	pDC->BitBlt(0, rcTitle.Height(), rcLeft.Width(), rcLeft.Height(), &memDC, 0, rcTitle.Height(), SRCCOPY);
	pDC->BitBlt(rcLeft.Width() + rcClient.Width(), rcTitle.Height(), rcRight.Width(), rcRight.Height(), &memDC, rcLeft.Width() + rcClient.Width(), rcTitle.Height(), SRCCOPY);
	pDC->BitBlt(rcLeft.Width(), rcTitle.Height() + rcClient.Height(), rcBottom.Width(), rcBottom.Height(), &memDC, rcLeft.Width(), rcTitle.Height() + rcClient.Height(), SRCCOPY);

	Default();
}

void AFilePreviewWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	//if (m_pAdditionalView)
	//	m_pAdditionalView->OnSize();

}

void AFilePreviewWnd::OnMove(int x, int y)
{
	int i = 2;
}

void AFilePreviewWnd::OnSizing(UINT nSide, LPRECT lpRect)
{
	CWnd::OnSizing(nSide, lpRect);
	switch( nSide )
	{
	case WMSZ_LEFT:

		if (lpRect->right - lpRect->left < PREVIEW_SIZE) {
			lpRect->right = lpRect->left + PREVIEW_SIZE;
		}
		lpRect->bottom = lpRect->top + (lpRect->right - lpRect->left);

		break;

	case WMSZ_TOP:

		if (lpRect->bottom - lpRect->top < PREVIEW_SIZE) {
			lpRect->bottom = lpRect->top + PREVIEW_SIZE;
		}
		lpRect->right = lpRect->left + (lpRect->bottom - lpRect->top);

		break;

	case WMSZ_RIGHT:

		if (lpRect->right - lpRect->left < PREVIEW_SIZE) {
			lpRect->right = lpRect->left + PREVIEW_SIZE;
		}
		lpRect->bottom = lpRect->top + (lpRect->right - lpRect->left);

		break;

	case WMSZ_BOTTOM:

		if (lpRect->bottom - lpRect->top < PREVIEW_SIZE) {
			lpRect->bottom = lpRect->top + PREVIEW_SIZE;
		}
		lpRect->right = lpRect->left + (lpRect->bottom - lpRect->top);

		break;

	case WMSZ_TOPLEFT:

		if (lpRect->bottom - lpRect->top < PREVIEW_SIZE) {
			lpRect->top = lpRect->bottom - PREVIEW_SIZE;
		}

		if (lpRect->right - lpRect->left < PREVIEW_SIZE) {
			lpRect->left = lpRect->right - PREVIEW_SIZE;
		}
		lpRect->bottom = lpRect->top + (lpRect->right - lpRect->left);			

		break;

	case WMSZ_BOTTOMLEFT:

		if (lpRect->bottom - lpRect->top < PREVIEW_SIZE) {
			lpRect->bottom = lpRect->top + PREVIEW_SIZE;
		}

		if (lpRect->right - lpRect->left < PREVIEW_SIZE) {
			lpRect->left = lpRect->right - PREVIEW_SIZE;
		}
		lpRect->bottom = lpRect->top + (lpRect->right - lpRect->left);

		break;

	case WMSZ_BOTTOMRIGHT:

		if (lpRect->bottom - lpRect->top < PREVIEW_SIZE) {
			lpRect->bottom = lpRect->top + PREVIEW_SIZE;
		}

		if (lpRect->right - lpRect->left < PREVIEW_SIZE) {
			lpRect->right = lpRect->left + PREVIEW_SIZE;
		}
		lpRect->bottom = lpRect->top + (lpRect->right - lpRect->left);

		break;

	case WMSZ_TOPRIGHT:

		if (lpRect->bottom - lpRect->top < PREVIEW_SIZE) {
			lpRect->top = lpRect->bottom - PREVIEW_SIZE;
		}

		if (lpRect->right - lpRect->left < PREVIEW_SIZE) {
			lpRect->right = lpRect->left + PREVIEW_SIZE;
		}
		lpRect->bottom = lpRect->top + (lpRect->right - lpRect->left);

		break;
	}

}

//zjy#endif