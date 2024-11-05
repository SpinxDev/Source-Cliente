// PropertyForm.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "PropertyForm.h"
#include "A3D.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// 
//	CPropertyForm
//
///////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPropertyForm, CFormView)

CPropertyForm::CPropertyForm()
	: CFormView(CPropertyForm::IDD)
{
	//{{AFX_DATA_INIT(CPropertyForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_hCurItem	= NULL;

	m_pCurShader	 = NULL;
	m_hStageMenu	 = NULL;
	m_hStagePropMenu = NULL;
	m_hGeneralMenu	 = NULL;
	m_hGenPropMenu	 = NULL;
	m_hTCModMenu	 = NULL;
	m_hTCMPropMenu	 = NULL;
}

CPropertyForm::~CPropertyForm()
{
}

void CPropertyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyForm)
	DDX_Control(pDX, IDC_PROPTREE, m_PropTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyForm, CFormView)
	//{{AFX_MSG_MAP(CPropertyForm)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_PROPTREE, OnRclickProptree)
	ON_COMMAND(ID_GEN_ZCOMPARE, OnGenZcompare)
	ON_COMMAND(ID_GEN_CULLMODE, OnGenCullmode)
	ON_COMMAND(ID_GEN_ZENABLE, OnGenZenable)
	ON_COMMAND(ID_GEN_ZWRITE, OnGenZwrite)
	ON_COMMAND(ID_GEN_ALPHACOMPARE, OnGenAlphacompare)
	ON_COMMAND(ID_GEN_BLEND, OnGenBlend)
	ON_COMMAND(ID_GPROP_EDIT, OnGpropEdit)
	ON_COMMAND(ID_GPROP_REMOVE, OnGpropRemove)
	ON_COMMAND(ID_STG_HIDE, OnStgHide)
	ON_COMMAND(ID_STG_ADDBEFORE, OnStgAddbefore)
	ON_COMMAND(ID_STG_ADDAFTER, OnStgAddafter)
	ON_COMMAND(ID_STG_MOVEUP, OnStgMoveup)
	ON_COMMAND(ID_STG_MOVEDOWN, OnStgMovedown)
	ON_COMMAND(ID_STG_TEXTURE, OnStgTexture)
	ON_COMMAND(ID_STG_BUMPMAT, OnStgBumpmat)
	ON_COMMAND(ID_STG_BUMPOFFSET, OnStgBumpoffset)
	ON_COMMAND(ID_STG_BUMPSCALE, OnStgBumpscale)
	ON_COMMAND(ID_SPROP_EDIT, OnSpropEdit)
	ON_COMMAND(ID_STG_FILTER, OnStgFilter)
	ON_COMMAND(ID_STG_ADDRESS, OnStgAddress)
	ON_COMMAND(ID_STG_ALPHAOP, OnStgAlphaop)
	ON_COMMAND(ID_STG_COLOROP, OnStgColorop)
	ON_COMMAND(ID_SPROP_REMOVE, OnSpropRemove)
	ON_COMMAND(ID_STG_REMOVE, OnStgRemove)
	ON_COMMAND(ID_GEN_COPY, OnGenCopy)
	ON_COMMAND(ID_GEN_PASTE, OnGenPaste)
	ON_NOTIFY(NM_DBLCLK, IDC_PROPTREE, OnDblclkProptree)
	ON_COMMAND(ID_GEN_COLORGEN, OnGenColorgen)
	ON_COMMAND(ID_GEN_ALPHAGEN, OnGenAlphagen)
	ON_COMMAND(ID_STG_TCSCALE, OnStgTcscale)
	ON_COMMAND(ID_STG_TCSCROLL, OnStgTcscroll)
	ON_COMMAND(ID_STG_TCROTATE, OnStgTcrotate)
	ON_COMMAND(ID_STG_TCSTRETCH, OnStgTcstretch)
	ON_COMMAND(ID_TCMP_EDIT, OnTcmpEdit)
	ON_COMMAND(ID_TCMP_REMOVE, OnTcmpRemove)
	ON_COMMAND(ID_TCMP_MOVEUP, OnTcmpMoveup)
	ON_COMMAND(ID_TCMP_MOVEDOWN, OnTcmpMovedown)
	ON_COMMAND(ID_STG_TCTURB, OnStgTcturb)
	ON_COMMAND(ID_GEN_PIXELSHADER, OnGenPixelshader)
	ON_COMMAND(ID_STG_COPY, OnStgCopy)
	ON_COMMAND(ID_STG_CUT, OnStgCut)
	ON_COMMAND(ID_STG_PASTE, OnStgPaste)
	ON_COMMAND(ID_STG_RESULTARG, OnStgResultarg)
	ON_COMMAND(ID_GEN_TFACTOR, OnGenTfactor)
	ON_COMMAND(ID_STG_TEXCOORD, OnStgTexcoord)
	ON_COMMAND(ID_STG_TEXTRANSFORM, OnStgTextransform)
	ON_COMMAND(ID_GEN_ALPHATEX, OnGenAlphatex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyForm diagnostics

#ifdef _DEBUG
void CPropertyForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CPropertyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
//
//	CPropertyForm message handlers
//
/////////////////////////////////////////////////////////////////////////////

/*	Initialize object.

	Return true for success, otherwise return false.
*/
bool CPropertyForm::Initialize()
{
	//	Initialize image list
	if (!m_PropImageList.Create(16, 16, ILC_COLOR4, 5, 0))
	{
		TRACE("CPropertyForm::OnCreate, Failed to create property image list.\n");
		return false;
	}
	
	HINSTANCE hInst = GetModuleHandle(NULL);

	HBITMAP hBmp;

	hBmp = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_GENERAL), IMAGE_BITMAP, 16, 16, LR_LOADTRANSPARENT);
	m_PropImageList.Add(CBitmap::FromHandle(hBmp), RGB(0, 0, 0));
	DeleteObject(hBmp);

	hBmp = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_SELECTED), IMAGE_BITMAP, 16, 16, LR_LOADTRANSPARENT);
	m_PropImageList.Add(CBitmap::FromHandle(hBmp), RGB(0, 0, 0));
	DeleteObject(hBmp);

	hBmp = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_UNSELECTED), IMAGE_BITMAP, 16, 16, LR_LOADTRANSPARENT);
	m_PropImageList.Add(CBitmap::FromHandle(hBmp), RGB(0, 0, 0));
	DeleteObject(hBmp);

	hBmp = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_TREELINE), IMAGE_BITMAP, 16, 16, LR_LOADTRANSPARENT);
	m_PropImageList.Add(CBitmap::FromHandle(hBmp), RGB(0, 0, 0));
	DeleteObject(hBmp);

	hBmp = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_TCMODS), IMAGE_BITMAP, 16, 16, LR_LOADTRANSPARENT);
	m_PropImageList.Add(CBitmap::FromHandle(hBmp), RGB(0, 0, 0));
	DeleteObject(hBmp);

	//	Associated tree control and image list
	m_PropTree.SetImageList(&m_PropImageList, TVSIL_NORMAL);

	//	Load menus
	m_hStageMenu	 = LoadMenu(hInst, MAKEINTRESOURCE(IDR_STAGEMENU));
	m_hStagePropMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_STGPROPMENU));
	m_hGeneralMenu	 = LoadMenu(hInst, MAKEINTRESOURCE(IDR_GENERALMENU));
	m_hGenPropMenu	 = LoadMenu(hInst, MAKEINTRESOURCE(IDR_GENPROPMENU));
	m_hTCModMenu	 = LoadMenu(hInst, MAKEINTRESOURCE(IDR_TCMODMENU));
	m_hTCMPropMenu	 = LoadMenu(hInst, MAKEINTRESOURCE(IDR_TCMODPROPMENU));

	return true;
}

