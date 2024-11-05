/*
 * FILE: Decal.h
 *
 * DESCRIPTION: A simple 2D decal using shader
 *
 * CREATED BY: duyuxin, 2002/2/5
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DECAL_H_
#define _DECAL_H_

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
class A3DStream;
class A3DShader;
class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	CDecal
//
///////////////////////////////////////////////////////////////////////////

class CDecal
{
public:		//	Type

public:		//	Constructors and Destructors

	CDecal();
	virtual ~CDecal();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pDevice);	//	Initialize object
	void		Release();					//	Release object

	bool		Render(A3DViewport* pView);	//	Render routine
	bool		RenderAsBackground(A3DViewport* pView);		//	Render as background

	void		SetTexCoor(float u, float v)		{	m_fTexU = u; m_fTexV = v;		}
	void		GetTexCoor(float* pu, float* pv)	{	*pu = m_fTexU; *pv = m_fTexV;	}
	void		SetTexSize(float fWid, float fHei)	{	m_fTexWid = fWid; m_fTexHei = fHei;	}
	void		GetTexSize(float* pfWid, float* pfHei)	{	*pfWid = m_fTexWid;	*pfHei = m_fTexHei;		}
	void		SetColor(DWORD dwCol)				{	m_dwCol = dwCol;		}
	DWORD		GetColor()							{	return m_dwCol;			}
	void		SetShader(A3DShader* pShader)		{	m_pShader = pShader;	}

protected:	//	Attributes

	A3DDevice*	m_pA3DDevice;		//	A3D device object
	A3DStream*	m_pA3DStream;		//	A3D stream object, used when m_bOptimized = false

	A3DShader*	m_pShader;			//	Shader associated with this decal

	float		m_fTexU;			//	Texture coordinates
	float		m_fTexV;
	float		m_fTexWid;			//	Texture width
	float		m_fTexHei;

	DWORD		m_dwCol;			//	Color

protected:	//	Operations

};


#endif	//	_DECAL_H_

