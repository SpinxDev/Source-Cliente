#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DCamera.h"
#include "A3DCursor.h"
#include "A3DRenderTarget.h"
#include "A3DViewport.h"
#include "A3DLightMan.h"
#include "A3DConfig.h"
#include "A3DDevObject.h"
#include "A3DVertexShader.h"
#include "A3DPixelShader.h"
#include "A3DAdditionalView.h"
#include "AList.h"
#include "ACSWrapper.h"

bool GetProperDisplayFreq(DWORD dwWidth, DWORD dwHeight, DWORD dwBitsPerPixel, DWORD * pdwFrequency)
{
	int indexMode = 0;
	DWORD dwHighestCertFreq = 0;

	DEVMODE WorkingDM;

	while( EnumDisplaySettings(NULL, indexMode, &WorkingDM) )
	{
		if( dwWidth == WorkingDM.dmPelsWidth 
			&& dwHeight == WorkingDM.dmPelsHeight 
			&& dwBitsPerPixel == WorkingDM.dmBitsPerPel )
		{
			if( dwHighestCertFreq < WorkingDM.dmDisplayFrequency )
			{
				if( DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&WorkingDM, CDS_TEST) )
				{
					dwHighestCertFreq = WorkingDM.dmDisplayFrequency;
					if( dwHighestCertFreq >= 85 )
						break;
				}
			}
		}
		
		indexMode ++;
	}

	*pdwFrequency = dwHighestCertFreq;
	return true;
}

A3DDevice::A3DDevice() : m_UnmanagedDevObjectTab(1024)
{
	m_dwDeviceFlags		= 0;
	m_bHWIDevice		= false;
	m_A3DDevType		= A3DDEVTYPE_HAL;

	m_bAutoReset		= true;
	m_bNeedReset		= false;

	m_hDeviceWnd		= NULL;
	m_pD3DDevice		= NULL;
	m_pBackBuffer		= NULL;
	m_pRenderTarget		= NULL;
	m_pDepthBuffer		= NULL;
	m_pRestoreToRenderTarget = m_pRenderTarget;
	m_pRestoreToDepthBuffer = m_pDepthBuffer;
	m_pA3DEngine		= NULL;

	m_dwVertexShader	= 0;
	m_dwPixelShader		= 0;

	m_dwGammaLevel		= 100;

	m_A3DCullType		= A3DCULL_CCW;
	m_bHardwareTL		= false;

	m_bShowCursor		= false;
	m_pA3DCursor		= NULL;

	m_bFogEnable		= false;
	m_bFogTableEnable	= false;
	m_bFogVertEnable	= false;
	m_fogMode			= A3DFOG_LINEAR;
	m_iSkinModelRender	= SMRD_VERTEXSHADER;

	InitializeCriticalSection(&m_csDevObj);
	
	m_bEnableSetAlphaBlend = true;
}

A3DDevice::~A3DDevice()
{
	DeleteCriticalSection(&m_csDevObj);
}

bool A3DDevice::InitAsHWI(A3DEngine* pA3DEngine, HWND hDeviceWnd, A3DDEVFMT* pA3DDevFmt, DWORD dwDevFlags, A3DDEVTYPE devType)
{
	m_dwDeviceFlags	= dwDevFlags;
	m_bHWIDevice	= true;

	// Retrieve current desktop display mode and keep it;
	HDC	hdc = GetDC(NULL);
	m_DesktopMode.nBitsPerPixel		= GetDeviceCaps(hdc, BITSPIXEL);
	m_DesktopMode.nFrequency		= GetDeviceCaps(hdc, VREFRESH);
	m_DesktopMode.nWidth			= GetSystemMetrics(SM_CXSCREEN);
	m_DesktopMode.nHeight			= GetSystemMetrics(SM_CYSCREEN);
	ReleaseDC(NULL, hdc);

	m_pA3DEngine = pA3DEngine;
	m_hDeviceWnd = hDeviceWnd;
	m_A3DDevType = devType;
	
	//Keep them;
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth			= pA3DDevFmt->nWidth;
	d3dpp.BackBufferHeight			= pA3DDevFmt->nHeight;
	d3dpp.Windowed					= pA3DDevFmt->bWindowed;
	d3dpp.hDeviceWindow				= hDeviceWnd;
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
	d3dpp.BackBufferFormat			= D3DFMT_X8R8G8B8;
	m_d3dpp = d3dpp;

	pA3DDevFmt->fmtTarget			= A3DFMT_X8R8G8B8;
	pA3DDevFmt->fmtDepth			= A3DFMT_D16;
	m_A3DDevFmt	= *pA3DDevFmt;

	// Get Normal Texture, ColorKey Texture and AlphaTexture format;
	m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;

	switch(m_A3DDevFmt.fmtTarget)
	{
	case A3DFMT_X1R5G5B5:
	case A3DFMT_R5G6B5:
		m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
		m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
		break;
	case A3DFMT_X8R8G8B8:
	case A3DFMT_A8R8G8B8:
		m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
		m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
		break;
	}

	ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));	
	ConfigDevice();

	return true;
}

bool A3DDevice::Init(A3DEngine* pA3DEngine, HWND hDeviceWnd, A3DDEVFMT* pA3DDevFmt, DWORD dwDevFlags, A3DDEVTYPE devType)
{
	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
		return InitAsHWI(pA3DEngine, hDeviceWnd, pA3DDevFmt, dwDevFlags, devType);

	D3DDISPLAYMODE d3ddm;
	HRESULT	hval;

	//	Init some members;
	m_pA3DEngine	= pA3DEngine;
	m_hDeviceWnd	= hDeviceWnd;
	m_A3DDevType	= devType;
	m_dwDeviceFlags	= dwDevFlags;
	
	// First let's find what is 16-bit target format and what is 32-bit target format;
	m_fmtTarget16 = A3DFMT_UNKNOWN;
	m_fmtTarget32 = A3DFMT_UNKNOWN;
	hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_R5G6B5, D3DFMT_R5G6B5, false);
	if( D3D_OK != hval )
	{
		hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_X1R5G5B5, D3DFMT_X1R5G5B5, false);
		if( D3D_OK != hval )
		{
			g_A3DErrLog.Log("Can not determine the 16-bit target format!");
			return false;
		}
		else
		{
			m_fmtAdapter16 = A3DFMT_X1R5G5B5;
			m_fmtTarget16 = A3DFMT_X1R5G5B5;
		}
	}
	else
	{
		m_fmtAdapter16 = A3DFMT_R5G6B5;
		m_fmtTarget16 = A3DFMT_R5G6B5;
	}

	hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false);
	if( D3D_OK == hval )
	{
		m_fmtAdapter32 = A3DFMT_X8R8G8B8;
		if( dwDevFlags & A3DDEV_ALPHATARGETFIRST )
		{
			hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, false);
			if( D3D_OK == hval )
				m_fmtTarget32 = A3DFMT_A8R8G8B8;
		}
		else
			m_fmtTarget32 = A3DFMT_X8R8G8B8;
	}

	// Retrieve current desktop display mode and keep it;
	HDC	hdc = GetDC(NULL);
	m_DesktopMode.nBitsPerPixel		= GetDeviceCaps(hdc, BITSPIXEL);
	m_DesktopMode.nFrequency		= GetDeviceCaps(hdc, VREFRESH);
	m_DesktopMode.nWidth			= GetSystemMetrics(SM_CXSCREEN);
	m_DesktopMode.nHeight			= GetSystemMetrics(SM_CYSCREEN);
	ReleaseDC(NULL, hdc);

	hval = m_pA3DEngine->GetD3D()->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("Can not get adapter display mode");
		return false;
	}

	A3DFORMAT	fmtAdapter;
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth	= pA3DDevFmt->nWidth;
	d3dpp.BackBufferHeight	= pA3DDevFmt->nHeight;
	d3dpp.Windowed			= pA3DDevFmt->bWindowed;
	d3dpp.hDeviceWindow		= hDeviceWnd;

	if( pA3DDevFmt->bVSync )
	{
		if (pA3DDevFmt->bWindowed)
			d3dpp.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
		else 
		{
			// full screen
			d3dpp.SwapEffect = D3DSWAPEFFECT_COPY; // we must use copy instead of flip or discard if we want to use alpha render target for fullscreen
			d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		}
	}
	else
	{
		if (pA3DDevFmt->bWindowed)
			d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
		else 
		{
			// full screen
			d3dpp.SwapEffect = D3DSWAPEFFECT_COPY; // we must use copy instead of flip or discard if we want to use alpha render target for fullscreen
			d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}
	}

	d3dpp.BackBufferCount	= 1;
	
	if( pA3DDevFmt->bWindowed )
	{
		// see whether 16bit or 32bit color depth we are
		if( (D3DFORMAT)m_fmtAdapter32 == d3ddm.Format )
		{
			fmtAdapter = m_fmtAdapter32;
			d3dpp.BackBufferFormat = (D3DFORMAT)m_fmtTarget32;
		}
		else
		{
			fmtAdapter = m_fmtAdapter16;
			d3dpp.BackBufferFormat = (D3DFORMAT)m_fmtTarget16;
		}
	}
	else
	{
		if( pA3DDevFmt->fmtTarget == 0 )
		{
			// Use 32-bit first
			if( m_fmtTarget32 != A3DFMT_UNKNOWN )
			{
				fmtAdapter = m_fmtAdapter32;
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget32;
			}
			else
			{
				fmtAdapter = m_fmtAdapter16;
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget16;
			}
		}
		else 
		{
			if( pA3DDevFmt->fmtTarget == A3DFMT_X1R5G5B5 || pA3DDevFmt->fmtTarget == A3DFMT_R5G6B5 )
			{
				fmtAdapter = m_fmtAdapter16;
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget16;
			}
			else
			{
				fmtAdapter = m_fmtAdapter32;
				d3dpp.BackBufferFormat = (D3DFORMAT) m_fmtTarget32;
			}
		}
	}
	d3dpp.EnableAutoDepthStencil = TRUE;

