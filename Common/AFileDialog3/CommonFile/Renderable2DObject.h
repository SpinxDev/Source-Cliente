// Renderable2DObject.h: interface for the CRenderable2DObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERABLE2DOBJECT_H__97D5AA39_0A47_4127_A941_96567C18A33F__INCLUDED_)
#define AFX_RENDERABLE2DOBJECT_H__97D5AA39_0A47_4127_A941_96567C18A33F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RenderableObject.h"
class A2DSprite;

class CRenderable2DObject : public CRenderableObject 
{
public:
	CRenderable2DObject();
	virtual ~CRenderable2DObject();
public:
	virtual A3DMATRIX4 GetAbsoluteTM()
	{
		A3DMATRIX4 mat;
		return mat;
	};
	virtual void Render(A3DViewport *pA3dViewPort);
	virtual void Tick(DWORD dwTimeDelta){};
	virtual void Release();
	

	virtual bool Load(CString& path);
	void MoveObject( int x , int y);
	void SetPos(int x, int y);
	void GetPos(int& x, int& y);
	void ScaleObject( float s);
	virtual void ResetScale();
	int  GetWidth();
	int  GetHeight();
private:
	A2DSprite*					m_pSprite;
	float                       m_fScale;
};

#endif // !defined(AFX_RENDERABLE2DOBJECT_H__97D5AA39_0A47_4127_A941_96567C18A33F__INCLUDED_)
