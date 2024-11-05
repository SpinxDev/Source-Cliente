// RenderableObject.cpp: implementation of the CRenderableObject class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
#include "RenderableObject.h"


#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderableObject::CRenderableObject()
{
	m_vRotate = A3DVECTOR3(0,0,0);
}

CRenderableObject::~CRenderableObject()
{

}

void CRenderableObject::Release()
{
}

#endif