#include "StdAfx.h"
#include "A3DGFXRenderSlot.h"
#include "A3DDevice.h"
#include "A3DGFXExMan.h"
#include "A3DVertexShader.h"
#include "A3DShaderMan.h"
#include "A3DCameraBase.h"
#include <AFI.h>

#if defined(_ANGELICA21)

#include <A3DSceneRenderConfig.h>

static const D3DVERTEXELEMENT9 aGfxVertexShaderDecl[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
	{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1 },
	{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

static const D3DVERTEXELEMENT9 aGfxDirectPassDecl[] =
{
	{0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	{0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1},
	{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

#elif defined(_ANGELICA22)
	
#include "A3DHLSL.h"
#include "A3DEnvironment.h"

    static const D3DVERTEXELEMENT9 aGfxVertexShaderDecl[] =
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
		{0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		{0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	static const D3DVERTEXELEMENT9 aGfxDirectPassDecl[] =
	{
		{0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0},
		{0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		{0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

    static const D3DVERTEXELEMENT9 aGfxDirectPassDeclTL[] =
    {
        {0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
        {0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()
    };
#endif

extern int _gfx_draw_count;
extern int _gfx_prim_count;
extern float _gfx_stat_render_time;

GFX_RENDER_MODE A3DGFXRenderSlotMan::g_RenderMode = GRMSoftware;

void A3DGFXRenderSlotMan::Init(A3DDevice* pDevice)
{
	m_pDevice = pDevice;

	if (m_pDevice->GetD3DCaps().MaxVertexBlendMatrixIndex > 4)
	{
		g_RenderMode = GRMBlendMatrix;
		m_nMaxMatrixCount = m_pDevice->GetD3DCaps().MaxVertexBlendMatrixIndex;
	}

#ifdef _ANGELICA21

	// now load the vertex shader
	m_pGFX_Pallete_Shader = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\hlsl\\gfx.hlslv#FOG$1", false);
	m_pGFX_Pallete_Shader->SetDecl(aGfxVertexShaderDecl);
	m_pGFX_Pallete_Shader_No_Fog = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\hlsl\\gfx.hlslv", false);
	m_pGFX_Pallete_Shader_No_Fog->SetDecl(aGfxVertexShaderDecl);
	m_pGfx_TLDirectPass_Shader = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\hlsl\\gfx.hlslv#TLVERTEX$1#FOG$1", false);
	m_pGfx_TLDirectPass_Shader->SetDecl(aGfxDirectPassDecl);
	m_pGfx_TLDirectPass_Shader_No_Fog = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\hlsl\\gfx.hlslv#TLVERTEX$1", false);
	m_pGfx_TLDirectPass_Shader_No_Fog->SetDecl(aGfxDirectPassDecl);
	g_RenderMode = GRMVertexShader;
	m_nMaxMatrixCount = (m_pDevice->GetD3DCaps().MaxVertexShaderConst - GFX_VS_CONST_BASE) / 3;
	m_pDefaultPS = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\hlsl\\gfx.hlslp", false);
	m_pDefaultPS_Alpha = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\hlsl\\gfx.hlslp#ALPHA$1", false);
	m_pDefaultPS_Hilight = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\hlsl\\gfx.hlslp#HILIGHT$1", false);
	m_pDefaultPS_S_Hilight = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\hlsl\\gfx.hlslp#S_HILIGHT$1", false);
	m_pDefaultPS_NoTex = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\hlsl\\gfx.hlslp#NOTEX$1", false);

#elif defined(_ANGELICA22)
	// now load the vertex shader
	m_pGFX_Pallete_Shader = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(SHADERS_ROOT_DIR"vs\\gfx_pallete_vs.txt", false, (D3DVERTEXELEMENT9*)aGfxVertexShaderDecl);

	if (m_pGFX_Pallete_Shader && m_pDevice->GetD3DCaps().MaxVertexShaderConst >= 13)
	{
		//m_pGFX_Pallete_Shader->SetDecl(aGfxVertexShaderDecl);
        int nVSMaxCount = g_pA3DConfig->GetFlagHLSLEffectEnable() ? 70 : (m_pDevice->GetD3DCaps().MaxVertexShaderConst - 10) / 3;

		if (nVSMaxCount >= m_nMaxMatrixCount)
		{
			g_RenderMode = GRMVertexShader;
			m_nMaxMatrixCount = nVSMaxCount;
		}
	}

	static const char* _stereo = SHADERS_ROOT_DIR"vs\\stereo_vs.txt";

	if (af_IsFileExist(_stereo))
	{
		m_pGfx_TLDirectPass_Shader = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(_stereo, false, (D3DVERTEXELEMENT9*)aGfxDirectPassDecl);
		//m_pGfx_TLDirectPass_Shader->SetDecl(aGfxDirectPassDecl);
	}
	else
		m_pGfx_TLDirectPass_Shader = NULL;

	m_pDefaultPS_Alpha = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADERS_ROOT_DIR"ps\\gfx_default_ps_alpha.txt", false);
	m_pDefaultPS_Hilight = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADERS_ROOT_DIR"ps\\gfx_default_ps_hilight.txt", false);
	m_pDefaultPS_S_Hilight = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADERS_ROOT_DIR"ps\\gfx_default_ps_s_hilight.txt", false);

    m_bUseHLSL = true;
    m_pHLSL = m_pDevice->GetA3DEngine()->GetA3DHLSLMan()->LoadShader(SHADERS_ROOT_DIR"HLSL\\gfx\\gfx.hlsl", SHADERS_ROOT_DIR"HLSL\\gfx\\gfx.hlsl", "_FOG_");
    m_pHLSL_TL = m_pDevice->GetA3DEngine()->GetA3DHLSLMan()->LoadShader(SHADERS_ROOT_DIR"HLSL\\gfx\\gfx.hlsl", "vs_main_tl",
        SHADERS_ROOT_DIR"HLSL\\gfx\\gfx.hlsl", "ps_main", "_FOG_", 0);
    m_bUseHLSL = g_pA3DConfig->GetFlagHLSLEffectEnable() && m_pHLSL != NULL && m_pHLSL_TL != NULL;

    A3DEnvironment* pEnvironment = m_pDevice->GetA3DEngine()->GetA3DEnvironment();	// 取环境对象
    A3DCCDBinder* pBinder = pEnvironment->GetCCDBinder();			// 取环境对象的绑定对象
    if (m_pHLSL) m_pHLSL->BindCommConstData(pBinder);
    if (m_pHLSL_TL) m_pHLSL_TL->BindCommConstData(pBinder);

    m_pTLDecl = new A3DVertexDecl;
    m_pTLDecl->Init(m_pDevice, aGfxDirectPassDeclTL);

    //m_bUseHLSL = false;
#else

	// now load the vertex shader
	DWORD	dwVSDecl[20];
	int		n = 0;				
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);		// x, y, z in world
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_D3DCOLOR);	// blend indices.
	dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_D3DCOLOR);	// diffuse.
	dwVSDecl[n ++] = D3DVSD_REG(3, D3DVSDT_D3DCOLOR);	// specular.
	dwVSDecl[n ++] = D3DVSD_REG(4, D3DVSDT_FLOAT2);		// u, v coords.
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.

	m_pGFX_Pallete_Shader = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(SHADERS_ROOT_DIR"vs\\gfx_pallete_vs.txt", false, dwVSDecl);

	if (m_pGFX_Pallete_Shader && m_pDevice->GetD3DCaps().MaxVertexShaderConst >= 13)
	{
		int nVSMaxCount = (m_pDevice->GetD3DCaps().MaxVertexShaderConst - 10) / 3;

		if (nVSMaxCount >= m_nMaxMatrixCount)
		{
			g_RenderMode = GRMVertexShader;
			m_nMaxMatrixCount = nVSMaxCount;
		}
	}

	n = 0;				
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT4);		// x, y, z and w in cuboid space
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_D3DCOLOR);	// diffuse color
	dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_D3DCOLOR);	// specular color
	dwVSDecl[n ++] = D3DVSD_REG(3, D3DVSDT_FLOAT2);		// u, v texture coord
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.

	static const char* _stereo = SHADERS_ROOT_DIR"vs\\stereo_vs.txt";

	if (af_IsFileExist(_stereo))
		m_pGfx_TLDirectPass_Shader = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(_stereo, false, dwVSDecl);
	else
		m_pGfx_TLDirectPass_Shader = NULL;

#endif

}

void A3DGFXRenderSlotMan::Release()
{
	Lock();

	if( m_pGFX_Pallete_Shader )
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pGFX_Pallete_Shader);
		m_pGFX_Pallete_Shader = NULL;
	}

	if( m_pGFX_Pallete_Shader_No_Fog )
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pGFX_Pallete_Shader_No_Fog);
		m_pGFX_Pallete_Shader_No_Fog = NULL;
	}

	if( m_pGfx_TLDirectPass_Shader )
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pGfx_TLDirectPass_Shader);
		m_pGfx_TLDirectPass_Shader = NULL;
	}

	if( m_pGfx_TLDirectPass_Shader_No_Fog )
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pGfx_TLDirectPass_Shader_No_Fog);
		m_pGfx_TLDirectPass_Shader_No_Fog = NULL;
	}

	if (m_pDefaultPS)
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pDefaultPS);
		m_pDefaultPS = NULL;
	}

	if (m_pDefaultPS_Alpha)
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pDefaultPS_Alpha);
		m_pDefaultPS_Alpha = NULL;
	}

	if (m_pDefaultPS_Hilight)
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pDefaultPS_Hilight);
		m_pDefaultPS_Hilight = NULL;
	}

	if (m_pDefaultPS_S_Hilight)
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pDefaultPS_S_Hilight);
		m_pDefaultPS_S_Hilight = NULL;
	}

	if (m_pDefaultPS_NoTex)
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pDefaultPS_NoTex);
		m_pDefaultPS_NoTex = NULL;
	}

	A3DGFXRenderSlotMap::iterator it = m_RenderSlotMap.begin();

	for (; it != m_RenderSlotMap.end(); ++it)
	{
		A3DGFXRenderSlot* pSlot = it->second;

		if (pSlot->m_pTexture)
			m_pDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(pSlot->m_pTexture);

		delete pSlot;
	}

	m_RenderSlotMap.clear();
	m_nMaxMatrixCount = 0;