//	Release all resources
void CPropertyForm::Release()
{
	//	Release menus
	if (m_hStageMenu)
	{
		DestroyMenu(m_hStageMenu);
		m_hStageMenu = NULL;
	}

	if (m_hStagePropMenu)
	{
		DestroyMenu(m_hStagePropMenu);
		m_hStagePropMenu = NULL;
	}

	if (m_hGeneralMenu)
	{
		DestroyMenu(m_hGeneralMenu);
		m_hGeneralMenu = NULL;
	}

	if (m_hGenPropMenu)
	{
		DestroyMenu(m_hGenPropMenu);
		m_hGenPropMenu = NULL;
	}

	if (m_hTCModMenu)
	{
		DestroyMenu(m_hTCModMenu);
		m_hTCModMenu = NULL;
	}

	if (m_hTCMPropMenu)
	{
		DestroyMenu(m_hTCMPropMenu);
		m_hTCMPropMenu = NULL;
	}
}

void CPropertyForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	if (m_nMapMode == -1)
	{
		//	Force recalculation of scale to fit parameters
		SetScaleToFitSize(m_totalLog);
	}
	else
	{
		//	UpdateBars() handles locking out recursion
		//	UpdateBars();
		EnableScrollBarCtrl(SB_HORZ, FALSE);
		EnableScrollBarCtrl(SB_VERT, FALSE);
	}

	if (!m_PropTree.GetSafeHwnd())
		return;

	m_PropTree.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
}

/*	Change current shader.

	Return true for success, otherwise return false

	pShaderInfo: new shader's information. If this parameter is NULL, property tree will
				 be cleard.
*/
bool CPropertyForm::ChangeShader(SHADERINFO* pShaderInfo)
{
	//	Clear previous property tree
	ClearWholeTree();

	m_pCurShader = pShaderInfo;

	if (!pShaderInfo)
		return true;

	//	Build a new tree
	HTREEITEM hGeneral, hStage;

	//	Create general property root node
	hGeneral = m_PropTree.InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE, 
									 "General Properties", IMG_GENERAL, IMG_GENERAL, 
									 TVIS_BOLD | TVIS_EXPANDED, TVIS_BOLD | TVIS_EXPANDED,
									 ND_GENERALPROPS, TVI_ROOT, NULL);

	CreateGeneralProps(hGeneral, pShaderInfo);

	int i, iImage;
	char szTitle[64];

	for (i=0; i < pShaderInfo->pShader->GetStageNum(); i++)
	{
		sprintf(szTitle, "Stage %d", i);

		iImage = pShaderInfo->pShader->StageIsVisible(i) ? IMG_SELECTED : IMG_UNSELECTED;

		//	Create stage property root node
		hStage = m_PropTree.InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE, 
									   szTitle, iImage, iImage, 
									   TVIS_BOLD | TVIS_EXPANDED, TVIS_BOLD | TVIS_EXPANDED,
									   MAKESTAGENODEID(i), TVI_ROOT, NULL);

		CreateStageProps(hStage, pShaderInfo, i);
	}

	return true;
}

