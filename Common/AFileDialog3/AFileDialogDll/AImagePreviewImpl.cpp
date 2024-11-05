/*
* FILE: AImagePreviewImpl.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/08/31
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"

#ifdef _ANGELICA31
#include <d3d9.h>
#elif defined _ANGELICA2
#include <d3d8.h>
#endif

#include <process.h>
#include "Render.h"
#include "SyncWrapper.h"
#include "AImagePreviewImpl.h"
#include "AFilePreview.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

template< class T >
struct auto_release_ptr
{
	auto_release_ptr(T * pData)
		: m_pData(pData)
	{
		
	}

	T* operator -> ()
	{
		return m_pData;
	}

	T& operator * ()
	{
		return *m_pData;
	}

	T* get () const
	{
		return m_pData;
	}

	~auto_release_ptr()
	{
		A3DRELEASE(m_pData);
	}

private:
	T * m_pData;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////

extern CAFileDialogDllApp theApp;
extern HWND CreatePreviewWnd(HINSTANCE hInstance);
extern BOOL RegisterMyWndClass(HINSTANCE hInstance, const TCHAR* szClassName, WNDPROC lpWndProc);

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

LONG _OutPreviewThreadEndRequest = 0;
AFilePreview _OutFilePreviewObject;
static AImagePreviewWrapper::Listener* _pOutUserListener;

#if defined _ANGELICA31 || defined _ANGELICA22 || defined _ANGELICA21
static IDirect3DSurface9* _pOutTmpSurface;
#elif defined _ANGELICA2
static IDirect3DSurface8* _pOutTmpSurface;
#endif

static DWORD _pOutRenderBuffer[PREVIEW_SIZE * PREVIEW_SIZE];
static DWORD _dwOutTickCount;

#ifdef _ANGELICA31
static uintptr_t _OutThreadId;
#elif defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
static unsigned int _OutThreadId;
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

void _OutPreviewProc(PVOID pParam);

void _RequestAndWaitOutPreviewProcEnd()
{
	InterlockedDecrement(&_OutPreviewThreadEndRequest);
	ASSERT( _OutPreviewThreadEndRequest == 0 );
	WaitForSingleObject((HANDLE)_OutThreadId, INFINITE);
}

void _RequestOutPreviewProcStart()
{
	_OutThreadId = _beginthread(_OutPreviewProc, 0, NULL);
}

static LRESULT CALLBACK _OutPreviewWndMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static HWND _CreateOutPreviewWnd(HINSTANCE hInstance)
{
//zjy#ifdef _ANGELICA31
	if (!RegisterMyWndClass(hInstance, TEXT("OutPreviewWndClass"), _OutPreviewWndMsgProc))
		return NULL;
//zjy#endif

	return CreateWindow( 
		TEXT("OutPreviewWndClass"),        // name of window class 
		TEXT(""),						// title-bar string 
		WS_CAPTION|WS_SIZEBOX, // top-level window 
		0,       // default horizontal position 
		0,       // default vertical position 
		256,       // default width 
		256,       // default height 
		(HWND) NULL,         // no owner window 
		(HMENU) NULL,        // use class menu 
		hInstance,           // handle to application instance 
		(LPVOID) NULL);      // no window-creation data 
}

static void _UpdateCamera(DWORD dwTickDelta, A3DCamera* pA3DCamera)
{
	if (!_OutFilePreviewObject.IsPlaying())
		return;

	A3DAABB aabb = _OutFilePreviewObject.GetItemAABB();
	float fHeight = aabb.Maxs.y - aabb.Mins.y;
	a_ClampFloor(fHeight, 1.f);

	A3DVECTOR3 vExt(aabb.Maxs.x - aabb.Mins.x, 0, aabb.Maxs.z - aabb.Mins.z);
	float fRadius = vExt.Magnitude() * 2;
	a_ClampFloor(fRadius, 2.f);


	float fRad = DEG2RAD(dwTickDelta / 100);

	A3DVECTOR3 vLookAt(0, fHeight / 2, 0);
	vLookAt += _OutFilePreviewObject.GetItemPos();
	A3DVECTOR3 vPos(cos(fRad) * fRadius, fHeight / 2.0f + 2.0f, sin(fRad) * fRadius);

	A3DVECTOR3 vDir (vLookAt - vPos);
	pA3DCamera->SetPos(vPos);
	pA3DCamera->SetDirAndUp(vDir, A3DVECTOR3(0,1,0));
}

#ifdef _ANGELICA31
static void _AfterRender(IDirect3DSurface9* pBackBuffer)
{
	if (!pBackBuffer)
		return;
	
	
	D3DSURFACE_DESC desc;
	pBackBuffer->GetDesc(&desc);
	
	IDirect3DSurface9* pDestBuffer = _pOutTmpSurface;
	if (!pDestBuffer)
		return;
	
	D3DSURFACE_DESC destDesc;
	pDestBuffer->GetDesc(&destDesc);
	
	if (destDesc.Width != PREVIEW_SIZE || destDesc.Height != PREVIEW_SIZE)
		return;
	
	RECT destRc;
	destRc.left = destRc.top = 0;
	destRc.right = destDesc.Width;
	destRc.bottom = destDesc.Height;
	
	IDirect3DDevice9* pD3DDevice = CRender::GetInstance()->GetA3DDevice()->GetD3DDevice();
	if (!pD3DDevice)
		return;
	
	if (FAILED(pD3DDevice->GetRenderTargetData(pBackBuffer, pDestBuffer)))
		return;
	
	D3DLOCKED_RECT lockRc;
	if (FAILED(pDestBuffer->LockRect(&lockRc, NULL, D3DLOCK_READONLY)))
		return;
	
	DWORD* pBuffer = (DWORD*)lockRc.pBits;
	DWORD dwSrcOffset = 0;
	DWORD dwDestOffset = 0;
	for (DWORD dwRowIdx = 0; dwRowIdx < destDesc.Height; ++dwRowIdx)
	{
		memcpy(_pOutRenderBuffer + dwDestOffset, pBuffer + dwSrcOffset, destDesc.Width * 4);
		dwDestOffset += destDesc.Width;
		dwSrcOffset += lockRc.Pitch / 4;
	}
	
	pDestBuffer->UnlockRect();
	
	if (_pOutUserListener)
		_pOutUserListener->OneFrameRendered(_pOutRenderBuffer, PREVIEW_SIZE);
}

#elif defined _ANGELICA2
static void _AfterRender(IDirect3DSurface8* pBackBuffer)
{
	if (!pBackBuffer)
		return;
	
	
	D3DSURFACE_DESC desc;
	pBackBuffer->GetDesc(&desc);
	
	IDirect3DSurface8* pDestBuffer = _pOutTmpSurface;
	if (!pDestBuffer)
		return;
	
	D3DSURFACE_DESC destDesc;
	pDestBuffer->GetDesc(&destDesc);
	
	if (destDesc.Width != PREVIEW_SIZE || destDesc.Height != PREVIEW_SIZE)
		return;
	
	RECT destRc;
	destRc.left = destRc.top = 0;
	destRc.right = destDesc.Width;
	destRc.bottom = destDesc.Height;
	
	IDirect3DDevice8* pD3DDevice = CRender::GetInstance()->GetA3DDevice()->GetD3DDevice();
	if (!pD3DDevice)
		return;
	
	if (HRESULT hval = (pD3DDevice->CopyRects(pBackBuffer, NULL, 0, pDestBuffer, NULL)) )
		return;
	
	D3DLOCKED_RECT lockRc;
	if (FAILED(pDestBuffer->LockRect(&lockRc, NULL, D3DLOCK_READONLY)))
		return;
	
	DWORD* pBuffer = (DWORD*)lockRc.pBits;
	DWORD dwSrcOffset = 0;
	DWORD dwDestOffset = 0;
	for (DWORD dwRowIdx = 0; dwRowIdx < destDesc.Height; ++dwRowIdx)
	{
		memcpy(_pOutRenderBuffer + dwDestOffset, pBuffer + dwSrcOffset, destDesc.Width * 4);
		dwDestOffset += destDesc.Width;
		dwSrcOffset += lockRc.Pitch / 4;
	}
	
	pDestBuffer->UnlockRect();
	
	if (_pOutUserListener)
		_pOutUserListener->OneFrameRendered(_pOutRenderBuffer, PREVIEW_SIZE);
}
#endif


void _OutPreviewProc(PVOID pParam)
{

	A3DDevice* pA3DDevice = CRender::GetInstance()->GetA3DDevice();
	A3DEngine* pA3DEngine = pA3DDevice->GetA3DEngine();
	A3DGfxEngine* pA3DGFXEngine = CRender::GetInstance()->GetA3DGfxEngine();

	if (!_OutFilePreviewObject.Init(pA3DDevice, pA3DGFXEngine))
	{
		a_LogOutput(1, "Error %s, Failed to init _OutFilePreviewObject", "_OutPreviewProc");
		return;
	}

#ifdef _ANGELICA31
	if (FAILED(pA3DDevice->GetD3DDevice()->CreateOffscreenPlainSurface(PREVIEW_SIZE, PREVIEW_SIZE, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &_pOutTmpSurface, NULL)))
	{
		a_LogOutput(1, "Error %s, Failed to create offscreen plain surface.", __FUNCTION__);
		return;
	}
#elif defined _ANGELICA2
	if (FAILED(pA3DDevice->GetD3DDevice()->CreateImageSurface(PREVIEW_SIZE, PREVIEW_SIZE, D3DFMT_X8R8G8B8, &_pOutTmpSurface)))
	{
		a_LogOutput(1, "Error %s, Failed to create offscreen plain surface.", "_OutPreviewProc");
		return;
	}
#endif

	HWND hOutPrevWnd = _CreateOutPreviewWnd(theApp.m_hInstance);
	if (!hOutPrevWnd)
	{
		a_LogOutput(1, "Error %s, Failed to create out preview window.", "_OutPreviewProc");
		return;
	}

	CRect rcRequire = CRect(0, 0, PREVIEW_SIZE, PREVIEW_SIZE);
	AdjustWindowRect(&rcRequire, GetWindowStyle(hOutPrevWnd), FALSE);
	MoveWindow(hOutPrevWnd, 0, 0, rcRequire.Width(), rcRequire.Height(), FALSE);
	ShowWindow(hOutPrevWnd, SW_HIDE);

	auto_release_ptr<A3DCamera> pA3DCamera(new A3DCamera);
	if (!pA3DCamera->Init(pA3DDevice))
	{
		a_LogOutput(1, "Error %s, Failed to init a3dcamera", "_OutPreviewProc");
		return;
	}

	CRect rcClient;
	GetClientRect(hOutPrevWnd, &rcClient);
	if (rcClient.Width() != PREVIEW_SIZE || rcClient.Height() != PREVIEW_SIZE)
	{
		a_LogOutput(1, "Error %s, Client rect is not the preview size.", "_OutPreviewProc");
		return;
	}

	auto_release_ptr<A3DViewport> pA3DViewport(new A3DViewport);
#ifdef _ANGELICA31
	if (!pA3DViewport->Init(pA3DDevice, 0, 0, rcClient.Width(), rcClient.Height(), 0.0f, 1.0f, true, true, A3DCOLORRGB(50, 50, 50)))
	{
		a_LogOutput(1, "Error %s, Failed to init A3DViewport.", __FUNCTION__);
		return;
	}
#elif defined _ANGELICA2
	A3DViewport* pViewPort = pA3DViewport.get();
	if (!pA3DDevice->CreateViewport(&pViewPort, 0, 0, rcClient.Width(), rcClient.Height(), 0.0f, 1.0f, true, true, 0xff404040))
	{
		a_LogOutput(1, "Error %s, Failed to init A3DViewport.", "_OutPreviewProc");
		return;
	}
#endif

	pA3DViewport->SetCamera(pA3DCamera.get());

/*
	auto_release_ptr<A3DAdditionalView> pAdditionalView(new A3DAdditionalView);
	if (!pAdditionalView->Init(pA3DDevice->GetA3DEngine(), hOutPrevWnd, true))
	{
		a_LogOutput(1, "Error %s, Failed to init A3DAdditionalView.", "_OutPreviewProc");
		return;
	}

	pAdditionalView->SetActiveCamera(pA3DCamera.get());
	pAdditionalView->SetActiveViewport(pA3DViewport.get());
*/
#if defined _ANGELICA31 || defined _ANGELICA22 || defined _ANGELICA21
	A3DRenderTarget::RTFMT rtfmt;
	
	rtfmt.iWidth = rcClient.Width();
	rtfmt.iHeight = rcClient.Height();
	rtfmt.fmtTarget = A3DFMT_A8R8G8B8;
	rtfmt.fmtDepth = A3DFMT_D24X8;
	
