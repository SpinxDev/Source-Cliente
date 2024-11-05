// ShaderForm.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "ShaderForm.h"
#include "MainForm.h"
#include "SaveAskDlg.h"
#include "SingleValDlg.h"
#include "A3D.h"
#include "AF.h"

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
//	CShaderForm
//
///////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CShaderForm, CFormView)

CShaderForm::CShaderForm()
	: CFormView(CShaderForm::IDD)
{
	//{{AFX_DATA_INIT(CShaderForm)
	//}}AFX_DATA_INIT

	m_hShaderMenu = NULL;
}

CShaderForm::~CShaderForm()
{
}

void CShaderForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShaderForm)
	DDX_Control(pDX, IDC_SHADERLIST, m_ShaderList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShaderForm, CFormView)
	//{{AFX_MSG_MAP(CShaderForm)
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_SHADERLIST, OnSelchangeShaderlist)
	ON_COMMAND(ID_SHADER_RENAME, OnShaderRename)
	ON_COMMAND(ID_SHADER_REMOVE, OnShaderRemove)
	ON_COMMAND(ID_SHADER_ADDSTAGE, OnShaderAddstage)
	ON_COMMAND(ID_SHADER_SAVE, OnShaderSave)
	ON_COMMAND(ID_SHADER_SAVEAS, OnShaderSaveas)
	ON_COMMAND(ID_SHADER_COPY, OnShaderCopy)
	ON_COMMAND(ID_SHADER_PASTE, OnShaderPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShaderForm diagnostics

#ifdef _DEBUG
void CShaderForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CShaderForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// 
//	CShaderForm message handlers
//
/////////////////////////////////////////////////////////////////////////////

void CShaderForm::OnSize(UINT nType, int cx, int cy) 
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

	if (!m_ShaderList.GetSafeHwnd())
		return;

	m_ShaderList.SetWindowPos(NULL, 0, 20, cx, cy-20, SWP_NOZORDER);
}

/*	Create a new empty shader.

	Return true for success, otherwise return false.

	szName: shader's name.
	szFile: shader file's name
*/
bool CShaderForm::NewShader(char* szName, char* szFile)
{
	int iIndex;
	SHADERINFO* pShaderInfo = NULL;

	A3DShader* pShader = new A3DShader;
	if (!pShader)
	{
		TRACE("CShaderForm::NewShader, not enough memory.\n");
		return false;
	}

	if (!pShader->Init(g_pA3DEngine))
	{
		TRACE("CShaderForm::NewShader, Failed to initialze shader object.\n");
		goto Error;
	}

	if (!(pShaderInfo = (SHADERINFO*)malloc(sizeof (SHADERINFO))))
	{
		TRACE("CShaderForm::NewShader, not enough memory.\n");
		goto Error;
	}

	memset(pShaderInfo, 0, sizeof (SHADERINFO));
	pShaderInfo->pShader = pShader;

	strcpy(pShaderInfo->szFile, szFile);

	//	Set shader's name
	pShader->SetName(szName);

	iIndex = m_ShaderList.InsertString(0, szName);
	if (iIndex == LB_ERR)
	{
		TRACE("CShaderForm::NewShader, Failed to insert shader %s into list.\n", szFile);
		goto Error;
	}

	m_ShaderList.SetItemData(iIndex, (DWORD)pShaderInfo);

	return true;

Error:

	if (pShader)
	{
		pShader->Release();
		delete pShader;
	}

	if (pShaderInfo)
		free(pShaderInfo);

	return false;
}