TryAgain:
	if( d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	else
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

	hval = m_pA3DEngine->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, (D3DFORMAT)fmtAdapter, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat);
	if FAILED(hval)
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	else
	{
		hval = m_pA3DEngine->GetD3D()->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, (D3DFORMAT)fmtAdapter, 
			d3dpp.BackBufferFormat, d3dpp.AutoDepthStencilFormat);
		if FAILED(hval)
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	
	}

	g_A3DErrLog.Log("Chosen Device Format: BackBuffer [%s], DepthStencil [%s]", GetFormatString((A3DFORMAT)d3dpp.BackBufferFormat), GetFormatString((A3DFORMAT)d3dpp.AutoDepthStencilFormat));
	
	ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));	
	hval = m_pA3DEngine->GetD3D()->GetDeviceCaps(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, &m_d3dcaps);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::Init(), GetDeviceCaps() Fail.");
		return false;
	}

	DWORD dwFlags = 0;
	if (m_dwDeviceFlags & A3DDEV_ALLOWMULTITHREAD)
		dwFlags |= D3DCREATE_MULTITHREADED;
	if (m_dwDeviceFlags & A3DDEV_FPU_PRESERVE)
		dwFlags	|= D3DCREATE_FPU_PRESERVE;

	if (!(m_dwDeviceFlags & A3DDEV_FORCESOFTWARETL) && 
		(m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
	{
		if( 0 )//m_d3dcaps.VertexShaderVersion < D3DVS_VERSION(1, 1) )
		{
			g_A3DErrLog.Log("A3DDevice::Init(), Chosen Mixed Device...");
			hval = m_pA3DEngine->GetD3D()->CreateDevice(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, m_hDeviceWnd,
				dwFlags | D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice);
			m_bHardwareTL = true;
		}
		else
		{
			g_A3DErrLog.Log("A3DDevice::Init(), Chosen T&L Device...");
			hval = m_pA3DEngine->GetD3D()->CreateDevice(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, m_hDeviceWnd,
				dwFlags | D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice);
			m_bHardwareTL = true;
		}
	}
	else
	{
		g_A3DErrLog.Log("A3DDevice::Init(), Chosen SOFTWARE Vertex Processing...");
		hval = m_pA3DEngine->GetD3D()->CreateDevice(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, m_hDeviceWnd,
			dwFlags | D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice);
		m_bHardwareTL = false;
	}

	if( D3D_OK != hval )
	{
		char szErrorCode[1024];
		D3DXGetErrorStringA(hval, szErrorCode, 1024);
		g_A3DErrLog.Log("Can not create device[%dx%dx%d], reason: %s", pA3DDevFmt->nWidth, pA3DDevFmt->nHeight, (pA3DDevFmt->fmtTarget == A3DFMT_X8R8G8B8 || pA3DDevFmt->fmtTarget == A3DFMT_A8R8G8B8) ? 32 : 16, szErrorCode);

		if( hval == D3DERR_OUTOFVIDEOMEMORY )
		{
			if( d3dpp.BackBufferFormat == D3DFMT_X8R8G8B8 || d3dpp.BackBufferFormat == D3DFMT_A8R8G8B8 )
			{
				fmtAdapter = m_fmtAdapter16;

				hval = m_pA3DEngine->GetD3D()->CheckDeviceType(D3DADAPTER_DEFAULT, 
					(D3DDEVTYPE) m_A3DDevType, D3DFMT_R5G6B5, D3DFMT_R5G6B5, false);
				if FAILED(hval) 
					d3dpp.BackBufferFormat = D3DFMT_X1R5G5B5;
				else
					d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
				g_A3DErrLog.Log("Now switch to 16-bit color depth, and try again...");
				goto TryAgain;
			}
		}
		
		return false;
	}

	// Keep them;
	m_d3dpp = d3dpp;

	pA3DDevFmt->fmtAdapter = fmtAdapter;
	pA3DDevFmt->fmtTarget = (A3DFORMAT) d3dpp.BackBufferFormat;
	pA3DDevFmt->fmtDepth = (A3DFORMAT) d3dpp.AutoDepthStencilFormat;
	m_A3DDevFmt	= *pA3DDevFmt;

	// Log chosen format;
	g_A3DErrLog.Log("A3DDevice::Init(), Create device %dx%dx%d", pA3DDevFmt->nWidth, pA3DDevFmt->nHeight, (pA3DDevFmt->fmtTarget == A3DFMT_X8R8G8B8 || pA3DDevFmt->fmtTarget == A3DFMT_A8R8G8B8) ? 32 : 16);

	//Get some surface pointers;
	hval = m_pD3DDevice->GetRenderTarget(&m_pRenderTarget);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("Can not get rendertarget pointer!");
		return false;
	}
	hval = m_pD3DDevice->GetDepthStencilSurface(&m_pDepthBuffer);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("Can not get depth stencil surface pointer!");
		return false;
	}
	hval = m_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("Can not get backbuffer pointer!");
		return false;
	}

	m_pRestoreToRenderTarget = m_pRenderTarget;
	m_pRestoreToDepthBuffer = m_pDepthBuffer;

	// Get Normal Texture, ColorKey Texture and AlphaTexture format;
	m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;

	switch(m_A3DDevFmt.fmtTarget)
	{
	case A3DFMT_X1R5G5B5:
	case A3DFMT_R5G6B5:
		m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
		m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
		break;
	case A3DFMT_X8R8G8B8:
	case A3DFMT_A8R8G8B8:
		m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
		m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
		break;
	}

	// Enumerate all display modes.
	EnumDisplayModes(m_pA3DEngine->GetD3D(), m_A3DDevFmt.fmtAdapter, m_DisplayModes);
	ConfigDevice();

	// For the first time, we set the fog data;
	SetFogEnable(true);
	SetFogTableEnable(true);
	SetFogColor(A3DCOLORRGBA(255, 255, 255, 255));
	SetFogStart(500.0f);
	SetFogEnd(800.0f);
	SetFogDensity(1.0f);

	return true;
}

bool A3DDevice::Release()
{
	EnterCriticalSection(&m_csDevObj);
	m_UnmanagedDevObjectTab.clear();
	LeaveCriticalSection(&m_csDevObj);

	if( m_pRenderTarget )
	{
		m_pRenderTarget->Release();
		m_pRenderTarget = NULL;
	}
	if( m_pDepthBuffer )
	{
		m_pDepthBuffer->Release();
		m_pDepthBuffer = NULL;
	}
	if( m_pBackBuffer )
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}
	if( m_pD3DDevice )
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}
	return true;
}

bool A3DDevice::ConfigDevice()
{
	if( m_bHWIDevice )
		return true;

	HRESULT hval;

	ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));
	hval = m_pD3DDevice->GetDeviceCaps(&m_d3dcaps);
	if FAILED(hval) return false;

	m_RenderStates.Reset();
	m_TextureStageStates.Reset();
	m_TextureCoordIndex.Reset();

	for(int i=0; i<MAX_TEX_LAYERS; i++)
	{
		m_matTextureMatrix[i] = IdentityMatrix();
		SetTextureCoordIndex(i, i);
	}

	m_dwVertexShader	= 0;
	m_dwPixelShader		= 0;

	SetFaceCull(A3DCULL_CCW);

	//We use depth buffer;
	SetZTestEnable(true);
	SetZWriteEnable(true);

	//Initialize the device variables;
	SetAlphaBlendEnable(true);
	SetSourceAlpha(A3DBLEND_SRCALPHA);
	SetDestAlpha(A3DBLEND_INVSRCALPHA);

	SetDitherEnable(true);
	SetSpecularEnable(false); // If you want to make specular enable, you must set it explictly
	
	SetTextureFilterType(0, A3DTEXF_LINEAR);
	SetTextureColorOP(0, A3DTOP_MODULATE);
	SetTextureAlphaOP(0, A3DTOP_MODULATE);

	SetDeviceRenderState(D3DRS_FOGENABLE, TRUE);

	g_A3DErrLog.Log("Max vertex blend matrix index: %d", GetMaxVertBlendMatrixIndex());
	g_A3DErrLog.Log("VS version: %d.%d", D3DSHADER_VERSION_MAJOR(m_d3dcaps.VertexShaderVersion), D3DSHADER_VERSION_MINOR(m_d3dcaps.VertexShaderVersion));
	g_A3DErrLog.Log("VS constant registers number: %d", GetVSConstantNum());
	g_A3DErrLog.Log("PS version: %d.%d", D3DSHADER_VERSION_MAJOR(m_d3dcaps.PixelShaderVersion), D3DSHADER_VERSION_MINOR(m_d3dcaps.PixelShaderVersion));

	//	Determine render method of skin model
	DetermineSkinModelRenderMethod();

	return true;
}

