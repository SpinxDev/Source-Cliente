/*
 * FILE: CommonDef.h
 *
 * DESCRIPTION: Common definition
 *
 * CREATED BY: duyuxin, 2002/2/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "CommonDef.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	Strings
const char* g_aBlendStrs[] = 
{
	"Blend operations",
	"BLEND_ZERO",  
	"BLEND_ONE",
	"BLEND_SRCCOLOR",
	"BLEND_INVSRCCOLOR",
	"BLEND_SRCALPHA",
	"BLEND_INVSRCALPHA",
	"BLEND_DESTALPHA",
	"BLEND_INVDESTALPHA",
	"BLEND_DESTCOLOR",
	"BLEND_INVDESTCOLOR",
	"BLEND_SRCALPHASAT",
	"BLEND_BOTHSRCALPHA",
	"BLEND_BOTHINVSRCALPHA"
};

const char* g_aCompStrs[] = 
{
	"Compare functions",
	"CMP_NEVER",
	"CMP_LESS",
	"CMP_EQUAL",
	"CMP_LESSEQUAL",
	"CMP_GREATER",
	"CMP_NOTEQUAL",
	"CMP_GREATEREQUAL",
	"CMP_ALWAYS"
};

const char* g_aFilterStrs[] = 
{
	"Texture filters",
	"TEXF_NONE",
	"TEXF_POINT",
	"TEXF_LINEAR",
	"TEXF_ANISOTROPIC",
	"TEXF_FLATCUBIC",
	"TEXF_GAUSSIANCUBIC"
};

const char* g_aTexOpStrs[] = 
{
	"Texture operations",
	"TOP_DISABLE",
	"TOP_SELECTARG1",
	"TOP_SELECTARG2",
	"TOP_MODULATE",
	"TOP_MODULATE2X",
	"TOP_MODULATE4X",
	"TOP_ADD",
	"TOP_ADDSIGNED",
	"TOP_ADDSIGNED2X",
	"TOP_SUBTRACT",
	"TOP_ADDSMOOTH",
	"TOP_BLENDDIFFUSEALPHA",
	"TOP_BLENDTEXTUREALPHA",
	"TOP_BLENDFACTORALPHA",
	"TOP_BLENDTEXTUREALPHAPM",
	"TOP_BLENDCURRENTALPHA",
	"TOP_PREMODULATE",
	"TOP_MODULATEALPHA_ADDCOLOR",
	"TOP_MODULATECOLOR_ADDALPHA",
	"TOP_MODULATEINVALPHA_ADDCOLOR",
	"TOP_MODULATEINVCOLOR_ADDALPHA",
	"TOP_BUMPENVMAP",
	"TOP_BUMPENVMAPLUMINANCE",
	"TOP_DOTPRODUCT3",
	"TOP_MULTIPLYADD",
	"TOP_LERP"
};

const char* g_aTexTypeStrs[] =
{
	"Texture types",
	"TEX_NORMAL",
	"TEX_LIGHTMAP",
	"TEX_BUMPMAP",
	"TEX_PROCESS",
	"TEX_DETAIL",
	"TEX_ANIMATION",
	"TEX_D3DTEX",
};

const char* g_aTexAddrStrs[] = 
{
	"Texture address types",
	"TADDR_WRAP",
	"TADDR_MIRROR",
	"TADDR_CLAMP",
	"TADDR_BORDER",
	"TADDR_MIRRORONCE"
};

const char* g_aCullStrs[] =
{
	"Cull modes",
	"CULL_NONE",
	"CULL_CW",
	"CULL_CCW"
};

const char* g_aTexArgStrs[] = 
{
	"Texture arguments",
	"TA_DIFFUSE",
	"TA_CURRENT",
	"TA_TEXTURE",
	"TA_TFACTOR",
	"TA_SPECULAR",
	"TA_TEMP",
};

const char* g_aTexModStrs[] = 
{
	"Texture modifiers",
	"TA_COMPLEMENT",
	"TA_ALPHAREPLICATE"
};

const char* g_aWaveStrs[] = 
{
	"Wave functions",
	"W_SIN",
	"W_TRIANGLE",
	"W_SQUARE",
	"W_SAWTOOTH",
	"W_INVSAWTOOTH"
};

const char*	g_aResultArgStrs[] = 
{
	"Texture result argument",
	"TA_CURRENT",
	"TA_TEMP",
};

const FORMATSTR	g_aFormatStrs[] =
{
	{A3DFMT_UNKNOWN,		"A3DFMT_UNKNOWN"},
	{A3DFMT_R8G8B8,			"A3DFMT_R8G8B8"},     
	{A3DFMT_A8R8G8B8,		"A3DFMT_A8R8G8B8"},   
	{A3DFMT_X8R8G8B8,     	"A3DFMT_X8R8G8B8"},   
	{A3DFMT_R5G6B5,       	"A3DFMT_R5G6B5"},   
	{A3DFMT_X1R5G5B5,     	"A3DFMT_X1R5G5B5"},   
	{A3DFMT_A1R5G5B5,     	"A3DFMT_A1R5G5B5"},   
	{A3DFMT_A4R4G4B4,     	"A3DFMT_A4R4G4B4"},   
	{A3DFMT_R3G3B2,       	"A3DFMT_R3G3B2"},   
	{A3DFMT_A8,           	"A3DFMT_A8"},   
	{A3DFMT_A8R3G3B2,     	"A3DFMT_A8R3G3B2"},   
	{A3DFMT_X4R4G4B4,     	"A3DFMT_X4R4G4B4"},   
	{A3DFMT_A8P8,           "A3DFMT_A8P8"},
	{A3DFMT_P8,             "A3DFMT_P8"},        
	{A3DFMT_L8,             "A3DFMT_L8"},        
	{A3DFMT_A8L8,           "A3DFMT_A8L8"},        
	{A3DFMT_A4L4,           "A3DFMT_A4L4"},        
	{A3DFMT_V8U8,           "A3DFMT_V8U8"},        
	{A3DFMT_L6V5U5,         "A3DFMT_L6V5U5"},    
	{A3DFMT_X8L8V8U8,       "A3DFMT_X8L8V8U8"},    
	{A3DFMT_Q8W8V8U8,       "A3DFMT_Q8W8V8U8"},    
	{A3DFMT_V16U16,         "A3DFMT_V16U16"},      
	{A3DFMT_W11V11U10,      "A3DFMT_W11V11U10"},   
	{A3DFMT_DXT1,           "A3DFMT_DXT1"},
	{A3DFMT_DXT2,           "A3DFMT_DXT2"},        
	{A3DFMT_DXT3,           "A3DFMT_DXT3"},        
	{A3DFMT_DXT4,           "A3DFMT_DXT4"},        
	{A3DFMT_DXT5,           "A3DFMT_DXT5"},       
};

const char*	g_aTexCoordFlags[] =
{
	"Texture coordinates flags",
	"TCI_PASSTHRU",
	"TCI_CAMERASPACENORMAL",
	"TCI_CAMERASPACEPOSITION",
	"TCI_CAMERASPACEREFLECTIONVECTOR",
};

const char*	g_aTexTransFlags[] =
{
	"Texture transfrom flags",
	"TTFF_DISABLE",
	"TTFF_COUNT1",
	"TTFF_COUNT2",
	"TTFF_COUNT3",
	"TTFF_COUNT4",
};

char	g_szWorkDir[MAX_PATH];	//	Work directory
char	g_szIniFile[MAX_PATH];	//	.ini file name
char	g_szTexDir[MAX_PATH] = "Shaders\\Textures";		//	Shader texture path

bool		g_bActive = false;			//	App active flag
CClipboard	g_Clipboard;				//	Clipboard

AArray<D3DTEXTURE, D3DTEXTURE&>	g_aD3DTexs;	//	D3D textures

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

//	Convert texture operation modifier flag to 1-based index.
//	Return 0 means no modifier flag exists
int ModFlagToIndex(DWORD dwMod)
{
	int iIndex = 0;

	switch (dwMod)
	{
	case A3DTA_COMPLEMENT:		iIndex = 1;		break;
	case A3DTA_ALPHAREPLICATE:	iIndex = 2;		break;
	}

	return iIndex;
}

/*	Convert a texture file name to animation-texture format.
	A normal texture file name may be 'texture00.jpg', then animation-texture format
	will be 'texture.jpg'.
*/
void AnimTextureFileName(char* szFileName)
{
	char* pTemp = strrchr(szFileName, '.');
	if (!pTemp)
		return;

	char c = *(pTemp-1);
	if (c >= '0' && c <= '9')
	{
		if (pTemp-szFileName < 2)
		{
			MessageBox(NULL, "动画贴图文件名不合法！", "提示", MB_OK);
			return;
		}

		char* pPre = pTemp - 2;
		while (*pTemp)
			*pPre++ = *pTemp++;

		*pPre = '\0';
	}
}

int glb_SearchD3DTexSlot(A3DTextureProxy* pTexProxy)
{
	if (!pTexProxy)
		return -1;

	for (int i=0; i < g_aD3DTexs.GetSize(); i++)
	{
		if (g_aD3DTexs[i].pTexProxy == pTexProxy)
			return i;
	}

	return -1;
}