#ifdef _ANGELICA22
    if( m_pHLSL )
    {
        m_pDevice->GetA3DEngine()->GetA3DHLSLMan()->ReleaseShader(m_pHLSL);
        m_pHLSL = NULL;
    }

    if( m_pHLSL_TL )
    {
        m_pDevice->GetA3DEngine()->GetA3DHLSLMan()->ReleaseShader(m_pHLSL_TL);
        m_pHLSL_TL = NULL;
    }

    A3DRELEASE(m_pTLDecl);
#endif

	Unlock();
}

DWORD A3DGFXRenderSlotMan::RegisterSlot(
	int nRenderLayer,
	int nVertType,
	size_t VertSize,
	int PrimType,
	const AString& strTexPath,
	const A3DSHADER& sh,
	bool bTileMode,
	bool bNoDownSample,
	bool bZTestEnable,
	bool bPixelShader)
{
	Lock();

	if (!bPixelShader)
	{
		A3DGFXRenderSlotMap::iterator it = m_RenderSlotMap.begin();

		for (; it != m_RenderSlotMap.end(); ++it)
		{
			A3DGFXRenderSlot* pSlot = it->second;

			if (pSlot->m_nRenderLayer == nRenderLayer
			 && pSlot->m_nVertType == nVertType
			 && pSlot->m_PrimType == PrimType
			 &&	pSlot->m_strTexPath == strTexPath
			 && pSlot->m_bNoDownSample == bNoDownSample
			 && pSlot->m_Shader.SrcBlend == sh.SrcBlend
			 && pSlot->m_Shader.DestBlend == sh.DestBlend
			 && pSlot->m_bTileMode == bTileMode
			 && pSlot->m_bZTestEnable == bZTestEnable
			 && pSlot->m_bPixelShader == false)
			{
				pSlot->m_nRef++;
				DWORD dwSlot = it->first;
				Unlock();
				return dwSlot;
			}
		}
	}

	Unlock();
	A3DGFXRenderSlot* pSlot = new A3DGFXRenderSlot;
	pSlot->m_nRenderLayer	= nRenderLayer;
	pSlot->m_nVertType		= nVertType;
	pSlot->m_VertSize		= VertSize;
	pSlot->m_PrimType		= PrimType;
	pSlot->m_strTexPath		= strTexPath;
	pSlot->m_bNoDownSample	= bNoDownSample;
	pSlot->m_Shader			= sh;
	pSlot->m_bTileMode		= bTileMode;
	pSlot->m_bZTestEnable	= bZTestEnable;
	pSlot->m_nRef			= 1;
	pSlot->m_nTotalVertCount= 0;
	pSlot->m_bPixelShader	= bPixelShader;
	pSlot->m_enumFogFlag	= GFX_FOG_CURRENT;
	pSlot->m_pDefaultShader = m_pDefaultPS;

	if (sh.DestBlend == A3DBLEND_ONE)
	{
		if (sh.SrcBlend == A3DBLEND_SRCALPHA)
			pSlot->m_pDefaultShader = m_pDefaultPS_Hilight;
		else if (sh.SrcBlend == A3DBLEND_ONE)
			pSlot->m_pDefaultShader = m_pDefaultPS_S_Hilight;

		pSlot->m_enumFogFlag = GFX_FOG_ZERO;
	}
	else if (sh.DestBlend == A3DBLEND_INVSRCALPHA)
	{
		if (sh.SrcBlend == A3DBLEND_SRCALPHA)
			pSlot->m_pDefaultShader = m_pDefaultPS_Alpha;
	}

	if (bPixelShader || pSlot->m_strTexPath.IsEmpty())
	{
		pSlot->m_pTexture = NULL;
#ifdef _ANGELICA21
		pSlot->m_pDefaultShader = m_pDefaultPS_NoTex;
#endif
	}
	else
	{
		pSlot->m_pTexture = AfxGetGFXTexMan().QueryTexture(pSlot->m_strTexPath, bNoDownSample);

		if (!pSlot->m_pTexture || !pSlot->m_pTexture->GetD3DTexture())
		{
#ifdef _ANGELICA21
			pSlot->m_pDefaultShader = m_pDefaultPS_NoTex;
#endif
			// assert(false);
			a_LogOutput(1, "RegisterSlot, Load texture failed, texture = %s", pSlot->m_strTexPath);
		}
	}

	Lock();

	DWORD dwIndex = m_dwCurIndex;
	m_RenderSlotMap[dwIndex] = pSlot;
	m_dwCurIndex++;

	Unlock();
	return dwIndex;
}