void A3DDevice::DetermineSkinModelRenderMethod()
{
	//	For software TL, we consider indexed blend matrix first. For hardware TL
	//	we consider vertex shader first.
	if (!IsHardwareTL() && GetMaxVertBlendMatrixIndex() > 200)
	{
		m_iSkinModelRender = SMRD_INDEXEDVERTMATRIX;
	//	m_iSkinModelRender = SMRD_SOFTWARE;
	}
	else
	{
		if (TestVertexShaderVersion(1, 1))
		{
			m_iSkinModelRender = SMRD_VERTEXSHADER;
		}
		else if (GetMaxVertBlendMatrixIndex() > 200)
		{
			//	200 blending matrices means skin model can have 100 bone at most.
			//	Is there any display card support so many blend matrices now ?
			//	I don't down, but ATI 9700 only supports 38 blending matrices.
			m_iSkinModelRender = SMRD_INDEXEDVERTMATRIX;
		}
		else
		{
			m_iSkinModelRender = SMRD_SOFTWARE;
		}
	}

//	m_iSkinModelRender = SMRD_SOFTWARE;
	
	switch (m_iSkinModelRender)
	{
	case SMRD_VERTEXSHADER:

		g_A3DErrLog.Log("Skin model render method: Vertex shader.");
		break;

	case SMRD_INDEXEDVERTMATRIX:

		g_A3DErrLog.Log("Skin model render method: Indexed Vertex Blend Matrix.");
		break;

	case SMRD_SOFTWARE:

		g_A3DErrLog.Log("Skin model render method: Software.");
		break;
	}
}

//	Handle device lost
bool A3DDevice::HandleDeviceLost()
{
	HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
	if (hr == D3DERR_DEVICENOTRESET)
	{
		if (!m_bAutoReset)
			m_bNeedReset = true;
		else if (!Reset())
			return false;
	}	

	return true;
}

bool A3DDevice::SetWorldMatrix(const A3DMATRIX4& matWorld)
{
	if( m_bHWIDevice )	return true;

	if FAILED(m_pD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matWorld))
	{
		g_A3DErrLog.Log("D3DDevice SetTransform D3DTS_WORLD Failure"); 
		return false;
	}

	m_matWorldMatrix = matWorld;
	return true;
}

bool A3DDevice::SetViewMatrix(const A3DMATRIX4& matView)
{
	if( m_bHWIDevice )	return true;

	if FAILED(m_pD3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&matView))
	{
		g_A3DErrLog.Log("D3DDevice SetTransform D3DTS_VIEW Failure"); 
		return false;
	}

	m_matViewMatrix = matView;
	return true;
}

bool A3DDevice::SetProjectionMatrix(const A3DMATRIX4& matProject)
{
	if( m_bHWIDevice )	return true;

	if FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&matProject))
	{
		g_A3DErrLog.Log("D3DDevice SetTransform D3DTS_PROJECT Failure"); 
		return false;
	}

	m_matProjectionMatrix = matProject;
	return true;
}

bool A3DDevice::SetTextureMatrix(int iStage, const A3DMATRIX4& matTexture)
{
	if( m_bHWIDevice )	return true;

	if FAILED(m_pD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(iStage + D3DTS_TEXTURE0), (D3DMATRIX *)&matTexture))
	{
		g_A3DErrLog.Log("D3DDevice SetTextureMatrix Failure"); 
		return false;
	}

	if( iStage >=0 && iStage < MAX_TEX_LAYERS )
		m_matTextureMatrix[iStage] = matTexture;
	return true;
}

bool A3DDevice::BeginRender()
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	hval = m_pD3DDevice->BeginScene();
	if FAILED(hval)
	{
		g_A3DErrLog.Log("A3DDevcie BeginScene Failure!");
		return false;
	}

	m_dwDrawVertCount = 0;
	m_dwDrawFaceCount = 0;
	m_dwDrawCount = 0;
	
	return true;
}

bool A3DDevice::EndRender()
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;
	
	// We should show cursor here;
	if( m_pA3DCursor && m_bShowCursor )
	{
		m_pA3DCursor->TickAnimation();
		m_pA3DCursor->Draw();
	}

	hval = m_pD3DDevice->EndScene();
	if FAILED(hval)
	{
		g_A3DErrLog.Log("A3DDevcie EndScene Failure!");
		return false;
	}

	return true;
}

bool A3DDevice::Present(HWND hWnd/* NULL */)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	hval = m_pD3DDevice->Present(NULL, NULL, hWnd, NULL);
	if FAILED(hval)
	{
		if( hval == D3DERR_DEVICELOST )
			return HandleDeviceLost();
		else
		{
			g_A3DErrLog.Log("A3DDevice: Present Failure!");
			return false;
		}
	}

	return true;
}

bool A3DDevice::Clear(DWORD dwFlags, A3DCOLOR color, FLOAT vZ, DWORD dwStencil)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	hval = m_pD3DDevice->Clear(0, NULL, dwFlags, color, vZ, dwStencil);
	if FAILED(hval)
	{
		g_A3DErrLog.Log("A3DDevice: Clear() Failure!");
		return false;
	}

	return true;
}

bool A3DDevice::SetTextureFilterType(int nTextureStage, A3DTEXTUREFILTERTYPE filterType)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_MAGFILTER, (D3DTEXTUREFILTERTYPE) filterType);
	SetDeviceTextureStageState(nTextureStage, D3DTSS_MINFILTER, (D3DTEXTUREFILTERTYPE) filterType);

	if( g_pA3DConfig->GetFlagTrilinear() )
		SetDeviceTextureStageState(nTextureStage, D3DTSS_MIPFILTER, (D3DTEXTUREFILTERTYPE) filterType);
	else
		SetDeviceTextureStageState(nTextureStage, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	
	return true;
}

bool A3DDevice::SetTextureColorOP(int nTextureStage, A3DTEXTUREOP op)
{
	if( m_bHWIDevice )	return true;
	SetDeviceTextureStageState(nTextureStage, D3DTSS_COLOROP, (D3DTEXTUREOP) op);
	return true;
}

bool A3DDevice::SetTextureColorArgs(int nTextureStage, DWORD dwArg1, DWORD dwArg2)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_COLORARG1, dwArg1);
	SetDeviceTextureStageState(nTextureStage, D3DTSS_COLORARG2, dwArg2);
	return true;
}

bool A3DDevice::SetTextureAlphaOP(int nTextureStage, A3DTEXTUREOP op)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_ALPHAOP, (D3DTEXTUREOP) op);
	return true;
}

bool A3DDevice::SetTextureAlphaArgs(int nTextureStage, DWORD dwArg1, DWORD dwArg2)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_ALPHAARG1, dwArg1);
	SetDeviceTextureStageState(nTextureStage, D3DTSS_ALPHAARG2, dwArg2);
	return true;
}

bool A3DDevice::SetTextureResultArg(int nTextureStage, DWORD dwArg)
{
	if( m_bHWIDevice )	return true;
	SetDeviceTextureStageState(nTextureStage, D3DTSS_RESULTARG, dwArg);
	return true;
}

A3DTEXTUREOP A3DDevice::GetTextureColorOP(int iTextureStage)
{
	if (m_bHWIDevice)
		return A3DTOP_DISABLE;

	DWORD op = GetDeviceTextureStageState(iTextureStage, D3DTSS_COLOROP);
	return (A3DTEXTUREOP)op;
}

A3DTEXTUREOP A3DDevice::GetTextureAlphaOP(int iTextureStage)
{
	if (m_bHWIDevice)
		return A3DTOP_DISABLE;

	DWORD op = GetDeviceTextureStageState(iTextureStage, D3DTSS_ALPHAOP);
	return (A3DTEXTUREOP)op;
}

bool A3DDevice::SetTextureAddress(int nTextureStage, A3DTEXTUREADDR UAddrMethod, A3DTEXTUREADDR VAddrMethod)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_ADDRESSU, (D3DTEXTUREADDRESS) UAddrMethod);
	SetDeviceTextureStageState(nTextureStage, D3DTSS_ADDRESSV, (D3DTEXTUREADDRESS) VAddrMethod);
	return true;
}

bool A3DDevice::SetTextureCoordIndex(int nTextureStage, int iIndex)
{
	if( m_bHWIDevice )	return true;

	if( nTextureStage >= 0 && nTextureStage < MAX_TEX_LAYERS )
	{
		if( m_TextureCoordIndex.index[nTextureStage] == iIndex )
			return true;

		m_TextureCoordIndex.index[nTextureStage] = iIndex;
	}

	m_pD3DDevice->SetTextureStageState(nTextureStage, D3DTSS_TEXCOORDINDEX, iIndex);
	return true;
}