//	Clear whole tree
void CPropertyForm::ClearWholeTree()
{
	m_PropTree.DeleteAllItems();
}

/*	Create general property tree

	hGeneral: general property's root node
	pShaderInfo: shader's information
*/
void CPropertyForm::CreateGeneralProps(HTREEITEM hGeneral, SHADERINFO* pShaderInfo)
{
	A3DShader* pShader = pShaderInfo->pShader;
	SHADERGENERAL& General = pShader->GetGeneralProps();

	if (General.dwMask & SGMF_ZFUNC)
		CreateOneGenProp(hGeneral, GND_ZFUNC, NULL, pShader);

	if (General.dwMask & SGMF_ZENABLE)
		CreateOneGenProp(hGeneral, GND_ZENABLE, NULL, pShader);

	if (General.dwMask & SGMF_ZWENABLE)
		CreateOneGenProp(hGeneral, GND_ZWENABLE, NULL, pShader);

	if (General.dwMask & SGMF_ALPHACOMP)
		CreateOneGenProp(hGeneral, GND_ALPHACOMP, NULL, pShader);

	if (General.dwMask & SGMF_CULLMODE)
		CreateOneGenProp(hGeneral, GND_CULLMODE, NULL, pShader);

	if (General.dwMask & SGMF_BLEND)
		CreateOneGenProp(hGeneral, GND_BLEND, NULL, pShader);

	if (General.dwMask & SGMF_COLORGEN)
		CreateOneGenProp(hGeneral, GND_COLORGEN, NULL, pShader);

	if (General.dwMask & SGMF_ALPHAGEN)
		CreateOneGenProp(hGeneral, GND_ALPHAGEN, NULL, pShader);

	if (General.dwMask & SGMF_TFACTOR)
		CreateOneGenProp(hGeneral, GND_TFACTOR, NULL, pShader);

	if (General.pPShader)
		CreateOneGenProp(hGeneral, GND_PIXELSHADER, NULL, pShader);

	CreateOneGenProp(hGeneral, GND_ALPHATEX, NULL, pShader);
}

/*	Create one stage property tree

	hStage: stage property's root node
	pShaderInfo: shader's information
	iStage: index of stage
*/
void CPropertyForm::CreateStageProps(HTREEITEM hStage, SHADERINFO* pShaderInfo, int iStage)
{
	A3DShader* pShader = pShaderInfo->pShader;
	SHADERSTAGE Stage  = pShader->GetStage(iStage);

	//	Texture's type
	if (Stage.iTexType)
		CreateOneStgProp(hStage, SND_TEXTURE, NULL, Stage);

	if (Stage.dwMask & SSMF_FILTER)
		CreateOneStgProp(hStage, SND_FILTER, NULL, Stage);

	if (Stage.dwMask & SSMF_ADDRESS)
		CreateOneStgProp(hStage, SND_ADDRESS, NULL, Stage);

	if (Stage.dwMask & SSMF_COLOROP)
	{
		CreateOneStgProp(hStage, SND_COLOROP, NULL, Stage);
		CreateOneStgProp(hStage, SND_COLORARG1, NULL, Stage);
		CreateOneStgProp(hStage, SND_COLORARG2, NULL, Stage);
	}

	if (Stage.dwMask & SSMF_ALPHAOP)
	{
		CreateOneStgProp(hStage, SND_ALPHAOP, NULL, Stage);
		CreateOneStgProp(hStage, SND_ALPHAARG1, NULL, Stage);
		CreateOneStgProp(hStage, SND_ALPHAARG2, NULL, Stage);
	}

	if (Stage.dwMask & SSMF_COLORGEN)
		CreateOneStgProp(hStage, SND_COLORGEN, NULL, Stage);

	if (Stage.dwMask & SSMF_ALPHAGEN)
		CreateOneStgProp(hStage, SND_ALPHAGEN, NULL, Stage);

	if (Stage.dwMask & SSMF_ALPHACOMP)
		CreateOneStgProp(hStage, SND_ALPHACOMP, NULL, Stage);

	if (Stage.dwMask & SSMF_BUMPMAT)
		CreateOneStgProp(hStage, SND_BUMPMAT, NULL, Stage);

	if (Stage.dwMask & SSMF_BUMPSCALE)
		CreateOneStgProp(hStage, SND_BUMPSCALE, NULL, Stage);

	if (Stage.dwMask & SSMF_BUMPOFFSET)
		CreateOneStgProp(hStage, SND_BUMPOFFSET, NULL, Stage);

	if (Stage.dwMask & SSMF_RESULTARG)
		CreateOneStgProp(hStage, SND_RESULTARG, NULL, Stage);

	if (Stage.dwMask & SSMF_TEXCOORD)
		CreateOneStgProp(hStage, SND_TEXCOORD, NULL, Stage);

	if (Stage.dwMask & SSMF_TEXTRANS)
		CreateOneStgProp(hStage, SND_TEXTRANS, NULL, Stage);

	if (Stage.iNumtcMod)
		CreateOneStgProp(hStage, ND_TCMODS, NULL, Stage);

	//	TODO: handle these...
	//	Stage.iProTexFunc
	//	Stage.iUVGenFunc
}