void A3DGFXRenderSlotMan::UnregisterSlot(DWORD dwIndex)
{
	Lock();

	A3DGFXRenderSlotMap::iterator it = m_RenderSlotMap.find(dwIndex);

	if (it != m_RenderSlotMap.end())
	{
		A3DGFXRenderSlot* pSlot = it->second;

		if (--pSlot->m_nRef == 0)
		{
			m_RenderSlotMap.erase(it);

			if (pSlot->m_pTexture)
				m_pDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(pSlot->m_pTexture);

			delete pSlot;
		}
	}

	Unlock();
}

void A3DGFXRenderSlotMan::RegisterEleForRender(DWORD dwIndex, A3DGFXElement* pEle)
{
	Lock();

	A3DGFXRenderSlotMap::iterator it = m_RenderSlotMap.find(dwIndex);

	if (it != m_RenderSlotMap.end())
	{
		A3DGFXRenderSlot* pSlot = it->second;
		pSlot->m_EleArr.push_back(pEle);
		pSlot->m_nTotalVertCount += pEle->GetVertsCount();
	}

	Unlock();
}

void A3DGFXRenderSlotMan::Render(A3DViewport* pView)
{
#ifdef GFX_STAT
	__int64 _start, _end;
	_start = ACounter::GetMicroSecondNow();
#endif

	m_bUserClip = false;
	DWORD dwState = m_pDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);

	if (dwState == D3DCLIPPLANE0) // only one clip plane supported now
	{
		m_bUserClip = true;
		m_pDevice->GetD3DDevice()->GetClipPlane(0, (float *)&m_cp);
		A3DMATRIX4 matVP = pView->GetCamera()->GetVPTM();
		matVP.InverseTM();
		matVP.Transpose();
		D3DXPlaneTransform(&m_hcp, &m_cp, (D3DXMATRIX *) &matVP);
	}

	bool bOldFogEnable = m_pDevice->GetFogEnable();
	m_bCurFogEnabled = (m_bFogEnable && bOldFogEnable);

#ifdef _ANGELICA21
	m_pDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	bool bAlphaBlend = (m_pDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) != 0);
	m_pDevice->SetAlphaBlendEnable(true);
#endif

	if (m_bCurFogEnabled)
		m_clCurFog = m_pDevice->GetFogColor();

	Lock();

	A3DGFXRenderSlotMap::iterator it = m_RenderSlotMap.begin();

	for (; it != m_RenderSlotMap.end(); ++it)
	{
		A3DGFXRenderSlot* pSlot = it->second;

		if (pSlot->m_PrimType != GFX_MODEL_PRIM_TYPE && pSlot->m_nTotalVertCount < 2)
		{
			pSlot->m_EleArr.clear();
			pSlot->m_nTotalVertCount = 0;
			continue;
		}

		if (pSlot->m_nRenderLayer < GFX_RENDER_LAYER_NORMAL || pSlot->m_nRenderLayer >= GFX_RENDER_LAYER_COUNT)
			RenderOneSlot(pSlot, pView);
		else
			m_DelayedSlots[pSlot->m_nRenderLayer].push_back(pSlot);
	}

	RenderDelayedSlots(GFX_RENDER_LAYER_BASE4, pView);
	RenderDelayedSlots(GFX_RENDER_LAYER_BASE3, pView);
	RenderDelayedSlots(GFX_RENDER_LAYER_BASE2, pView);
	RenderDelayedSlots(GFX_RENDER_LAYER_BASE1, pView);
	RenderDelayedSlots(GFX_RENDER_LAYER_NORMAL, pView);
	RenderDelayedSlots(GFX_RENDER_LAYER_FRONT1, pView);
	RenderDelayedSlots(GFX_RENDER_LAYER_FRONT2, pView);
	RenderDelayedSlots(GFX_RENDER_LAYER_FRONT3, pView);
	RenderDelayedSlots(GFX_RENDER_LAYER_FRONT4, pView);

	Unlock();

#ifdef _ANGELICA21

	m_pDevice->ClearVertexShader();
	m_pDevice->ClearPixelShader();
	m_pDevice->SetAlphaBlendEnable(bAlphaBlend);

#endif

	m_pDevice->SetFogEnable(bOldFogEnable);

	if (m_bCurFogEnabled)
		m_pDevice->SetFogColor(m_clCurFog);

#ifdef GFX_STAT
	_end = ACounter::GetMicroSecondNow();
	float _tick_time = (_end - _start) * 0.001f;
	_gfx_stat_render_time += _tick_time;
#endif
}

