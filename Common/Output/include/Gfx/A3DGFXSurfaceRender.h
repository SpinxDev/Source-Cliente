#ifndef _A3DGFXSURFACERENDER_H_
#define _A3DGFXSURFACERENDER_H_

#include "A3DVector.h"
#include "A3DTypes.h"
#include "vector.h"

const int _max_verts	= 4096;
const int _max_indices	= 4096 * 3;

class A3DViewport;
class A3DOrthoCamera;
class A3DStream;
class A3DTexture;

#define GFXSURFACEVERT_FVF	D3DFVF_XYZ | D3DFVF_DIFFUSE

struct GFX_SURFACEVERTEX
{
	A3DVECTOR3 pos;
	A3DCOLOR diffuse;
};

class A3DGFXSurfaceRender
{
public:

	A3DGFXSurfaceRender();
	~A3DGFXSurfaceRender();

private:

	bool						m_bInit;
	A3DViewport*				m_pViewport;
	A3DOrthoCamera*				m_pCamera;
	A3DStream*					m_pStream;

protected:

	bool SetupSteam(A3DVECTOR3* pVerts, int nVertCount, WORD* pIndices, int nIndexCount, A3DCOLOR diffuse);

public:

	bool Init();
	void Release();
	bool Render(A3DViewport* pViewport,
		A3DVECTOR3* pVerts, int nVertCount, WORD* pIndices, int nIndexCount,
		const A3DVECTOR3& vCenter, float fHalfWidth, float fHalfHeight, float fRadius, 
		A3DTexture* pTex, A3DBLEND SrcBlend, A3DBLEND DestBlend, A3DCOLOR diffuse, const A3DVECTOR3& vDir,
		bool bUReverse, bool bVReverse, bool bUVInterChange);
};

#endif
