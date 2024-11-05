// MainForm.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "MainForm.h"
#include "NewShaderDlg.h"
#include "DlgSelectFile.h"
#include "DlgColorValue.h"
#include "DlgD3DTextures.h"
#include <A3D.h>
#include <AF.h>

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
//	CMainForm
//
///////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMainForm, CFrameWnd)

CMainForm::CMainForm()
{
	m_pShaderForm	= NULL;
	m_pPropForm		= NULL;
}

CMainForm::~CMainForm()
{
}


BEGIN_MESSAGE_MAP(CMainForm, CFrameWnd)
	//{{AFX_MSG_MAP(CMainForm)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_FILE_EXIT, OnUpdateFileExit)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_SETWORKPATH, OnFileSetworkpath)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEALL, OnUpdateFileSaveall)
	ON_UPDATE_COMMAND_UI(ID_FILE_SETWORKPATH, OnUpdateFileSetworkpath)
	ON_COMMAND(ID_FILE_SAVEALL, OnFileSaveall)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_OPT_BGCOLOR, OnOptBgcolor)
	ON_COMMAND(ID_OPT_BGTEXTURE, OnOptBgtexture)
	ON_COMMAND(ID_OPT_VERTCOLOR, OnOptVertcolor)
	ON_COMMAND(ID_OPT_D3DTEXTURE, OnOptD3dtexture)
	ON_WM_ENTERIDLE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SYSCOMMAND, OnSysCommand)
	ON_MESSAGE(WM_SELECTCHANGE, OnSelectChange)
	ON_MESSAGE(WM_ADDONESTAGE, OnAddOneStage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//	CMainForm message handlers
//
/////////////////////////////////////////////////////////////////////////////

void CMainForm::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	
	g_bActive = bActive ? true : false;
}

BOOL CMainForm::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if (!m_SplitWnd.CreateStatic(this, 1, 2))
	{
		TRACE("CMainForm::OnCreateClient, Failed to create split window\n");
		return FALSE;
	}

	if (!m_SplitWnd.CreateView(0, 0, RUNTIME_CLASS(CShaderForm), CSize(200, 300), pContext))
	{
		TRACE("CMainForm::OnCreateClient, Failed to create shader form\n");
		return FALSE;
	}

	if (!m_SplitWnd.CreateView(0, 1, RUNTIME_CLASS(CPropertyForm), CSize(200, 300), pContext))
	{
		TRACE("CMainForm::OnCreateClient, Failed to create property form\n");
		return FALSE;
	}
	
	if (!(m_pShaderForm = GetShaderForm()))
	{
		TRACE("CMainForm::OnCreateClient, Cannot find shader form\n");
		return FALSE;
	}

	if (!(m_pPropForm = GetPropertyForm()))
	{
		TRACE("CMainForm::OnCreateClient, Cannot find property form\n");
		return FALSE;
	}

	//	Own initializatoin function
	if (!m_pShaderForm->Initialize())
	{
		TRACE("CMainForm::OnCreateClient, Cannot initialize shader form\n");
		return FALSE;
	}

	//	Own initialization function
	if (!m_pPropForm->Initialize())
	{
		TRACE("CMainForm::OnCreateClient, Cannot initialize property form\n");
		return FALSE;
	}

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

/*	Get shader form.

	Return shader form's address for success, otherwise return NULL
*/
CShaderForm* CMainForm::GetShaderForm()
{
	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild)
	{
		if (pChild->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
			break;

		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}

	if (!pChild)
		return NULL;

	pChild = pChild->GetWindow(GW_CHILD);
	while (pChild)
	{
		if (pChild->IsKindOf(RUNTIME_CLASS(CShaderForm)))
			return (CShaderForm*)pChild;

		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}

	return NULL;
}

/*	Get property form.

	Return property form's address for success, otherwise return NULL
*/
CPropertyForm* CMainForm::GetPropertyForm()
{
	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild)
	{
		if (pChild->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
			break;

		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}

	if (!pChild)
		return NULL;

	pChild = pChild->GetWindow(GW_CHILD);
	while (pChild)
	{
		if (pChild->IsKindOf(RUNTIME_CLASS(CPropertyForm)))
			return (CPropertyForm*)pChild;

		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}

	return NULL;
}