bool A3DDevice::SetTextureBumpMat(int nTextureStage, float _00, float _01, float _10, float _11)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT00, *((DWORD*)(&_00)));
	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT01, *((DWORD*)(&_01)));
	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT10, *((DWORD*)(&_10)));
	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVMAT11, *((DWORD*)(&_11)));
	return true;
}

bool A3DDevice::SetTextureBumpScale(int nTextureStage, float fScale)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVLSCALE, *((DWORD*)(&fScale)));
	return true;
}

bool A3DDevice::SetTextureBumpOffset(int nTextureStage, float fOffset)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_BUMPENVLOFFSET, *((DWORD*)(&fOffset)));
	return true;
}

bool A3DDevice::SetTextureTransformFlags(int nTextureStage, A3DTEXTURETRANSFLAGS Flags)
{
	if( m_bHWIDevice )	return true;

	SetDeviceTextureStageState(nTextureStage, D3DTSS_TEXTURETRANSFORMFLAGS, Flags);
	return true;
}

bool A3DDevice::SetZTestEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ZENABLE, bEnable ? TRUE : FALSE);
	return true;								
}

bool A3DDevice::SetZWriteEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ZWRITEENABLE, bEnable ? TRUE : FALSE);
	return true;								
}

bool A3DDevice::SetZBias(DWORD dwBias)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ZBIAS, dwBias);
	return true;
}

bool A3DDevice::SetZFunc(DWORD dwFunc)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ZFUNC, dwFunc);
	return true;
}

bool A3DDevice::SetAlphaBlendEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	if( !m_bEnableSetAlphaBlend) 
		return true;
	SetDeviceRenderState(D3DRS_ALPHABLENDENABLE, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::SetSourceAlpha(A3DBLEND alphaBlendParam)
{
	if( m_bHWIDevice )	return true;

	if( !m_bEnableSetAlphaBlend) 
		return true;
	SetDeviceRenderState(D3DRS_SRCBLEND, alphaBlendParam);
	return true;
}

bool A3DDevice::SetDestAlpha(A3DBLEND alphaBlendParam)
{
	if( m_bHWIDevice )	return true;
	
	if( !m_bEnableSetAlphaBlend) 
		return true;
	SetDeviceRenderState(D3DRS_DESTBLEND, alphaBlendParam);
	return true;
}

bool A3DDevice::SetAlphaTestEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ALPHATESTENABLE, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::GetAlphaTestEnable()
{
	if( m_bHWIDevice )	return false;

	DWORD dwValue = GetDeviceRenderState(D3DRS_ALPHATESTENABLE);
	return dwValue ? true : false;
}

bool A3DDevice::SetAlphaFunction(A3DCMPFUNC cmpFunc)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ALPHAFUNC, cmpFunc);
	return true;
}

A3DCMPFUNC A3DDevice::GetAlphaFunction()
{
	if( m_bHWIDevice )	return A3DCMP_LESS;

	DWORD dwValue = GetDeviceRenderState(D3DRS_ALPHAFUNC);
	return (A3DCMPFUNC)dwValue;
}

bool A3DDevice::SetAlphaRef(DWORD dwRefValue)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_ALPHAREF, dwRefValue);
	return true;
}

DWORD A3DDevice::GetAlphaRef()
{
	if( m_bHWIDevice )	return 0;

	DWORD dwValue = GetDeviceRenderState(D3DRS_ALPHAREF);
	return dwValue;
}

bool A3DDevice::SetLightingEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	SetDeviceRenderState(D3DRS_LIGHTING, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::SetLightParam(int nLightID, A3DLIGHTPARAM * pLightParam)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->SetLight(nLightID, (D3DLIGHT8 *)pLightParam);
	return true;
}

bool A3DDevice::LightEnable(int nLightID, bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_pD3DDevice->LightEnable(nLightID, bEnable ? TRUE : FALSE);
	return true;
}

bool A3DDevice::SetAmbient(A3DCOLOR colorAmbient)
{
	if( m_bHWIDevice )	return true;

	m_colorAmbient = colorAmbient;
	m_colorValueAmbient = a3d_ColorRGBAToColorValue(colorAmbient);

	SetDeviceRenderState(D3DRS_AMBIENT, colorAmbient);
	return true;
}

bool A3DDevice::SetMaterial(const A3DMATERIALPARAM* pMaterialParam, bool b2Sided)
{
	if (m_bHWIDevice)
		return true;

	m_pD3DDevice->SetMaterial((D3DMATERIAL8*)pMaterialParam);
	if (b2Sided)
	{
		SetDeviceRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
	else
	{
		//	Set To Original Configuration;
		SetFaceCull(m_A3DCullType);
	}
	return true;
}

bool A3DDevice::SetViewport(A3DVIEWPORTPARAM * pViewportParam)
{
	if( m_bHWIDevice )	return true;

	D3DVIEWPORT8 viewport;

	memcpy(&viewport, pViewportParam, sizeof(A3DVIEWPORTPARAM));
	m_pD3DDevice->SetViewport(&viewport);

	return true;
}

bool A3DDevice::SetFaceCull(A3DCULLTYPE type)
{
	if (m_bHWIDevice)
		return true;

	m_A3DCullType = type;

	switch (type)
	{
	case A3DCULL_CW:	SetDeviceRenderState(D3DRS_CULLMODE, D3DCULL_CW);	break;
	case A3DCULL_CCW:	SetDeviceRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	break;
	case A3DCULL_NONE:	SetDeviceRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	break;
	default:
		ASSERT(0);
		return false;
	}

	return true;
}

bool A3DDevice::SetFillMode(A3DFILLMODE mode)
{
	if( m_bHWIDevice )	return true;

	switch(mode)
	{
	case A3DFILL_POINT:
		m_A3DFillMode = mode;
		SetDeviceRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
		break;
	case A3DFILL_WIREFRAME:
		m_A3DFillMode = mode;
		SetDeviceRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		break;
	case A3DFILL_SOLID:
		m_A3DFillMode = mode;
		SetDeviceRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		break;
	default:
		return false;
	}

	return true;
}

bool A3DDevice::DrawIndexedPrimitive(A3DPRIMITIVETYPE Type, DWORD MinIndex, DWORD NumVertices, DWORD StartIndex, DWORD PrimitiveCount)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;
	
	if( PrimitiveCount == 0 )
		return true; 

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);

	if( m_dwVertexShader & 0x1 )
	{
		// we use a vertex shader now, so we have to make all texture coord index to its layer index
		for(int i=0; i<MAX_TEX_LAYERS; i++)
		{
			if( m_TextureCoordIndex.index[i] != i )
				SetTextureCoordIndex(i, i);
			SetTextureTransformFlags(i, A3DTTFF_DISABLE);
		}
	}

	hval = m_pD3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)Type, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::DrawIndexedPrimitive() Fail");
		return false;
	}

	m_dwDrawVertCount += NumVertices;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);
	return true;
}

bool A3DDevice::DrawPrimitive(A3DPRIMITIVETYPE Type, DWORD StartVertex, DWORD PrimitiveCount)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	if( PrimitiveCount == 0 )
		return true; 

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);

	if( m_dwVertexShader & 0x1 )
	{
		// we use a vertex shader now, so we have to make all texture coord index to its layer index
		for(int i=0; i<MAX_TEX_LAYERS; i++)
		{
			if( m_TextureCoordIndex.index[i] != i )
				SetTextureCoordIndex(i, i);
			SetTextureTransformFlags(i, A3DTTFF_DISABLE);
		}
	}

	hval = m_pD3DDevice->DrawPrimitive((D3DPRIMITIVETYPE)Type, StartVertex, PrimitiveCount);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::DrawPrimitive() Fail");
		return false;
	}

	m_dwDrawVertCount += PrimitiveCount * 3;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);
	return true;
}

bool A3DDevice::DrawIndexedPrimitiveUP(A3DPRIMITIVETYPE Type, DWORD MinIndex, DWORD NumVertices, DWORD PrimitiveCount, void* pIndexData, A3DFORMAT IndexDataFormat, void* pVertexStreamZeroData, DWORD VertexStreamZeroStride)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	if( PrimitiveCount == 0 )
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);

	if( m_dwVertexShader & 0x1 )
	{
		// we use a vertex shader now, so we have to make all texture coord index to its layer index
		for(int i=0; i<MAX_TEX_LAYERS; i++)
		{
			if( m_TextureCoordIndex.index[i] != i )
				SetTextureCoordIndex(i, i);
			SetTextureTransformFlags(i, A3DTTFF_DISABLE);
		}
	}

	hval = m_pD3DDevice->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)Type, MinIndex, NumVertices, PrimitiveCount, pIndexData, (D3DFORMAT) IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::DrawIndexedPrimitiveUP() Fail");
		return false;
	}

	m_dwDrawVertCount += NumVertices;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);
	return true;
}