/*	Create or modify one general property

	hGeneral: general property's root node, if hNode is valid, this can be NULL
	dwID: property's node ID.
	hNode: property node's handle. if NULL, a new node will be created with dwID
	pShader: shader object which contains new general properties
*/
void CPropertyForm::CreateOneGenProp(HTREEITEM hGeneral, DWORD dwID, HTREEITEM hNode, 
									 A3DShader* pShader)
{
	SHADERGENERAL& General = pShader->GetGeneralProps();
	char szText[256];
	SHADERWAVE* pWave;

	switch (dwID)
	{
	case GND_ZFUNC:

		sprintf(szText, "Z compare:  %s", g_aCompStrs[General.dwZFunc]);
		break;
	
	case GND_ZENABLE:

		if (General.bZEnable)
			sprintf(szText, "Z enable:  true");
		else
			sprintf(szText, "Z enable:  false");

		break;

	case GND_ZWENABLE:

		if (General.bZWriteEnable)
			sprintf(szText, "Z write:  true");
		else
			sprintf(szText, "Z write:  false");

		break;

	case GND_ALPHACOMP:

		sprintf(szText, "Alpha compare:  %s  %d", g_aCompStrs[General.dwAlphaFunc], General.iAlphaRef);
		break;

	case GND_CULLMODE:

		sprintf(szText, "Cull mode:  %s", g_aCullStrs[General.dwCullMode]);
		break;

	case GND_BLEND:

		sprintf(szText, "Blend func:  %s  %s", g_aBlendStrs[General.dwSrcBlend], g_aBlendStrs[General.dwDstBlend]);
		break;

	case GND_COLORGEN:

		pWave = &General.rgbGen;
		sprintf(szText, "Color gen:  %s  %f  %f  %f  %d", g_aWaveStrs[pWave->iFunc], 
				pWave->fBase, pWave->fAmp, pWave->fPhase, pWave->iPeriod);
		break;

	case GND_ALPHAGEN:

		pWave = &General.AlphaGen;
		sprintf(szText, "Alpha gen:  %s  %f  %f  %f  %d", g_aWaveStrs[pWave->iFunc], 
				pWave->fBase, pWave->fAmp, pWave->fPhase, pWave->iPeriod);
		break;

	case GND_TFACTOR:
	{
		A3DCOLORVALUE col = General.dwTFactor;
		sprintf(szText, "Texture factor:  %.3f  %.3f  %.3f  %.3f", col.r, col.g, col.b, col.a);
		break;
	}
	case GND_PIXELSHADER:

		if (General.pPShader)
			sprintf(szText, "Pixel Shader:  %s", General.pPShader->GetFileName());
		else if (hNode)
		{
			//	Remove this property
			m_PropTree.DeleteItem(hNode);
			return;
		}
		else	//	No pixel shader exist
			return;

		break;

	case GND_ALPHATEX:

		sprintf(szText, "alpha texture flag:  %s", pShader->IsAlphaTexture() ? "true" : "false");
		break;

	default:

		return;
	}

	//	Only modify existed node
	if (hNode)
		m_PropTree.SetItemText(hNode, szText);
	else
		CreatePropertyNode(dwID, szText, hGeneral);
}