/*	Add a shader.

	Return true for success, otherwise return false.

	szFileName: shader file's name
*/
bool CShaderForm::AddShader(const char* szFileName)
{
	int iIndex;
	SHADERINFO* pShaderInfo = NULL;

	A3DShader* pShader = new A3DShader;
	if (!pShader)
	{
		TRACE("CShaderForm::AddShader, not enough memory.\n");
		return false;
	}

	if (!pShader->Init(g_pA3DEngine))
	{
		TRACE("CShaderForm::AddShader, Failed to initialze shader object.\n");
		goto Error;
	}

	if (!pShader->Load(szFileName))
	{
		TRACE("CShaderForm::AddShader, Failed to load data of shader %s.\n", szFileName);
		goto Error;
	}

	if (!(pShaderInfo = (SHADERINFO*)malloc(sizeof (SHADERINFO))))
	{
		TRACE("CShaderForm::AddShader, not enough memory.\n");
		goto Error;
	}

	memset(pShaderInfo, 0, sizeof (SHADERINFO));
	pShaderInfo->pShader = pShader;

	strcpy(pShaderInfo->szFile, szFileName);

	//	Allocate a item from shader list and bind it with this shader
	iIndex = m_ShaderList.InsertString(0, pShader->GetName());
	if (iIndex == LB_ERR)
	{
		TRACE("CShaderForm::AddShader, Failed to insert shader %s into list.\n", szFileName);
		goto Error;
	}

	m_ShaderList.SetItemData(iIndex, (DWORD)pShaderInfo);

	return true;

Error:

	if (pShader)
	{
		pShader->Release();
		delete pShader;
	}

	if (pShaderInfo)
		free(pShaderInfo);

	return false;
}

/*	Remove a shader.

	iIndex: shader's index in list.
*/
void CShaderForm::RemoveShader(int iIndex)
{
	int iNumShader = m_ShaderList.GetCount();
	if (iIndex < 0 || iIndex >= iNumShader)
		return;

	//	Release bound shader
	SHADERINFO* pShaderInfo = (SHADERINFO*)m_ShaderList.GetItemData(iIndex);

	if (pShaderInfo)
	{
		if (pShaderInfo->pShader)
		{
			pShaderInfo->pShader->Release();
			delete pShaderInfo->pShader;
		}

		free(pShaderInfo);
	}

	m_ShaderList.DeleteString(iIndex);
}

//	Remove all shaders
void CShaderForm::RemoveAllShaders()
{
	int i, iNumShader = m_ShaderList.GetCount();

	for (i=0; i < iNumShader; i++)
	{
		//	Release bound shader
		SHADERINFO* pShaderInfo = (SHADERINFO*)m_ShaderList.GetItemData(i);
		if (pShaderInfo->pShader)
		{
			pShaderInfo->pShader->Release();
			delete pShaderInfo->pShader;
		}

		free(pShaderInfo);
	}

	m_ShaderList.ResetContent();
}

//	Own initial routine
bool CShaderForm::Initialize()
{
	HINSTANCE hInst = GetModuleHandle(NULL);

	m_hShaderMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_SHADERMENU));

	return true;
}

//	Release all resources
void CShaderForm::Release()
{
	//	Release all shaders
	RemoveAllShaders();

	if (m_hShaderMenu)
	{
		DestroyMenu(m_hShaderMenu);
		m_hShaderMenu = NULL;
	}
}

/*	Get current shader's informatoin

	Return address of selected shader's information. If no shader is selected, NULL will
	be returned.
*/
SHADERINFO*	CShaderForm::GetCurrentShader()
{
	int iCurSel = m_ShaderList.GetCurSel();

	SHADERINFO* pShaderInfo = (SHADERINFO*)m_ShaderList.GetItemData(iCurSel);

	return pShaderInfo;
}

//	Another shader is selected
void CShaderForm::OnSelchangeShaderlist() 
{
	CMainForm* pMainWnd = (CMainForm*)AfxGetMainWnd();
	ASSERT(pMainWnd);

	pMainWnd->SendMessage(WM_SELECTCHANGE, (WPARAM)GetCurrentShader(), 0);
}