bool A3DDevice::DrawPrimitiveUP(A3DPRIMITIVETYPE Type, DWORD PrimitiveCount, void* pVertexStreamZeroData, DWORD VertexStreamZeroStride)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	if( PrimitiveCount == 0 )
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);

	if( m_dwVertexShader & 0x1 )
	{
		// we use a vertex shader now, so we have to make all texture coord index to its layer index
		for(int i=0; i<MAX_TEX_LAYERS; i++)
		{
			if( m_TextureCoordIndex.index[i] != i )
				SetTextureCoordIndex(i, i);
			SetTextureTransformFlags(i, A3DTTFF_DISABLE);
		}
	}

	hval = m_pD3DDevice->DrawPrimitiveUP((D3DPRIMITIVETYPE)Type, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::DrawPrimitiveUP() Fail");
		return false;
	}

	m_dwDrawVertCount += PrimitiveCount * 3;
	m_dwDrawFaceCount += PrimitiveCount;
	m_dwDrawCount ++;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_DRAWPRIMITIVE);
	return true;
}

bool A3DDevice::CopyToBack(int x, int y, int width, int height, IDirect3DSurface8 * pSurface)
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;
	RECT	rect;
	POINT	offset;

	rect.left		= 0;
	rect.top		= 0;
	rect.right		= width;
	rect.bottom		= height;
	offset.x		= x;
	offset.y		= y;

	hval = m_pD3DDevice->CopyRects(pSurface, &rect, 1, m_pBackBuffer, &offset);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::CopyToBack() Fail");
		return false;
	}

	return true;
}

bool A3DDevice::CreateViewport(A3DViewport ** ppViewport, DWORD x, DWORD y, DWORD width, DWORD height, FLOAT minZ, FLOAT maxZ,
		bool bClearColor, bool bClearZ, A3DCOLOR colorClear)
{
	A3DViewport *		pViewport;
	A3DVIEWPORTPARAM	param;

	pViewport = new A3DViewport();
	if( NULL == pViewport )
	{
		g_A3DErrLog.Log("A3DDevice::CreateViewport() Not enough memory!");
		return false;
	}

	ZeroMemory(&param, sizeof(param));
	param.X = x;
	param.Y = y;
	param.Width = width;
	param.Height = height;
	param.MinZ = minZ;
	param.MaxZ = maxZ;

	if( !pViewport->Init(this, &param, bClearColor, bClearZ, colorClear) )
	{
		delete pViewport;
		pViewport = NULL;
		*ppViewport = NULL;
		g_A3DErrLog.Log("A3DDevice::CreateViewport() Viewport Init Fail!");
		return false;
	}

	*ppViewport = pViewport;
	return true;
}

bool A3DDevice::CreateRenderTarget(A3DRenderTarget ** ppRenderTarget, int width, int height,
								   A3DFORMAT fmtTarget, A3DFORMAT fmtDepth, bool bNewTarget, bool bNewDepth)
{
	A3DDEVFMT			devFmt;	
	A3DRenderTarget *	pRenderTarget;

	pRenderTarget = new A3DRenderTarget();
	if( NULL == pRenderTarget )
	{
		g_A3DErrLog.Log("A3DDevice::CreateRenderTarget() Not enough memory!");
		return false;
	}

	ZeroMemory(&devFmt, sizeof(A3DDEVFMT));
	devFmt.nWidth = width;
	devFmt.nHeight = height;
	if( fmtTarget != A3DFMT_UNKNOWN )
		devFmt.fmtTarget = fmtTarget;
	else
		devFmt.fmtTarget = m_A3DDevFmt.fmtTarget;
	if( fmtDepth != A3DFMT_UNKNOWN )
		devFmt.fmtDepth = fmtDepth;
	else
		devFmt.fmtDepth = m_A3DDevFmt.fmtDepth;

	if( !pRenderTarget->Init(this, devFmt, bNewTarget, bNewDepth) )
	{
		delete pRenderTarget;
		pRenderTarget = NULL;
		*ppRenderTarget = NULL;
		g_A3DErrLog.Log("A3DDevice::CreateRenderTarget() RenderTarget Init Fail!");
		return false;
	}

	*ppRenderTarget = pRenderTarget;
	return true;
}

bool A3DDevice::SetRenderTarget(A3DRenderTarget * pRenderTarget)
{
	if( m_bHWIDevice )	return true;

	HRESULT				hval;

	IDirect3DSurface8 * pNewTarget = pRenderTarget->GetTargetSurface();
	IDirect3DSurface8 * pNewDepth = pRenderTarget->GetDepthSurface();

	if( NULL == pNewTarget )
		pNewTarget = m_pRestoreToRenderTarget;
	if( NULL == pNewDepth )
		pNewDepth = m_pRestoreToDepthBuffer;

	hval = m_pD3DDevice->SetRenderTarget(pNewTarget, pNewDepth);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::SetRenderTarget() D3DDevice SetRenderTarget Fail!");
		return false;
	}

	return true;
}

bool A3DDevice::SetRestoreToRenderTarget(A3DRenderTarget * pRenderTarget)
{
	if( m_bHWIDevice )	return true;

	IDirect3DSurface8 * pNewTarget = pRenderTarget->GetTargetSurface();
	IDirect3DSurface8 * pNewDepth = pRenderTarget->GetDepthSurface();

	if( NULL == pNewTarget )
		pNewTarget = m_pRestoreToRenderTarget;
	if( NULL == pNewDepth )
		pNewDepth = m_pRestoreToDepthBuffer;

	m_pRestoreToRenderTarget = pNewTarget;
	m_pRestoreToDepthBuffer = pNewDepth;

	return true;
}

bool A3DDevice::SetRenderTarget(A3DAdditionalView* pAdditionalView)
{
	if( m_bHWIDevice )	return true;

	HRESULT				hval;

	IDirect3DSurface8 * pNewTarget = pAdditionalView->GetBackBuffer();
	IDirect3DSurface8 * pNewDepth = pAdditionalView->GetDepthBuffer();

	if( NULL == pNewTarget )
		pNewTarget = m_pRestoreToRenderTarget;
	if( NULL == pNewDepth )
		pNewDepth = m_pRestoreToDepthBuffer;

	hval = m_pD3DDevice->SetRenderTarget(pNewTarget, pNewDepth);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::SetRenderTarget() D3DDevice SetRenderTarget Fail!");
		return false;
	}

	return true;
}
					
bool A3DDevice::SetDefaultRestoreToRenderTarget()
{
	m_pRestoreToRenderTarget = m_pRenderTarget;
	m_pRestoreToDepthBuffer = m_pDepthBuffer;
	return true;
}
					 
bool A3DDevice::RestoreRenderTarget()
{
	if( m_bHWIDevice )	return true;

	HRESULT hval;

	hval = m_pD3DDevice->SetRenderTarget(m_pRestoreToRenderTarget, m_pRestoreToDepthBuffer);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::RestoreRenderTarget() D3DDevice SetRenderTarget Fail!");
		return false;
	}
 
	return true;
}

bool A3DDevice::IsDetailMethodSupported()
{
	if( m_bHWIDevice )	return true;

	if( (m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE) && 
		(m_d3dcaps.SrcBlendCaps & D3DPBLENDCAPS_DESTCOLOR) &&
		(m_d3dcaps.DestBlendCaps & D3DPBLENDCAPS_SRCCOLOR) )
		return true;

	return false;
}

bool A3DDevice::ClearTexture(int nLayer)
{
	if( m_bHWIDevice )	return true;

	HRESULT		hval;

	hval = m_pD3DDevice->SetTexture(nLayer, NULL);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice::ClearTexture() Fail!");
		return false;
	}
	return true;
}

//	Set texture factor
bool A3DDevice::SetTextureFactor(DWORD dwValue)
{
	if( m_bHWIDevice )	return true;
	SetDeviceRenderState(D3DRS_TEXTUREFACTOR, dwValue);
	return true;
}

bool A3DDevice::SetFogEnable(bool bEnable)
{
	m_bFogEnable = bEnable;

	if( m_bFogEnable )
	{
		SetDeviceRenderState(D3DRS_FOGENABLE, TRUE);
		SetFogTableEnable(m_bFogTableEnable);
	}
	else
	{
		// We set both fog algorithm to none to allow user defined fog data in LVertex;
		// And we can turn off Vertex's fog data by doint this!
		SetDeviceRenderState(D3DRS_FOGENABLE, FALSE);
		SetDeviceRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		SetDeviceRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	}

	return true;
}

bool A3DDevice::SetFogTableEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_bFogTableEnable = bEnable;
	
	if( m_bFogTableEnable && (m_d3dcaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		SetDeviceRenderState(D3DRS_FOGTABLEMODE, (D3DFOGMODE) m_fogMode);
		SetDeviceRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	}
	else
	{
		SetDeviceRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		SetDeviceRenderState(D3DRS_FOGVERTEXMODE, (D3DFOGMODE) m_fogMode);
	}

	return true;
}