/*	Create or modify one stage property

	hStage: stage's root node, if hNode is valid, this can be NULL
	dwID: stage property's node ID.
	hNode: property node's handle. if NULL, a new node will be created with dwID
	Stage: contain property values
*/
void CPropertyForm::CreateOneStgProp(HTREEITEM hStage, DWORD dwID, HTREEITEM hNode, 
									 SHADERSTAGE& Stage)
{
	char szText[256];

	switch (dwID)
	{
	case SND_TEXTURE:
	{
		int i, iFormat = 0;
		for (i=0; i < NUM_TEXFORMATSTR; i++)
		{
			if (Stage.TexFormat == g_aFormatStrs[i].Fmt)
				iFormat = i;
		}

		strcpy(szText, "Texture");

		int iNumTex = Stage.aTextures.GetSize();
		if (iNumTex)
		{
			if (Stage.iTexType == A3DSDTEX_NORMAL)
			{
				A3DTexture* pTexture = (A3DTexture*)Stage.aTextures[0];
				sprintf(szText, "Texture:  %s  %s  %s", g_aTexTypeStrs[Stage.iTexType], 
						g_aFormatStrs[iFormat].szName, pTexture->GetMapFile());
			}
			else if (Stage.iTexType == A3DSDTEX_ANIMATION)
			{
				A3DTexture* pTexture = (A3DTexture*)Stage.aTextures[0];
				char szFile[MAX_PATH];
				strcpy(szFile, pTexture->GetMapFile());
				AnimTextureFileName(szFile);

				sprintf(szText, "Texture:  %s  %s  %d  %u  %s", g_aTexTypeStrs[Stage.iTexType], 
						g_aFormatStrs[iFormat].szName, iNumTex, Stage.dwFrameTime, szFile);
			}
			else if (Stage.iTexType == A3DSDTEX_D3DTEX)
			{
				A3DTextureProxy* pTexProxy = (A3DTextureProxy*)Stage.aTextures[0];
				int iSlot = glb_SearchD3DTexSlot(pTexProxy);
				if (iSlot >= 0)
					sprintf(szText, "Texture:  %s  %s", g_aTexTypeStrs[Stage.iTexType], g_aD3DTexs[iSlot].szFile);
			}
			else
			{
				//	TODO: handle other texture type
			}
		}
		else
		{
			if (Stage.iTexType == A3DSDTEX_D3DTEX)
				sprintf(szText, "Texture:  %s  [null]", g_aTexTypeStrs[Stage.iTexType]);
		}

		break;
	}
	case SND_FILTER:

		sprintf(szText, "Filter:  %s", g_aFilterStrs[Stage.dwFilter+1]);
		break;

	case SND_ADDRESS:

		sprintf(szText, "Address:  %s", g_aTexAddrStrs[Stage.dwAddress]);
		break;

	case SND_COLOROP:

		sprintf(szText, "Color operations:  %s", g_aTexOpStrs[Stage.dwColorOp]);
		break;

	case SND_COLORARG1:
	{
		DWORD dwModIdx;

		if (Stage.dwColorArg1 & ~A3DTA_SELECTMASK)
		{
			dwModIdx = ModFlagToIndex(Stage.dwColorArg1 & ~A3DTA_SELECTMASK);
			sprintf(szText, "Color arg1:  %s  %s", g_aTexArgStrs[(Stage.dwColorArg1 & A3DTA_SELECTMASK) + 1], g_aTexModStrs[dwModIdx]);
		}
		else
			sprintf(szText, "Color arg1:  %s", g_aTexArgStrs[Stage.dwColorArg1+1]);

		break;
	}
	case SND_COLORARG2:
	{
		DWORD dwModIdx;

		if (Stage.dwColorArg2 & ~A3DTA_SELECTMASK)
		{
			dwModIdx = ModFlagToIndex(Stage.dwColorArg2 & ~A3DTA_SELECTMASK);
			sprintf(szText, "Color arg2:  %s  %s", g_aTexArgStrs[(Stage.dwColorArg2 & A3DTA_SELECTMASK) + 1], g_aTexModStrs[dwModIdx]);
		}
		else
			sprintf(szText, "Color arg2:  %s", g_aTexArgStrs[Stage.dwColorArg2+1]);

		break;
	}
	case SND_ALPHAOP:

		sprintf(szText, "Alpha operations:  %s", g_aTexOpStrs[Stage.dwAlphaOp]);
		break;

	case SND_ALPHAARG1:
	{
		DWORD dwModIdx;

		if (Stage.dwAlphaArg1 & ~A3DTA_SELECTMASK)
		{
			dwModIdx = ModFlagToIndex(Stage.dwAlphaArg1 & ~A3DTA_SELECTMASK);
			sprintf(szText, "Alpha arg1:  %s  %s", g_aTexArgStrs[(Stage.dwAlphaArg1 & A3DTA_SELECTMASK) + 1], g_aTexModStrs[dwModIdx]);
		}
		else
			sprintf(szText, "Alpha arg1:  %s", g_aTexArgStrs[Stage.dwAlphaArg1+1]);

		break;
	}
	case SND_ALPHAARG2:
	{
		DWORD dwModIdx;

		if (Stage.dwAlphaArg2 & ~A3DTA_SELECTMASK)
		{
			dwModIdx = ModFlagToIndex(Stage.dwAlphaArg2 & ~A3DTA_SELECTMASK);
			sprintf(szText, "Alpha arg2:  %s  %s", g_aTexArgStrs[(Stage.dwAlphaArg2 & A3DTA_SELECTMASK) + 1], g_aTexModStrs[dwModIdx]);
		}
		else
			sprintf(szText, "Alpha arg2:  %s", g_aTexArgStrs[Stage.dwAlphaArg2+1]);

		break;
	}
	case SND_ALPHACOMP:

		sprintf(szText, "Alpha compare:  %s  %d", g_aCompStrs[Stage.dwAlphaFunc], Stage.iAlphaRef);
		break;

	case SND_BUMPMAT:

		sprintf(szText, "Bump mat:  %f  %f  %f  %f", Stage.fBumpEnvMat00, Stage.fBumpEnvMat01, Stage.fBumpEnvMat10, Stage.fBumpEnvMat11);
		break;

	case SND_BUMPSCALE:

		sprintf(szText, "Bump scale:  %f", Stage.fBumpEnvScale);
		break;

	case SND_BUMPOFFSET:

		sprintf(szText, "Bump offset:  %f", Stage.fBumpEnvOffset);
		break;

	case SND_RESULTARG:
	{
		int iIndex = Stage.dwResultArg == A3DTA_CURRENT ? 1 : 2;
		sprintf(szText, "Result arg:  %s", g_aResultArgStrs[iIndex]);
		break;
	}
	case SND_TEXCOORD:
	{
		int iIndex = Stage.dwTexCoord & 0x0000000f;
		int iFlag = 1;
		switch (Stage.dwTexCoord & 0x000F0000)
		{
		case A3DTCI_CAMERASPACENORMAL:				iFlag = 2;	break;
		case A3DTCI_CAMERASPACEPOSITION:			iFlag = 3;	break;
		case A3DTCI_CAMERASPACEREFLECTIONVECTOR:	iFlag = 4;	break;
		}
		
		sprintf(szText, "Texture coords: %d  %s", iIndex, g_aTexCoordFlags[iFlag]);
		break;
	}
	case SND_TEXTRANS:
	{
		int iIndex = Stage.dwTTFF & 0x0000000f;
		if (Stage.dwTTFF & A3DTTFF_PROJECTED)
			sprintf(szText, "Texture TM: %s | TTFF_PROJECTED", g_aTexTransFlags[iIndex+1]);
		else
			sprintf(szText, "Texture TM: %s", g_aTexTransFlags[iIndex+1]);
		break;
	}
	case ND_TCMODS:		//	TC modifies

		sprintf(szText, "TC modifies");
		if (!hNode)
			hNode = CreatePropertyNode(dwID, szText, hStage);

		CreateTCModifies(hNode, Stage);
		return;

	//	TODO: handle these...
	//	Stage.iProTexFunc
	//	Stage.iUVGenFunc
	//	Alpha and color generate...

	case SND_COLORGEN:
	case SND_ALPHAGEN:
	default:

		return;
	}

	if (hNode)
		m_PropTree.SetItemText(hNode, szText);
	else
		CreatePropertyNode(dwID, szText, hStage);
}