void A3DGFXRenderSlotMan::RenderOneSlot(A3DGFXRenderSlot* pSlot, A3DViewport* pView)
{
	switch (pSlot->m_PrimType)
	{
	case A3DPT_TRIANGLELIST:
		if (pSlot->m_nVertType == A3DFVF_GFXVERTEX)
			RenderTriangleList(pSlot, pView);
		else
			RenderTLTriangleList(pSlot, pView);
		break;

	case A3DPT_TRIANGLESTRIP:
		RenderTriangleStrip(pSlot, pView);
		break;

	case GFX_MODEL_PRIM_TYPE:
		RenderSkinModel(pSlot, pView);
		break;
		
	case A3DPT_POINTLIST:
		RenderPointList(pSlot, pView);
		break;
	}

	pSlot->m_EleArr.clear();
	pSlot->m_nTotalVertCount = 0;
}

void A3DGFXRenderSlotMan::PreRender(A3DGFXRenderSlot* pSlot)
{
	if (pSlot->m_bTileMode)
		m_pDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	else
		m_pDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	m_pDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	m_pDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pDevice->SetZWriteEnable(false);
	m_pDevice->SetZTestEnable(pSlot->m_bZTestEnable);
	m_pDevice->SetFaceCull(A3DCULL_NONE);
	m_pDevice->SetLightingEnable(false);
	m_pDevice->SetFogEnable(m_bCurFogEnabled);
	m_pDevice->SetSourceAlpha(pSlot->m_Shader.SrcBlend);
	m_pDevice->SetDestAlpha(pSlot->m_Shader.DestBlend);

#ifndef _ANGELICA21

	if (m_bCurFogEnabled)
	{
		switch (pSlot->m_enumFogFlag)
		{
		case GFX_FOG_CURRENT:
			m_pDevice->SetFogColor(m_clCurFog);
			break;
		default:
			m_pDevice->SetFogColor(0);
		}
	}

#endif

	if (pSlot->m_bPixelShader)
	{
		if (pSlot->m_EleArr.size())
		{
			A3DGFXElement* pEle = pSlot->m_EleArr[0];
			pEle->ApplyPixelShader();
		}
	}
	else
	{
		A3DTexture* pTex = pSlot->m_pTexture;

		if (pTex == NULL)
		{
			m_pDevice->ClearTexture(0);
#ifdef _ANGELICA21
			if (A3DPixelShader::m_pCurShader != pSlot->m_pDefaultShader)
			{
				if (pSlot->m_pDefaultShader)
					pSlot->m_pDefaultShader->Appear();
				else
					m_pDevice->ClearPixelShader();
			}
#endif
		}
		else
		{
#ifdef _ANGELICA21
			if (A3DPixelShader::m_pCurShader != pSlot->m_pDefaultShader)
			{
				if (pSlot->m_pDefaultShader)
					pSlot->m_pDefaultShader->Appear();
				else
					m_pDevice->ClearPixelShader();
			}
#else
			if (m_bAlphaTestEnable && pTex->IsAlphaTexture())
			{
				m_pDevice->SetAlphaTestEnable(true);
				m_pDevice->SetAlphaFunction(A3DCMP_GREATER);
				m_pDevice->SetAlphaRef(1);
			}

#endif

			pTex->Appear(0);
		}
	}
}

void A3DGFXRenderSlotMan::PostRender(A3DGFXRenderSlot* pSlot)
{
	m_pDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pDevice->SetZWriteEnable(true);
	m_pDevice->SetZTestEnable(true);
	m_pDevice->SetFaceCull(A3DCULL_CCW);
	m_pDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

#ifndef _ANGELICA21

	 m_pDevice->SetFogEnable(true);
	 m_pDevice->SetLightingEnable(true);

#endif

	if (pSlot->m_bPixelShader)
	{
		if (pSlot->m_EleArr.size())
		{
			A3DGFXElement* pEle = pSlot->m_EleArr[0];
			pEle->RestorePixelShader();
		}
	}
	else
	{
		A3DTexture* pTex = pSlot->m_pTexture;

		if (pTex)
		{
#ifndef _ANGELICA21
			if (m_bAlphaTestEnable && pTex->IsAlphaTexture())
				m_pDevice->SetAlphaTestEnable(false);
#endif

			pTex->Disappear(0);
		}
	}

#ifdef _ANGELICA21
	m_pDevice->ClearPixelShader();
#endif
}

void A3DGFXRenderSlotMan::ApplyVertexShaderConsts(A3DViewport* pView)
{
#ifdef _ANGELICA21

	if (m_bCurFogEnabled)
	{
		A3DSceneRenderConfig* pRenderCfg = m_pDevice->GetA3DEngine()->GetSceneRenderConfig();
		A3DVECTOR4 eyePos = pView->GetCamera()->GetPos();
		pRenderCfg->AppearFog(9, true);
		m_pDevice->SetVertexShaderConstants(13, &eyePos, 1);
	}

	if (A3DVertexShader::m_pCurShader == m_pGFX_Pallete_Shader ||
		A3DVertexShader::m_pCurShader == m_pGfx_TLDirectPass_Shader ||
		A3DVertexShader::m_pCurShader == m_pGFX_Pallete_Shader_No_Fog ||
		A3DVertexShader::m_pCurShader == m_pGfx_TLDirectPass_Shader_No_Fog)
		return;

	A3DMATRIX4 matVP = pView->GetCamera()->GetVPTM();
	A3DMATRIX4 matInvVPScale = matVP * pView->GetViewScale();
	matVP.Transpose();
	m_pDevice->SetVertexShaderConstants(1, &matVP, 4);
	matInvVPScale.InverseTM();
	matInvVPScale.Transpose();
	m_pDevice->SetVertexShaderConstants(5, &matInvVPScale, 4);

#endif
}

#ifdef _ANGELICA22
void A3DGFXRenderSlotMan::ApplyHLSL(A3DViewport* pView, A3DHLSL* pHLSL, A3DGFXRenderSlot* pSlot)
{
    m_pDevice->SetFogEnable(false);
    pHLSL->SetValue3f("g_vecEyePos", &pView->GetCamera()->GetPos());
    pHLSL->SetTexture("g_Sampler", pSlot->m_pTexture);
    A3DEnvironment* pEnv = m_pDevice->GetA3DEngine()->GetA3DEnvironment();
    A3DEnvironment::FOG oldFog = *pEnv->GetFogParam();
    bool bIsAdditive = pSlot->m_Shader.DestBlend == A3DBLEND_ONE ||
        (pSlot->m_Shader.SrcBlend == A3DBLEND_ONE && pSlot->m_Shader.DestBlend != A3DBLEND_ZERO);
    if (bIsAdditive)
    {
        A3DEnvironment::FOG fog = *pEnv->GetFogParam();
        fog.crFog1 &= 0xFF000000;
        fog.crFog2 &= 0xFF000000;
		pEnv->SetFogParamAboveWater(&fog);
    }
    pHLSL->Appear(m_pDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());
    if (bIsAdditive)
    {
        pEnv->SetFogParamAboveWater(&oldFog);
    }
}
#endif