bool A3DDevice::SetFogVertEnable(bool bEnable)
{
	if( m_bHWIDevice )	return true;

	m_bFogVertEnable = bEnable;
	
	if( m_bFogVertEnable )
	{
		SetDeviceRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		SetDeviceRenderState(D3DRS_FOGVERTEXMODE, (D3DFOGMODE) m_fogMode);
	}
	else
	{
		SetDeviceRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	}

	return true;
}

bool A3DDevice::SetFogMode(A3DFOGMODE fogMode)
{
	m_fogMode = fogMode;

	SetFogTableEnable(m_bFogTableEnable);
	SetFogVertEnable(m_bFogVertEnable);
	return true;
}
	
bool A3DDevice::SetFogColor(A3DCOLOR fogColor)
{
	if( m_bHWIDevice )	return true;

	m_colorFog = fogColor;
	SetDeviceRenderState(D3DRS_FOGCOLOR, m_colorFog);

	return true;
}

bool A3DDevice::SetFogStart(FLOAT vStart)
{
	if( m_bHWIDevice )	return true;

	m_vFogStart = vStart;
	SetDeviceRenderState(D3DRS_FOGSTART, *(DWORD *)&m_vFogStart);

	return true;
}

bool A3DDevice::SetFogEnd(FLOAT vEnd)
{
	if( m_bHWIDevice )	return true;

	m_vFogEnd = vEnd;
	SetDeviceRenderState(D3DRS_FOGEND, *(DWORD *)&m_vFogEnd);

	return true;
}

bool A3DDevice::SetFogDensity(FLOAT vDensity)
{
	if( m_bHWIDevice )	return true;

	m_vFogDensity = vDensity;
	SetDeviceRenderState(D3DRS_FOGDENSITY, *(DWORD *)&m_vFogDensity);

	return true;
}

char * A3DDevice::GetFormatString(A3DFORMAT format)
{
	switch(format)
	{
    case A3DFMT_R8G8B8:
		return "R8G8B8";

    case A3DFMT_A8R8G8B8:
		return "A8R8G8B8";

    case A3DFMT_X8R8G8B8:
		return "X8R8G8B8";

    case A3DFMT_R5G6B5:
		return "R5G6B5";

    case A3DFMT_X1R5G5B5:
		return "X1R5G5B5";

    case A3DFMT_A1R5G5B5:
		return "A1R5G5B5";

	case A3DFMT_A4R4G4B4:
		return "A4R4G4B4";

    case A3DFMT_R3G3B2:
		return "R3G3B2";

    case A3DFMT_A8:
		return "A8";

	case A3DFMT_A8R3G3B2:
		return "A8R3G3B2";

    case A3DFMT_X4R4G4B4:
		return "A8R3G3B2";

    case A3DFMT_A8P8:
		return "A8P8";

    case A3DFMT_P8:
		return "P8";

    case A3DFMT_L8:
		return "L8";

    case A3DFMT_A8L8:
		return "A8L8";

    case A3DFMT_A4L4:
		return "A4L4";

    case A3DFMT_V8U8:
		return "V8U8";

    case A3DFMT_L6V5U5:
		return "L6V5U5";

    case A3DFMT_X8L8V8U8:
		return "X8L8V8U8";

    case A3DFMT_Q8W8V8U8:
		return "Q8W8V8U8";

    case A3DFMT_V16U16:
		return "V16U16";

    case A3DFMT_W11V11U10:
		return "W11V11U10";

	case A3DFMT_D16_LOCKABLE:
		return "D16_LOCKABLE";

    case A3DFMT_D32:
		return "D32";

    case A3DFMT_D15S1:
		return "D15S1";

    case A3DFMT_D24S8:
		return "D24S8";

    case A3DFMT_D16:
		return "D16";

    case A3DFMT_D24X8:
		return "D24X8";

    case A3DFMT_D24X4S4:
		return "D24X4S4";
	}
	return "Unkown Format";
}

bool A3DDevice::SetDisplayMode(int nWidth, int nHeight, A3DFORMAT format, bool bWindowed, bool bVSync, DWORD dwFlags)
{
	if( m_bHWIDevice )
	{
		// We only take notes of the new parameters, assuming we have set it already!
		if( dwFlags & SDM_WIDTH )
			m_d3dpp.BackBufferWidth = nWidth;
		if( dwFlags & SDM_HEIGHT )
			m_d3dpp.BackBufferHeight = nHeight;
		if( dwFlags & SDM_FORMAT )
			m_d3dpp.BackBufferFormat = (D3DFORMAT) format;
		if( dwFlags & SDM_WINDOW )
			m_d3dpp.Windowed = bWindowed;
		
		if( m_d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || m_d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
			m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		else
			m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

		m_A3DDevFmt.nWidth = m_d3dpp.BackBufferWidth;
		m_A3DDevFmt.nHeight = m_d3dpp.BackBufferHeight;

		m_A3DDevFmt.bWindowed = m_d3dpp.Windowed ? true : false;

		m_A3DDevFmt.fmtAdapter = (A3DFORMAT) m_d3dpp.BackBufferFormat;
		m_A3DDevFmt.fmtTarget = (A3DFORMAT) m_d3dpp.BackBufferFormat;
		m_A3DDevFmt.fmtDepth = (A3DFORMAT) m_d3dpp.AutoDepthStencilFormat;

		// Reget Normal Texture, ColorKey Texture and AlphaTexture format;
		m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;
		switch(m_A3DDevFmt.fmtTarget)
		{
		case A3DFMT_X1R5G5B5:
		case A3DFMT_R5G6B5:
			m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
			m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
			break;
		case A3DFMT_X8R8G8B8:
		case A3DFMT_A8R8G8B8:
			m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
			m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
			break;
		}
		return true;
	}

	// Now it is a real hardware related device, we have to do normal work;
	if( dwFlags & SDM_WIDTH )
		m_d3dpp.BackBufferWidth = nWidth;
	if( dwFlags & SDM_HEIGHT )
		m_d3dpp.BackBufferHeight = nHeight;
	if( (dwFlags & SDM_FORMAT) && !m_A3DDevFmt.bWindowed )
		m_d3dpp.BackBufferFormat = (D3DFORMAT) format;
	if( dwFlags & SDM_WINDOW )
		m_d3dpp.Windowed = bWindowed;
	if( dwFlags & SDM_VSYNC )
	{
		if( bVSync )
		{
			if (m_d3dpp.Windowed)
				m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;			
			else 
			{
				// full screen
				m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
				m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			}
		}
		else
		{
			if (m_d3dpp.Windowed)
				m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
			else 
			{
				// full screen
				m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
				m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			}
		}
	}
	
	A3DFORMAT fmtAdapter;
	if( m_d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || m_d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
	{
		fmtAdapter = m_fmtAdapter16;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	}
	else
	{
		fmtAdapter = m_fmtAdapter32;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	}

	HRESULT hval;
	hval = m_pA3DEngine->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, (D3DFORMAT)fmtAdapter, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, m_d3dpp.AutoDepthStencilFormat);
	if FAILED(hval)
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	bool bval = Reset();
	if (bval)
	{
		// Update Device information;
		m_A3DDevFmt.nWidth = m_d3dpp.BackBufferWidth;
		m_A3DDevFmt.nHeight = m_d3dpp.BackBufferHeight;
		m_A3DDevFmt.bWindowed = m_d3dpp.Windowed ? true : false;

		m_A3DDevFmt.fmtAdapter = fmtAdapter;
		m_A3DDevFmt.fmtTarget = (A3DFORMAT) m_d3dpp.BackBufferFormat;
		m_A3DDevFmt.fmtDepth = (A3DFORMAT) m_d3dpp.AutoDepthStencilFormat;
	}

	// Reget Normal Texture, ColorKey Texture and AlphaTexture format;
	m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;
	switch(m_A3DDevFmt.fmtTarget)
	{
	case A3DFMT_X1R5G5B5:
	case A3DFMT_R5G6B5:
		m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
		m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
		break;
	case A3DFMT_X8R8G8B8:
	case A3DFMT_A8R8G8B8:
		m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
		m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
		break;
	}

	g_A3DErrLog.Log("A3DDevice::SetDisplayMode(), Set to %dx%dx%d", m_A3DDevFmt.nWidth, m_A3DDevFmt.nHeight, (m_A3DDevFmt.fmtTarget == A3DFMT_X8R8G8B8 || m_A3DDevFmt.fmtTarget == A3DFMT_A8R8G8B8) ? 32 : 16);
	return bval;
}

bool A3DDevice::Reset()
{
	if( m_bHWIDevice ) return true;
	
	HRESULT hval;

	// We must test the cooperative level here again to ensure we can call to reset;
	hval = m_pD3DDevice->TestCooperativeLevel();
	if (hval != D3DERR_DEVICENOTRESET && hval != D3D_OK)
		return true;

	if( m_pRenderTarget )
		m_pRenderTarget->Release();
	if( m_pDepthBuffer )
		m_pDepthBuffer->Release();
	if( m_pBackBuffer )
		m_pBackBuffer->Release();

	//	Handle unmanaged device objects
	ACSWrapper csa(&m_csDevObj);

	DevObjTable::iterator it = m_UnmanagedDevObjectTab.begin();
	for (; it != m_UnmanagedDevObjectTab.end(); ++it)
	{
		A3DDevObject* pObject = *it.value();
		pObject->BeforeDeviceReset();
	}

	// confirm desktop bit depth if reset in window mode
	if( m_d3dpp.Windowed && m_A3DDevFmt.bWindowed )
	{
		HDC	hdc = GetDC(NULL);
		int nBitsNow = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC(NULL, hdc);
		if( nBitsNow != m_DesktopMode.nBitsPerPixel )
		{
			m_d3dpp.BackBufferFormat = (D3DFORMAT) (nBitsNow == 16 ? m_fmtTarget16 : m_fmtTarget32);
			m_DesktopMode.nBitsPerPixel = nBitsNow;
			A3DFORMAT fmtAdapter;
			if( m_d3dpp.BackBufferFormat == D3DFMT_R5G6B5 || m_d3dpp.BackBufferFormat == D3DFMT_X1R5G5B5 )
			{
				fmtAdapter = m_fmtAdapter16;
				m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
			}
			else
			{
				fmtAdapter = m_fmtAdapter32;
				m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
			}

			hval = m_pA3DEngine->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, (D3DDEVTYPE) m_A3DDevType, (D3DFORMAT)fmtAdapter, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, m_d3dpp.AutoDepthStencilFormat);
			if FAILED(hval)
				m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

			m_A3DDevFmt.fmtAdapter = fmtAdapter;
			m_A3DDevFmt.fmtTarget = (A3DFORMAT) m_d3dpp.BackBufferFormat;
			m_A3DDevFmt.fmtDepth = (A3DFORMAT) m_d3dpp.AutoDepthStencilFormat;
			// Reget Normal Texture, ColorKey Texture and AlphaTexture format;
			m_fmtNormalTexture = m_A3DDevFmt.fmtTarget;
			switch(m_A3DDevFmt.fmtTarget)
			{
			case A3DFMT_X1R5G5B5:
			case A3DFMT_R5G6B5:
				m_fmtColorKeyTexture = A3DFMT_A1R5G5B5;
				m_fmtAlphaTexture = A3DFMT_A4R4G4B4;
				break;
			case A3DFMT_X8R8G8B8:
			case A3DFMT_A8R8G8B8:
				m_fmtColorKeyTexture = A3DFMT_A8R8G8B8;
				m_fmtAlphaTexture = A3DFMT_A8R8G8B8;
				break;
			}
		}
	}

	// clean up memory here first before we reset to avoid small fragments in video memory
	m_pD3DDevice->ResourceManagerDiscardBytes(0);

	g_A3DErrLog.Log("A3DDevice:Reset() Device Lost, now try to reset it...");
	hval = m_pD3DDevice->Reset(&m_d3dpp);
	if FAILED(hval)
	{
		g_A3DErrLog.Log("A3DDevice:Reset() Reset Device Failure, ERROR=%x!", hval);
		return false;
	}

	g_A3DErrLog.Log("A3DDevice:Reset() Reset Device Succeed.");

	//	Handle unmanaged device objects
	it = m_UnmanagedDevObjectTab.begin();
	for (; it != m_UnmanagedDevObjectTab.end(); ++it)
	{
		A3DDevObject* pObject = *it.value();
		pObject->AfterDeviceReset();
	}

	csa.Detach(true);

	//Get some surface pointers;
	hval = m_pD3DDevice->GetRenderTarget(&m_pRenderTarget);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice:Reset() Can not get rendertarget pointer!");
		return false;
	}

	hval = m_pD3DDevice->GetDepthStencilSurface(&m_pDepthBuffer);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DDevice:Reset() Can not get depth stencil surface pointer!");
		return false;
	}

	hval = m_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
	if (D3D_OK != hval)
	{
		g_A3DErrLog.Log("A3DDevice:Reset() Can not get backbuffer pointer!");
		return false;
	}

	m_pRestoreToRenderTarget = m_pRenderTarget;
	m_pRestoreToDepthBuffer = m_pDepthBuffer;
	ConfigDevice();

	// Now set some special parameters;
	SetAmbient(m_colorAmbient);
	SetFogTableEnable(m_bFogTableEnable);
	SetFogColor(m_colorFog);
	SetFogStart(m_vFogStart);
	SetFogEnd(m_vFogEnd);
	SetFogDensity(m_vFogDensity);
	m_pA3DEngine->GetA3DLightMan()->RestoreLightResource();

	m_bNeedReset = false;
	return true;
}