int CMainForm::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//	Initialize render window
	if (!InitRenderWnd())
		return -1;

	//	Initialize menu's state
	InitMainMenuState();
	
	return 0;
}

//	Initialize main menu's state
void CMainForm::InitMainMenuState()	
{
	memset(&g_MainMenuUpdate, 1, sizeof (g_MainMenuUpdate));
	memset(&g_GenMenuUpdate, 1, sizeof (g_GenMenuUpdate));
	memset(&g_ShaderMenuUpdate, 1, sizeof (g_ShaderMenuUpdate));
	memset(&g_StageMenuUpdate, 1, sizeof (g_StageMenuUpdate));
}

//	Initialize render window.
//	Return true for success, otherwise return false.
bool CMainForm::InitRenderWnd()
{
	CRect Rect(100, 100, 300, 300);

	if (!m_RenderWnd.Create(NULL, "Preview", WS_POPUP | WS_CAPTION, Rect, this, NULL))
	{
		TRACE("CMainForm::InitRenderWnd, Failed to create render window.\n");
		return false;
	}

	//	Adjust window's size so that it's client area size is RENDERAREA_WIDTH * RENDERAREA_HEIGHT
	Rect.SetRect(0, 0, RENDERAREA_WIDTH, RENDERAREA_HEIGHT);
	::AdjustWindowRect(&Rect, WS_POPUP, FALSE);
	OffsetRect(&Rect, 100, 300);
	m_RenderWnd.MoveWindow(&Rect, FALSE);

	if (!m_RenderWnd.InitA3DEngine(GetModuleHandle(NULL)))
	{
		TRACE("CMainForm::InitRenderWnd, Failed to initialize A3D engine.\n");
		return false;
	}

	m_RenderWnd.ShowWindow(SW_SHOW);

	return true;
}

void CMainForm::OnDestroy() 
{
	//	Release global D3D textures
	for (int i=0; i < g_aD3DTexs.GetSize(); i++)
	{
		D3DTEXTURE& tex = g_aD3DTexs[i];
		if (tex.pTexProxy)
		{
			IDirect3DTexture8* pTexture = tex.pTexProxy->GetD3DTexture();
			if (pTexture)
				pTexture->Release();

			delete tex.pTexProxy;
		}
	}

	g_aD3DTexs.RemoveAll();

	//	Release child windows
	m_pShaderForm->Release();
	m_pPropForm->Release();

	m_RenderWnd.ReleaseA3DEngine();	

	CFrameWnd::OnDestroy();
}

//	Create a new shader
void CMainForm::OnFileNew() 
{
	CNewShaderDlg NewDlg;
	
	if (NewDlg.DoModal() == IDOK)
	{
		if (!m_pShaderForm->NewShader((char*)(LPCTSTR)NewDlg.m_strName, (char*)(LPCTSTR)NewDlg.m_strFile))
			MessageBox("创建新 Shader 失败。");
	}
}

//	Open a shader file
void CMainForm::OnFileOpen() 
{
	CString strPath = g_szWorkDir;
	strPath += "Shaders";

	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= true;
	Params.szFileFilter	= "*.sdr";
	Params.szPrompt		= "打开一个或多个 Shader 文件";
	Params.szRootDir	= strPath;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;

	CString strFile;
	DWORD pos = FileDlg.GetStartPosition();
	while (pos)
	{
		strFile = FileDlg.GetNextFullPathName(&pos);

		if (!m_pShaderForm->AddShader(strFile))
		{
			char szMsg[256];
			sprintf(szMsg, "Failed to open shader %s.", strFile);
			MessageBox(szMsg);
		}
	}
}

void CMainForm::OnFileSaveall() 
{
	m_pShaderForm->SaveAllShaders(false);
}