void A3DGFXRenderSlotMan::RenderPointList(A3DGFXRenderSlot* pSlot, A3DViewport* pView)
{
	if (g_RenderMode == GRMSoftware)
		return;

	const int nMaxVert	= A3DGFXStreamMan::GetMaxVertexBufferSize() / pSlot->m_VertSize;
	const int nMaxCount	= nMaxVert;
	int nRenderCount	= pSlot->m_nTotalVertCount;
	
	BYTE* pVerts;

	PreRender(pSlot);
	m_pDevice->SetDeviceRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pDevice->SetDeviceRenderState(D3DRS_POINTSCALEENABLE, true);

	const size_t nCount	= pSlot->m_EleArr.size();
	int nTotalVertCount = 0;

	for (size_t i = 0 ; i < nCount ; ++i) {
		A3DGFXElement* pEle = pSlot->m_EleArr[i];
		int nVertCount = pEle->GetVertsCount();
		if (nVertCount > nMaxCount)
			continue;

		PointSpriteInfo psi;
		memset(&psi, 0, sizeof(psi));
		pEle->GetPointSpriteInfo(&psi);
		m_pDevice->SetDeviceRenderState(D3DRS_POINTSIZE, *(DWORD*)(&psi.fPointSize));
		m_pDevice->SetDeviceRenderState(D3DRS_POINTSCALE_A, *(DWORD*)(&psi.fScaleA));
		m_pDevice->SetDeviceRenderState(D3DRS_POINTSCALE_B, *(DWORD*)(&psi.fScaleB));
 		m_pDevice->SetDeviceRenderState(D3DRS_POINTSCALE_C, *(DWORD*)(&psi.fScaleC));

		int nSizeToLock	= min(nRenderCount, nMaxCount) * pSlot->m_VertSize;
		if (!AfxGetGFXStreamMan().LockVertexBuffer(0, nSizeToLock, &pVerts, 0))
			return;

		AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, true);

		int nCurCount = pEle->FillVertexBuffer(pVerts, 0, pView);

		AfxGetGFXStreamMan().UnlockVertexBuffer();
			
		if (!nCurCount)
			continue;

#ifdef _ANGELICA21
		m_pDevice->ClearVertexShader();
		m_pDevice->SetDeviceVertexShader(pSlot->m_nVertType);
#endif
		m_pDevice->DrawPrimitive(A3DPT_POINTLIST, 0, nCurCount);

		_gfx_draw_count++;
		AfxGetGFXExMan()->AddTotalPrimCount(nCurCount);
		AfxGetGFXExMan()->AddElementPrimCount(pEle->GetEleTypeId(), nCurCount);

		nRenderCount -= nCurCount;
	}	

	m_pDevice->SetDeviceRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pDevice->SetDeviceRenderState(D3DRS_POINTSCALEENABLE, false);
	PostRender(pSlot);
}

void A3DGFXRenderSlotMan::RenderSkinModel(A3DGFXRenderSlot* pSlot, A3DViewport* pView)
{
	const size_t count = pSlot->m_EleArr.size();
	m_pDevice->SetFogEnable(m_bCurFogEnabled);

	if (m_bCurFogEnabled)
	{
		switch (pSlot->m_enumFogFlag)
		{
		case GFX_FOG_CURRENT:
			m_pDevice->SetFogColor(m_clCurFog);
			break;
		default:
			m_pDevice->SetFogColor(0);
		}
	}

	for (size_t i = 0; i < count; i++)
	{
		A3DGFXElement* pEle = pSlot->m_EleArr[i];
		A3DSkinModel* pSkinModel = pEle->GetSkinModel();
		
		if (!pSkinModel)
			continue;
		
// 		A3DCameraBase* pOldCamera = pView->GetCamera();
// 		if (AfxGetGFXExMan()->Is2DViewport(pView) && pEle->IsForceUse3DCamera())
// 		{
// 			A3DCamera* pFixedCamera = AfxGetGFXExMan()->GetFixed3DCamera();
// 			pView->SetCamera(pFixedCamera);
// 			pView->Active();
// 		}

		AfxGetGFXExMan()->GetModelMan().Lock();
		pEle->RenderSkinModel(pView, pSkinModel);
		m_pDevice->SetAlphaBlendEnable(true);
		m_pDevice->SetZWriteEnable(true);
		AfxGetGFXExMan()->GetModelMan().Unlock();

// 		if (pOldCamera != pView->GetCamera())
// 		{
// 			pView->SetCamera(pOldCamera);
// 			pView->Active();
// 		}
	}

#ifndef _ANGELICA21
	m_pDevice->SetFogEnable(true);
#endif
}

