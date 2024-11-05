#include "A3DRenderTarget.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DEngine.h"
#include "A3DConfig.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DStream.h"

A3DRenderTarget::A3DRenderTarget()
{
	m_pA3DDevice		= NULL;
	m_pTargetSurface	= NULL;
	m_pTargetTexture	= NULL;
	m_pDepthSurface		= NULL;
	m_pDepthTexture		= NULL;

	m_pColorSurface		= NULL;
	m_pA3DStream		= NULL;

	m_bNewTarget		= false;
	m_bNewDepth			= false;

	m_bUsingTexture		= true;
	m_bHasFilled		= false;
}

A3DRenderTarget::~A3DRenderTarget()
{
}

bool A3DRenderTarget::Init(A3DDevice * pA3DDevice, A3DDEVFMT& devFmt, bool bNewTarget, bool bNewDepth)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	m_pA3DDevice = pA3DDevice;

	m_bNewTarget = bNewTarget;
	m_bNewDepth = bNewDepth;

	m_DevFormat = devFmt;

	if( !CreatePlains() )
	{
		g_A3DErrLog.Log("A3DRenderTarget::Init(), failed to create plains!");
		return false;
	}

	m_ListViewport.Init();

	// Now init an a3dstream for later use;
	static WORD index[] = {0, 1, 2, 2, 1, 3};
	m_pA3DStream = new A3DStream();
	if( NULL == m_pA3DStream )
	{
		g_A3DErrLog.Log("A3DRenderTarget::Init() Not enough memory!");
		return false;
	}
	if (!m_pA3DStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DRenderTarget::Init() Not enough memory!");
		return false;
	}
	m_pA3DStream->SetIndices(index, 6);

	m_pA3DDevice->AddUnmanagedDevObject(this);
	return true;
}

bool A3DRenderTarget::Release()
{
	m_pA3DDevice->RemoveUnmanagedDevObject(this);

	if( m_pA3DStream )
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}

	m_ListViewport.Release();

	ReleasePlains();

	return true;
}

bool A3DRenderTarget::AddViewport(A3DViewport * pViewport, ALISTELEMENT ** ppElement)
{
	if( !m_pA3DDevice ) return true;
	return m_ListViewport.Append((LPVOID)pViewport, ppElement);
}

bool A3DRenderTarget::RenderAllViewport()
{
	if( !m_pA3DDevice ) return true;

	if( m_ListViewport.GetSize() == 0 )
		return true;

	ALISTELEMENT * pThis = m_ListViewport.GetFirst();

	while( pThis != m_ListViewport.GetTail() )
	{
		A3DViewport * pViewport = (A3DViewport *)(pThis->pData);

		pViewport->Active();

		if( !pViewport->ClearDevice() )
			return false;
		
		if( !m_pA3DDevice->GetA3DEngine()->RenderScene(pViewport) )
			return false;

		pThis = pThis->pNext;
	}
	return true;
}