BOOL CShaderForm::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->hwnd == m_ShaderList.GetSafeHwnd() && pMsg->message == WM_RBUTTONDOWN)
	{
		POINT pt;
		pt.x = (WORD)(pMsg->lParam);
		pt.y = (WORD)(pMsg->lParam >> 16);

		BOOL bOut;
		RECT rcItem;
		int iIndex = m_ShaderList.ItemFromPoint(pt, bOut);

		m_ShaderList.GetItemRect(iIndex, &rcItem);

		if (!bOut && PtInRect(&rcItem, pt))
		{
			//	Force to change current shader
			m_ShaderList.SetCurSel(iIndex);
			OnSelchangeShaderlist();

			//	Popup shader menu
			m_ShaderList.ClientToScreen(&pt);

			HMENU hMenu = GetSubMenu(m_hShaderMenu, 0);
			TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON, 
						   pt.x, pt.y, 0, m_hWnd, NULL);
		}
	}
	
	return CFormView::PreTranslateMessage(pMsg);
}

//	Rename shader
void CShaderForm::OnShaderRename() 
{
	int iCurSel = m_ShaderList.GetCurSel();
	if (iCurSel == LB_ERR)
		return;

	SHADERINFO* pShaderInfo = (SHADERINFO*)m_ShaderList.GetItemData(iCurSel);

	//	Get old name
	CSingleValDlg ValDlg;
	ValDlg.Init("Shader 重命名", pShaderInfo->pShader->GetName(), LEN_SHADERNAME-1);

	if (ValDlg.DoModal() == IDOK)
	{
		char szName[LEN_SHADERNAME];
		ValDlg.GetValue(szName);
		pShaderInfo->pShader->SetName(szName);

		//	List box don't has a function to change item's string, so we have to
		//	Insert a new string and remove the old one
		m_ShaderList.InsertString(iCurSel, szName);
		m_ShaderList.SetItemData(iCurSel, (DWORD)pShaderInfo);
		m_ShaderList.SetCurSel(iCurSel);

		m_ShaderList.DeleteString(iCurSel+1);

		pShaderInfo->bModified = true;
	}
}

//	Remove current shader
void CShaderForm::OnShaderRemove() 
{
	int iCurSel = m_ShaderList.GetCurSel();
	if (iCurSel == LB_ERR)
		return;

	SHADERINFO* pShaderInfo = (SHADERINFO*)m_ShaderList.GetItemData(iCurSel);

	if (pShaderInfo->bModified)
	{
		if (MessageBox("该 Shader 已被修改，确实要不保存就删除吗？", "ShaderEditor", MB_YESNO) == IDNO)
			return;
	}

	//	Remove this shader
	RemoveShader(iCurSel);

	//	Notify property window and render window this change
	CMainForm* pMainWnd = (CMainForm*)AfxGetMainWnd();
	ASSERT(pMainWnd);

	pMainWnd->SendMessage(WM_SELECTCHANGE, NULL, 0);
}

//	Add to stage to current shader
void CShaderForm::OnShaderAddstage() 
{
	CMainForm* pMainWnd = (CMainForm*)AfxGetMainWnd();
	ASSERT(pMainWnd);

	pMainWnd->SendMessage(WM_ADDONESTAGE, 0, 0);
}

//	Save selected shader
void CShaderForm::OnShaderSave() 
{
	int iCurSel = m_ShaderList.GetCurSel();
	if (iCurSel == LB_ERR)
		return;

	SaveOneShader(iCurSel);
}

//	Save selected shader to another file
void CShaderForm::OnShaderSaveas() 
{
	char szFilter[] = "Shader 文件(*.sdr)|*.sdr|所有文件(*.*)|*.*||";
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	CFileDialog	FileDia(FALSE, "*.sdr", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDOK)
	{
		int iCurSel = m_ShaderList.GetCurSel();
		if (iCurSel == LB_ERR)
			return;

		//	Change shader file's name
		SHADERINFO* pShaderInfo = (SHADERINFO*)m_ShaderList.GetItemData(iCurSel);
		strcpy(pShaderInfo->szFile, (LPCTSTR)FileDia.GetPathName());

		SaveOneShader(iCurSel);
	}
}

