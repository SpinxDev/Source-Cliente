/*
 * FILE: A3DVertexShader.h
 *
 * DESCRIPTION: Routines for vertex shader
 *
 * CREATED BY: duyuxin, 2003/10/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DVERTEXSHADER_H_
#define _A3DVERTEXSHADER_H_

#include "A3DVertex.h"
#include "A3DDevObject.h"
#include "AString.h"
#include <D3dx8core.h>

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

class A3DDevice;
	
///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DVertexShader
//
///////////////////////////////////////////////////////////////////////////

class A3DVertexShader : public A3DDevObject
{
public:		//	Types

	friend class A3DShaderMan;

public:		//	Constructors and Destructors

	A3DVertexShader();
	virtual ~A3DVertexShader();

public:		//	Attributes

	static A3DVertexShader*	m_pCurShader;	//	Current applied vertex shader

public:		//	Operations

	//	Initialize object
	bool Init(A3DDevice* pDevice);
	//	Release object
	void Release();	

	//	Load vertx shader from file
	bool Load(const char* szFile, bool bBinary, DWORD* pdwDecl);

	//	Apply this vertex shader
	bool Appear();
	//	Abolish vertex shader
	bool Disappear();

	//	Before device reset
	virtual bool BeforeDeviceReset() { return true; }
	//	After device reset
	virtual bool AfterDeviceReset() { return true; }

	//	Get vertex shader file name
	const char* GetFileName() { return m_strFileName; }
	//	Get vertex shader ID
	DWORD GetShaderID() { return m_dwShaderID; }

protected:	//	Attributes

	A3DDevice*		m_pA3DDevice;			//	Device pointer
	AString			m_strFileName;			//	File name relative to shader path
	LPD3DXBUFFER	m_pD3DXShaderBuf;		//	Vertex shader code buffer
	DWORD			m_hD3DShader;			//	D3D vertex shader handle
	DWORD			m_dwShaderID;			//	Vertex shader ID
	DWORD			m_dwPosInMan;			//	Position in manager

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DVERTEXSHADER_H_