/*	Create a property node

	Return node's handle for success, otherwise return NULL

	dwID: node's ID.
	szText: node text string.
	hParent: parent node.
*/
HTREEITEM CPropertyForm::CreatePropertyNode(DWORD dwID, char* szText, HTREEITEM hParent)
{
	DWORD dwFlags  = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	DWORD dwStates = 0;
	int iImage = IMG_TREELINE;

	if (dwID == ND_TCMODS)
	{
		dwFlags  |= TVIF_STATE;
		dwStates |= TVIS_EXPANDED;
		iImage	  = IMG_TCMODS;
	}

	HTREEITEM hNode = m_PropTree.InsertItem(dwFlags, szText, iImage, iImage, 
											dwStates, dwStates, dwID, hParent, NULL);
	return hNode;
}

/*	Create TC modifies nodes

	hTCMod: TC modifies root node
	Stage: stage data
*/
void CPropertyForm::CreateTCModifies(HTREEITEM hTCMod, SHADERSTAGE& Stage)
{
	//	Clear all old nodes
	HTREEITEM hTemp, hNode = m_PropTree.GetChildItem(hTCMod);
	while (hNode)
	{
		hTemp = hNode;
		hNode = m_PropTree.GetNextSiblingItem(hNode);
		m_PropTree.DeleteItem(hTemp);
	}
	
	if (!Stage.iNumtcMod)	//	No tcmod property exist
	{
		m_PropTree.DeleteItem(hTCMod);
		return;
	}

	int i;
	char szText[256];
	SHADERWAVE* pWave;
	DWORD dwID;

	for (i=0; i < Stage.iNumtcMod; i++)
	{
		pWave = &Stage.atcMods[i];
		switch (pWave->iType)
		{
		case SWO_TCSCALE:

			dwID = TCND_SCALE;
			sprintf(szText, "TC scale:  %f  %f", pWave->fBase, pWave->fAmp);
			break;

		case SWO_TCSCROLL:

			dwID = TCND_SCROLL;
			sprintf(szText, "TC scroll:  %f  %f", pWave->fBase, pWave->fAmp);
			break;

		case SWO_TCROTATE:

			dwID = TCND_ROTATE;
			sprintf(szText, "TC rotate:  %f", RAD2DEG(pWave->fBase));
			break;

		case SWO_TCSTRETCH:

			dwID = TCND_STRETCH;
			sprintf(szText, "TC stretch:  %s  %f  %f  %f  %d", g_aWaveStrs[pWave->iFunc], 
					pWave->fBase, pWave->fAmp, pWave->fPhase, pWave->iPeriod);
			break;

		case SWO_TCTURB:

			dwID = TCND_TURB;
			sprintf(szText, "TC turb:  %f  %f  %d", pWave->fAmp, pWave->fPhase, pWave->iPeriod);
			break;

		default:
			continue;	//	Ignore this property
		}

		CreatePropertyNode(dwID, szText, hTCMod);
	}
}

//	Right button click
void CPropertyForm::OnRclickProptree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POINT ptScreen, pt;
	HTREEITEM hHitItem;
	UINT uFlags;

	//	Check whether cursor hit on a item
	//	Get cursor's position in tree control
	GetCursorPos(&ptScreen);
	pt = ptScreen;
	m_PropTree.ScreenToClient(&pt);

	hHitItem = m_PropTree.HitTest(pt, &uFlags);
	if (hHitItem && (uFlags & (TVHT_ONITEM | TVHT_ONITEMRIGHT)))
	{
		m_hCurItem = hHitItem;

		//	Select hit item
		m_PropTree.Select(hHitItem, TVGN_CARET);

		HMENU hMenu;
		DWORD dwID = m_PropTree.GetItemData(hHitItem);
		if (dwID == ND_GENERALPROPS)
			hMenu = GetSubMenu(m_hGeneralMenu, 0);
		else if (dwID == ND_TCMODS)
			hMenu = GetSubMenu(m_hTCModMenu, 0);
		else if (dwID > ND_TCMODS)
			hMenu = GetSubMenu(m_hTCMPropMenu, 0);
		else if (dwID == ND_STAGE0PROPS || dwID >= MAKESTAGENODEID(1))
			hMenu = GetSubMenu(m_hStageMenu, 0);
		else if (dwID & ND_STAGE0PROPS)
			hMenu = GetSubMenu(m_hStagePropMenu, 0);
		else
			hMenu = GetSubMenu(m_hGenPropMenu, 0);
			
		TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON, 
					   ptScreen.x, ptScreen.y, 0, m_hWnd, NULL);
	}

	*pResult = 0;
}