void A3DGFXRenderSlotMan::RenderTriangleStrip(A3DGFXRenderSlot* pSlot, A3DViewport* pView)
{
	if (g_RenderMode == GRMSoftware)
	{
		RenderTriangleStripSoftware(pSlot, pView);
		return;
	}

	BYTE* pVerts;

	const int _max_count = A3DGFXStreamMan::GetMaxVertexBufferSize() / pSlot->m_VertSize;
	int _render_count = pSlot->m_nTotalVertCount + pSlot->m_EleArr.size() * 2;
	int size_to_lock = min(_render_count, _max_count) * pSlot->m_VertSize;

	if (!AfxGetGFXStreamMan().LockVertexBuffer(0, size_to_lock, &pVerts, 0))
		return;

	PreRender(pSlot);

	if (g_RenderMode == GRMVertexShader)
	{
		AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, false);

#ifdef _ANGELICA21
		ApplyVertexShaderConsts(pView);

		if (m_bCurFogEnabled)
			m_pGFX_Pallete_Shader->Appear();
		else
			m_pGFX_Pallete_Shader_No_Fog->Appear();
#else

#ifdef _ANGELICA22

        if (m_bUseHLSL && m_pHLSL) 
        {
            ApplyHLSL(pView, m_pHLSL, pSlot);
        }
        else
            m_pGFX_Pallete_Shader->Appear();
#else
        m_pGFX_Pallete_Shader->Appear();
#endif
		// set vertex shader consts
		A3DVECTOR4 c0(1.0f, 0.0f, 0.0f, 765.01f);
		m_pDevice->SetVertexShaderConstants(0, &c0, 1);
		A3DMATRIX4 matView = pView->GetCamera()->GetViewTM();
		matView.Transpose();
		m_pDevice->SetVertexShaderConstants(1, &matView, 4);
		A3DMATRIX4 matProjection = pView->GetCamera()->GetProjectionTM() * pView->GetCamera()->GetPostProjectTM();
		matProjection.Transpose();
		m_pDevice->SetVertexShaderConstants(5, &matProjection, 4);
#endif

		if (m_bUserClip)
		{
			// set clip plane to the one in homegeneous space
			m_pDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_hcp);
		}
	}
	else
	{
		m_pDevice->SetVertexBlendFlag(A3DVBF_0WEIGHTS);
		m_pDevice->SetIndexedVertexBlendEnable(true);
		AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, false);
	}

	const size_t count = pSlot->m_EleArr.size();
	int nTotalVertCount = 0;
	BYTE* pLastVert = NULL;
	int nMatrixIndex = 0;

	for (size_t i = 0; i < count; i++)
	{
		A3DGFXElement* pEle = pSlot->m_EleArr[i];
		int nVertCount = pEle->GetVertsCount() + 2;

		if (nVertCount > _max_count)
			continue;

		if (nMatrixIndex >= m_nMaxMatrixCount || nTotalVertCount + nVertCount > _max_count)
		{
			AfxGetGFXStreamMan().UnlockVertexBuffer();
			m_pDevice->DrawPrimitive(A3DPT_TRIANGLESTRIP, 0, nTotalVertCount - 2);
			_gfx_draw_count++;
			
			AfxGetGFXExMan()->AddTotalPrimCount(nTotalVertCount-2);

			_render_count -= nTotalVertCount;
			size_to_lock = min(_render_count, _max_count) * pSlot->m_VertSize;
			nTotalVertCount = 0;
			pLastVert = NULL;
			nMatrixIndex = 0;

			if (!AfxGetGFXStreamMan().LockVertexBuffer(0, size_to_lock, &pVerts, 0))
			{
				if (g_RenderMode == GRMVertexShader)
                {
					m_pDevice->ClearVertexShader();
                    m_pDevice->ClearPixelShader();
                }
				else
				{
					m_pDevice->SetVertexBlendFlag(A3DVBF_DISABLE);
					m_pDevice->SetIndexedVertexBlendEnable(false);
				}

				PostRender(pSlot);

				if (g_RenderMode == GRMVertexShader && m_bUserClip)
				{
					// restore clip plane to the one in world space
					m_pDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_cp);
				}

				return;
			}
			else
			{
#if defined (_ANGELICA21) || defined(_ANGELICA22)
				AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, false);
#endif
			}
		}

		if (pLastVert)
		{
			memcpy(pVerts, pLastVert, pSlot->m_VertSize);
			pVerts += pSlot->m_VertSize * 2;
			nTotalVertCount += 2;
		}

		int nCurCount = pEle->FillVertexBuffer(pVerts, nMatrixIndex, pView);

		if (nCurCount > 0)
		{
			if (pLastVert)
				memcpy(pVerts - pSlot->m_VertSize, pVerts, pSlot->m_VertSize);

			pVerts += nCurCount * pSlot->m_VertSize;
			nTotalVertCount += nCurCount;
			pLastVert = pVerts - pSlot->m_VertSize;
			nMatrixIndex++;
		}
		else if (pLastVert)
		{
			pVerts -= pSlot->m_VertSize * 2;
			nTotalVertCount -= 2;
		}

		if(nCurCount > 2)
			AfxGetGFXExMan()->AddElementPrimCount(pEle->GetEleTypeId(), nCurCount-2);
	}

	AfxGetGFXStreamMan().UnlockVertexBuffer();

	if (nTotalVertCount > 2)
	{
		m_pDevice->DrawPrimitive(A3DPT_TRIANGLESTRIP, 0, nTotalVertCount - 2);
		_gfx_draw_count++;

		AfxGetGFXExMan()->AddTotalPrimCount(nTotalVertCount-2);
	}

	if (g_RenderMode == GRMVertexShader)
	{
#ifndef _ANGELICA21
		m_pDevice->ClearVertexShader();
        m_pDevice->ClearPixelShader();
#endif
	}
	else
	{
		m_pDevice->SetVertexBlendFlag(A3DVBF_DISABLE);
		m_pDevice->SetIndexedVertexBlendEnable(false);
	}

	PostRender(pSlot);

	if (g_RenderMode == GRMVertexShader && m_bUserClip)
	{
		// restore clip plane to the one in world space
		m_pDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_cp);
	}
}