bool A3DDevice::SetSpecularEnable(bool bEnable)
{
	if( m_bHWIDevice ) return true;

	SetDeviceRenderState(D3DRS_SPECULARENABLE, bEnable ? TRUE : FALSE);	
	return true;
}

bool A3DDevice::SetDitherEnable(bool bEnable)
{
	if( m_bHWIDevice ) return true;

	SetDeviceRenderState(D3DRS_DITHERENABLE, bEnable ? TRUE : FALSE);	
	return true;
}

bool A3DDevice::SetPointSize(FLOAT vPointSize)
{
	if( m_bHWIDevice ) return true;

	SetDeviceRenderState(D3DRS_POINTSIZE, *(DWORD *)&vPointSize);
	return true;
}

bool A3DDevice::SetPointSpriteEnable(bool bEnable)
{
	if( m_bHWIDevice ) return true;

	SetDeviceRenderState(D3DRS_POINTSPRITEENABLE, bEnable ? TRUE : FALSE);
	return true;	
}

bool A3DDevice::SetSoftwareVertexProcessing(bool bEnable)
{
	if (m_bHWIDevice) return true;

	if (!IsHardwareTL() || TestVertexShaderVersion(1, 1))
		return true;
	
	if (bEnable)
		SetDeviceRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, TRUE);
	else
		SetDeviceRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, FALSE);

	// for switch to or from software vertex processing in mixed mode, all render states may lost, 
	// so reset them to unset state.
	m_TextureStageStates.Reset();
	m_TextureCoordIndex.Reset();
	
	return true;
}

//	Add unmanaged device object
bool A3DDevice::AddUnmanagedDevObject(A3DDevObject* pObject)
{
	if (!pObject)
		return false;

	if (m_bHWIDevice)
		return true;
	
	ACSWrapper csa(&m_csDevObj);
	bool bRet = (DWORD)m_UnmanagedDevObjectTab.put((int)pObject, pObject);
	ASSERT(bRet);	//	Ensure don't add same object twice
	return bRet;
}

//	Remove unmanaged devcie object
void A3DDevice::RemoveUnmanagedDevObject(A3DDevObject* pObject)
{
	if (m_bHWIDevice || !pObject)
		return;

	ACSWrapper csa(&m_csDevObj);
	m_UnmanagedDevObjectTab.erase((int)pObject);
}

int A3DDevice::GetGammaLevel()
{
	return m_dwGammaLevel;
}

bool A3DDevice::SetGammaLevel(DWORD dwLevel)
{
	m_dwGammaLevel = dwLevel;

	if( m_bHWIDevice ) return true;

	D3DGAMMARAMP gammaRamp;

	float gamma = dwLevel / 100.0f;
	if( gamma == 0.0f )
	{
		memset(&gammaRamp, 0, sizeof(gammaRamp));
	}
	else
	{
		for(int i=0; i<256; i++)
		{
			WORD out = (WORD)(65535 * pow(i / 255.0f, 1.0f / gamma));
			if( i > 0 )
				out = max2(1, out);
			gammaRamp.red[i] = gammaRamp.green[i] = gammaRamp.blue[i] = out;
		}
	}

	m_pD3DDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION, &gammaRamp);
	return true;
}

void A3DDevice::ShowCursor(bool bShow)
{
	m_bShowCursor = bShow;
	if( m_pA3DCursor )
		m_pA3DCursor->ShowCursor(bShow);
}

void A3DDevice::SetCursor(A3DCursor * pA3DCursor)
{
	m_pA3DCursor = pA3DCursor;
}

bool A3DDevice::ExportColorToFile(char * szFullpath)
{
	if( m_bHWIDevice ) return true;

	if( !m_pBackBuffer )
		return false;

	HRESULT hval;
	hval = D3DXSaveSurfaceToFileA(szFullpath, D3DXIFF_BMP, m_pBackBuffer, NULL, NULL);
	if FAILED(hval)
	{
		g_A3DErrLog.Log("A3DDevice::ExportColorToFile(), Can not save color surface to file [%s]", szFullpath);
		return false;
	}
	return true;
}

bool A3DDevice::ExportDepthToFile(char * szFullpath)
{
	if( m_bHWIDevice ) return true;

	// Currently not working;
	if( !m_pDepthBuffer )
		return false;

	HRESULT hval;

	hval = D3DXSaveSurfaceToFileA(szFullpath, D3DXIFF_BMP, m_pDepthBuffer, NULL, NULL);
	if FAILED(hval)
	{
		g_A3DErrLog.Log("A3DDevice::ExportDepthToFile(), Can not save depth surface to file [%s]", szFullpath);
		return false;
	}
	return true;
}

