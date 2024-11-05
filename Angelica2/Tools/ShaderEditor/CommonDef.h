/*
 * FILE: CommonDef.h
 *
 * DESCRIPTION: Common definition
 *
 * CREATED BY: duyuxin, 2002/1/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _COMMONDEF_H_
#define _COMMONDEF_H_

#include "stdafx.h"
#include "Clipboard.h"
#include <AString.h>
#include <AArray.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Own defined message
#define WM_SELECTCHANGE		WM_USER + 100		//	New shader is selected
#define WM_ADDONESTAGE		WM_USER + 101		//	Add a stage

//	Number of strings in each string array
#define NUM_BLENDSTR		14
#define NUM_COMPSTR			9
#define NUM_FILTERSTR		7
#define NUM_TEXOPSTR		27
#define NUM_TEXTYPESTR		8
#define NUM_TEXADDRSTR		6
#define NUM_CULLSTR			4
#define NUM_TEXARGSTR		7
#define NUM_TEXMODSTR		3
#define NUM_WAVESTR			6
#define NUM_TEXFORMATSTR	28
#define NUM_RESULTARGSTR	3
#define NUM_TEXCOORDSTR		5
#define NUM_TEXTRANSSTR		6

#define LEN_SHADERNAME		64

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DShaer;
class A3DEngine;
class A3DCamera;
class A3DViewport;
class A3DTextureProxy;

struct SHADERINFO
{
	bool		bModified;			//	Modified flag
	char		szFile[MAX_PATH];	//	Shader file full name
	A3DShader*	pShader;			//	A3D shader object
};

//	IDR_MAINMENU states
struct MAINMENUUPDATE
{
	BYTE	bFile_New;		//	File->New
	BYTE	bFile_Open;		//	File->Open
	BYTE	bFile_SaveAll;	//	File->Save all
	BYTE	bFile_WorkDir;	//	File->Set work dir
	BYTE	bFile_BackCol;	//	File->Set background color
	BYTE	bFile_Exit;		//	File->Exit
};

//	IDR_GENERALMENU and IDR_GENPROPMENU states
struct GENERALMENUUPDATE
{
	//	IDR_GENERALMENU
	BYTE	bGen_Copy;
	BYTE	bGen_Paste;
	BYTE	bGen_ZCompare;
	BYTE	bGen_ZEnable;
	BYTE	bGen_ZWriteEnable;
	BYTE	bGen_AlphaCompare;
	BYTE	bGen_AlphaRef;
	BYTE	bGen_CullMode;
	BYTE	bGen_SrcBlend;
	BYTE	bGen_DestBlend;

	//	IDR_GENPROPMENU
	BYTE	bGenProp_Edit;
	BYTE	bGenProp_Remove;
};

//	IDR_SHADERMENU states
struct SHADERMENUUPDATE
{
	BYTE	bSdr_Cut;
	BYTE	bSdr_Copy;
	BYTE	bSdr_Paste;
	BYTE	bSdr_Rename;
	BYTE	bSdr_AddStage;
	BYTE	bSdr_Remove;
};

//	IDR_STAGEMENU, IDR_STGPROPMENU states
struct STAGEMENUUPDATE
{
	//	IDR_STAGEMENU
	BYTE	bStg_Hide;
	BYTE	bStg_Cut;
	BYTE	bStg_Copy;
	BYTE	bStg_Paste;
	BYTE	bStg_AddBefore;
	BYTE	bStg_AddAfter;
	BYTE	bStg_MoveUp;
	BYTE	bStg_MoveDown;
	BYTE	bStg_Texture;
	BYTE	bStg_ColorOp;
	BYTE	bStg_AlphaOp;
	BYTE	bStg_BumpMat;
	BYTE	bStg_BumpScale;
	BYTE	bStg_BumpOffset;
	BYTE	bStg_Remove;

	//	IDR_STGPROPMENU
	BYTE	bStgProp_Edit;
	BYTE	bStgProp_MoveUp;
	BYTE	bStgProp_MoveDown;
	BYTE	bStgProp_AddBefore;
	BYTE	bStgProp_AddAfter;
	BYTE	bStgProp_Remove;
};

//	Format strings
struct FORMATSTR
{
	A3DFORMAT	Fmt;
	const char*	szName;
};

//	D3D texture
struct D3DTEXTURE
{
	A3DTextureProxy*	pTexProxy;
	char				szFile[MAX_PATH];
};

extern A3DEngine*		g_pA3DEngine;
extern A3DCamera*		g_pA3DCamera;
extern A3DViewport*		g_pA3DViewport;

//	Strings
extern const char*		g_aBlendStrs[];
extern const char*		g_aCompStrs[];
extern const char*		g_aFilterStrs[];
extern const char*		g_aTexOpStrs[];
extern const char*		g_aTexTypeStrs[];
extern const char*		g_aTexAddrStrs[];
extern const char*		g_aCullStrs[];
extern const char*		g_aTexArgStrs[];
extern const char*		g_aTexModStrs[];
extern const char*		g_aWaveStrs[];
extern const FORMATSTR	g_aFormatStrs[];
extern const char*		g_aResultArgStrs[];
extern const char*		g_aTexCoordFlags[];
extern const char*		g_aTexTransFlags[];

//	Menu states
extern MAINMENUUPDATE		g_MainMenuUpdate;
extern GENERALMENUUPDATE	g_GenMenuUpdate;
extern SHADERMENUUPDATE		g_ShaderMenuUpdate;
extern STAGEMENUUPDATE		g_StageMenuUpdate;

extern char			g_szWorkDir[];		//	Work directory
extern char			g_szIniFile[];		//	.ini file name
extern bool			g_bActive;			//	App active flag
extern CClipboard	g_Clipboard;		//	Clipboard

extern AArray<D3DTEXTURE, D3DTEXTURE&> g_aD3DTexs;	//	D3D textures

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

int ModFlagToIndex(DWORD dwMod);
void AnimTextureFileName(char* szFileName);
int glb_SearchD3DTexSlot(A3DTextureProxy* pTexProxy);


#endif	//	_COMMONDEF_H_