void A3DGFXRenderSlotMan::RenderTriangleList(A3DGFXRenderSlot* pSlot, A3DViewport* pView)
{
	if (g_RenderMode == GRMSoftware)
	{
		RenderTriangleListSoftware(pSlot, pView);
		return;
	}

	BYTE* pVerts;

	const int _max_index = A3DGFXStreamMan::GetMaxIndexCount() * 4 / 6;
	const int _max_vert = A3DGFXStreamMan::GetMaxVertexBufferSize() / pSlot->m_VertSize;
	const int _max_count = min(_max_index, _max_vert);

	int _render_count = pSlot->m_nTotalVertCount;
	int size_to_lock = min(_render_count, _max_count) * pSlot->m_VertSize;

	if (!AfxGetGFXStreamMan().LockVertexBuffer(0, size_to_lock, &pVerts, 0))
		return;

	PreRender(pSlot);
	
	if (g_RenderMode == GRMVertexShader)
	{
		AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, true);

#ifdef _ANGELICA21
		ApplyVertexShaderConsts(pView);

		if (m_bCurFogEnabled)
			m_pGFX_Pallete_Shader->Appear();
		else
			m_pGFX_Pallete_Shader_No_Fog->Appear();
#else

#ifdef _ANGELICA22

        if (m_bUseHLSL && m_pHLSL) 
        {
            ApplyHLSL(pView, m_pHLSL, pSlot);
        }
        else
            m_pGFX_Pallete_Shader->Appear();
#else
		m_pGFX_Pallete_Shader->Appear();
#endif

		// set vertex shader consts
		A3DVECTOR4 c0(1.0f, 0.0f, 0.0f, 765.01f);
		m_pDevice->SetVertexShaderConstants(0, &c0, 1);
		A3DMATRIX4 matView = pView->GetCamera()->GetViewTM();
		matView.Transpose();
		m_pDevice->SetVertexShaderConstants(1, &matView, 4);
		A3DMATRIX4 matProjection = pView->GetCamera()->GetProjectionTM() * pView->GetCamera()->GetPostProjectTM();
		matProjection.Transpose();
		m_pDevice->SetVertexShaderConstants(5, &matProjection, 4);
#endif

		if (m_bUserClip)
		{
			// set clip plane to the one in homegeneous space
			m_pDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_hcp);
		}
	}
	else
	{
		m_pDevice->SetVertexBlendFlag(A3DVBF_0WEIGHTS);
		m_pDevice->SetIndexedVertexBlendEnable(true);
		AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, true);
	}

	const size_t count = pSlot->m_EleArr.size();
	int nTotalVertCount = 0;
	int nMatrixIndex = 0;

	for (size_t i = 0; i < count; i++)
	{
		A3DGFXElement* pEle = pSlot->m_EleArr[i];
		int nVertCount = pEle->GetVertsCount();

		if (nVertCount > _max_count)
			continue;

		if (nMatrixIndex >= m_nMaxMatrixCount || nTotalVertCount + nVertCount > _max_count)
		{
			AfxGetGFXStreamMan().UnlockVertexBuffer();
			m_pDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nTotalVertCount, 0, nTotalVertCount / 2);
			_gfx_draw_count++;

			AfxGetGFXExMan()->AddTotalPrimCount(nTotalVertCount/2);

			_render_count -= nTotalVertCount;
			size_to_lock = min(_render_count, _max_count) * pSlot->m_VertSize;
			nTotalVertCount = 0;
			nMatrixIndex = 0;

			if (!AfxGetGFXStreamMan().LockVertexBuffer(0, size_to_lock, &pVerts, 0))
			{
				if (g_RenderMode == GRMVertexShader)
                {
					m_pDevice->ClearVertexShader();
                    m_pDevice->ClearPixelShader();
                }
				else
				{
					m_pDevice->SetVertexBlendFlag(A3DVBF_DISABLE);
					m_pDevice->SetIndexedVertexBlendEnable(false);
				}

				PostRender(pSlot);

				if (g_RenderMode == GRMVertexShader && m_bUserClip)
				{
					// restore clip plane to the one in world space
					m_pDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_cp);
				}

				return;
			}
			else
			{
#if defined _ANGELICA21 || defined _ANGELICA22
				AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, true);
#endif
			}
		}

		int nCurCount = pEle->FillVertexBuffer(pVerts, nMatrixIndex, pView);

		if (nCurCount > 0)
		{
			pVerts += nCurCount * pSlot->m_VertSize;
			nTotalVertCount += nCurCount;
			nMatrixIndex++;
		}

		if (nTotalVertCount > 2)
		{
			AfxGetGFXExMan()->AddElementPrimCount(pEle->GetEleTypeId(), nCurCount / 2);
		}
	}

	AfxGetGFXStreamMan().UnlockVertexBuffer();

	if (nTotalVertCount > 2)
	{
		m_pDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nTotalVertCount, 0, nTotalVertCount / 2);
		_gfx_draw_count++;
		AfxGetGFXExMan()->AddTotalPrimCount(nTotalVertCount/2);
	}

	if (g_RenderMode == GRMVertexShader)
	{
#ifndef _ANGELICA21
		m_pDevice->ClearVertexShader();
        m_pDevice->ClearPixelShader();
#endif
	}
	else
	{
		m_pDevice->SetVertexBlendFlag(A3DVBF_DISABLE);
		m_pDevice->SetIndexedVertexBlendEnable(false);
	}

	PostRender(pSlot);

	if (g_RenderMode == GRMVertexShader && m_bUserClip)
	{
		// restore clip plane to the one in world space
		m_pDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_cp);
	}
}

void A3DGFXRenderSlotMan::RenderTLTriangleList(A3DGFXRenderSlot* pSlot, A3DViewport* pView)
{
	BYTE* pVerts;

	const int _max_index = A3DGFXStreamMan::GetMaxIndexCount();
	const int _max_vert = A3DGFXStreamMan::GetMaxVertexBufferSize() / pSlot->m_VertSize;
	const int _max_count = min(_max_index, _max_vert);

	int _render_count = pSlot->m_nTotalVertCount;
	int size_to_lock = min(_render_count, _max_count) * pSlot->m_VertSize;

	if (!AfxGetGFXStreamMan().LockVertexBuffer(0, size_to_lock, &pVerts, 0))
		return;

	PreRender(pSlot);

	const size_t count = pSlot->m_EleArr.size();
	int nTotalVertCount = 0;

	for (size_t i = 0; i < count; i++)
	{
		A3DGFXElement* pEle = pSlot->m_EleArr[i];
		int nVertCount = pEle->GetVertsCount();

		if (nVertCount > _max_count)
			continue;

		if (nTotalVertCount + nVertCount > _max_count)
		{
			AfxGetGFXStreamMan().UnlockVertexBuffer();
			AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, true);

			if (m_pGfx_TLDirectPass_Shader)
			{
#ifdef _ANGELICA21
				ApplyVertexShaderConsts(pView);

				if (m_bCurFogEnabled)
					m_pGfx_TLDirectPass_Shader->Appear();
				else
					m_pGfx_TLDirectPass_Shader_No_Fog->Appear();
#else
#ifdef _ANGELICA22
                if (m_bUseHLSL && m_pHLSL_TL) 
                {
                    m_pTLDecl->Appear();
                    ApplyHLSL(pView, m_pHLSL_TL, pSlot);
                }
                else
                    m_pGfx_TLDirectPass_Shader->Appear();
#else
				m_pGfx_TLDirectPass_Shader->Appear();
#endif
				A3DVECTOR4 c0(2.0f / pView->GetParam()->Width, -2.0f / pView->GetParam()->Height, 1.0f, 0.0f);
				m_pDevice->SetVertexShaderConstants(0, &c0, 1);
				A3DVECTOR4 c1(static_cast<float>(pView->GetParam()->X)
					, static_cast<float>(pView->GetParam()->Y)
					, 0.0f
					, 0.0f);
				m_pDevice->SetVertexShaderConstants(1, &c1, 1);
#endif
			}
			m_pDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nTotalVertCount, 0, nTotalVertCount / 2);
			_gfx_draw_count++;

			AfxGetGFXExMan()->AddTotalPrimCount(nTotalVertCount/2);

			_render_count -= nTotalVertCount;
			size_to_lock = min(_render_count, _max_count) * pSlot->m_VertSize;
			nTotalVertCount = 0;

			if (!AfxGetGFXStreamMan().LockVertexBuffer(0, size_to_lock, &pVerts, 0))
			{
				m_pDevice->ClearVertexShader();
                m_pDevice->ClearPixelShader();
				PostRender(pSlot);
				return;
			}
			else
			{
#if defined _ANGELICA21 || defined _ANGELICA22
				AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, true);
#endif
			}
		}

		int nCurCount = pEle->FillVertexBuffer(pVerts, 0, pView);

		if (nCurCount > 0)
		{
			pVerts += nCurCount * pSlot->m_VertSize;
			nTotalVertCount += nCurCount;
		}

		if (nCurCount > 2)
			AfxGetGFXExMan()->AddElementPrimCount(pEle->GetEleTypeId(), nCurCount/2);
	}

	AfxGetGFXStreamMan().UnlockVertexBuffer();

	if (nTotalVertCount > 2)
	{
		AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, true);

		if (m_pGfx_TLDirectPass_Shader)
		{
#ifdef _ANGELICA21
			ApplyVertexShaderConsts(pView);

			if (m_bCurFogEnabled)
				m_pGfx_TLDirectPass_Shader->Appear();
			else
				m_pGfx_TLDirectPass_Shader_No_Fog->Appear();
#else

#ifdef _ANGELICA22
            if (m_bUseHLSL && m_pHLSL_TL) 
            {
                m_pTLDecl->Appear();
                ApplyHLSL(pView, m_pHLSL_TL, pSlot);
            }
            else
                m_pGfx_TLDirectPass_Shader->Appear();
#else
            m_pGfx_TLDirectPass_Shader->Appear();
#endif

            A3DVECTOR4 c0(2.0f / pView->GetParam()->Width, -2.0f / pView->GetParam()->Height, 1.0f, 0.0f);
			m_pDevice->SetVertexShaderConstants(0, &c0, 1);
			A3DVECTOR4 c1(static_cast<float>(pView->GetParam()->X)
				, static_cast<float>(pView->GetParam()->Y)
				, 0.0f
				, 0.0f);
			m_pDevice->SetVertexShaderConstants(1, &c1, 1);
#endif
		}
		m_pDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nTotalVertCount, 0, nTotalVertCount / 2);
		_gfx_draw_count++;
		AfxGetGFXExMan()->AddTotalPrimCount(nTotalVertCount / 2);
	}