/*	Save all shaders.

	Return true if this operation was completed, otherwise return false is user cancel
	this operation

	bAsk: true, popup a dialog when meet a unsaved shader to let user decide whether save
		  it. false, save all shaders whithout prompt.
*/
bool CShaderForm::SaveAllShaders(bool bAsk)
{
	CSaveAskDlg AskDlg;
	int i, iNumShader, iRet;
	SHADERINFO* pShaderInfo;

	if (!(iNumShader = m_ShaderList.GetCount()))
		return true;
	
	for (i=0; i < iNumShader; i++)
	{
		pShaderInfo = (SHADERINFO*)m_ShaderList.GetItemData(i);
	
		if (!pShaderInfo->bModified)
			continue;

		if (bAsk)
		{
			AskDlg.Init(pShaderInfo->pShader->GetName());
			iRet = AskDlg.DoModal();

			if (iRet == IDCANCEL)	//	User cancel this operation
				return false;
			else if (iRet == IDC_NOT)
				continue;
			else if (iRet == IDC_NOTTOALL)
				return true;
			else if (iRet == IDC_YESTOALL)
				bAsk = false;		//	Don't ask again
		}

		//	Save this shader
		if (!SaveOneShader(i))
			MessageBox("Failed to save shader %s", "shader name go here");
	}

	return true;
}

/*	Save one shader

	Return true for success, otherwise return false.

	iIndex: shader's index in shader list
*/
bool CShaderForm::SaveOneShader(int iIndex)
{
	SHADERINFO* pShaderInfo = (SHADERINFO*)m_ShaderList.GetItemData(iIndex);

	FILE* fp = fopen(pShaderInfo->szFile, "wr+");
	if (!fp)
	{
		TRACE("Failed to create file %s\n", pShaderInfo->szFile);
		return false;
	}

	//	Comment
	fprintf(fp, "//  ************* Angelica 3D shader ***************\n\n");

	//	Shader version
	fprintf(fp, "version %d\n\n", A3DShader::VERSION);

	//	Shader name and '{'
	fprintf(fp, "\"%s\"\n{\n", pShaderInfo->pShader->GetName());

	//	Commont
	fprintf(fp, "\t//  General properties\n");

	//	General propertyes
	SaveShaderGeneral(fp, pShaderInfo->pShader);

	//	Stages
	int i, iNumStage = pShaderInfo->pShader->GetStageNum();
	for (i=0; i < iNumStage; i++)
		SaveShaderStage(fp, i, pShaderInfo->pShader->GetStage(i));

	//	The last '}'
	fprintf(fp, "}\n");

	fclose(fp);

	pShaderInfo->bModified = false;

	return true;
}

/*	Save shader's general properties.

	Return true for success, otherwise return false.

	fp: file pointer
	pShader: shader object
*/
bool CShaderForm::SaveShaderGeneral(FILE* fp, A3DShader* pShader)
{
	SHADERGENERAL& General = pShader->GetGeneralProps();

	if (General.dwMask & SGMF_ZFUNC)
		fprintf(fp, "\t%-15s  %s\n", "depthfunc", g_aCompStrs[General.dwZFunc]);
	
	if (General.dwMask & SGMF_ZENABLE)
	{
		if (General.bZEnable)
			fprintf(fp, "\t%-15s  true\n", "zenable");
		else
			fprintf(fp, "\t%-15s  false\n", "zenable");
	}

	if (General.dwMask & SGMF_ZWENABLE)
	{
		if (General.bZWriteEnable)
			fprintf(fp, "\t%-15s  true\n", "zwriteenable");
		else
			fprintf(fp, "\t%-15s  false\n", "zwriteenable");
	}

	if (General.dwMask & SGMF_ALPHACOMP)
		fprintf(fp, "\t%-15s  %s  %d\n", "alphacomp", g_aCompStrs[General.dwAlphaFunc], General.iAlphaRef);

	if (General.dwMask & SGMF_CULLMODE)
		fprintf(fp, "\t%-15s  %s\n", "cullmode", g_aCullStrs[General.dwCullMode]);

	if (General.dwMask & SGMF_BLEND)
		fprintf(fp, "\t%-15s  %s  %s\n", "blend", g_aBlendStrs[General.dwSrcBlend], g_aBlendStrs[General.dwDstBlend]);

	if (General.dwMask & SGMF_COLORGEN)
	{
		SHADERWAVE* pWave = &General.rgbGen;
		fprintf(fp, "\t%-15s  %s  %f  %f  %f  %d\n", "colorgen", g_aWaveStrs[pWave->iFunc], 
				pWave->fBase, pWave->fAmp, pWave->fPhase, pWave->iPeriod);
	}

	if (General.dwMask & SGMF_ALPHAGEN)
	{
		SHADERWAVE* pWave = &General.AlphaGen;
		fprintf(fp, "\t%-15s  %s  %f  %f  %f  %d\n", "alphagen", g_aWaveStrs[pWave->iFunc], 
				pWave->fBase, pWave->fAmp, pWave->fPhase, pWave->iPeriod);
	}

	if (General.dwMask & SGMF_TFACTOR)
	{
		A3DCOLORVALUE col = General.dwTFactor;
		fprintf(fp, "\t%-15s  %f  %f  %f  %f\n", "texturefactor", col.r, col.g, col.b, col.a);
	}

	if (General.pPShader)
		fprintf(fp, "\t%-15s  \"%s\"\n", "pixelshader", General.pPShader->GetFileName());

	fprintf(fp, "\t%-15s  %d\n", "alphatexflag", pShader->IsAlphaTexture() ? 1 : 0);

	fprintf(fp, "\n");

	return true;
}

