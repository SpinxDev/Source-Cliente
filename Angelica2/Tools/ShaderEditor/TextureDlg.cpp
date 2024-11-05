// TextureDlg.cpp : implementation file
//

#include "CommonDef.h"
#include "TextureDlg.h"
#include "DlgSelectFile.h"
#include "DlgD3DTextures.h"

#define MAXNUM_ANIMTEXTURE	64

///////////////////////////////////////////////////////////////////////////
//
//	CTextureDlg dialog
//
///////////////////////////////////////////////////////////////////////////

CTextureDlg::CTextureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextureDlg)
	m_iFrameTime = 0;
	m_iNumFrame = 1;
	m_strTexFile = _T("");
	//}}AFX_DATA_INIT

	memset(&m_Params, 0, sizeof (m_Params));
}


void CTextureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextureDlg)
	DDX_Control(pDX, IDC_TEXFORMATLIST, m_FormatCombo);
	DDX_Control(pDX, IDC_TEXTYPELIST, m_TypeCombo);
	DDX_Text(pDX, IDC_FRAMETIME, m_iFrameTime);
	DDX_Text(pDX, IDC_NUMFRAME, m_iNumFrame);
	DDV_MinMaxInt(pDX, m_iNumFrame, 0, 32);
	DDX_Text(pDX, IDC_EDIT_TEXTUREFILE, m_strTexFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextureDlg, CDialog)
	//{{AFX_MSG_MAP(CTextureDlg)
	ON_CBN_SELCHANGE(IDC_TEXTYPELIST, OnSelchangeTextypelist)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_CBN_SELCHANGE(IDC_TEXFORMATLIST, OnSelchangeTexformatlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
//
//	CTextureDlg message handlers
//
///////////////////////////////////////////////////////////////////////////

/*	Initialize data.

	Return true for success, otherwise return false.

	Params: data needed by this dialog
*/
bool CTextureDlg::Init(TEXTUREDLGPARAMS& Params)
{
	m_Params.iType			= Params.iType;
	m_Params.iNumFrame		= Params.iNumFrame;
	m_Params.dwFrameTime	= Params.dwFrameTime;
	m_Params.iFormat		= Params.iFormat;
	m_Params.iD3DSlot		= Params.iD3DSlot;
	m_Params.bTexChanged	= false;

	if (Params.iType != A3DSDTEX_D3DTEX)
		strcpy(m_Params.szFile, Params.szFile);
	else if (Params.iD3DSlot >= 0)
		strcpy(m_Params.szFile, g_aD3DTexs[Params.iD3DSlot].szFile);

	return true;
}

/*	Get data

	Params: object used to receive data
*/
void CTextureDlg::GetData(TEXTUREDLGPARAMS& Params)
{
	Params.iType		= m_Params.iType;
	Params.iNumFrame	= m_Params.iNumFrame;
	Params.dwFrameTime	= m_Params.dwFrameTime;
	Params.bTexChanged	= m_Params.bTexChanged;
	Params.iFormat		= m_Params.iFormat;
	Params.iD3DSlot		= m_Params.iD3DSlot;

	strcpy(Params.szFile, m_Params.szFile);
}

BOOL CTextureDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int i, iCurFmt = 0;

	//	Initialize texture type list
	for (i=0; i < NUM_TEXTYPESTR; i++)
		m_TypeCombo.InsertString(-1, g_aTexTypeStrs[i]);

	//	Initialize texture format list
	for (i=0; i < NUM_TEXFORMATSTR; i++)
	{
		m_FormatCombo.InsertString(-1, g_aFormatStrs[i].szName);

		if ((int)g_aFormatStrs[i].Fmt == m_Params.iFormat)
			iCurFmt = i;
	}

	//	Select current type
	m_TypeCombo.SetCurSel(m_Params.iType);
	OnSelchangeTextypelist();

	//	Select current format
	m_FormatCombo.SetCurSel(iCurFmt);

	m_strTexFile	= m_Params.szFile;
	m_iNumFrame		= m_Params.iNumFrame;
	m_iFrameTime	= m_Params.dwFrameTime;

	//	This has been changed by OnSelchangeTextypelist(), restore it
	m_Params.bTexChanged = false;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	Texture type has been changed
void CTextureDlg::OnSelchangeTextypelist()
{
	int iCurSel = m_TypeCombo.GetCurSel();
	if (iCurSel == CB_ERR)
		return;

	if (iCurSel == A3DSDTEX_ANIMATION)
	{
		GetDlgItem(IDC_NUMFRAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_FRAMETIME)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_NUMFRAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAMETIME)->EnableWindow(FALSE);
	}

	m_Params.bTexChanged = true;
}

void CTextureDlg::OnOK() 
{
	UpdateData(TRUE);

	//	Texture type
	int iType = m_TypeCombo.GetCurSel();
	if (iType == A3DSDTEX_NONE)
	{
		MessageBox("必须选择一个贴图类型。");
		return;
	}

	if (iType == A3DSDTEX_NORMAL || 
		iType == A3DSDTEX_ANIMATION || 
		iType == A3DSDTEX_D3DTEX)
	{
		if (m_strTexFile == "")
		{
			MessageBox("必须选择一个贴图文件。");
			return;
		}
	}

	//	Texture file name
	if (m_strTexFile != "")
		strcpy(m_Params.szFile, m_strTexFile);
	else
		m_Params.szFile[0] = '\0';

	//	Animation texture properties
	if (iType == A3DSDTEX_ANIMATION)
	{
		if (m_iNumFrame <= 0 || m_iNumFrame > MAXNUM_ANIMTEXTURE)
		{
			MessageBox("帧数必须在 [1, 64] 区间内。");
			return;
		}
		else if (m_iFrameTime < 0)
		{
			MessageBox("请输入 >= 0 的帧时间。");
			return;
		}

		AnimTextureFileName(m_Params.szFile);
	}

	//	Texture format
	int iFormat = m_FormatCombo.GetCurSel();

	m_Params.iType			= iType;
	m_Params.iNumFrame		= m_iNumFrame;
	m_Params.dwFrameTime	= m_iFrameTime;
	m_Params.iFormat		= (int)g_aFormatStrs[iFormat].Fmt;

	CDialog::OnOK();
}

void CTextureDlg::OnBtnBrowse() 
{
	int iType = m_TypeCombo.GetCurSel();

	if (iType != A3DSDTEX_D3DTEX)
	{
		CDlgSelectFile::PARAMS Params;
		Params.bMultiSelect	= false;
		Params.szFileFilter	= "*.*";
		Params.szPrompt		= "请选择一个贴图文件";
		Params.szRootDir	= g_szWorkDir;

		CDlgSelectFile FileDlg(Params);
		if (FileDlg.DoModal() != IDOK)
			return;

		m_strTexFile = FileDlg.GetRelativePathName();
	}
	else
	{
		CDlgD3DTextures dlg;
		dlg.SetCurSelSlot(m_Params.iD3DSlot);

		int iSlot = -1;
		if (dlg.DoModal() != IDOK || (iSlot = dlg.GetCurSelSlot()) < 0)
			return;

		m_Params.iD3DSlot = iSlot;
		m_strTexFile = g_aD3DTexs[iSlot].szFile;
	}

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_TEXTUREFILE, m_strTexFile);

	m_Params.bTexChanged = true;
}

void CTextureDlg::OnSelchangeTexformatlist() 
{
	m_Params.bTexChanged = true;
}
