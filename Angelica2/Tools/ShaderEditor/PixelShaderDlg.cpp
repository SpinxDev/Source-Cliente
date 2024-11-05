// PixelShaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "PixelShaderDlg.h"
#include "DlgSelectFile.h"
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
//	Implement CPixelShaderDlg
//
///////////////////////////////////////////////////////////////////////////

CPixelShaderDlg::CPixelShaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPixelShaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPixelShaderDlg)
	m_bUsePShader = FALSE;
	m_strPrompt = _T("");
	m_strPixelFile = _T("");
	//}}AFX_DATA_INIT
}


void CPixelShaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPixelShaderDlg)
	DDX_Control(pDX, IDC_CONTENT, m_Content);
	DDX_Check(pDX, IDC_USEPIXELSHADER, m_bUsePShader);
	DDX_Text(pDX, IDC_PROMPT, m_strPrompt);
	DDX_Text(pDX, IDC_EDIT_PIXELFILE, m_strPixelFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPixelShaderDlg, CDialog)
	//{{AFX_MSG_MAP(CPixelShaderDlg)
	ON_BN_CLICKED(IDC_USEPIXELSHADER, OnUsepixelshader)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_COMPILE, OnCompile)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
//
//	CPixelShaderDlg message handlers
//
///////////////////////////////////////////////////////////////////////////

//	Initialize data
bool CPixelShaderDlg::Init(const char* szFileName)
{
	if (szFileName)
	{
		m_bUsePShader	= true;
		m_strPixelFile	= szFileName;
	}
	else
	{
		m_bUsePShader	= false;
		m_strPixelFile	= "";
	}

	return true;
}

BOOL CPixelShaderDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_strPrompt = "";

	OnUsepixelshader();

	if (m_strPixelFile != "")
		OpenPixelShaderFile(m_strPixelFile);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPixelShaderDlg::OnUsepixelshader() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_USEPIXELSHADER, m_bUsePShader);

	if (m_bUsePShader)
	{
		GetDlgItem(IDC_BTN_BROWSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CONTENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMPILE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_BROWSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONTENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMPILE)->EnableWindow(FALSE);
	}
}

void CPixelShaderDlg::OnOK() 
{
	UpdateData(TRUE);

	CDialog::OnOK();
}

//	Open a pixel shader file and load it's content
bool CPixelShaderDlg::OpenPixelShaderFile(const char *szFileName)
{
	if (!szFileName || !szFileName[0])
		return false;

	char szFile[MAX_PATH];
	sprintf(szFile, "%s%s", g_szWorkDir, szFileName);

	FILE* fp = fopen(szFile, "rb");
	if (!fp)
	{
		TRACE("CPixelShaderDlg::OpenPixelShaderFile, failed to open file %s\n", szFile);
		return false;
	}

	int iLen;
	char* szText;

	//	Get file length
	fseek(fp, 0, SEEK_END);
	iLen = ftell(fp);

	if (!(szText = (char*)malloc(iLen + 1)))
	{
		TRACE("CPixelShaderDlg::OpenPixelShaderFile, not enough memory\n");
		fclose(fp);
		return false;
	}

	//	Read whole file
	fseek(fp, 0, SEEK_SET);
	fread(szText, 1, iLen, fp);

	szText[iLen] = 0;

	fclose(fp);

	//	Output file to edit box
	m_Content.SetWindowText(szText);

	free(szText);

	return true;
}

void CPixelShaderDlg::OnSave() 
{
	if (m_strPixelFile == "")
		return;

	int iLen = m_Content.GetWindowTextLength();
	if (!iLen)
		return;

	char* szText = (char*)malloc(iLen + 1);
	if (!szText)
	{
		MessageBox("内存不足");
		return;
	}

	//	Get content
	m_Content.GetWindowText(szText, iLen+1);

	//	Open file to save
	char szFile[MAX_PATH];
	sprintf(szFile, "%s\\%s", g_szWorkDir, m_strPixelFile);

	FILE* fp = fopen(szFile, "wb+");
	if (!fp)
	{
		free(szText);

		char szMsg[300];
		sprintf(szMsg, "无法创建文件 %s", szFile);
		MessageBox(szMsg);
		return;
	}

	fprintf(fp, "%s", szText);

	fclose(fp);
	free(szText);

	Prompt("保存完毕!");
}

//	Test whether selected pixel shader can be compiled by D3D
void CPixelShaderDlg::OnCompile() 
{
	if (m_strPixelFile == "")
		return;

	char szFile[MAX_PATH];
	sprintf(szFile, "%s\\%s", g_szWorkDir, m_strPixelFile);

	A3DPixelShader PixelShader;

	if (!PixelShader.Init(g_pA3DEngine->GetA3DDevice()))
	{
		TRACE("CPixelShaderDlg::OnCompile, failed to initialize pixel shader\n");
		return;
	}

	if (!PixelShader.Load(szFile, false))
		Prompt("测试失败!");
	else
		Prompt("测试通过!");

	PixelShader.Release();
}

//	Display prompt
void CPixelShaderDlg::Prompt(char* szPrompt)
{
	m_strPrompt = szPrompt;

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_PROMPT, m_strPrompt);
}

void CPixelShaderDlg::OnBtnBrowse() 
{
	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= false;
	Params.szFileFilter	= "*.*";
	Params.szPrompt		= "请选择一个 Pixel Shader 文件";
	Params.szRootDir	= g_szWorkDir;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;

	m_strPixelFile = FileDlg.GetRelativePathName();

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_PIXELFILE, m_strPixelFile);

	//	Open pixel file
	OpenPixelShaderFile(m_strPixelFile);
}


