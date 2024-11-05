// RenderableObject.h: interface for the CRenderableObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERABLEOBJECT_H__0A8E6F2E_348E_4353_8573_7D502AEEFEF5__INCLUDED_)
#define AFX_RENDERABLEOBJECT_H__0A8E6F2E_348E_4353_8573_7D502AEEFEF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vector.h"
#include <a3dtypes.h>
#include <astring.h>

class A3DViewport;
class A3DSkin;
class A3DMATRIX4;


enum RENDERABLE_OBJECT_TYPE
{
	RENDERABLE_OBJECT_2D = 0,
	RENDERABLE_OBJECT_3D,
	RENDERABLE_OBJECT_NUM,
};

class CRenderableObject  
{
public:
	CRenderableObject();
	virtual ~CRenderableObject();

	virtual void Rotate(A3DVECTOR3& vDelta){m_vRotate += vDelta; }
	virtual A3DMATRIX4 GetAbsoluteTM() = 0;
	virtual void Render(A3DViewport *pA3dViewPort) = 0;
	virtual void Tick(DWORD dwTimeDelta) = 0;
	virtual void ResetScale() = 0;
	virtual bool Load(CString& path) = 0;
	virtual void Release();
	virtual RENDERABLE_OBJECT_TYPE GetObjectType(){ return m_nObjectType; }

protected:
	RENDERABLE_OBJECT_TYPE m_nObjectType;
	A3DVECTOR3 m_vRotate;

	A3DVECTOR3 m_vPos;
	A3DMATRIX4 m_matRotate;
};

#endif // !defined(AFX_RENDERABLEOBJECT_H__0A8E6F2E_348E_4353_8573_7D502AEEFEF5__INCLUDED_)