//	Double left-button click
void CPropertyForm::OnDblclkProptree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POINT ptScreen, pt;
	HTREEITEM hHitItem;
	UINT uFlags;

	//	Check whether cursor hit on a item
	//	Get cursor's position in tree control
	GetCursorPos(&ptScreen);
	pt = ptScreen;
	m_PropTree.ScreenToClient(&pt);

	hHitItem = m_PropTree.HitTest(pt, &uFlags);
	if (hHitItem && (uFlags & (TVHT_ONITEM | TVHT_ONITEMRIGHT)))
	{
		bool bValid = true;
		DWORD dwID  = m_PropTree.GetItemData(hHitItem);

		if (dwID == ND_GENERALPROPS || dwID == ND_TCMODS)
			bValid = false;
		else if (dwID == ND_STAGE0PROPS || (dwID >= MAKESTAGENODEID(1) && dwID < ND_TCMODS))
			bValid = false;

		if (!bValid)
		{
			*pResult = 0;
			return;
		}

		m_hCurItem = hHitItem;

		//	Select hit item
		m_PropTree.Select(hHitItem, TVGN_CARET);

		if (dwID > ND_TCMODS)
			OnTcmpEdit();
		else if (dwID & ND_STAGE0PROPS)
			OnSpropEdit();
		else
			OnGpropEdit();
	}

	*pResult = 0;
}

/*	Update one general property.

	dwID: general node's ID
	General: new general data.
*/
void CPropertyForm::UpdateGeneralProp(DWORD dwID, SHADERGENERAL& General)
{
	HTREEITEM hGeneral, hChild = m_PropTree.GetRootItem();

	//	Update data
	m_pCurShader->pShader->SetGeneralProps(General);

	//	Search general root item
//	hChild = m_PropTree.GetChildItem(hChild);
	while (hChild)
	{
		if (m_PropTree.GetItemData(hChild) == ND_GENERALPROPS)
			break;

		hChild = m_PropTree.GetNextSiblingItem(hChild);
	}

	if (!hChild)
		return;

	hGeneral = hChild;

	//	Now, search specified property item
	hChild = m_PropTree.GetChildItem(hChild);
	while (hChild)
	{
		if (m_PropTree.GetItemData(hChild) == dwID)
		{
			CreateOneGenProp(NULL, dwID, hChild, m_pCurShader->pShader);
			m_pCurShader->bModified = true;
			return;
		}

		hChild = m_PropTree.GetNextSiblingItem(hChild);
	}

	CreateOneGenProp(hGeneral, dwID, NULL, m_pCurShader->pShader);
	m_pCurShader->bModified = true;
}

/*	Update one property in specified stage.

	iStage: stage's index.
	dwID: ID of stage property node
	Stage: Stage data
*/
void CPropertyForm::UpdateStageProp(int iStage, DWORD dwID, SHADERSTAGE& Stage)
{
	HTREEITEM hStage, hChild = m_PropTree.GetRootItem();
	DWORD dwStageID = MAKESTAGENODEID(iStage);

	//	Update data
	m_pCurShader->pShader->SetStage(iStage, Stage);

	//	Search stage root item
//	hChild = m_PropTree.GetChildItem(hChild);
	while (hChild)
	{
		if (m_PropTree.GetItemData(hChild) == dwStageID)
			break;

		hChild = m_PropTree.GetNextSiblingItem(hChild);
	}

	if (!hChild)
		return;

	hStage = hChild;

	//	Now, search specified property item
	hChild = m_PropTree.GetChildItem(hChild);
	while (hChild)
	{
		if (m_PropTree.GetItemData(hChild) == dwID)
		{
			CreateOneStgProp(NULL, dwID, hChild, Stage);
			m_pCurShader->bModified = true;
			return;
		}

		hChild = m_PropTree.GetNextSiblingItem(hChild);
	}

	CreateOneStgProp(hStage, dwID, NULL, Stage);
	m_pCurShader->bModified = true;
}