/*	Save one stage in shader.

	Return true for success, otherwise return false.

	fp: file pointer
	iStage: stage's index
	Stage: stage data
*/
bool CShaderForm::SaveShaderStage(FILE* fp, int iStage, SHADERSTAGE& Stage)
{
	//	Commont and '{'
	fprintf(fp, "\t//  Stage %d\n\t{\n", iStage);

	//	Texture's type
	if (Stage.iTexType)
	{
		int iNumTex = Stage.aTextures.GetSize();
		if (iNumTex)
		{
			char szText[256] = "Texture";

			if (Stage.iTexType == A3DSDTEX_NORMAL)
			{
				A3DTexture* pTexture = (A3DTexture*)Stage.aTextures[0];
				sprintf(szText, "\t\t%-15s  %s  %d  \"%s\"", "texture", g_aTexTypeStrs[Stage.iTexType],
						(int)Stage.TexFormat, pTexture->GetMapFile());
			}
			else if (Stage.iTexType == A3DSDTEX_ANIMATION)
			{
				A3DTexture* pTexture = (A3DTexture*)Stage.aTextures[0];
				char szFile[MAX_PATH];
				strcpy(szFile, pTexture->GetMapFile());
				AnimTextureFileName(szFile);

				sprintf(szText, "\t\t%-15s  %s  %d  %d  %u  \"%s\"", "texture", g_aTexTypeStrs[Stage.iTexType], 
						(int)Stage.TexFormat, iNumTex, Stage.dwFrameTime, szFile);
			}
			else if (Stage.iTexType == A3DSDTEX_D3DTEX)
			{
				sprintf(szText, "\t\t%-15s  %s", "texture", g_aTexTypeStrs[Stage.iTexType]);
			}
			else
			{
				//	TODO: handle other texture type
			}

			fprintf(fp, szText);
			fprintf(fp, "\n");
		}
	}

	if (Stage.dwMask & SSMF_FILTER)
		fprintf(fp, "\t\t%-15s  %s\n", "filter", g_aFilterStrs[Stage.dwFilter+1]);

	if (Stage.dwMask & SSMF_ADDRESS)
		fprintf(fp, "\t\t%-15s  %s\n", "address", g_aTexAddrStrs[Stage.dwAddress]);

	if (Stage.dwMask & SSMF_COLOROP)
	{
		fprintf(fp, "\t\t%-15s  %s\n", "colorop", g_aTexOpStrs[Stage.dwColorOp]);

		DWORD dwModIdx;

		//	Color argument 1
		if (Stage.dwColorArg1 & ~A3DTA_SELECTMASK)
		{
			dwModIdx = ModFlagToIndex(Stage.dwColorArg1 & ~A3DTA_SELECTMASK);
			fprintf(fp, "\t\t%-15s  %s  %s\n", "colorarg1", g_aTexArgStrs[(Stage.dwColorArg1 & A3DTA_SELECTMASK) + 1], g_aTexModStrs[dwModIdx]);
		}
		else
			fprintf(fp, "\t\t%-15s  %s\n", "colorarg1", g_aTexArgStrs[Stage.dwColorArg1+1]);

		//	Color argument 2
		if (Stage.dwColorArg2 & ~A3DTA_SELECTMASK)
		{
			dwModIdx = ModFlagToIndex(Stage.dwColorArg2 & ~A3DTA_SELECTMASK);
			fprintf(fp, "\t\t%-15s  %s  %s\n", "colorarg2", g_aTexArgStrs[(Stage.dwColorArg2 & A3DTA_SELECTMASK) + 1], g_aTexModStrs[dwModIdx]);
		}
		else
			fprintf(fp, "\t\t%-15s  %s\n", "colorarg2", g_aTexArgStrs[Stage.dwColorArg2+1]);
	}

	if (Stage.dwMask & SSMF_ALPHAOP)
	{
		fprintf(fp, "\t\t%-15s  %s\n", "alphaop", g_aTexOpStrs[Stage.dwAlphaOp]);

		DWORD dwModIdx;

		//	Alpha argument 1
		if (Stage.dwAlphaArg1 & ~A3DTA_SELECTMASK)
		{
			dwModIdx = ModFlagToIndex(Stage.dwAlphaArg1 & ~A3DTA_SELECTMASK);
			fprintf(fp, "\t\t%-15s  %s  %s\n", "alphaarg1", g_aTexArgStrs[(Stage.dwAlphaArg1 & A3DTA_SELECTMASK) + 1], g_aTexModStrs[dwModIdx]);
		}
		else
			fprintf(fp, "\t\t%-15s  %s\n", "alphaarg1", g_aTexArgStrs[Stage.dwAlphaArg1+1]);

		//	Alpha argument 2
		if (Stage.dwAlphaArg2 & ~A3DTA_SELECTMASK)
		{
			dwModIdx = ModFlagToIndex(Stage.dwAlphaArg2 & ~A3DTA_SELECTMASK);
			fprintf(fp, "\t\t%-15s  %s  %s\n", "alphaarg2", g_aTexArgStrs[(Stage.dwAlphaArg2 & A3DTA_SELECTMASK) + 1], g_aTexModStrs[dwModIdx]);
		}
		else
			fprintf(fp, "\t\t%-15s  %s\n", "alphaarg2", g_aTexArgStrs[Stage.dwAlphaArg2+1]);
	}

	if (Stage.dwMask & SSMF_ALPHACOMP)
		fprintf(fp, "\t\t%-15s  %s  %d\n", "alphacomp", g_aCompStrs[Stage.dwAlphaFunc], Stage.iAlphaRef);

	if (Stage.dwMask & SSMF_BUMPMAT)
		fprintf(fp, "\t\t%-15s  %f  %f  %f  %f\n", "bumpmat", Stage.fBumpEnvMat00, Stage.fBumpEnvMat01, Stage.fBumpEnvMat10, Stage.fBumpEnvMat11);

	if (Stage.dwMask & SSMF_BUMPSCALE)
		fprintf(fp, "\t\t%-15s  %f\n", "bumpscale", Stage.fBumpEnvScale);

	if (Stage.dwMask & SSMF_BUMPOFFSET)
		fprintf(fp, "\t\t%-15s  %f\n", "bumpoff", Stage.fBumpEnvOffset);

	if (Stage.dwMask & SSMF_RESULTARG)
	{
		int iIndex = Stage.dwResultArg == A3DTA_CURRENT ? 1 : 2;
		fprintf(fp, "\t\t%-15s  %s\n", "resultarg", g_aResultArgStrs[iIndex]);
	}

	if (Stage.dwMask & SSMF_TEXTRANS)
	{
		fprintf(fp, "\t\t%-15s  %d  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f  %f\n", "textrans", Stage.dwTTFF, 
			Stage.matTexTrans[0], Stage.matTexTrans[1], Stage.matTexTrans[2], Stage.matTexTrans[3],
			Stage.matTexTrans[4], Stage.matTexTrans[5], Stage.matTexTrans[6], Stage.matTexTrans[7],
			Stage.matTexTrans[8], Stage.matTexTrans[9], Stage.matTexTrans[10], Stage.matTexTrans[11],
			Stage.matTexTrans[12], Stage.matTexTrans[13], Stage.matTexTrans[14], Stage.matTexTrans[15]);
	}

	if (Stage.dwMask & SSMF_TEXCOORD)
	{
		int iIndex = Stage.dwTexCoord & 0x0000000F;
		int iFlag = 1;
		switch (Stage.dwTexCoord & 0x000F0000)
		{
		case A3DTCI_CAMERASPACENORMAL:				iFlag = 2;	break;
		case A3DTCI_CAMERASPACEPOSITION:			iFlag = 3;	break;
		case A3DTCI_CAMERASPACEREFLECTIONVECTOR:	iFlag = 4;	break;
		}

		fprintf(fp, "\t\t%-15s  %d  %s\n", "texcoord", iIndex, g_aTexCoordFlags[iFlag]);
	}

	//	Save tc modifiers
	SHADERWAVE* pWave;
	int i;

	for (i=0; i < Stage.iNumtcMod; i++)
	{
		pWave = &Stage.atcMods[i];
		switch (pWave->iType)
		{
		case SWO_TCSCALE:

			fprintf(fp, "\t\t%-15s  %f  %f\n", "tcscale", pWave->fBase, pWave->fAmp);
			break;

		case SWO_TCSCROLL:

			fprintf(fp, "\t\t%-15s  %f  %f\n", "tcscroll", pWave->fBase, pWave->fAmp);
			break;

		case SWO_TCROTATE:

			fprintf(fp, "\t\t%-15s  %f\n", "tcrotate", RAD2DEG(pWave->fBase));
			break;

		case SWO_TCSTRETCH:

			fprintf(fp, "\t\t%-15s  %s  %f  %f  %f  %d\n", "tcstretch", g_aWaveStrs[pWave->iFunc], 
					pWave->fBase, pWave->fAmp, pWave->fPhase, pWave->iPeriod);
			break;

		case SWO_TCTURB:

			fprintf(fp, "\t\t%-15s  %f  %f  %d\n", "tcturb", pWave->fAmp, pWave->fPhase, pWave->iPeriod);
			break;

		default:
			continue;	//	Ignore this property
		}
	}

	//	TODO: handle these...
	//	Stage.iProTexFunc
	//	Stage.iUVGenFunc
	//	Alpha and color generate...

	fprintf(fp, "\t}\n\n");

	return true;
}

//	Copy shader's data into clipboard
void CShaderForm::OnShaderCopy() 
{
	SHADERINFO* pShaderInfo = GetCurrentShader();

	if (!g_Clipboard.SetData(CBDT_SHADER, 0, (void*) pShaderInfo->pShader))
	{
		MessageBox("操作失败!");
		return;
	}
}

//	Paste shader's data from clipboard
void CShaderForm::OnShaderPaste() 
{
	if (!g_Clipboard.MatchType(CBDT_SHADER))
		return;

	SHADERINFO* pShaderInfo = GetCurrentShader();
	A3DShader* pShader = pShaderInfo->pShader;

	//	Release old data
	A3DRELEASE(pShader);

	if (!(pShader = new A3DShader))
	{
		pShaderInfo->pShader = NULL;
		return;
	}

	pShader->Init(g_pA3DEngine);

	//	Create empty stages
	SHADERSTAGE Stage;

	int i, iNumStage = g_Clipboard.GetStageNum();
	for (i=0; i < iNumStage; i++)
		pShader->InsertStage(-1, Stage);

	g_Clipboard.GetData(CBDT_SHADER, 0, (void*) pShader);

	//	Force to update properties tree control
	OnSelchangeShaderlist();
}

