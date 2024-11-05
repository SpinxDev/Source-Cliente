/*
 * FILE: SceneTexture.h
 *
 * DESCRIPTION: Texture scene class
 *
 * CREATED BY: Duyuxin, 2003/7/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_SCENETEXTURE_H_
#define _SCENETEXTURE_H_

#include "A3DIBLScene.h"
#include "A3DPortal.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class CRender;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CTextureScene
//
///////////////////////////////////////////////////////////////////////////

class CTextureScene : public A3DIBLScene
{
public:		//	Types

	//	Render mode
	enum
	{
		RENDER_ALL = 0,
		RENDER_PORTAL,
		RENDER_SELECT,
	};

public:		//	Constructor and Destructor

	CTextureScene();
	virtual ~CTextureScene() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release objects
	virtual bool Release();

	//	Update visible set
	virtual	bool UpdateVisibleSets(A3DViewport* pCurrentViewport);

	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();

	//	Load portal file
	bool LoadPortalFile(const char* szFile);

	//	Set visible surfaces
	void SetVisibleSurfs(int* aVisSurfs, int iNumSurf)
	{
		m_aVisSurfs1	= aVisSurfs;
		m_iNumVisSurf1	= iNumSurf;
	}

	//	Set/Get render mode
	void SetRenderMode(int iMode);
	//	Get force render flag
	int GetRenderMode() { return m_iRenderMode; }
	//	Get portal interface
	A3DPortal* GetPortal() { return m_pPortal; }
	//	Re-fill stream
	void RefillStream() { m_bFillStream = true; }

protected:	//	Attributes

	A3DPortal*	m_pPortal;

	int*	m_aVisSurfs1;		//	Visible surface array
	int		m_iNumVisSurf1;		//	Number of visible surface
	int		m_iRenderMode;		//	Render mode
	bool	m_bFillStream;		//	Re-fill stream flags

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_SCENETEXTURE_H_
