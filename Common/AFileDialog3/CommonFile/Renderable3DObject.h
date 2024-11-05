// Renderable3DObject.h: interface for the Renderable3DObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERABLE3DOBJECT_H__6351672C_8330_40DB_8A83_B2F86F6BB62A__INCLUDED_)
#define AFX_RENDERABLE3DOBJECT_H__6351672C_8330_40DB_8A83_B2F86F6BB62A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RenderableObject.h"


class A3DGFXEx;
class CECModel;
class CELBuilding;

class CRenderable3DObject :public CRenderableObject  
{
public:
	CRenderable3DObject();
	virtual ~CRenderable3DObject();
public:
	virtual A3DMATRIX4 GetAbsoluteTM();
	virtual void Render(A3DViewport *pA3dViewPort);
	virtual void Tick(DWORD dwTimeDelta);
	virtual void ResetScale();
	virtual void Release();
	
	A3DVECTOR3 GetDefaultCamaraPos();
	virtual bool Load(CString& path);
	
	void GetAABB(A3DAABB *aabb);
	A3DVECTOR3 GetPos();

private:

	A3DGFXEx*      m_pGfxEx;
	CECModel*      m_pECModel;
	//CELBuilding*   m_pELBuilding;
};

#endif // !defined(AFX_RENDERABLE3DOBJECT_H__6351672C_8330_40DB_8A83_B2F86F6BB62A__INCLUDED_)
