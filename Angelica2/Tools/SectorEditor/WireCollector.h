/*
 * FILE: WireCollector.h
 *
 * DESCRIPTION: Wire collector class
 *
 * CREATED BY: Duyuxin, 2003/7/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_WIRECOLLECTOR_H_
#define _WIRECOLLECTOR_H_

#include "A3DTypes.h"
#include "A3DVertex.h"

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
class A3DStream;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CWireCollector
//
///////////////////////////////////////////////////////////////////////////

class CWireCollector
{
public:		//	Types

	enum
	{
		SIZE_VERTEXBUF	= 4096,
		SIZE_INDEXBUF	= 16384,
	};

public:		//	Constructor and Destructor

	CWireCollector();
	virtual ~CWireCollector() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize wire collector
	bool Init(CRender* pRender);
	//	Release wire collector
	void Release();

	//	Add vertex and index to render buffer
	bool AddRenderData(A3DVECTOR3* aVerts, int iNumVert, WORD* aIndices, int iNumIdx, DWORD dwCol);
	//	Flush sector
	bool Flush();
	//	Reset buffer
	void ResetBuffer();

protected:	//	Attributes

	CRender*	m_pRender;		//	Render object
	A3DStream*	m_pStream;
	A3DLVERTEX*	m_aVertBuf;		//	Locked vertex buffer
	WORD*		m_aIdxBuf;		//	Locked index buffer
	int			m_iVertCnt;		//	Vertex fill counter
	int			m_iIdxCnt;		//	Index fill counter

protected:	//	Operations

	//	Lock/Unlock stream
	bool LockStream(bool bLock);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_WIRECOLLECTOR_H_
