// Renderable2DObject.cpp: implementation of the CRenderable2DObject class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
#include "Render.h"
#include "Renderable2DObject.h"
#include <A2DSprite.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderable2DObject::CRenderable2DObject()
{
	m_pSprite = 0;
	m_fScale = 1.0f;
	m_nObjectType = RENDERABLE_OBJECT_2D;
}

CRenderable2DObject::~CRenderable2DObject()
{
}

bool CRenderable2DObject::Load(CString& path)
{
	if(m_pSprite==0) m_pSprite = new A2DSprite;
	if(m_pSprite==0) return false;

	AString filepath = AC2AS(path);
	m_pSprite->Init(CRender::GetInstance()->GetA3DDevice(),filepath,A3DCOLORRGBA(0,0,0,0));
	return true;
}

void CRenderable2DObject::Render(A3DViewport *pA3dViewPort)
{
	if(m_pSprite)
	{
		m_pSprite->DrawToBack();	
	}
}

void CRenderable2DObject::MoveObject( int x , int y)
{
	APointF pt = m_pSprite->GetPosition();
	m_pSprite->SetPosition(pt.x + x, pt.y + y);
}

void CRenderable2DObject::SetPos(int x, int y)
{
	m_pSprite->SetPosition(x, y);
}

void CRenderable2DObject::GetPos(int& x, int& y)
{
	APointF pt = m_pSprite->GetPosition();
	x = pt.x; y = pt.y;
}

void CRenderable2DObject::ScaleObject( float s)
{
	m_fScale *=s;
	m_pSprite->SetScaleX(m_fScale);
	m_pSprite->SetScaleY(m_fScale);
}

void CRenderable2DObject::ResetScale()
{
	m_fScale = 1.0f;
	m_pSprite->SetScaleX(m_fScale);
	m_pSprite->SetScaleY(m_fScale);
}

void CRenderable2DObject::Release()
{
	if(m_pSprite) 
	{
		m_pSprite->Release();
		delete m_pSprite;
	}
}

int CRenderable2DObject::GetWidth()
{
	if(m_pSprite) return (int)(m_pSprite->GetWidth()*m_fScale);
	return 0;
}

int CRenderable2DObject::GetHeight()
{
	if(m_pSprite) return (int)(m_pSprite->GetHeight()*m_fScale);
	return 0;
}

#endif