#ifndef _ANGELICA21
	m_pDevice->ClearVertexShader();
    m_pDevice->ClearPixelShader();
#endif

	PostRender(pSlot);
}

void A3DGFXRenderSlotMan::RenderTriangleStripSoftware(A3DGFXRenderSlot* pSlot, A3DViewport* pView)
{
	PreRender(pSlot);

	BYTE* pVerts;
	const int _max_count = A3DGFXStreamMan::GetMaxVertexBufferSize() / pSlot->m_VertSize;
	const size_t count = pSlot->m_EleArr.size();

	for (size_t i = 0; i < count; i++)
	{
		A3DGFXElement* pEle = pSlot->m_EleArr[i];
		int nVertCount = pEle->GetVertsCount();

		if (nVertCount > _max_count)
			continue;

		int size_to_lock = nVertCount * pSlot->m_VertSize;

		if (!AfxGetGFXStreamMan().LockVertexBuffer(0, size_to_lock, &pVerts, 0))
		{
			PostRender(pSlot);
			return;
		}

		int nCurCount = pEle->FillVertexBuffer(pVerts, 0, pView);
		AfxGetGFXStreamMan().UnlockVertexBuffer();

		if (nCurCount > 2)
		{
			AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, false);
			m_pDevice->DrawPrimitive(A3DPT_TRIANGLESTRIP, 0, nCurCount - 2);
			_gfx_draw_count++;
			AfxGetGFXExMan()->AddTotalPrimCount(nCurCount - 2);
			AfxGetGFXExMan()->AddElementPrimCount(pEle->GetEleTypeId(), nCurCount - 2);
		}
	}

	PostRender(pSlot);
}

void A3DGFXRenderSlotMan::RenderTriangleListSoftware(A3DGFXRenderSlot* pSlot, A3DViewport* pView)
{
	PreRender(pSlot);

	BYTE* pVerts;
	const int _max_index = A3DGFXStreamMan::GetMaxIndexCount() * 4 / 6;
	const int _max_vert = A3DGFXStreamMan::GetMaxVertexBufferSize() / pSlot->m_VertSize;
	const int _max_count = min(_max_index, _max_vert);
	const size_t count = pSlot->m_EleArr.size();

	for (size_t i = 0; i < count; i++)
	{
		A3DGFXElement* pEle = pSlot->m_EleArr[i];
		int nVertCount = pEle->GetVertsCount();

		if (nVertCount > _max_count)
			continue;

		int size_to_lock = nVertCount * pSlot->m_VertSize;

		if (!AfxGetGFXStreamMan().LockVertexBuffer(0, size_to_lock, &pVerts, 0))
		{
			PostRender(pSlot);
			return;
		}

		int nCurCount = pEle->FillVertexBuffer(pVerts, 0, pView);
		AfxGetGFXStreamMan().UnlockVertexBuffer();

		if (nCurCount > 2)
		{
			AfxGetGFXStreamMan().Appear(pSlot->m_nVertType, pSlot->m_VertSize, true);
			m_pDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nCurCount, 0, nCurCount / 2);
			_gfx_draw_count++;
			AfxGetGFXExMan()->AddTotalPrimCount(nCurCount / 2);
			AfxGetGFXExMan()->AddElementPrimCount(pEle->GetEleTypeId(), nCurCount / 2);
		}
	}

	PostRender(pSlot);
}

void A3DGFXRenderSlotMan::ReloadTex()
{
	/*
	A3DTextureMan* pMan = m_pDevice->GetA3DEngine()->GetA3DTextureMan();
	A3DGFXRenderSlotMap::iterator it = m_RenderSlotMap.begin();

	for (; it != m_RenderSlotMap.end(); ++it)
	{
		A3DGFXRenderSlot* pSlot = it->second;

		if (pSlot->m_pTexture)
			m_pDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(pSlot->m_pTexture);

		pMan->LoadTextureFromFileInFolder(
			pSlot->m_strTexPath,
			m_pDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(),
			&pSlot->m_pTexture,
			pSlot->m_bNoDownSample ? A3DTF_NODOWNSAMPLE : A3DTF_MIPLEVEL,
			1);
	}
	*/
	m_pDevice->GetA3DEngine()->GetA3DTextureMan()->ReloadAllTextures(true);
}
