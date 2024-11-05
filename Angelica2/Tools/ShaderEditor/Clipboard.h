/*
 * FILE: Clipboard.h
 *
 * DESCRIPTION: clipboard routines
 *
 * CREATED BY: duyuxin, 2002/2/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

#include <A3DShader.h>
#include <AString.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Type of data in clipboard
#define CBDT_NONE			0		//	None data
#define CBDT_GENERAL		1		//	Whole general properties
#define CBDT_GENPROP		2		//	Just one property of general
#define CBDT_STAGE			3		//	One whole stage
#define CBDT_STGPROP		4		//	Just one property of stage
#define CBDT_SHADER			5		//	Whole shader data

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	CClipboard
//
///////////////////////////////////////////////////////////////////////////

class CClipboard
{
public:		//	Types

	//	Texture info of each stage
	struct TEXINFO
	{
		AString		strFile;
		int			iD3DSlot;
	};

public:		//	Constructors and Destructors

	CClipboard();
	virtual ~CClipboard();

public:		//	Attributes

public:		//	Operations

	bool		SetData(int iType, int idData, void* pData);	//	Set data
	bool		GetData(int iType, int idData, void* pData);	//	Get data

	bool		MatchType(int iType)	{	return (iType == m_iDataType) ? true : false;	}
	int			GetStageNum()			{	return m_iNumStage;		}

protected:	//	Attributes

	int			m_iDataType;		//	Data type
	int			m_idData;			//	Specific data ID

	AString			m_strName;		//	Name
	SHADERSTAGE		m_aStages[A3DShader::MAXNUM_STAGE];		//	Stages
	SHADERGENERAL	m_General;		//	General properties

	int			m_iNumStage;		//	Number of stages
	AString		m_strPixelShader;	//	Pixel shader file name

	TEXINFO		m_aTexInfos[A3DShader::MAXNUM_STAGE];		//	Texture files for each stage

protected:	//	Operations

	//	Clear texture info
	void ClearTextureInfo();
};



#endif	//	_CLIPBOARD_H_


