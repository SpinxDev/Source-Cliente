/*
 * FILE: SceneFrame.h
 *
 * DESCRIPTION: Frame scene class
 *
 * CREATED BY: Duyuxin, 2003/7/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_SCENEFRAME_H_
#define _SCENEFRAME_H_

#include "A3DVertex.h"
#include "A3DDevObject.h"
#include "AArray.h"

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
class A3DIBLScene;
class A3DStream;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CFrameScene
//
///////////////////////////////////////////////////////////////////////////

class CFrameScene : public A3DDevObject
{
public:		//	Types

	enum
	{
		STREAM_SIZE	= 30000,
	};

	struct STREAM
	{
		A3DStream*	pA3DStream;
		int			iNumRdVert;
	};

public:		//	Constructor and Destructor

	CFrameScene();
	virtual ~CFrameScene() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize viewport
	bool Init(CRender* pRender);
	//	Release viewport
	void Release();

	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();

	//	Load scene
	bool Load(A3DIBLScene* pScene);
	//	Render viewport
	bool Render();

	//	Update visible set
	bool UpdateVisibleSets();

	//	Set visible surfaces
	void SetVisibleSurfs(int* aVisSurfs, int iNumSurf)
	{
		m_aVisSurfs		= aVisSurfs;
		m_iNumVisSurf	= iNumSurf;
	}

	//	Force to render all surfaces
	void ForceToRenderAll(bool bTrue) { m_bRenderAll = bTrue; }
	//	Get force render flag
	bool GetRenderAllFlag() { return m_bRenderAll; }
	//	Re-fill stream
	void RefillStream() { m_bFillStream = true; }

protected:	//	Attributes

	CRender*	m_pRender;			//	Render object

	A3DLVERTEX*	m_aVerts;			//	Face vertices
	int			m_iNumFace;			//	Number of total face
	int*		m_aVisSurfs;		//	Visible surface array
	int			m_iNumVisSurf;		//	Number of visible surface
	bool		m_bRenderAll;		//	true, force to render all surfaces
	bool		m_bFillStream;		//	Re-fill stream flags
	DWORD		m_dwUnmanagedPos;	//	Position in unmanaged resource list

	AArray<STREAM, STREAM>	m_aStreams;

protected:	//	Operations

	//	Release scene data
	void ReleaseSceneData();
	//	Create streams
	bool CreateStreams();
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_SCENEFRAME_H_