#elif defined _ANGELICA2
	A3DDEVFMT rtfmt;
	rtfmt.nWidth = rcClient.Width();
	rtfmt.nHeight = rcClient.Height();
	rtfmt.fmtTarget = A3DFMT_A8R8G8B8;
	rtfmt.fmtDepth = A3DFMT_D24X8;
#endif
	
	auto_release_ptr<A3DRenderTarget> pA3DRenderTarget(new A3DRenderTarget);
	if (!pA3DRenderTarget->Init(pA3DDevice, rtfmt, true, false))
	{
		a_LogOutput(1, "Error %s, Failed to init A3DRenderTarget.", "_OutPreviewProc");
		return;
	}


	InterlockedIncrement(&_OutPreviewThreadEndRequest);
	DWORD dwLastTick = GetTickCount();
	MSG msg;
	while (true)
	{
		if (_OutPreviewThreadEndRequest == 0)
			break;

		if (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

			if (!_OutFilePreviewObject.IsPlaying())
			{
				Sleep(1);
				continue;
			}

			DWORD dwTickDelta = GetTickCount() - dwLastTick;
			if (dwTickDelta < 33)
			{
				Sleep(1);
				continue;
			}

			dwLastTick = GetTickCount();
			_dwOutTickCount += dwTickDelta;

			_UpdateCamera(_dwOutTickCount, pA3DCamera.get());
			_OutFilePreviewObject.Tick(dwTickDelta);

			if (!pA3DEngine->BeginRender())
				continue;

#ifdef _PROJ_IN_ANGELICA_
			if (!pA3DRenderTarget.get()->ApplyToDevice())
				return false;
#elif defined _ANGELICA2
			pA3DDevice->SetRenderTarget(pA3DRenderTarget.get());
#endif

//			pAdditionalView->ApplyToDevice();

			pA3DViewport->ClearDevice();
			_OutFilePreviewObject.Render(pA3DViewport.get());
			if (!pA3DEngine->EndRender())
				continue;

//			pAdditionalView->WithdrawFromDevice();
#ifdef _ANGELICA31
			pA3DRenderTarget.get()->WithdrawFromDevice();
			_AfterRender(pA3DRenderTarget.get()->GetTargetSurface()->m_pD3DSurface);
#elif defined _ANGELICA2
			pA3DDevice->RestoreRenderTarget();
			_AfterRender(pA3DRenderTarget.get()->GetTargetSurface());
#endif
			//pAdditionalView->Present();
			pA3DRenderTarget.get()->PresentToBack(0,0);
		}
	}


	_OutFilePreviewObject.Release();

}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AImagePreviewImpl
//	
///////////////////////////////////////////////////////////////////////////

AImagePreviewImpl::AImagePreviewImpl(void)
{
}

AImagePreviewImpl::~AImagePreviewImpl(void)
{
}

bool AImagePreviewImpl::SetPreviewFile(const TCHAR* szFile)
{
	return _OutFilePreviewObject.Play(szFile);
}

void AImagePreviewImpl::SetListener(AImagePreviewWrapper::Listener* pUserListener)
{
	SyncWrapper<CCriticalSection> __locker(m_cs);
	_pOutUserListener = pUserListener;
}