bool A3DRenderTarget::PresentToBack(int x, int y, FLOAT z)
{
	if( !m_pA3DDevice ) return true;

	if( 1 ) //!m_bUsingTexture )
	{
		//A3DCOLOR	color = A3DCOLORRGBA(255, 255, 255, m_pA3DDevice->GetA3DEngine()->GetMotionBlurValue());
		A3DCOLOR	color = A3DCOLORRGBA(255, 255, 255, 255);
		A3DCOLOR	specular = A3DCOLORRGBA(0, 0, 0, 255);
		A3DTLVERTEX verts[4];
		FLOAT		l, t, r, b;

		l = (FLOAT) x; t = (FLOAT) y;
		r = l + (FLOAT) m_DevFormat.nWidth;
		b = t + (FLOAT) m_DevFormat.nHeight;
		
		verts[0] = A3DTLVERTEX(A3DVECTOR4(l, t, z, 1.0f), color, specular, 0.0f, 0.0f);
		verts[1] = A3DTLVERTEX(A3DVECTOR4(r, t, z, 1.0f), color, specular, 1.0f, 0.0f);
		verts[2] = A3DTLVERTEX(A3DVECTOR4(l, b, z, 1.0f), color, specular, 0.0f, 1.0f);
		verts[3] = A3DTLVERTEX(A3DVECTOR4(r, b, z, 1.0f), color, specular, 1.0f, 1.0f);

		RECT rect;
		POINT pt;

		rect.left = rect.top = 0;
		rect.right = m_DevFormat.nWidth;
		rect.bottom = m_DevFormat.nHeight;

		pt.x = pt.y = 0;

		//m_pA3DDevice->GetD3DDevice()->CopyRects(m_pColorSurface, &rect, 1, m_pTargetSurface, &pt);

		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
		m_pA3DDevice->SetZTestEnable(false);

		m_pA3DDevice->GetD3DDevice()->SetTexture(0, m_pTargetTexture);
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
		m_pA3DStream->SetVerts((LPBYTE) verts, 4);
		m_pA3DStream->Appear();

		if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2) )
		{
			g_A3DErrLog.Log("A3DRenderTarget::PresentToBack() DrawIndexedPrimitive fail");
			return false;
		}
		
		m_pA3DDevice->SetZTestEnable(true);
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	}
	else
	{
		if( !m_pColorSurface )
			return true;

		if( !m_pA3DDevice->CopyToBack(x, y, m_DevFormat.nWidth, m_DevFormat.nHeight, m_pColorSurface) )
		{
			g_A3DErrLog.Log("A3DRenderTarget::PresentToBack() CopyRects fail");
			return false;
		}
	}

	return true;
}

bool A3DRenderTarget::ExportColorToFile(char * szFullpath)
{
	if( !m_pA3DDevice ) return true;

	if( !m_pColorSurface )
		return false;

	HRESULT	hval;

	hval = D3DXSaveSurfaceToFileA(szFullpath, D3DXIFF_BMP, m_pColorSurface, NULL, NULL);
	if FAILED(hval)
	{
		g_A3DErrLog.Log("A3DRenderTarget::ExportColorToFile(), Can not save color surface to file [%s]", szFullpath);
		return false;
	}
	return true;
}

bool A3DRenderTarget::ExportDepthToFile(char * szFullpath)
{
	if( !m_pA3DDevice ) return true;

	// Currently not working;
	if( !m_pDepthSurface )
		return false;

	HRESULT		hval;

	hval = D3DXSaveSurfaceToFileA(szFullpath, D3DXIFF_BMP, m_pDepthSurface, NULL, NULL);
	if FAILED(hval)
	{
		g_A3DErrLog.Log("A3DRenderTarget::ExportDepthToFile(), Can not save depth surface to file [%s]", szFullpath);
		return false;
	}
	return true;
}

bool A3DRenderTarget::BeforeDeviceReset()
{
	ReleasePlains();

	return true;
}

bool A3DRenderTarget::AfterDeviceReset()
{
	if( !CreatePlains() )
	{
		g_A3DErrLog.Log("A3DRenderTarget::AfterDeviceReset(), failed to create plains!");
		return false;
	}

	return true;
}

