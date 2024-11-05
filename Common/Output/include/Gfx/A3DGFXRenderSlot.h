#ifndef A3DGFXRENDERSLOT_H_
#define A3DGFXRENDERSLOT_H_

#include "A3DTypes.h"
#include "AString.h"
#include "hashmap.h"
#include "A3DDevice.h"
#include "A3DGFXElement.h"

class A3DGFXElement;
class A3DDevice;
class A3DTexture;
class A3DViewport;
class A3DVertexShader;
class A3DPixelShader;
class A3DVertexDecl;

#ifdef _ANGELICA22
class A3DHLSL;
#endif

enum GFX_RENDER_MODE
{
	GRMSoftware,
	GRMVertexShader,
	GRMBlendMatrix
};

enum GFX_FOG_FLAG
{
	GFX_FOG_CURRENT,
	GFX_FOG_ZERO,
};

#ifdef _ANGELICA21
#define GFX_VS_CONST_BASE		16
#else
#define GFX_VS_CONST_BASE		10
#endif

#define GFX_MODEL_PRIM_TYPE		0x1000

struct A3DGFXRenderSlot
{
	int					m_nRenderLayer;	
	int					m_nVertType;
	size_t				m_VertSize;
	int					m_PrimType;
	AString				m_strTexPath;
	bool				m_bNoDownSample;
	A3DSHADER			m_Shader;
	bool				m_bTileMode;
	bool				m_bZTestEnable;
	int					m_nRef;
	int					m_nTotalVertCount;
	abase::vector<A3DGFXElement*> m_EleArr;
	A3DTexture*			m_pTexture;
	bool				m_bPixelShader;
	A3DPixelShader*		m_pDefaultShader;
	GFX_FOG_FLAG		m_enumFogFlag;
};

typedef abase::hash_map<DWORD, A3DGFXRenderSlot*> A3DGFXRenderSlotMap;
typedef abase::vector<A3DGFXRenderSlot*> A3DGFXRenderSlotArray;

class A3DGFXRenderSlotMan
{
public:

	A3DGFXRenderSlotMan() :
	m_RenderSlotMap(512),
	m_dwCurIndex(1),
	m_pGFX_Pallete_Shader(NULL),
	m_pGFX_Pallete_Shader_No_Fog(NULL),
	m_pGfx_TLDirectPass_Shader(NULL),
	m_pGfx_TLDirectPass_Shader_No_Fog(NULL),
	m_pDefaultPS(NULL),
	m_pDefaultPS_Alpha(NULL),
	m_pDefaultPS_Hilight(NULL),
	m_pDefaultPS_S_Hilight(NULL),
	m_pDefaultPS_NoTex(NULL),
	m_pDevice(0),
	m_nMaxMatrixCount(0),
	m_bAlphaTestEnable(false),
	m_bFogEnable(false),
	m_bCurFogEnabled(false),
	m_clCurFog(0),
	m_bUserClip(false)
	{
#if _ANGELICA22
        m_pHLSL = NULL;
        m_pHLSL_TL = NULL;
        m_pTLDecl = NULL;
        m_bUseHLSL = false;
#endif

		::InitializeCriticalSection(&m_cs);

		for (int i = 0; i < GFX_RENDER_LAYER_COUNT; i++)
			m_DelayedSlots[i].reserve(128);
	}

	~A3DGFXRenderSlotMan() { ::DeleteCriticalSection(&m_cs); }

protected:

	A3DGFXRenderSlotMap m_RenderSlotMap;
	A3DGFXRenderSlotArray m_DelayedSlots[GFX_RENDER_LAYER_COUNT];
	DWORD m_dwCurIndex;
	A3DDevice* m_pDevice;
	A3DVertexShader * m_pGFX_Pallete_Shader;
	A3DVertexShader * m_pGFX_Pallete_Shader_No_Fog;
	A3DVertexShader * m_pGfx_TLDirectPass_Shader;
	A3DVertexShader * m_pGfx_TLDirectPass_Shader_No_Fog;
	A3DPixelShader * m_pDefaultPS;
	A3DPixelShader * m_pDefaultPS_Alpha;
	A3DPixelShader * m_pDefaultPS_Hilight;
	A3DPixelShader * m_pDefaultPS_S_Hilight;
	A3DPixelShader * m_pDefaultPS_NoTex;

#ifdef _ANGELICA22
    A3DHLSL* m_pHLSL;
    A3DHLSL* m_pHLSL_TL;
    bool m_bUseHLSL;
    A3DVertexDecl* m_pTLDecl;
#endif

	CRITICAL_SECTION m_cs;
	int m_nMaxMatrixCount;
	bool m_bAlphaTestEnable;
	bool m_bFogEnable;
	bool m_bCurFogEnabled;
	A3DCOLOR m_clCurFog;

	bool		m_bUserClip;		//	flag indicates whether user clip plane has been enabled
	D3DXPLANE	m_cp;				//	user clip plane in world space
	D3DXPLANE	m_hcp;				//	user clip plane in homogeneous space

public:

	static GFX_RENDER_MODE g_RenderMode;

protected:

	void RenderOneSlot(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderTriangleStrip(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderTriangleList(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderTLTriangleList(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderTriangleStripSoftware(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderTriangleListSoftware(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderSkinModel(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderPointList(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void PreRender(A3DGFXRenderSlot* pSlot);
	void PostRender(A3DGFXRenderSlot* pSlot);
	void Lock() { ::EnterCriticalSection(&m_cs); }
	void Unlock() { ::LeaveCriticalSection(&m_cs); }
	void ApplyVertexShaderConsts(A3DViewport* pView);
#ifdef _ANGELICA22
    void ApplyHLSL(A3DViewport* pView, A3DHLSL* pHLSL, A3DGFXRenderSlot* pSlot);
#endif

	void RenderDelayedSlots(int nLayer, A3DViewport* pView)
	{
		A3DGFXRenderSlotArray& arr = m_DelayedSlots[nLayer];

		for (size_t i = 0; i < arr.size(); i++)
			RenderOneSlot(arr[i], pView);

		arr.clear();
	}

public:

	void Init(A3DDevice* pDevice);
	void Release();
	DWORD RegisterSlot(int nRenderLayer, int nVertType, size_t VertSize, int PrimType, const AString& strTexPath, const A3DSHADER& sh, bool bTileMode, bool bNoDownSample, bool bZTestEnable, bool bPixelShader);
	void UnregisterSlot(DWORD dwIndex);
	void RegisterEleForRender(DWORD dwIndex, A3DGFXElement* pEle);
	void Render(A3DViewport* pView);
	bool IsAlphaTestEnable() const { return m_bAlphaTestEnable; }
	void SetAlphaTestEnable(bool b) { m_bAlphaTestEnable = b; }
	bool IsFogEnable() const { return m_bFogEnable; }
	void SetFogEnable(bool b) { m_bFogEnable = b; }

	// for editor use
	void ReloadTex();
};

#endif
