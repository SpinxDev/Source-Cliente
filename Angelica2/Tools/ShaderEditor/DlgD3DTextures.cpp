// DlgD3DTextures.cpp : implementation file
//

#include "CommonDef.h"
#include "shadereditor.h"
#include "DlgD3DTextures.h"
#include "DlgSelectFile.h"

#include <A3DEngine.h>
#include <A3DDevice.h>
#include <A3DTexture.h>

/////////////////////////////////////////////////////////////////////////////
// CDlgD3DTextures dialog


CDlgD3DTextures::CDlgD3DTextures(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgD3DTextures::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgD3DTextures)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_iCurSelSlot = -1;
}


void CDlgD3DTextures::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgD3DTextures)
	DDX_Control(pDX, IDC_LIST_TEXTURES, m_TexList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgD3DTextures, CDialog)
	//{{AFX_MSG_MAP(CDlgD3DTextures)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgD3DTextures message handlers

BOOL CDlgD3DTextures::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//	Initialize texture list
	m_TexList.InsertColumn(0, "文件", LVCFMT_LEFT, 450);
	m_TexList.SetExtendedStyle(m_TexList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	for (int i=0; i < g_aD3DTexs.GetSize(); i++)
	{
		int iItem = m_TexList.InsertItem(i, g_aD3DTexs[i].szFile);
		m_TexList.SetItemData(iItem, i);
	}

	if (m_iCurSelSlot >= 0 && m_iCurSelSlot < g_aD3DTexs.GetSize())
		m_TexList.SetItemState(m_iCurSelSlot, LVIS_SELECTED, LVIS_SELECTED);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgD3DTextures::OnBtnBrowse() 
{
	CString strPath = g_szWorkDir;

	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= false;
	Params.szFileFilter	= "*.*";
	Params.szPrompt		= "选择一个贴图文件";
	Params.szRootDir	= strPath;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;

	LPDIRECT3DTEXTURE8 pD3DTex = NULL;
	if (D3DXCreateTextureFromFile(g_pA3DEngine->GetA3DDevice()->GetD3DDevice(), 
			FileDlg.GetFullPathName(), &pD3DTex) != D3D_OK)
	{
		MessageBox("贴图加载失败！");
		return;
	}

	D3DTEXTURE tex;
	strcpy(tex.szFile, FileDlg.GetRelativePathName());
	tex.pTexProxy = new A3DTextureProxy(g_pA3DEngine, pD3DTex);

	int iSlot = g_aD3DTexs.Add(tex);
	int iItem = m_TexList.InsertItem(m_TexList.GetItemCount(), tex.szFile);
	m_TexList.SetItemData(iItem, iSlot);
}

void CDlgD3DTextures::OnOK()
{
	if (!m_TexList.GetSelectedCount())
	{
		m_iCurSelSlot = -1;
	}
	else
	{
		POSITION pos = m_TexList.GetFirstSelectedItemPosition();
		int iItem = m_TexList.GetNextSelectedItem(pos);
		m_iCurSelSlot = m_TexList.GetItemData(iItem);
	}

	CDialog::OnOK();
}


