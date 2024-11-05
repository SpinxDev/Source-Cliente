#include "StdAfx.h"
#include "A3DGFXSurfaceRender.h"
#include "A3DStream.h"
#include "A3DOrthoCamera.h"
#include "A3DViewport.h"
#include "A3DGFXExMan.h"

A3DGFXSurfaceRender::A3DGFXSurfaceRender()
{
	m_bInit		= false;
	m_pViewport	= 0;
	m_pCamera	= 0;
	m_pStream	= 0;
}

A3DGFXSurfaceRender::~A3DGFXSurfaceRender()
{
}

bool A3DGFXSurfaceRender::Init()
{
	A3DDevice* pDevice = AfxGetA3DDevice();
	m_pCamera = new A3DOrthoCamera();

	if (!m_pCamera->Init(pDevice, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f))
		return false;

	RECT rc;
	::GetClientRect(pDevice->GetDeviceWnd(), &rc);
	DWORD w = (rc.right - rc.left);
	DWORD h = (rc.bottom - rc.top);

	if (!pDevice->CreateViewport(&m_pViewport, 0, 0, w, h, 0.0f, 1.0f, true, true, 0))
		return false;

	m_pViewport->SetCamera(m_pCamera);
	m_pCamera->SetDirAndUp(-_unit_y, _unit_z);
	m_pStream = new A3DStream;

	if (!m_pStream->Init(pDevice, sizeof(GFX_SURFACEVERTEX), GFXSURFACEVERT_FVF, _max_verts, _max_indices, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR))
		return false;

	m_bInit = true;
	return true;
}

void A3DGFXSurfaceRender::Release()
{
	m_bInit = false;
	A3DRELEASE(m_pViewport);
	A3DRELEASE(m_pCamera);
	A3DRELEASE(m_pStream);
}

bool A3DGFXSurfaceRender::SetupSteam(A3DVECTOR3* pVerts, int nVertCount, WORD* pIndices, int nIndexCount, A3DCOLOR diffuse)
{
	GFX_SURFACEVERTEX* pv;
	WORD* pi;

	if (!m_pStream->LockVertexBuffer(0, 0, (BYTE**)&pv, 0))
		return false;

	int i;

	for (i = 0; i < nVertCount; i++)
	{
		GFX_SURFACEVERTEX& vert = pv[i];
		vert.pos = pVerts[i];
		vert.pos.y += .02f;
		vert.diffuse = diffuse;
	}

	m_pStream->UnlockVertexBuffer();

	if (!m_pStream->LockIndexBuffer(0, 0, (BYTE**)&pi, 0))
		return false;

	for (i = 0; i < nIndexCount; i++)
		pi[i] = pIndices[i];

	m_pStream->UnlockIndexBuffer();
	return true;
}

bool A3DGFXSurfaceRender::Render(
	A3DViewport* pViewport,
	A3DVECTOR3* pVerts,
	int nVertCount,
	WORD* pIndices,
	int nIndexCount,
	const A3DVECTOR3& vCenter,
	float fHalfWidth,
	float fHalfHeight,
	float fRadius,
	A3DTexture* pTex,
	A3DBLEND SrcBlend,
	A3DBLEND DestBlend,
	A3DCOLOR diffuse,
	const A3DVECTOR3& vDir,
	bool bUReverse, 
	bool bVReverse, 
	bool bUVInterChange)
{
	if (!m_bInit)
		return false;

	if (!SetupSteam(pVerts, nVertCount, pIndices, nIndexCount, diffuse))
		return false;

	A3DMATRIX4 matProjectedView;
	A3DMATRIX4 matScale = a3d_IdentityMatrix();
	// 默认以前都为False
	matScale._11 = bUReverse ? -0.5f : 0.5f;	// 这里原本就是0.5f, 而_22则原本是-0.5f，导致了UV两种Reverse的解释不一致了
	matScale._22 = bVReverse ? 0.5f : -0.5f;	// 原本这里就是-0.5f，那么当加入对VReverse的支持时，只能把0.5作为reverse了
	matScale._41 = 0.5f;
	matScale._42 = 0.5f;

	if (bUVInterChange)
	{
		A3DMATRIX4 matInterChangeUV = a3d_ZeroMatrix();
		matInterChangeUV._12 = 1.0f;
		matInterChangeUV._21 = 1.0f;
		matInterChangeUV._33 = 1.0f;
		matInterChangeUV._44 = 1.0f;
		matScale = matScale * matInterChangeUV;
	}

	A3DCameraBase * pHostCamera = pViewport->GetCamera();
	A3DDevice* pA3DDevice = AfxGetA3DDevice();
	m_pCamera->SetProjectionParam(-fHalfWidth, fHalfWidth, -fHalfHeight, fHalfHeight, -100.0f, 100.0f);
	m_pCamera->SetPos(vCenter + A3DVECTOR3(0, fRadius * 2.0f, 0));
	m_pCamera->SetDirAndUp(-_unit_y, vDir);

	pViewport->Active();
	pA3DDevice->SetLightingEnable(false);
	pA3DDevice->SetFogEnable(false);
	pA3DDevice->SetFaceCull(A3DCULL_NONE);
	pA3DDevice->SetZWriteEnable(false);
	pA3DDevice->SetWorldMatrix(_identity_mat4);
	pA3DDevice->SetSourceAlpha(SrcBlend);
	pA3DDevice->SetDestAlpha(DestBlend);

	if (AfxGetGFXExMan()->UsingZBias())
		pA3DDevice->SetZBias(3);

	m_pStream->Appear(0);

	if (pTex)
		pTex->Appear();

	matProjectedView = InverseTM(pHostCamera->GetViewTM()) * m_pCamera->GetVPTM() * matScale;
	pA3DDevice->SetTextureCoordIndex(0, D3DTSS_TCI_CAMERASPACEPOSITION);
	pA3DDevice->SetTextureTransformFlags(0, (A3DTEXTURETRANSFLAGS)(A3DTTFF_COUNT4 | A3DTTFF_PROJECTED));
	pA3DDevice->SetTextureMatrix(0, matProjectedView);
	pA3DDevice->SetTextureAddress(0, A3DTADDR_BORDER, A3DTADDR_BORDER);

#ifdef _ANGELICA21
	pA3DDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);
#elif defined(_ANGELICA22)
	pA3DDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);
#else
	pA3DDevice->SetDeviceTextureStageState(0, D3DTSS_BORDERCOLOR, 0);
#endif

	pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVertCount, 0, nIndexCount / 3);
	pA3DDevice->SetZWriteEnable(true);
	pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	pA3DDevice->ClearTexture(0);
	pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	pA3DDevice->SetTextureCoordIndex(0, 0);
	pA3DDevice->SetFaceCull(A3DCULL_CCW);
	pA3DDevice->SetFogEnable(true);
	pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	if (AfxGetGFXExMan()->UsingZBias())
		pA3DDevice->SetZBias(0);

	return true;
}