//	Set work path
void CMainForm::OnFileSetworkpath() 
{
	BROWSEINFO Info;
	LPITEMIDLIST pPIDL;
	char szPath[MAX_PATH];

	memset(&Info, 0, sizeof (Info));
	Info.hwndOwner		= m_hWnd;
	Info.pidlRoot		= NULL;
	Info.pszDisplayName	= szPath;
	Info.lpszTitle		= "请选择工作路径：";
	Info.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;

	if ((pPIDL = SHBrowseForFolder(&Info)))
	{
		SHGetPathFromIDList(pPIDL, szPath);

		//	Add a '\' at tail
		sprintf(g_szWorkDir, "%s\\", szPath);

		//	Save work path to .ini file
		WritePrivateProfileString("Path", "WorkPath", g_szWorkDir, g_szIniFile);

		//	Reset work path
		af_SetBaseDir(g_szWorkDir);
	}
}

//	Exit application
void CMainForm::OnFileExit() 
{
	//	Check whether all shaders have been saved
	if (!m_pShaderForm->SaveAllShaders(true))
		return;		//	User cancel this operation

	PostMessage(WM_CLOSE, 0, 0);
}

//	WM_SYSCOMMAND handler
void CMainForm::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == SC_CLOSE)
	{
		OnFileExit();
		return;
	}

	DefWindowProc(WM_SYSCOMMAND, wParam, lParam);
}

//	New shader was selected in ShaderForm
void CMainForm::OnSelectChange(WPARAM wParam, LPARAM lParam)
{
	SHADERINFO* pShaderInfo = (SHADERINFO*)wParam;

	m_pPropForm->ChangeShader(pShaderInfo);

	if (pShaderInfo)
		m_RenderWnd.ChangeShader(pShaderInfo->pShader);
	else
		m_RenderWnd.ChangeShader(NULL);
}

//	Add a stage to current shader
void CMainForm::OnAddOneStage(WPARAM wParam, LPARAM lParam)
{
	m_pPropForm->InsertStage(NULL);
}

//	Select background color
void CMainForm::OnOptBgcolor() 
{
	CColorDialog ColorDlg;
	BYTE r, g, b;
	DWORD dwCol = m_RenderWnd.GetBackColor();

	r = A3DCOLOR_GETRED(dwCol);
	g = A3DCOLOR_GETGREEN(dwCol);
	b = A3DCOLOR_GETBLUE(dwCol);

	ColorDlg.m_cc.rgbResult = RGB(r, g, b);
	ColorDlg.m_cc.Flags    |= CC_RGBINIT;

	if (ColorDlg.DoModal() == IDOK)
	{
		dwCol = ColorDlg.GetColor();
		r = GetRValue(dwCol);
		g = GetGValue(dwCol);
		b = GetBValue(dwCol);

		m_RenderWnd.SetBackColor(A3DCOLORRGBA(r, g, b, 255));
	}
}

//	Change background texture
void CMainForm::OnOptBgtexture() 
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

	if (!m_RenderWnd.ChangeBackgroundTexture(FileDlg.GetRelativePathName()))
		MessageBox("贴图加载失败！");
}

//	Set decal vertex color
void CMainForm::OnOptVertcolor() 
{
	CColorDialog ColorDlg;
	BYTE r, g, b;
	DWORD dwCol = m_RenderWnd.GetBackColor();

	r = A3DCOLOR_GETRED(dwCol);
	g = A3DCOLOR_GETGREEN(dwCol);
	b = A3DCOLOR_GETBLUE(dwCol);

	ColorDlg.m_cc.rgbResult = RGB(r, g, b);
	ColorDlg.m_cc.Flags    |= CC_RGBINIT;

	if (ColorDlg.DoModal() == IDOK)
	{
		dwCol = ColorDlg.GetColor();
		r = GetRValue(dwCol);
		g = GetGValue(dwCol);
		b = GetBValue(dwCol);

		m_RenderWnd.SetVertColor(A3DCOLORRGBA(r, g, b, 255));
	}
}

void CMainForm::OnOptD3dtexture()
{
	CDlgD3DTextures dlg;
	dlg.DoModal();
}

void CMainForm::OnEnterIdle(UINT nWhy, CWnd* pWho) 
{
	CFrameWnd::OnEnterIdle(nWhy, pWho);
	m_RenderWnd.Render();
}