bool A3DDevice::SetDeviceWnd(HWND hWnd)
{
	if (!m_d3dpp.Windowed)
	{
		g_A3DErrLog.Log("A3DDevice::SetDeviceWnd(), can not change device window in fullscreen mode!");
		return false;
	}

	m_hDeviceWnd = hWnd;
	m_d3dpp.hDeviceWindow = m_hDeviceWnd;

	if (!Reset())
		return false;
	
	return true;
}

//	indexed vertex matrix blend enable
bool A3DDevice::SetIndexedVertexBlendEnable(bool bEnable)
{
	if (m_bHWIDevice) 
		return true;

	SetDeviceRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, bEnable ? TRUE : FALSE); 
	return true;
}

//	Set blend matrix used in indexed vertex blending
bool A3DDevice::SetIndexedVertexBlendMatrix(int iIndex, const A3DMATRIX4& mat)
{
	if (m_bHWIDevice) 
		return true;

	if (FAILED(m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(iIndex), (D3DMATRIX*) &mat)))
	{
		g_A3DErrLog.Log("A3DDevice::SetIndexedVertexBlendMatrix, failed to set %d blend matrix", iIndex); 
		return false;
	}
	
	return true;
}

//	Set vertex blend flag
bool A3DDevice::SetVertexBlendFlag(A3DVERTEXBLENDFLAGS Flag)
{
	if (m_bHWIDevice) 
		return true;

	if (FAILED(SetDeviceRenderState(D3DRS_VERTEXBLEND, (D3DVERTEXBLENDFLAGS)Flag)))
	{
		g_A3DErrLog.Log("A3DDevice::SetVertexBlendFlag, failed to set vertex blend flag"); 
		return false;
	}

	return true;
}

//	Clear vertex stream source
bool A3DDevice::ClearStreamSource(int iStream)
{
	if (m_bHWIDevice) 
		return true;

	m_pD3DDevice->SetStreamSource(iStream, NULL, 0);
	return true;
}

//	Set pixel shader constants
bool A3DDevice::SetPixelShaderConstants(DWORD dwStart, const void* pData, DWORD dwNumConst)
{
	if (m_bHWIDevice) 
		return true;

	if (FAILED(m_pD3DDevice->SetPixelShaderConstant(dwStart, pData, dwNumConst)))
	{
		g_A3DErrLog.Log("A3DDevice::SetPixelShaderConstants, failed to set pixel shader constants"); 
		return false;
	}

	return true;
}

//	Set vertex shader constants
bool A3DDevice::SetVertexShaderConstants(DWORD dwStart, const void* pData, DWORD dwNumConst)
{
	if (m_bHWIDevice) 
		return true;

	if (FAILED(m_pD3DDevice->SetVertexShaderConstant(dwStart, pData, dwNumConst)))
	{
		g_A3DErrLog.Log("A3DDevice::SetVertexShaderConstants, failed to set vertex shader constants"); 
		return false;
	}

	return true;
}

/*	Test current pixel shader version with specified version

	Return true if current version >= specified version, otherwise return false.
*/
bool A3DDevice::TestPixelShaderVersion(int iMajor, int iMinor)
{
	return m_d3dcaps.PixelShaderVersion >= D3DPS_VERSION(iMajor, iMinor) ? true : false;
}

/*	Test current vertex shader version with specified version

	Return true if current version >= specified version, otherwise return false.
*/
bool A3DDevice::TestVertexShaderVersion(int iMajor, int iMinor)
{
	return m_d3dcaps.VertexShaderVersion >= D3DVS_VERSION(iMajor, iMinor) ? true : false;
}

//	Clear current vertex shader
bool A3DDevice::ClearVertexShader()
{
	A3DVertexShader::m_pCurShader = NULL;
	//	Pass NULL to D3D's SetvertexShader in D3D 8.1 will case a debug output
	//	string, so we set vertex shader to A3DFVF_A3DVERTEX. 
	//	But in D3D 9.0, NULL is	a good value which means to clear 
	//	current vertex shader.
	return SetDeviceVertexShader(A3DFVF_A3DVERTEX);
}

//	Clear current pixel shader
bool A3DDevice::ClearPixelShader()
{
	A3DPixelShader::m_pCurShader = NULL;
	return SetDevicePixelShader(NULL);
}

void A3DDevice::EnumDisplayModes(IDirect3D8 * pD3D, A3DFORMAT fmtTarget, DISPLAYMODEARRAY& displaymodes)
{
	displaymodes.RemoveAll();

	int nNumDisplayModes = pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT);

	D3DDISPLAYMODE mode;
	for(int i=0; i<nNumDisplayModes; i++)
	{
		if( D3D_OK != pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, i, &mode) )
			break;

		// first filter unregular display modes
		if( mode.Width < 640 )
		{
		}
		if( mode.Format == (D3DFORMAT) fmtTarget || fmtTarget == A3DFMT_UNKNOWN )
		{
			// first search if there is this display mode already
			for(int j=0; j<displaymodes.GetSize(); j++)
			{
				D3DDISPLAYMODE thisMode = displaymodes[j];
				if( mode.Width == thisMode.Width && mode.Height == thisMode.Height )
					break;
			}

			if( j == displaymodes.GetSize() )
			{
				mode.RefreshRate = 0;
				displaymodes.Add(mode);
			}
		}
	}
}

const D3DDISPLAYMODE& A3DDevice::GetDisplayMode(int nIndex)
{
	if( nIndex >= m_DisplayModes.GetSize() )
		return m_DisplayModes[0];
	else
		return m_DisplayModes[nIndex];
}

bool A3DDevice::SetAutoResetFlag(bool bFlag)
{
	m_bAutoReset = bFlag;
	return true;
}

bool A3DDevice::SetNeedResetFlag(bool bFlag)
{
	m_bNeedReset = bFlag;
	return true;
}

bool A3DDevice::SetDeviceRenderState(DWORD type, DWORD value)
{
	if( type >= RENDERSTATES::MAX_STATES )
		return false;

	if( m_RenderStates.hasset[type] && m_RenderStates.values[type] == value )
		return true;
	
	m_RenderStates.hasset[type] = true;
	m_RenderStates.values[type] = value;
	m_pD3DDevice->SetRenderState((D3DRENDERSTATETYPE) type, value);
	return true;
}

DWORD A3DDevice::GetDeviceRenderState(DWORD type)
{
	if( type >= RENDERSTATES::MAX_STATES )
		return 0;
	
	if( m_RenderStates.hasset[type] )
		return m_RenderStates.values[type];
	
	DWORD value;
	m_pD3DDevice->GetRenderState((D3DRENDERSTATETYPE) type, &value);
	m_RenderStates.hasset[type] = true;
	m_RenderStates.values[type] = value;
	return value;
}

bool A3DDevice::SetDeviceTextureStageState(int layer, DWORD type, DWORD value)
{
	if( layer < 0 || layer >= MAX_TEX_LAYERS )
		return false;

	if( type >= TEXTURESTAGESTATES::MAX_STATES )
		return false;

	if( m_TextureStageStates.hasset[layer][type] && m_TextureStageStates.values[layer][type] == value )
		return true;

	m_TextureStageStates.hasset[layer][type] = true;
	m_TextureStageStates.values[layer][type] = value;
	m_pD3DDevice->SetTextureStageState(layer, (D3DTEXTURESTAGESTATETYPE) type, value);
	return true;
}

DWORD A3DDevice::GetDeviceTextureStageState(int layer, DWORD type)
{
	if( layer < 0 || layer >= MAX_TEX_LAYERS )
		return 0;

	if( type >= TEXTURESTAGESTATES::MAX_STATES )
		return 0;

	if( m_TextureStageStates.hasset[layer][type] )
		return m_TextureStageStates.values[layer][type];
	
	DWORD value;
	m_pD3DDevice->GetTextureStageState(layer, (D3DTEXTURESTAGESTATETYPE) type, &value);
	// record it here, so next time we can use it directly
	m_TextureStageStates.hasset[layer][type] = true;
	m_TextureStageStates.values[layer][type] = value;
	return value;
}

bool A3DDevice::SetDeviceVertexShader(DWORD dwHandle)
{
	if( m_dwVertexShader && m_dwVertexShader == dwHandle )
		return true;

	m_dwVertexShader = dwHandle;
	m_pD3DDevice->SetVertexShader(dwHandle);
	return true;
}

bool A3DDevice::SetDevicePixelShader(DWORD dwHandle)
{
	if( m_dwPixelShader && m_dwPixelShader == dwHandle )
		return true;

	m_dwPixelShader = dwHandle;
	m_pD3DDevice->SetPixelShader(dwHandle);
	return true;
}

bool A3DDevice::GetCanResetFlag()
{
	HRESULT hval;
	hval = m_pD3DDevice->TestCooperativeLevel();

	if (hval != D3DERR_DEVICENOTRESET && hval != D3D_OK)
		return false;

	return true;
}