/*	Insert a stage.

	Return true for success, otherwise return false

	hPreNode: previous node, may be general property root node or other stage property
			  root node. NULL means add stage at tail
*/
bool CPropertyForm::InsertStage(HTREEITEM hPreNode)
{
	//	Check stage's number
	if (m_pCurShader->pShader->GetStageNum() >= A3DShader::MAXNUM_STAGE)
	{
		char szMsg[128];
		LoadString(GetModuleHandle(NULL), IDS_TOOMUCHSTAGE, szMsg, 128);
		MessageBox(szMsg);
		return false;
	}

	char szTitle[64];

	SHADERSTAGE Stage;

	if (!hPreNode)	//	Add a stage at tail
	{
		int iStage = m_pCurShader->pShader->GetStageNum();
		sprintf(szTitle, "Stage %d", iStage);

		//	Create stage property root node
		m_PropTree.InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE, 
							  szTitle, IMG_SELECTED, IMG_SELECTED, 
							  TVIS_BOLD | TVIS_EXPANDED, TVIS_BOLD | TVIS_EXPANDED,
							  MAKESTAGENODEID(iStage), TVI_ROOT, NULL);

		m_pCurShader->pShader->InsertStage(-1, Stage);

		m_pCurShader->bModified = true;
		return true;
	}

	int iIndex;
	HTREEITEM hStage;
	DWORD dwID = m_PropTree.GetItemData(hPreNode);

	if (dwID < ND_STAGE0PROPS)
		iIndex = 0;
	else
		iIndex = GETSTAGEFROMNODEID(dwID) + 1;

	sprintf(szTitle, "Stage %d", iIndex);
	
	//	Insert a stage property root node after hPreNode
	hStage = m_PropTree.InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE, 
								   szTitle, IMG_SELECTED, IMG_SELECTED, 
								   TVIS_BOLD | TVIS_EXPANDED, TVIS_BOLD | TVIS_EXPANDED,
								   MAKESTAGENODEID(iIndex), TVI_ROOT, hPreNode);
	
	m_pCurShader->pShader->InsertStage(iIndex, Stage);

	//	Adjust node's ID and title after the new stage node
	hStage = m_PropTree.GetNextSiblingItem(hStage);
	while (hStage)
	{
		m_PropTree.SetItemData(hStage, MAKESTAGENODEID(++iIndex));

		sprintf(szTitle, "Stage %d", iIndex);
		m_PropTree.SetItemText(hStage, szTitle);

		hStage = m_PropTree.GetNextSiblingItem(hStage);
	}

	m_pCurShader->bModified = true;

	return true;
}

/*	Convert node ID to corresponding shader mask flag.

	Return mask flag for success, otherwise return 0.

	dwID: node ID
*/
DWORD CPropertyForm::NodeIDToFlag(DWORD dwID)
{
	DWORD dwFlag = 0;

	switch (dwID)
	{
	case GND_ZFUNC:			dwFlag = SGMF_ZFUNC;		break;
	case GND_ZENABLE:		dwFlag = SGMF_ZENABLE;		break;
	case GND_ZWENABLE:		dwFlag = SGMF_ZWENABLE;		break;
	case GND_ALPHACOMP:		dwFlag = SGMF_ALPHACOMP;	break;
	case GND_CULLMODE:		dwFlag = SGMF_CULLMODE;		break;
	case GND_BLEND:			dwFlag = SGMF_BLEND;		break;
	case GND_COLORGEN:		dwFlag = SGMF_COLORGEN;		break;
	case GND_ALPHAGEN:		dwFlag = SGMF_ALPHAGEN;		break;
	case GND_TFACTOR:		dwFlag = SGMF_TFACTOR;		break;

	case SND_FILTER:		dwFlag = SSMF_FILTER;		break;
	case SND_ADDRESS:		dwFlag = SSMF_ADDRESS;		break;
	case SND_COLOROP:		dwFlag = SSMF_COLOROP;		break;
	case SND_ALPHAOP:		dwFlag = SSMF_ALPHAOP;		break;
	case SND_COLORGEN:		dwFlag = SSMF_COLORGEN;		break;
	case SND_ALPHAGEN:		dwFlag = SSMF_ALPHAGEN;		break;
	case SND_ALPHACOMP:		dwFlag = SSMF_ALPHACOMP;	break;
	case SND_BUMPMAT:		dwFlag = SSMF_BUMPMAT;		break;
	case SND_BUMPSCALE:		dwFlag = SSMF_BUMPSCALE;	break;
	case SND_BUMPOFFSET:	dwFlag = SSMF_BUMPOFFSET;	break;
	case SND_RESULTARG:		dwFlag = SSMF_RESULTARG;	break;
	case SND_TEXCOORD:		dwFlag = SSMF_TEXCOORD;		break;
	case SND_TEXTRANS:		dwFlag = SSMF_TEXTRANS;		break;

	case SND_COLORARG1:		dwFlag = SSMF_COLOROP;		break;
	case SND_COLORARG2:		dwFlag = SSMF_COLOROP;		break;
	case SND_ALPHAARG1:		dwFlag = SSMF_ALPHAOP;		break;
	case SND_ALPHAARG2:		dwFlag = SSMF_ALPHAOP;		break;

	case TCND_SCALE:		dwFlag = SWO_TCSCALE;		break;
	case TCND_SCROLL:		dwFlag = SWO_TCSCROLL;		break;
	case TCND_ROTATE:		dwFlag = SWO_TCROTATE;		break;
	case TCND_STRETCH:		dwFlag = SWO_TCSTRETCH;		break;
	case TCND_TURB:			dwFlag = SWO_TCTURB;		break;

	case GND_PIXELSHADER:
	case GND_ALPHATEX:
	case SND_TEXTURE:
	default:
		
		break;
	}

	return dwFlag;
}

//	Update texture alpha and color operations
void CPropertyForm::UpdateTextureOps(bool bAlpha, int iStage, SHADERSTAGE& Stage)
{
	if (bAlpha)
	{
		UpdateStageProp(iStage, SND_ALPHAOP, Stage);
		UpdateStageProp(iStage, SND_ALPHAARG1, Stage);
		UpdateStageProp(iStage, SND_ALPHAARG2, Stage);
	}
	else
	{
		UpdateStageProp(iStage, SND_COLOROP, Stage);
		UpdateStageProp(iStage, SND_COLORARG1, Stage);
		UpdateStageProp(iStage, SND_COLORARG2, Stage);
	}
}