bool A3DRenderTarget::CreatePlains()
{
	HRESULT		hval;

	if( m_bNewTarget )
	{
		if( !m_bUsingTexture )
		{
			hval = m_pA3DDevice->GetD3DDevice()->CreateRenderTarget(m_DevFormat.nWidth,
				m_DevFormat.nHeight, (D3DFORMAT)m_DevFormat.fmtTarget, D3DMULTISAMPLE_NONE,
				FALSE, &m_pColorSurface);
			if( D3D_OK != hval )
			{
				g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() CreateRenderTarget Fail error=%x!", hval);
				return false;
			}
			hval = m_pA3DDevice->GetD3DDevice()->CreateTexture(m_DevFormat.nWidth,
				m_DevFormat.nHeight, 1, NULL, (D3DFORMAT)m_DevFormat.fmtTarget,
				D3DPOOL_DEFAULT, &m_pTargetTexture);
			if( D3D_OK != hval )
			{
				g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() CreateTexture Fail error=%x!", hval);
				return false;
			}
			hval = m_pTargetTexture->GetSurfaceLevel(0, &m_pTargetSurface);
			if( D3D_OK != hval )
			{
				g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() GetSurfaceLevel Fail!");
				return false;
			}
		}
		else
		{
			hval = m_pA3DDevice->GetD3DDevice()->CreateTexture(m_DevFormat.nWidth,
				m_DevFormat.nHeight, 1, D3DUSAGE_RENDERTARGET, (D3DFORMAT)m_DevFormat.fmtTarget,
				D3DPOOL_DEFAULT, &m_pTargetTexture);
			if( D3D_OK != hval )
			{
				g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() CreateTexture[%dx%dx%d] Fail, error=%x!", m_DevFormat.nWidth, m_DevFormat.nHeight, m_DevFormat.fmtTarget, hval);
				return false;
			}
			hval = m_pTargetTexture->GetSurfaceLevel(0, &m_pTargetSurface);
			if( D3D_OK != hval )
			{
				g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() GetSurfaceLevel Fail, error=%x!",hval);
				return false;
			}
			m_pColorSurface = m_pTargetSurface;
			m_pTargetSurface->AddRef();
		}
	}

	if( m_bNewDepth )
	{
		bool bUsingTexture = m_bUsingTexture;

		if( bUsingTexture )
		{
			hval = m_pA3DDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, (D3DDEVTYPE)m_pA3DDevice->GetDevType(), 
				(D3DFORMAT)m_pA3DDevice->GetDevFormat().fmtAdapter, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, (D3DFORMAT)m_DevFormat.fmtDepth);
			if( D3D_OK != hval )
				bUsingTexture = false;
		}


		if( !bUsingTexture )
		{
			hval = m_pA3DDevice->GetD3DDevice()->CreateDepthStencilSurface(m_DevFormat.nWidth,
				m_DevFormat.nHeight, (D3DFORMAT)m_DevFormat.fmtDepth, D3DMULTISAMPLE_NONE, &m_pDepthSurface);
			if( D3D_OK != hval )
			{
				g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() CreateDepth Fail by CreateDepthStencilSurface error=%x!", hval);
				return false;
			}
		}
		else
		{
			hval = m_pA3DDevice->GetD3DDevice()->CreateTexture(m_DevFormat.nWidth,
				m_DevFormat.nHeight, 1, D3DUSAGE_DEPTHSTENCIL, (D3DFORMAT)m_DevFormat.fmtDepth,
				D3DPOOL_DEFAULT, &m_pDepthTexture);
			if( D3D_OK != hval )
			{
				g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() CreateDepth Fail error=%x!", hval);
				return false;
			}
			hval = m_pDepthTexture->GetSurfaceLevel(0, &m_pDepthSurface);
			if( D3D_OK != hval )
			{
				g_A3DErrLog.Log("A3DRenderTarget::CreatePlains() GetSurfaceLevel Fail!");
				return false;
			}
		}
	}

	m_bHasFilled = false;
	return true;
}

bool A3DRenderTarget::ReleasePlains()
{
	if( m_pColorSurface )
	{
		m_pColorSurface->Release();
		m_pColorSurface = NULL;
	}

	if( m_pTargetSurface )
	{
		m_pTargetSurface->Release();
		m_pTargetSurface = NULL;
	}

	if( m_pTargetTexture )
	{
		m_pTargetTexture->Release();
		m_pTargetTexture = NULL;
	}

	if( m_pDepthSurface )
	{
		m_pDepthSurface->Release();
		m_pDepthSurface = NULL;
	}

	if( m_pDepthTexture )
	{
		m_pDepthTexture->Release();
		m_pDepthTexture = NULL;
	}

	return true;
}

bool A3DRenderTarget::AppearAsTexture(int nLayer)
{
	if (!m_pA3DDevice || !m_pTargetTexture)
		return true;

	m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, m_pTargetTexture);
	return true;
}

bool A3DRenderTarget::DisappearAsTexture(int nLayer)
{
	if (!m_pA3DDevice)
		return true;

	m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, NULL);
	return true;
}