/*
 * FILE: PropFormCmd.cpp
 *
 * DESCRIPTION: Handler of property form's menu command
 *
 * CREATED BY: duyuxin, 2002/2/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "stdafx.h"
#include "CommonDef.h"
#include "PropertyForm.h"

#include "ComboListDlg.h"
#include "AlphaCompDlg.h"
#include "GenBlendDlg.h"
#include "TextureDlg.h"
#include "BumpMatDlg.h"
#include "SingleValDlg.h"
#include "TexOPDlg.h"
#include "WaveDlg.h"
#include "TwoValDlg.h"
#include "PixelShaderDlg.h"
#include "DlgColorValue.h"
#include "DlgTexCoord.h"
#include "DlgTexTransform.h"
#include "DlgComboVal.h"

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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

//	Copy all properties
void CPropertyForm::OnGenCopy() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();

	g_Clipboard.SetData(CBDT_GENERAL, 0, &General);
}

//	Paste all properties
void CPropertyForm::OnGenPaste() 
{
	//	Check data type
	if (!g_Clipboard.MatchType(CBDT_GENERAL))
		return;

	g_Clipboard.GetData(CBDT_GENERAL, 0, (void*)m_pCurShader->pShader);

	//	Clear all old nodes
	HTREEITEM hTemp, hNode = m_PropTree.GetChildItem(m_hCurItem);
	while (hNode)
	{
		hTemp = hNode;
		hNode = m_PropTree.GetNextSiblingItem(hNode);
		m_PropTree.DeleteItem(hTemp);
	}

	//	Create new property nodes
	CreateGeneralProps(m_hCurItem, m_pCurShader);

	m_pCurShader->bModified = true;
}

//	General Z compare property
void CPropertyForm::OnGenZcompare() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();
	CComboListDlg ValueDlg;

	int iValue = 0;

	if (General.dwMask & SGMF_ZFUNC)
		iValue = General.dwZFunc - 1;

	ValueDlg.Init(GND_ZFUNC, iValue);

	if (ValueDlg.DoModal() == IDOK)
	{
		iValue = ValueDlg.GetSelectedItem();

		General.dwMask |= SGMF_ZFUNC;
		General.dwZFunc = iValue + 1;

		UpdateGeneralProp(GND_ZFUNC, General);
	}
}

//	General cull mode property
void CPropertyForm::OnGenCullmode() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();
	CComboListDlg ValueDlg;

	int iValue = 0;

	if (General.dwMask & SGMF_CULLMODE)
		iValue = General.dwCullMode - 1;

	ValueDlg.Init(GND_CULLMODE, iValue);

	if (ValueDlg.DoModal() == IDOK)
	{
		iValue = ValueDlg.GetSelectedItem();

		General.dwMask	  |= SGMF_CULLMODE;
		General.dwCullMode = iValue + 1;

		UpdateGeneralProp(GND_CULLMODE, General);
	}
}

//	General Z compare enable property
void CPropertyForm::OnGenZenable() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();
	CComboListDlg ValueDlg;

	int iValue = 0;

	if (General.dwMask & SGMF_ZENABLE)
		iValue = General.bZEnable;

	ValueDlg.Init(GND_ZENABLE, iValue);

	if (ValueDlg.DoModal() == IDOK)
	{
		iValue = ValueDlg.GetSelectedItem();

		General.dwMask	|= SGMF_ZENABLE;
		General.bZEnable = iValue ? true : false;

		UpdateGeneralProp(GND_ZENABLE, General);
	}
}

//	General Z write enable property
void CPropertyForm::OnGenZwrite() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();
	CComboListDlg ValueDlg;

	int iValue = 0;

	if (General.dwMask & SGMF_ZWENABLE)
		iValue = General.bZWriteEnable;

	ValueDlg.Init(GND_ZWENABLE, iValue);

	if (ValueDlg.DoModal() == IDOK)
	{
		iValue = ValueDlg.GetSelectedItem();

		General.dwMask		 |= SGMF_ZWENABLE;
		General.bZWriteEnable = iValue ? true : false;

		UpdateGeneralProp(GND_ZWENABLE, General);
	}
}

//	General alpha compare property
void CPropertyForm::OnGenAlphacompare() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();
	CAlphaCompDlg CompDlg;
	
	if (General.dwMask & SGMF_ALPHACOMP)
		CompDlg.Init(General.dwAlphaFunc-1, General.iAlphaRef);
	else
		CompDlg.Init(0, 128);

	if (CompDlg.DoModal() == IDOK)
	{
		General.dwMask	   |= SGMF_ALPHACOMP;
		General.dwAlphaFunc	= CompDlg.GetSelectedFunc() + 1;
		General.iAlphaRef	= CompDlg.GetRefValue();

		UpdateGeneralProp(GND_ALPHACOMP, General);
	}
}

//	General blend property
void CPropertyForm::OnGenBlend() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();
	CGenBlendDlg BlendDlg;

	if (General.dwMask & SGMF_BLEND)
		BlendDlg.Init(General.dwSrcBlend-1, General.dwDstBlend-1);
	else
		BlendDlg.Init(0, 0);

	if (BlendDlg.DoModal() == IDOK)
	{
		General.dwMask	  |= SGMF_BLEND;
		General.dwSrcBlend = BlendDlg.GetSrcBlend() + 1;
		General.dwDstBlend = BlendDlg.GetDstBlend() + 1;

		UpdateGeneralProp(GND_BLEND, General);
	}
}

//	General color generate
void CPropertyForm::OnGenColorgen() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();
	SHADERWAVE Wave;
	CWaveDlg WaveDlg;

	memset(&Wave, 0, sizeof (Wave));
	Wave.iFunc = SWFT_SIN;

	if (General.dwMask & SGMF_COLORGEN)
		WaveDlg.Init("Color generate", General.rgbGen);
	else
		WaveDlg.Init("Color generate", Wave);

	if (WaveDlg.DoModal() == IDOK)
	{
		General.dwMask |= SGMF_COLORGEN;
		WaveDlg.GetData(General.rgbGen);

		UpdateGeneralProp(GND_COLORGEN, General);
	}
}

//	General alpha generate
void CPropertyForm::OnGenAlphagen() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();
	SHADERWAVE Wave;
	CWaveDlg WaveDlg;

	memset(&Wave, 0, sizeof (Wave));
	Wave.iFunc = SWFT_SIN;

	if (General.dwMask & SGMF_ALPHAGEN)
		WaveDlg.Init("Alpha generate", General.AlphaGen);
	else
		WaveDlg.Init("Alpha generate", Wave);

	if (WaveDlg.DoModal() == IDOK)
	{
		General.dwMask |= SGMF_ALPHAGEN;
		WaveDlg.GetData(General.AlphaGen);

		UpdateGeneralProp(GND_ALPHAGEN, General);
	}
}

//	Set texture factor
void CPropertyForm::OnGenTfactor() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();
	A3DCOLORVALUE col(General.dwTFactor);

	CDlgColorValue ColDlg(col);
	if (ColDlg.DoModal() == IDOK)
	{
		col = ColDlg.GetColorValue();
		General.dwTFactor = col.ToRGBAColor();
		General.dwMask |= SGMF_TFACTOR;

		UpdateGeneralProp(GND_TFACTOR, General);
	}
}

//	Set alpha texture flag
void CPropertyForm::OnGenAlphatex() 
{
	bool bAlpha = m_pCurShader->pShader->IsAlphaTexture();

	CDlgComboVal dlg;
	dlg.AddCandidate("true");
	dlg.AddCandidate("false");
	dlg.SetDefaultSel(bAlpha ? 0 : 1);
	dlg.SetDescString("设置 shader 的半透明标志");
	if (dlg.DoModal())
	{
		bAlpha = dlg.GetCurSel() == 0 ? true : false;
		m_pCurShader->pShader->SetAlphaTextureFlag(bAlpha);

		UpdateGeneralProp(GND_ALPHATEX, m_pCurShader->pShader->GetGeneralProps());
	}
}

//	Set pixel shader
void CPropertyForm::OnGenPixelshader() 
{
	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();
	CPixelShaderDlg ShaderDlg;
	
	if (General.pPShader)
		ShaderDlg.Init(General.pPShader->GetFileName());
	else
		ShaderDlg.Init(NULL);

	if (ShaderDlg.DoModal() == IDOK)
	{
		if (ShaderDlg.UsePixelShader())
		{
			if (!m_pCurShader->pShader->ChangePixelShader(ShaderDlg.GetShaderFile()))
				MessageBox("设置 pixel shader 失败!");
		}
		else	//	Not use pixel shader
			m_pCurShader->pShader->ChangePixelShader(NULL);

		//	Get new data again
		General = m_pCurShader->pShader->GetGeneralProps();
		UpdateGeneralProp(GND_PIXELSHADER, General);
	}
}

//	General property edit
void CPropertyForm::OnGpropEdit() 
{
	if (!m_hCurItem)
		return;

	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	switch (dwID)
	{
	case GND_ZFUNC:			OnGenZcompare();		break;
	case GND_ZENABLE:		OnGenZenable();			break;
	case GND_ZWENABLE:		OnGenZwrite();			break;
	case GND_ALPHACOMP:		OnGenAlphacompare();	break;
	case GND_CULLMODE:		OnGenCullmode();		break;
	case GND_BLEND:			OnGenBlend();			break;
	case GND_COLORGEN:		OnGenColorgen();		break;
	case GND_ALPHAGEN:		OnGenAlphagen();		break;
	case GND_TFACTOR:		OnGenTfactor();			break;
	case GND_PIXELSHADER:	OnGenPixelshader();		break;
	case GND_ALPHATEX:		OnGenAlphatex();		break;
	default:
		ASSERT(0);
		break;
	}
}

//	General property remove
void CPropertyForm::OnGpropRemove() 
{
	if (!m_hCurItem)
		return;

	//	Release pixel shader at first
	m_pCurShader->pShader->ChangePixelShader(NULL);

	SHADERGENERAL General = m_pCurShader->pShader->GetGeneralProps();

	DWORD dwID, dwFlag;
	dwID	= m_PropTree.GetItemData(m_hCurItem);
	dwFlag	= NodeIDToFlag(dwID);

	General.dwMask &= ~dwFlag;

	m_pCurShader->pShader->SetGeneralProps(General);

	m_PropTree.DeleteItem(m_hCurItem);

	m_pCurShader->bModified = true;
}

//	Show or hide selected stage
void CPropertyForm::OnStgHide() 
{
	if (!m_hCurItem)
		return;

	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);

	ASSERT(iIndex >= 0 && iIndex < m_pCurShader->pShader->GetStageNum());

	if (m_pCurShader->pShader->StageIsVisible(iIndex))
	{
		m_pCurShader->pShader->ShowStage(iIndex, false);

		//	Change node's icon
		m_PropTree.SetItemImage(m_hCurItem, IMG_UNSELECTED, IMG_UNSELECTED);
	}
	else
	{
		m_pCurShader->pShader->ShowStage(iIndex, true);

		//	Change node's icon
		m_PropTree.SetItemImage(m_hCurItem, IMG_SELECTED, IMG_SELECTED);
	}
}

//	Copy a stage to clipboard
void CPropertyForm::OnStgCopy() 
{
	int iIndex = GETSTAGEFROMNODEID(m_PropTree.GetItemData(m_hCurItem));
	
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iIndex);

	if (!g_Clipboard.SetData(CBDT_STAGE, iIndex, (void*) &Stage))
		MessageBox("操作失败!");
}

//	Cut a stage to clipboard
void CPropertyForm::OnStgCut() 
{
	OnStgCopy();
	OnStgRemove();
}

//	Paste a stage
void CPropertyForm::OnStgPaste() 
{
	//	Check data type
	if (!g_Clipboard.MatchType(CBDT_STAGE))
		return;

	HTREEITEM hPreNode = m_PropTree.GetPrevSiblingItem(m_hCurItem);
	if (!hPreNode)
		return;

	int iIndex = GETSTAGEFROMNODEID(m_PropTree.GetItemData(m_hCurItem));

	//	Remove old stage
	OnStgRemove();

	//	Insert new stage
	if (!InsertStage(hPreNode))
		return;

	g_Clipboard.GetData(CBDT_STAGE, iIndex, (void*) m_pCurShader->pShader);

	//	Update stage's properties
	HTREEITEM hStage = m_PropTree.GetNextSiblingItem(hPreNode);
	CreateStageProps(hStage, m_pCurShader, iIndex);
}

//	Add a stage before selected stage
void CPropertyForm::OnStgAddbefore() 
{
	HTREEITEM hPreNode = m_PropTree.GetPrevSiblingItem(m_hCurItem);
	if (!hPreNode)
		return;

	InsertStage(hPreNode);
}

//	Add a stage after selected stage
void CPropertyForm::OnStgAddafter() 
{
	InsertStage(m_hCurItem);
}

//	Move stage up
void CPropertyForm::OnStgMoveup() 
{
	//	Get current stage's index
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	if (dwID <= ND_STAGE0PROPS)
		return;		//	This stage has been the first stage

	char szTitle[64];
	int iIndex = GETSTAGEFROMNODEID(dwID);

	//	Delete previous stage node
	HTREEITEM hStage = m_PropTree.GetPrevSiblingItem(m_hCurItem);
	if (hStage)
		m_PropTree.DeleteItem(hStage);

	//	Create a new stage node after current stage
	sprintf(szTitle, "Stage %d", iIndex);

	//	Create stage property root node
	hStage = m_PropTree.InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE, 
								   szTitle, IMG_SELECTED, IMG_SELECTED, 
								   TVIS_BOLD | TVIS_EXPANDED, TVIS_BOLD | TVIS_EXPANDED,
								   MAKESTAGENODEID(iIndex), TVI_ROOT, m_hCurItem);

	CreateStageProps(hStage, m_pCurShader, iIndex-1);

	//	Change current stage node's name and ID
	sprintf(szTitle, "Stage %d", iIndex-1);
	m_PropTree.SetItemText(m_hCurItem, szTitle);
	m_PropTree.SetItemData(m_hCurItem, MAKESTAGENODEID(iIndex-1));

	//	Exchange stage's data
	SHADERSTAGE Stage1, Stage2;
	Stage1 = m_pCurShader->pShader->GetStage(iIndex-1);
	Stage2 = m_pCurShader->pShader->GetStage(iIndex);

	m_pCurShader->pShader->SetStage(iIndex-1, Stage2);
	m_pCurShader->pShader->SetStage(iIndex, Stage1);

	m_pCurShader->bModified = true;
}

//	Move stage down
void CPropertyForm::OnStgMovedown() 
{
	//	Get current stage's index
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	if ((int)GETSTAGEFROMNODEID(dwID) >= m_pCurShader->pShader->GetStageNum()-1)
		return;		//	This stage has been the last stage
	
	char szTitle[64];
	int iIndex = GETSTAGEFROMNODEID(dwID);

	//	Delete following stage node
	HTREEITEM hStage = m_PropTree.GetNextSiblingItem(m_hCurItem);
	if (hStage)
		m_PropTree.DeleteItem(hStage);

	//	Get previous stage node's handle
	hStage = m_PropTree.GetPrevSiblingItem(m_hCurItem);

	//	Create a new stage node before current stage
	sprintf(szTitle, "Stage %d", iIndex);

	//	Create stage property root node
	hStage = m_PropTree.InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE, 
								   szTitle, IMG_SELECTED, IMG_SELECTED, 
								   TVIS_BOLD | TVIS_EXPANDED, TVIS_BOLD | TVIS_EXPANDED,
								   MAKESTAGENODEID(iIndex), TVI_ROOT, hStage);

	CreateStageProps(hStage, m_pCurShader, iIndex+1);

	//	Change current stage node's name and ID
	sprintf(szTitle, "Stage %d", iIndex+1);
	m_PropTree.SetItemText(m_hCurItem, szTitle);
	m_PropTree.SetItemData(m_hCurItem, MAKESTAGENODEID(iIndex+1));

	//	Exchange stage's data
	SHADERSTAGE Stage1, Stage2;
	Stage1 = m_pCurShader->pShader->GetStage(iIndex);
	Stage2 = m_pCurShader->pShader->GetStage(iIndex+1);

	m_pCurShader->pShader->SetStage(iIndex, Stage2);
	m_pCurShader->pShader->SetStage(iIndex+1, Stage1);

	m_pCurShader->bModified = true;
}

//	Remove a stage
void CPropertyForm::OnStgRemove() 
{
	char szTitle[64];
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);
	
	m_pCurShader->pShader->RemoveStage(iIndex);

	//	Change all stage node's title and ID after this one
	HTREEITEM hStage = m_PropTree.GetNextSiblingItem(m_hCurItem);
	while (hStage)
	{
		sprintf(szTitle, "Stage %d", iIndex);
		m_PropTree.SetItemText(hStage, szTitle);
		m_PropTree.SetItemData(hStage, MAKESTAGENODEID(iIndex));

		iIndex++;

		hStage = m_PropTree.GetNextSiblingItem(hStage);
	}

	//	Remove stage node from tree
	m_PropTree.DeleteItem(m_hCurItem);

	m_pCurShader->bModified = true;
}

//	Edit stage texture
void CPropertyForm::OnStgTexture() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgTexture(iIndex);
}

//	Set stage's texture
void CPropertyForm::DoStgTexture(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	TEXTUREDLGPARAMS Params;
	memset(&Params, 0, sizeof (Params));

	Params.iType	= Stage.iTexType;
	Params.iFormat	= (int)Stage.TexFormat;
	Params.iD3DSlot	= -1;

	int iNumTex = Stage.aTextures.GetSize();
	if (iNumTex)
	{
		if (Params.iType == A3DSDTEX_NORMAL)
		{
			A3DTexture* pTexture = (A3DTexture*)Stage.aTextures[0];
			strcpy(Params.szFile, pTexture->GetMapFile());
		}
		else if (Params.iType == A3DSDTEX_ANIMATION)
		{
			Params.iNumFrame	= iNumTex;
			Params.dwFrameTime	= Stage.dwFrameTime;

			A3DTexture* pTexture = (A3DTexture*)Stage.aTextures[0];
			strcpy(Params.szFile, pTexture->GetMapFile());
		}
		else if (Params.iType == A3DSDTEX_D3DTEX)
		{
			A3DTextureProxy* pTexProxy = (A3DTextureProxy*)Stage.aTextures[0];
			Params.iD3DSlot = glb_SearchD3DTexSlot(pTexProxy);
		}
	}

	CTextureDlg TexDlg;

	TexDlg.Init(Params);

	if (TexDlg.DoModal() == IDOK)
	{
		TexDlg.GetData(Params);

		DWORD dwTex = 0;
		int iNumTex = 1;

		if (Params.iType == A3DSDTEX_NORMAL || 
			Params.iType == A3DSDTEX_ANIMATION)
		{
			dwTex = (DWORD)&Params.szFile[0];

			if (Params.iType == A3DSDTEX_ANIMATION)
				iNumTex = Params.iNumFrame;

			//	Change texture
			if (Params.bTexChanged)
			{
				m_pCurShader->pShader->ChangeStageTexture(iStage, (A3DSHADERTEXTYPE)Params.iType, dwTex, 
						(A3DFORMAT)Params.iFormat, iNumTex);
			}
		}
		else if (Params.iType == A3DSDTEX_D3DTEX)
		{
			if (Params.bTexChanged && Params.iD3DSlot >= 0)
			{
				A3DTextureProxy* pTexProxy = g_aD3DTexs[Params.iD3DSlot].pTexProxy;
				m_pCurShader->pShader->ChangeStageTexture_D3DTex(iStage, pTexProxy);
			}
		}

		//	Get new data and update tree
		Stage = m_pCurShader->pShader->GetStage(iStage);
		Stage.dwFrameTime = Params.dwFrameTime;
		UpdateStageProp(iStage, SND_TEXTURE, Stage);
	}
}

//	Set texture filter mode
void CPropertyForm::OnStgFilter() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgFilter(iIndex);
}

//	Set texture filter mode
void CPropertyForm::DoStgFilter(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	CComboListDlg ValueDlg;

	int iValue = 0;

	if (Stage.dwMask & SSMF_FILTER)
		iValue = Stage.dwFilter;

	ValueDlg.Init(SND_FILTER, iValue);

	if (ValueDlg.DoModal() == IDOK)
	{
		Stage.dwMask  |= SSMF_FILTER;
		Stage.dwFilter = ValueDlg.GetSelectedItem();

		UpdateStageProp(iStage, SND_FILTER, Stage);
	}
}

//	Set texture address mode
void CPropertyForm::OnStgAddress() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgAddress(iIndex);
}

//	Set texture address mode
void CPropertyForm::DoStgAddress(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	CComboListDlg ValueDlg;

	int iValue = 0;

	if (Stage.dwMask & SSMF_ADDRESS)
		iValue = Stage.dwAddress - 1;

	ValueDlg.Init(SND_ADDRESS, iValue);

	if (ValueDlg.DoModal() == IDOK)
	{
		Stage.dwMask   |= SSMF_ADDRESS;
		Stage.dwAddress = ValueDlg.GetSelectedItem() + 1;

		UpdateStageProp(iStage, SND_ADDRESS, Stage);
	}
}

//	Set bump-mapping matrix parameters
void CPropertyForm::OnStgBumpmat() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgBumpMat(iIndex);
}

//	Set bump-mapping matrix
void CPropertyForm::DoStgBumpMat(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	CBumpMatDlg MatDlg;

	if (Stage.dwMask & SSMF_BUMPMAT)
		MatDlg.Init(Stage.fBumpEnvMat00, Stage.fBumpEnvMat01, Stage.fBumpEnvMat10, Stage.fBumpEnvMat11);
	else
		MatDlg.Init(0.0f, 0.0f, 0.0f, 0.0f);

	if (MatDlg.DoModal() == IDOK)
	{
		Stage.dwMask |= SSMF_BUMPMAT;

		Stage.fBumpEnvMat00 = MatDlg.Get00();
		Stage.fBumpEnvMat01 = MatDlg.Get01();
		Stage.fBumpEnvMat10 = MatDlg.Get10();
		Stage.fBumpEnvMat11 = MatDlg.Get11();

		UpdateStageProp(iStage, SND_BUMPMAT, Stage);
	}
}

//	Set bump-mapping offset value
void CPropertyForm::OnStgBumpoffset() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgBumpOffset(iIndex);
}

//	Set bump-mapping offset value
void CPropertyForm::DoStgBumpOffset(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	CSingleValDlg ValDlg;

	if (Stage.dwMask & SSMF_BUMPOFFSET)
		ValDlg.Init("Bump-mapping luminance offset", Stage.fBumpEnvOffset);
	else
		ValDlg.Init("Bump-mapping luminance offset", 0.0f);

	if (ValDlg.DoModal() == IDOK)
	{
		Stage.dwMask |= SSMF_BUMPOFFSET;

		ValDlg.GetValue(&Stage.fBumpEnvOffset);

		UpdateStageProp(iStage, SND_BUMPOFFSET, Stage);
	}
}

//	Set bump-mapping scale value
void CPropertyForm::OnStgBumpscale() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgBumpScale(iIndex);
}

//	Set bump-mapping scale value
void CPropertyForm::DoStgBumpScale(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	CSingleValDlg ValDlg;

	if (Stage.dwMask & SSMF_BUMPSCALE)
		ValDlg.Init("Bump-mapping luminance scale", Stage.fBumpEnvScale);
	else
		ValDlg.Init("Bump-mapping luminance scale", 0.0f);

	if (ValDlg.DoModal() == IDOK)
	{
		Stage.dwMask |= SSMF_BUMPSCALE;

		ValDlg.GetValue(&Stage.fBumpEnvScale);

		UpdateStageProp(iStage, SND_BUMPSCALE, Stage);
	}
}

//	Set color operation
void CPropertyForm::OnStgAlphaop() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgAlphaOp(iIndex);
}

//	Set alpha operation
void CPropertyForm::DoStgAlphaOp(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	CTexOPDlg OPDlg;
	OPDlg.SetParam(iStage, CTexOPDlg::OP_ALPHA, Stage);
	OPDlg.DoModal();
}

//	Set alpha operation
void CPropertyForm::OnStgColorop() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgColorOp(iIndex);
}

//	Set color operation
void CPropertyForm::DoStgColorOp(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	CTexOPDlg OPDlg;
	OPDlg.SetParam(iStage, CTexOPDlg::OP_COLOR, Stage);
	OPDlg.DoModal();
}

//	Set result argument
void CPropertyForm::OnStgResultarg() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);
	DoStgResultArg(iIndex);
}

//	Set result argument
void CPropertyForm::DoStgResultArg(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	CComboListDlg ValueDlg;

	int iValue = 0;
	if (Stage.dwMask & SSMF_RESULTARG)
		iValue = Stage.dwResultArg == A3DTA_CURRENT ? 0 : 1;
	
	ValueDlg.Init(SND_RESULTARG, iValue);

	if (ValueDlg.DoModal() == IDOK)
	{
		Stage.dwMask	 |= SSMF_RESULTARG;
		Stage.dwResultArg = !ValueDlg.GetSelectedItem() ? A3DTA_CURRENT : A3DTA_TEMP;

		UpdateStageProp(iStage, SND_RESULTARG, Stage);
	}
}

void CPropertyForm::OnStgTexcoord() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);
	DoStgTexcoord(iIndex);
}

//	Set texture coordinates index
void CPropertyForm::DoStgTexcoord(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	DWORD dwIndex = 0;
	if (Stage.dwMask & SSMF_TEXCOORD)
		dwIndex = Stage.dwTexCoord;

	CDlgTexCoord TexCoordDlg(dwIndex);

	if (TexCoordDlg.DoModal() == IDOK)
	{
		Stage.dwMask |= SSMF_TEXCOORD;
		Stage.dwTexCoord = TexCoordDlg.GetTexCoord();

		UpdateStageProp(iStage, SND_TEXCOORD, Stage);
	}
}

void CPropertyForm::OnStgTextransform() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iIndex = GETSTAGEFROMNODEID(dwID);
	DoStgTextransform(iIndex);
}

//	Set texture transform matrix
void CPropertyForm::DoStgTextransform(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	DWORD dwFlag = 0;
	if (Stage.dwMask & SSMF_TEXTRANS)
		dwFlag = Stage.dwTTFF;

	A3DMATRIX4 mat(Stage.matTexTrans);

	CDlgTexTransform TexTransDlg(mat, dwFlag);

	if (TexTransDlg.DoModal() == IDOK)
	{
		Stage.dwMask |= SSMF_TEXTRANS;
		Stage.dwTTFF = TexTransDlg.GetFlag();
		mat = TexTransDlg.GetMatrix();

		for (int i=0; i < 16; i++)
		{
			int iRow = i >> 2;
			int iCol = i & 3;
			Stage.matTexTrans[i] = mat.m[iRow][iCol];
		}

		UpdateStageProp(iStage, SND_TEXTRANS, Stage);
	}
}

//	Set tc scale
void CPropertyForm::OnStgTcscale() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);

	if (dwID == ND_TCMODS)
	{
		HTREEITEM hNode = m_PropTree.GetParentItem(m_hCurItem);
		dwID = m_PropTree.GetItemData(hNode);
	}

	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgTCScale(iIndex);
}

//	Set tc scale
void CPropertyForm::DoStgTCScale(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	int i, iIndex;
	CTwoValDlg ValDlg;
	SHADERWAVE* pWave;

	for (i=0; i < Stage.iNumtcMod; i++)
	{
		if (Stage.atcMods[i].iType == SWO_TCSCALE)
			break;
	}

	if (i < Stage.iNumtcMod)	//	tc scale has existed
	{
		iIndex = i;
		pWave  = &Stage.atcMods[i];
		ValDlg.Init("TC scale", "u scale", "v scale", pWave->fBase, pWave->fAmp);
	}
	else
	{
		if (Stage.iNumtcMod >= A3DShader::MAXNUM_TCMOD)
		{
			MessageBox("不能再增加更多的 tc modify 属性");
			return;
		}

		iIndex = Stage.iNumtcMod;
		ValDlg.Init("TC scale", "u scale", "v scale", 0.0f, 0.0f);
	}

	if (ValDlg.DoModal() == IDOK)
	{
		pWave = &Stage.atcMods[iIndex];
		pWave->iType = SWO_TCSCALE;
		ValDlg.GetValue(&pWave->fBase, &pWave->fAmp);

		if (iIndex == Stage.iNumtcMod)
			Stage.iNumtcMod++;	//	Add a new tc modifier

		UpdateStageProp(iStage, ND_TCMODS, Stage);
	}
}

//	Set tc scroll
void CPropertyForm::OnStgTcscroll() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);

	if (dwID == ND_TCMODS)
	{
		HTREEITEM hNode = m_PropTree.GetParentItem(m_hCurItem);
		dwID = m_PropTree.GetItemData(hNode);
	}

	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgTCScroll(iIndex);
}

//	Set tc scroll
void CPropertyForm::DoStgTCScroll(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	int i, iIndex;
	CTwoValDlg ValDlg;
	SHADERWAVE* pWave;

	for (i=0; i < Stage.iNumtcMod; i++)
	{
		if (Stage.atcMods[i].iType == SWO_TCSCROLL)
			break;
	}

	if (i < Stage.iNumtcMod)	//	tc scroll has existed
	{
		iIndex = i;
		pWave  = &Stage.atcMods[i];
		ValDlg.Init("TC scroll", "u scroll", "v scroll", pWave->fBase, pWave->fAmp);
	}
	else
	{
		if (Stage.iNumtcMod >= A3DShader::MAXNUM_TCMOD)
		{
			MessageBox("不能再增加更多的 tc modify 属性");
			return;
		}

		iIndex = Stage.iNumtcMod;
		ValDlg.Init("TC scroll", "u scroll", "v scroll", 0.0f, 0.0f);
	}

	if (ValDlg.DoModal() == IDOK)
	{
		pWave = &Stage.atcMods[iIndex];
		pWave->iType = SWO_TCSCROLL;
		ValDlg.GetValue(&pWave->fBase, &pWave->fAmp);

		if (iIndex == Stage.iNumtcMod)
			Stage.iNumtcMod++;	//	Add a new tc modifier

		UpdateStageProp(iStage, ND_TCMODS, Stage);
	}
}

//	Set tc rotate
void CPropertyForm::OnStgTcrotate() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);

	if (dwID == ND_TCMODS)
	{
		HTREEITEM hNode = m_PropTree.GetParentItem(m_hCurItem);
		dwID = m_PropTree.GetItemData(hNode);
	}

	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgTCRotate(iIndex);
}

//	Set tc rotate
void CPropertyForm::DoStgTCRotate(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	int i, iIndex;
	CSingleValDlg ValDlg;
	SHADERWAVE* pWave;

	for (i=0; i < Stage.iNumtcMod; i++)
	{
		if (Stage.atcMods[i].iType == SWO_TCROTATE)
			break;
	}

	if (i < Stage.iNumtcMod)	//	tc rotate has existed
	{
		iIndex = i;
		pWave  = &Stage.atcMods[i];
		ValDlg.Init("TC rotate",  RAD2DEG(pWave->fBase));
	}
	else
	{
		if (Stage.iNumtcMod >= A3DShader::MAXNUM_TCMOD)
		{
			MessageBox("不能再增加更多的 tc modify 属性");
			return;
		}

		iIndex = Stage.iNumtcMod;
		ValDlg.Init("TC rotate", 0.0f);
	}

	if (ValDlg.DoModal() == IDOK)
	{
		pWave = &Stage.atcMods[iIndex];
		pWave->iType = SWO_TCROTATE;
		ValDlg.GetValue(&pWave->fBase);
		pWave->fBase = DEG2RAD(pWave->fBase);

		if (iIndex == Stage.iNumtcMod)
			Stage.iNumtcMod++;	//	Add a new tc modifier

		UpdateStageProp(iStage, ND_TCMODS, Stage);
	}
}

//	Set tc stretch
void CPropertyForm::OnStgTcstretch() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);

	if (dwID == ND_TCMODS)
	{
		HTREEITEM hNode = m_PropTree.GetParentItem(m_hCurItem);
		dwID = m_PropTree.GetItemData(hNode);
	}

	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgTCStretch(iIndex);
}

//	Set stretch
void CPropertyForm::DoStgTCStretch(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	int i, iIndex;
	SHADERWAVE Wave;
	CWaveDlg WaveDlg;

	for (i=0; i < Stage.iNumtcMod; i++)
	{
		if (Stage.atcMods[i].iType == SWO_TCSTRETCH)
			break;
	}

	if (i < Stage.iNumtcMod)	//	tc rotate has existed
	{
		iIndex	= i;
		Wave	= Stage.atcMods[i];
		WaveDlg.Init("TC stretch", Wave);
	}
	else
	{
		if (Stage.iNumtcMod >= A3DShader::MAXNUM_TCMOD)
		{
			MessageBox("不能再增加更多的 tc modify 属性");
			return;
		}

		iIndex = Stage.iNumtcMod;

		memset(&Wave, 0, sizeof (Wave));
		Wave.iFunc = SWFT_SIN;
		WaveDlg.Init("TC stretch", Wave);
	}

	if (WaveDlg.DoModal() == IDOK)
	{
		WaveDlg.GetData(Wave);
		Wave.iType = SWO_TCSTRETCH;

		Stage.atcMods[iIndex] = Wave;

		if (iIndex == Stage.iNumtcMod)
			Stage.iNumtcMod++;	//	Add a new tc modifier

		UpdateStageProp(iStage, ND_TCMODS, Stage);
	}
}

//	Set tc turbulence
void CPropertyForm::OnStgTcturb() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);

	if (dwID == ND_TCMODS)
	{
		HTREEITEM hNode = m_PropTree.GetParentItem(m_hCurItem);
		dwID = m_PropTree.GetItemData(hNode);
	}

	int iIndex = GETSTAGEFROMNODEID(dwID);

	DoStgTCTurb(iIndex);
}

//	Set stretch
void CPropertyForm::DoStgTCTurb(int iStage)
{
	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iStage);

	int i, iIndex;
	SHADERWAVE Wave;
	CWaveDlg WaveDlg;

	for (i=0; i < Stage.iNumtcMod; i++)
	{
		if (Stage.atcMods[i].iType == SWO_TCTURB)
			break;
	}

	if (i < Stage.iNumtcMod)	//	tc rotate has existed
	{
		iIndex	= i;
		Wave	= Stage.atcMods[i];
		WaveDlg.Init("TC turbulence", Wave);
	}
	else
	{
		if (Stage.iNumtcMod >= A3DShader::MAXNUM_TCMOD)
		{
			MessageBox("不能再增加更多的 tc modify 属性");
			return;
		}

		iIndex = Stage.iNumtcMod;

		memset(&Wave, 0, sizeof (Wave));
		Wave.iType = SWO_TCTURB;
		WaveDlg.Init("TC turbulence", Wave);
	}

	if (WaveDlg.DoModal() == IDOK)
	{
		WaveDlg.GetData(Wave);
		Wave.iType = SWO_TCTURB;

		Stage.atcMods[iIndex] = Wave;

		if (iIndex == Stage.iNumtcMod)
			Stage.iNumtcMod++;	//	Add a new tc modifier

		UpdateStageProp(iStage, ND_TCMODS, Stage);
	}
}

//	Edit one stage property
void CPropertyForm::OnSpropEdit() 
{
	HTREEITEM hStage = m_PropTree.GetParentItem(m_hCurItem);
	
	DWORD dwStageID = m_PropTree.GetItemData(hStage);
	int iIndex = GETSTAGEFROMNODEID(dwStageID);

	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);

	switch (dwID)
	{
	case SND_TEXTURE:		DoStgTexture(iIndex);		break;
	case SND_FILTER:		DoStgFilter(iIndex);		break;
	case SND_ADDRESS:		DoStgAddress(iIndex);		break;
	case SND_BUMPMAT:		DoStgBumpMat(iIndex);		break;
	case SND_BUMPSCALE:		DoStgBumpScale(iIndex);		break;
	case SND_BUMPOFFSET:	DoStgBumpOffset(iIndex);	break;
	case SND_RESULTARG:		DoStgResultArg(iIndex);		break;
	case SND_TEXCOORD:		DoStgTexcoord(iIndex);		break;
	case SND_TEXTRANS:		DoStgTextransform(iIndex);	break;

	case SND_COLOROP:
	case SND_COLORARG1:
	case SND_COLORARG2:		DoStgColorOp(iIndex);		break;

	case SND_ALPHAOP:
	case SND_ALPHAARG1:
	case SND_ALPHAARG2:		DoStgAlphaOp(iIndex);		break;

	case SND_COLORGEN:
	case SND_ALPHAGEN:
	case SND_ALPHACOMP:

		break;
	}
}

//	Remove a stage property
void CPropertyForm::OnSpropRemove() 
{
	if (!m_hCurItem)
		return;

	int iIndex;
	DWORD dwID, dwStageID, dwFlag;

	HTREEITEM hStage = m_PropTree.GetParentItem(m_hCurItem);

	dwStageID	= m_PropTree.GetItemData(hStage);
	iIndex		= GETSTAGEFROMNODEID(dwStageID);

	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iIndex);
	
	dwID	= m_PropTree.GetItemData(m_hCurItem);
	dwFlag	= NodeIDToFlag(dwID);

	if (dwID == SND_TEXTURE)	//	Texture property cannot be removed
	{
		char szMsg[128];
		LoadString(GetModuleHandle(NULL), IDS_CANNOTDELPROP, szMsg, 128);
		MessageBox(szMsg);
		return;
	}

	Stage.dwMask &= ~dwFlag;

	m_pCurShader->pShader->SetStage(iIndex, Stage);

	m_PropTree.DeleteItem(m_hCurItem);

	if (dwFlag == SSMF_COLOROP || dwFlag == SSMF_ALPHAOP)
	{
		//	Remove all relative nodes at same time
		DWORD dwID1, dwID2, dwID3;

		if (dwFlag == SSMF_COLOROP)
		{
			dwID1 = SND_COLOROP;
			dwID2 = SND_COLORARG1;
			dwID3 = SND_COLORARG2;
		}
		else
		{
			dwID1 = SND_ALPHAOP;
			dwID2 = SND_ALPHAARG1;
			dwID3 = SND_ALPHAARG2;
		}

		HTREEITEM hTemp, hItem = m_PropTree.GetChildItem(hStage);
		while (hItem)
		{
			hTemp = hItem;
			dwID  = m_PropTree.GetItemData(hItem);
			hItem = m_PropTree.GetNextSiblingItem(hItem);

			if (dwID == dwID1 || dwID == dwID2 || dwID == dwID3)
				m_PropTree.DeleteItem(hTemp);
		}
	}

	m_pCurShader->bModified = true;
}

//	Edit one tc modifier property
void CPropertyForm::OnTcmpEdit() 
{
	HTREEITEM hStage = m_PropTree.GetParentItem(m_hCurItem);
	hStage = m_PropTree.GetParentItem(hStage);
	
	DWORD dwStageID = m_PropTree.GetItemData(hStage);
	int iIndex = GETSTAGEFROMNODEID(dwStageID);

	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	switch (dwID)
	{
	case TCND_SCALE:	DoStgTCScale(iIndex);		break;
	case TCND_SCROLL:	DoStgTCScroll(iIndex);		break;
	case TCND_ROTATE:	DoStgTCRotate(iIndex);		break;
	case TCND_STRETCH:	DoStgTCStretch(iIndex);		break;
	case TCND_TURB:		DoStgTCTurb(iIndex);		break;
	}
}

//	Remove one tc modifier property
void CPropertyForm::OnTcmpRemove() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iType  = (int)NodeIDToFlag(dwID);

	//	Get stage's index
	HTREEITEM hStage = m_PropTree.GetParentItem(m_hCurItem);
	hStage = m_PropTree.GetParentItem(hStage);
	int i, iIndex = GETSTAGEFROMNODEID(m_PropTree.GetItemData(hStage));

	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iIndex);

	for (i=0; i < Stage.iNumtcMod; i++)
	{
		if (Stage.atcMods[i].iType == iType)
			break;
	}

	if (i >= Stage.iNumtcMod)	//	Didn't find the property, shouldn't happen
		return;

	//	Delete this property
	for (; i < Stage.iNumtcMod-1; i++)
		Stage.atcMods[i] = Stage.atcMods[i+1];

	Stage.atcMods[Stage.iNumtcMod-1].iType = 0;
	Stage.iNumtcMod--;

	//	Update data
	UpdateStageProp(iIndex, ND_TCMODS, Stage);
}

//	Move up selected tc modifier property one position 
void CPropertyForm::OnTcmpMoveup() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iType  = (int)NodeIDToFlag(dwID);

	//	Get stage's index
	HTREEITEM hStage = m_PropTree.GetParentItem(m_hCurItem);
	hStage = m_PropTree.GetParentItem(hStage);
	int i, iIndex = GETSTAGEFROMNODEID(m_PropTree.GetItemData(hStage));

	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iIndex);

	for (i=0; i < Stage.iNumtcMod; i++)
	{
		if (Stage.atcMods[i].iType == iType)
			break;
	}

	if (i >= Stage.iNumtcMod)	//	Didn't find the property, shouldn't happen
		return;

	if (!i)		//	This has been the first tcmod
		return;

	//	Exchange property i and i-1
	SHADERWAVE Wave		= Stage.atcMods[i-1];
	Stage.atcMods[i-1]	= Stage.atcMods[i];
	Stage.atcMods[i]	= Wave;

	//	Update data
	UpdateStageProp(iIndex, ND_TCMODS, Stage);
}

//	Move down selected tc modifier property one position 
void CPropertyForm::OnTcmpMovedown() 
{
	DWORD dwID = m_PropTree.GetItemData(m_hCurItem);
	int iType  = (int)NodeIDToFlag(dwID);

	//	Get stage's index
	HTREEITEM hStage = m_PropTree.GetParentItem(m_hCurItem);
	hStage = m_PropTree.GetParentItem(hStage);
	int i, iIndex = GETSTAGEFROMNODEID(m_PropTree.GetItemData(hStage));

	SHADERSTAGE Stage = m_pCurShader->pShader->GetStage(iIndex);

	for (i=0; i < Stage.iNumtcMod; i++)
	{
		if (Stage.atcMods[i].iType == iType)
			break;
	}

	if (i >= Stage.iNumtcMod)	//	Didn't find the property, shouldn't happen
		return;

	if (i == Stage.iNumtcMod-1)	//	This has been the last tcmod
		return;

	//	Exchange property i and i+1
	SHADERWAVE Wave		= Stage.atcMods[i+1];
	Stage.atcMods[i+1]	= Stage.atcMods[i];
	Stage.atcMods[i]	= Wave;

	//	Update data
	UpdateStageProp(iIndex, ND_TCMODS, Stage);
}

