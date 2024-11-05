/**********************************************************************
 *<
	FILE: MOXExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "MOXExport.h"
#include "STDMAT.H"
#include "A3DTypes.h"
#include "A3DFuncs.h"
#include "A3DFrame.h"
#include "A3DModel.h"
#include "A3DFile.h"
#include "D3DRenderer.h"
#include "D3DWindow.h"
#include "DlgAsk4.h"
#include "Dummy.h"

#include <d3d8.h>
#include <io.h> 
#include <IPathConfigMgr.h>

#define MOXEXPORT_CLASS_ID	Class_ID(0x6df3ea5, 0x69221f32)

#define COUNTER_NUM	16

int g_nLastAsk4Answer = -1;
A3DFrame * g_pScene = NULL;

typedef struct _MOX_OPTION
{
	bool		bNormalKeep;
	bool		bCopyMap;
	bool		bTextureDir; // Put textures into texture dir;
	bool		bGenerateMod; // Generate a mod file at the same time
	bool		bForceAnimation; // Force frame animation;
	bool		bPutIntoOneFrame;
	bool		bAllowObjectAnim;
	char		szMapDir[MAX_PATH];
}MOX_OPTION, * PMOX_OPTION;

bool AGetFileTitle(LPCTSTR lpszFile, LPTSTR lpszTitle, WORD cbBuf);
bool AGetFilePath(LPCTSTR lpszFile, LPTSTR lpszPath, WORD cbBuf);

class MOXExport : public UtilityObj {
	public:
		LARGE_INTEGER	m_TickUsed[COUNTER_NUM];
		LARGE_INTEGER	m_TickBegin[COUNTER_NUM];
		LARGE_INTEGER	m_TickFrequency;
		float			m_fTicks[COUNTER_NUM];
		HWND			hD3DShow;
		HWND			hPanel;
		IUtil			*iu;
		Interface		*ip;
		int				m_nStart, m_nEnd;
		int				m_nExportStart, m_nExportEnd;

		ISpinnerControl * m_pSpinStart;
		ISpinnerControl * m_pSpinEnd;
		ICustEdit 		* m_pEditStart;
		ICustEdit		* m_pEditEnd;
		
		MOX_OPTION		m_Options;

		void BeginEditParams(Interface *ip,IUtil *iu);
		void EndEditParams(Interface *ip,IUtil *iu);

		void SelectionSetChanged(Interface *ip,IUtil *iu);

		void Init(HWND hWnd);
		void Destroy(HWND hWnd);

		void DeleteThis() { }		
		//Constructor/Destructor

		void UpdateTime();
		void UpdateOptions(HWND hWnd, bool bSave=true);
		void UpdateFramePos(bool bUseStart, bool bLindViewport);
		void SaveIt(HWND hWnd, bool bBinary);
		void PunchIntoD3D(HWND hWnd, MOX_OPTION * pOption);
		bool SaveSelected(HWND hWnd, char * szFileName, MOX_OPTION * pOption, bool bBinary);
		bool AddNodeToFrame(HWND hD3DWnd, IDirect3DDevice8 * pDevice, A3DFrame * pParentFrame, INode * pNode, INode * pParentNode, bool bAnimatedParent, MOX_OPTION * pOption);
		bool IsNodeMatrixAnimated(INode * pNode, int nStart, int nEnd);
		bool IsNodeAbsoluteMatrixAnimated(INode * pNode, int nStart, int nEnd);
		bool GetNodeUnitTM(TimeValue t, INode * pNode, Matrix3 * pMatrix);
		bool GetRelativeTM(INode * pNode, INode * pParentNode, TimeValue t, A3DMATRIX4 * pMatrix);
		bool GetAbsoluteTM(INode * pNode, TimeValue t, A3DMATRIX4 * pMatrix);
		bool BeginPerformanceRecord(int n);
		bool EndPerformanceRecord(int n);
		bool ResetPerformanceRecord();
		
		MOXExport();
		~MOXExport();		
};

static MOXExport theMOXExport;

class MOXExportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return &theMOXExport; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return MOXEXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("MOXExport2"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};



static MOXExportClassDesc MOXExportDesc;
ClassDesc2* GetMOXExportDesc() { return &MOXExportDesc; }

static BOOL CALLBACK MOXExportDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			theMOXExport.Init(hWnd);
			break;

		case WM_DESTROY:
			theMOXExport.Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_CHECKKEEPNORMAL:
			case IDC_CHECKCOPYMAP:
			case IDC_CHECKTEXTUREDIR:
			case IDC_CHECKINTOONE:
			case IDC_CHECKGENERATEMOD:
			case IDC_CHECKFORCEANIM:
			case IDC_CHECKOBJANIM:
				theMOXExport.UpdateOptions(hWnd);
				break;
			case IDC_BUTTONSAVE:
				theMOXExport.SaveIt(hWnd, false);
				break;
			case IDC_BUTTONSAVEBIN:
				theMOXExport.SaveIt(hWnd, true);
				break;
			case IDC_BUTTONSHOW:
				theMOXExport.UpdateOptions(hWnd);
				theMOXExport.PunchIntoD3D(hWnd, &theMOXExport.m_Options);
				break;
			case IDC_BUTTONVIEWER:
				WinExec("d:\\bin\\MOXViewer.exe", SW_SHOW);
				break;
			case IDC_BUTTONVALID:
				INode * pNode;
				Interval range;
				ObjectState os;

				if( theMOXExport.ip->GetSelNodeCount() == 0 )
					return FALSE;
				pNode = theMOXExport.ip->GetSelNode(0);
				os = pNode->EvalWorldState(theMOXExport.ip->GetTime());
				range = os.obj->ObjectValidity(theMOXExport.ip->GetTime());
				char szInfo[256];
				sprintf(szInfo, "Animated: [%d], From: %d, To: %d", pNode->IsAnimated(), range.Start() / GetTicksPerFrame(),
					range.End() / GetTicksPerFrame());
				MessageBox(hWnd, szInfo, "INFO", MB_OK);

				break;
			}
			break;
		
		case CC_SPINNER_CHANGE:
		case CC_SPINNER_BUTTONUP:
			bool bLinkViewport;

			if( BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKLINKVIEW), BM_GETCHECK, 0, 0) )
				bLinkViewport = true;
			else
				bLinkViewport = false;

			switch(LOWORD(wParam))
			{
			case IDC_SPINSTART:
				theMOXExport.UpdateFramePos(true, bLinkViewport);
				break;
			case IDC_SPINEND:
				theMOXExport.UpdateFramePos(false, bLinkViewport);
				break;
			}
			break;
	
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			theMOXExport.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

class MyTimeChangeCallback : public TimeChangeCallback
{
	void TimeChanged(TimeValue t)
	{
		theMOXExport.UpdateTime();
	}
};

static MyTimeChangeCallback timeCallBack;

DWORD WINAPI dummy_prog_fn(LPVOID arg)
{
	return 0;
}


bool Util_CopyFile(const char* szSrcPath, const char* szDestPath, BOOL bFailIfExists)
{
	IPathConfigMgr* pPathConfigMgr = IPathConfigMgr::GetPathConfigMgr();
	TSTR szFinalSrcPath = pPathConfigMgr->GetFullFilePath(szSrcPath, true);
	if(szFinalSrcPath.isNull())
		return false;

	return CopyFile(szFinalSrcPath, szDestPath, bFailIfExists);
}

//--- MOXExport -------------------------------------------------------
MOXExport::MOXExport()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
	QueryPerformanceFrequency(&m_TickFrequency);

	// Set default values of options;
	ZeroMemory(&m_Options, sizeof(m_Options));
	m_Options.bNormalKeep = true;
	m_Options.bCopyMap = true;
	m_Options.bTextureDir = true;
	m_Options.bGenerateMod = true;
}

MOXExport::~MOXExport()
{

}

void MOXExport::UpdateOptions(HWND hWnd, bool bSave)
{
	if( bSave )
	{
		ZeroMemory(&m_Options, sizeof(m_Options));

		if( BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKKEEPNORMAL), BM_GETCHECK, 0, 0) )
			m_Options.bNormalKeep = true;
		else
			m_Options.bNormalKeep = false;
		
		if( BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKCOPYMAP), BM_GETCHECK, 0, 0) )
			m_Options.bCopyMap = true;
		else
			m_Options.bCopyMap = false;

		if( BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKTEXTUREDIR), BM_GETCHECK, 0, 0) )
			m_Options.bTextureDir = true;
		else
			m_Options.bTextureDir = false;
		
		if( BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKGENERATEMOD), BM_GETCHECK, 0, 0) )
			m_Options.bGenerateMod = true;
		else
			m_Options.bGenerateMod = false;

		if( BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKFORCEANIM), BM_GETCHECK, 0, 0) )
			m_Options.bForceAnimation = true;
		else
			m_Options.bForceAnimation = false;

		if( BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKINTOONE), BM_GETCHECK, 0, 0) )
			m_Options.bPutIntoOneFrame = true;
		else
			m_Options.bPutIntoOneFrame = false;

		if( BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECKOBJANIM), BM_GETCHECK, 0, 0) )
			m_Options.bAllowObjectAnim = true;
		else
			m_Options.bAllowObjectAnim = false;
								
		m_Options.szMapDir[0] = '\0';
	}
	else
	{
		SendMessage(GetDlgItem(hWnd, IDC_CHECKKEEPNORMAL), BM_SETCHECK, m_Options.bNormalKeep, 0);
		SendMessage(GetDlgItem(hWnd, IDC_CHECKCOPYMAP), BM_SETCHECK, m_Options.bCopyMap, 0);
		SendMessage(GetDlgItem(hWnd, IDC_CHECKTEXTUREDIR), BM_SETCHECK, m_Options.bTextureDir, 0);
		SendMessage(GetDlgItem(hWnd, IDC_CHECKGENERATEMOD), BM_SETCHECK, m_Options.bGenerateMod, 0);
		SendMessage(GetDlgItem(hWnd, IDC_CHECKFORCEANIM), BM_SETCHECK, m_Options.bForceAnimation, 0);
		SendMessage(GetDlgItem(hWnd, IDC_CHECKINTOONE), BM_SETCHECK, m_Options.bPutIntoOneFrame, 0);
		SendMessage(GetDlgItem(hWnd, IDC_CHECKOBJANIM), BM_SETCHECK, m_Options.bAllowObjectAnim, 0);
	}

	if( m_Options.bPutIntoOneFrame )
		EnableWindow(GetDlgItem(hWnd, IDC_CHECKOBJANIM), TRUE);
	else
		EnableWindow(GetDlgItem(hWnd, IDC_CHECKOBJANIM), FALSE);

	if( m_Options.bCopyMap )
		EnableWindow(GetDlgItem(hWnd, IDC_CHECKTEXTUREDIR), TRUE);
	else												 
		EnableWindow(GetDlgItem(hWnd, IDC_CHECKTEXTUREDIR), FALSE);

	if( m_Options.bPutIntoOneFrame && m_Options.bForceAnimation )
	{
		MessageBox(hPanel, "'Put Into One' 同 'Force Animation' 冲突!\n将自动去除选项'Force Animation'!", "选项冲突", MB_OK);

		m_Options.bForceAnimation = false;
		SendMessage(GetDlgItem(hWnd, IDC_CHECKFORCEANIM), BM_SETCHECK, m_Options.bForceAnimation, 0);
	}
	return;
}
void MOXExport::BeginEditParams(Interface *ip,IUtil *iu) 
{
	m_nStart = m_nEnd = 0;
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		MOXExportDlgProc,
		GetString(IDS_PARAMS),
		0);

	UpdateOptions(hPanel, false);
	ip->RegisterTimeChangeCallback(&timeCallBack);
}
	
void MOXExport::EndEditParams(Interface *ip,IUtil *iu) 
{
	UpdateOptions(hPanel);
	ip->UnRegisterTimeChangeCallback(&timeCallBack);
	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}

void MOXExport::Init(HWND hWnd)
{
	hD3DShow = NULL;

	m_pSpinStart = GetISpinner(GetDlgItem(hWnd, IDC_SPINSTART));
	m_pSpinStart->LinkToEdit(GetDlgItem(hWnd, IDC_EDITSTART), EDITTYPE_INT);
	m_pSpinEnd = GetISpinner(GetDlgItem(hWnd, IDC_SPINEND));
	m_pSpinEnd->LinkToEdit(GetDlgItem(hWnd, IDC_EDITEND), EDITTYPE_INT);
	UpdateTime();
	
	m_pEditStart = GetICustEdit(GetDlgItem(hWnd, IDC_EDITSTART));
	m_pEditEnd = GetICustEdit(GetDlgItem(hWnd, IDC_EDITEND));
}

void MOXExport::Destroy(HWND hWnd)
{
	if( m_pEditStart )
	{
		ReleaseICustEdit(m_pEditStart);
		m_pEditStart = NULL;
	}
	if( m_pEditEnd )
	{
		ReleaseICustEdit(m_pEditEnd);
		m_pEditEnd = NULL;
	}
	if( m_pSpinStart )
	{
		ReleaseISpinner(m_pSpinStart);
		m_pSpinStart = NULL;
	}
	if( m_pSpinEnd )
	{
		ReleaseISpinner(m_pSpinEnd);
		m_pSpinEnd = NULL;
	}
	
	if( hD3DShow )
		DestroyWindow(hD3DShow);
}

void MOXExport::SelectionSetChanged(Interface *ip,IUtil *iu)
{
	int nNodeCount = ip->GetSelNodeCount();		
}

class MyHitByNameDlgCallback : public HitByNameDlgCallback
{
	TCHAR *dialogTitle() { return _T("Select objects you want to export:"); }
	TCHAR *buttonText() { return _T("Select"); }
	BOOL singleSelect() { return FALSE; }
	BOOL useProc() { return FALSE; }
	BOOL filter(INode * pNode); 
};

BOOL MyHitByNameDlgCallback::filter(INode * pNode)
{
	if( pNode->IsGroupHead() )
		return true;
	ObjectState os = pNode->EvalWorldState(theMOXExport.ip->GetTime());
	if( os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0) )
	{
		char szName[256];
		strncpy(szName, pNode->GetName(), 256);
		if( strstr(szName, "BB_") != szName && strstr(szName, "HH_") != szName && strstr(szName, "CC_") != szName)
		{
			return FALSE;
		}
		return TRUE;
	}
	else if( os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
		return TRUE;
	return FALSE;
}
						
void MOXExport::UpdateFramePos(bool bUseStart, bool bLinkViewport)
{
	int nFramePos = 0;
	if( bUseStart )
	{
		nFramePos = m_pSpinStart->GetIVal();
		m_pSpinEnd->SetLimits(nFramePos, m_nEnd);
		m_nExportStart = nFramePos;
	}
	else
	{
		nFramePos = m_pSpinEnd->GetIVal();
		m_pSpinStart->SetLimits(m_nStart, nFramePos);
		m_nExportEnd = nFramePos;
	}

	if( bLinkViewport )
		ip->SetTime(nFramePos * GetTicksPerFrame());
}

void MOXExport::UpdateTime()
{
	Interval animRange = ip->GetAnimRange();
	int	nStart = animRange.Start() / GetTicksPerFrame();
	int	nEnd = animRange.End() / GetTicksPerFrame();

	if( nStart != m_nStart || nEnd != m_nEnd )
	{	//If change the animation range, we reset the whole animation range to be export;
		m_pSpinStart->SetLimits(nStart, nEnd, TRUE);
		m_pSpinEnd->SetLimits(nStart, nEnd, TRUE);
		m_pSpinStart->SetValue(nStart, FALSE);
		m_pSpinEnd->SetValue(nEnd, FALSE);
		m_nExportStart = nStart;
		m_nExportEnd = nEnd;
		m_nStart = nStart;
		m_nEnd = nEnd;
	}
	else
	{
		//User move the frame slider;
		//What to do now?
		//Nothing to do.
	}
}

void MOXExport::SaveIt(HWND hWnd, bool bBinary)
{
	MyHitByNameDlgCallback hitCallBack;
	if( !ip->DoHitByNameDialog(&hitCallBack) )
		return;

	int nSelCount = ip->GetSelNodeCount();
	if( nSelCount == 0 )
	{
		MessageBox(hWnd, "Nothing has been selected!\nSave Aborted!", "ABORTED", MB_ICONSTOP | MB_OK);
		return;
	}

	char * szMaxFilename = ip->GetCurFileName();
	char szFilename[MAX_PATH];
	char szPath[MAX_PATH];

	AGetFileTitle(szMaxFilename, szFilename, MAX_PATH);
	szFilename[strlen(szFilename) - 4] = '\0';
	strcat(szFilename, ".mox");
	OPENFILENAME	ofn;
	szPath[0] = '\0';
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = hInstance;
	ofn.lpstrFilter = "Model File(*.mox)\0*.mox\0";
	ofn.lpstrFile = szFilename;
	ofn.lpstrInitialDir = szPath;
	ofn.lpstrDefExt = ".mox";
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select the model filename to be saved";
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	if( !GetSaveFileName(&ofn) )
	{
		return;
	}
	UpdateOptions(hWnd); 

	if( m_Options.bCopyMap )
	{
		AGetFilePath(szFilename, szPath, MAX_PATH);
		strcpy(m_Options.szMapDir, szPath);
		g_nLastAsk4Answer = -1;

		if( m_Options.bTextureDir )
		{
			strcat(m_Options.szMapDir, "\\Textures");
			if( -1 == _access(m_Options.szMapDir, 0) )
				CreateDirectory(m_Options.szMapDir, NULL);
		}
	}

	if( !SaveSelected(hWnd, szFilename, &m_Options, bBinary) )
	{
		MessageBox(hWnd, "Error in saving the selected nodes!\nSave Aborted!", "ABORTED", MB_ICONSTOP | MB_OK);
		return;
	}
}

bool MOXExport::SaveSelected(HWND hWnd, char * szFileName, MOX_OPTION * pOption, bool bBinary)
{
	if( ip->GetSelNodeCount() == 0 )
		return false;

	if( pOption->bPutIntoOneFrame )
	{
		if( IDYES != MessageBox(hWnd, "Put all meshes into one frame will lose animation information!\nDo you want to continue?", 
			"PLEASE CONFIRM", MB_ICONQUESTION | MB_YESNO) )
			return true;
	}

	A3DFrame * pScene = NULL;
	INode * pRootNode = NULL;
	
	pScene = new A3DFrame();

	if( !pScene->Init(NULL, 1) )
		return false;

	pScene->SetTM(IdentityMatrix());
	pScene->SetTM(IdentityMatrix(), false);

	g_pScene = pScene;

	pRootNode = ip->GetRootNode();
	int nChildNodeNum;
	INode * pChildNode;
	int i;


	ip->ProgressStart(_T("Export Mox File..."), TRUE, dummy_prog_fn, NULL);
	nChildNodeNum = pRootNode->NumberOfChildren();
	for(i=0; i<nChildNodeNum; i++)
	{
		pChildNode = pRootNode->GetChildNode(i);
		AddNodeToFrame(theMOXExport.hPanel, NULL, pScene, pChildNode, pRootNode, false, pOption);
	}
	ip->ProgressEnd();

	A3DFile file;

	DWORD dwFileFlags;

	dwFileFlags = A3DFILE_CREATENEW;
	if( !bBinary )
		dwFileFlags |= A3DFILE_TEXT;
	if( !file.Open(szFileName, dwFileFlags) )
		goto Failure;

	pScene->Save(&file);

	file.Close();

	if( m_Options.bGenerateMod )
	{
		char		szModelPath[MAX_PATH];

		strncpy(szModelPath, szFileName, MAX_PATH);
		if( strlen(szModelPath) > 4 )
		{
			szModelPath[strlen(szModelPath) - 4] = '\0';
			strcat(szModelPath, ".mod");
		}

		if( -1 == _access(szModelPath, 0) )
		{
			// if the mod file does not exist, we should create a new .mod file;
			A3DModel	model;
			char		szName[MAX_PATH];

			AGetFileTitle(szFileName, szName, MAX_PATH);
			if( strlen(szName) > 4 )
				szName[strlen(szName) - 4] = '\0';

			if( !file.Open(szModelPath, A3DFILE_CREATENEW | A3DFILE_TEXT) )
				goto Failure;

			model.Init(NULL);
			model.SetName(szName);

			// Notice that the model will determine the mox file's name according to its
			// name plus .mox!!!!!!!!!!!!!!!!
			// This must be modified for the full version in which we have mox man;
			model.AddChildFrame(pScene);

			model.Save(&file);

			file.Close();
			model.Release();
		}
	}
	pScene->Release();
	delete pScene;
	pScene = NULL;

	return true;
Failure:
	if( pScene )
	{
		pScene->Release();
		delete pScene;
	}
	return false;
}

void MOXExport::PunchIntoD3D(HWND hWnd, MOX_OPTION * pOption)
{
	MyHitByNameDlgCallback hitCallBack;
	if( !ip->DoHitByNameDialog(&hitCallBack) )
		return;

	int nSelCount = ip->GetSelNodeCount();
	if( nSelCount == 0 )
	{
		MessageBox(hWnd, "Nothing has been selected!\nSave Aborted!", "ABORTED", MB_ICONSTOP | MB_OK);
		return;
	}

	if( pOption->bPutIntoOneFrame )
	{
		if( IDYES != MessageBox(hWnd, "Put all meshes into one frame will lose animation information!\nDo you want to continue?", 
			"PLEASE CONFIRM", MB_ICONQUESTION | MB_YESNO) )
			return;
	}

	A3DFrame * pScene = NULL;
	INode * pRootNode = NULL;
	IDirect3DDevice8* pDevice = NULL;

	EnableWindow(GetDlgItem(hWnd, IDC_BUTTONSHOW), FALSE);
	D3DWindow d3dWindow;

	if( !d3dWindow.D3DInitWindow(hInstance, hWnd, &hD3DShow) )
		goto Failure;

	pScene = new A3DFrame();
	pDevice = d3dWindow.GetRenderer().GetDevice();

	if( !pScene->Init(pDevice, 1) )
		return;

	pScene->SetTM(IdentityMatrix());
	pScene->SetTM(IdentityMatrix(), false);

	g_pScene = pScene;

	pRootNode = ip->GetRootNode();
	int nChildNodeNum;
	INode * pChildNode;
	int i;

	ResetPerformanceRecord();

	ip->ProgressStart(_T("Export Mox File..."), TRUE, dummy_prog_fn, NULL);
	BeginPerformanceRecord(8);
	nChildNodeNum = pRootNode->NumberOfChildren();
	for(i=0; i<nChildNodeNum; i++)
	{
		pChildNode = pRootNode->GetChildNode(i);
		if( !AddNodeToFrame(hD3DShow, pDevice, pScene, pChildNode, pRootNode, false, pOption) )
			break;
	}
	EndPerformanceRecord(8);
	ip->ProgressEnd();

	LARGE_INTEGER tickAll;
	tickAll.QuadPart = m_TickUsed[0].QuadPart + m_TickUsed[1].QuadPart + m_TickUsed[2].QuadPart + m_TickUsed[3].QuadPart + m_TickUsed[4].QuadPart;
	float fTicks;
	fTicks = tickAll.QuadPart * 1000.0f / m_TickFrequency.QuadPart;
	
	for(i=0; i<COUNTER_NUM; i++)
	{
		m_fTicks[i] = m_TickUsed[i].QuadPart * 1000.0f / m_TickFrequency.QuadPart;
	}

	d3dWindow.D3DRunWindow(pScene, GetFrameRate());

Failure:
	EnableWindow(GetDlgItem(hWnd, IDC_BUTTONSHOW), TRUE);
	hD3DShow = NULL;

	if( pScene )
	{
		pScene->Release();
		delete pScene;
	}

	if( !d3dWindow.D3DReleaseWindow() )
	return;
}

A3DMesh * FindSameMesh(A3DFrame * pFrame, char * szTextureName, A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShiness, bool b2Sided, const A3DMESHPROP& prop)
{
	A3DMesh * pMesh = pFrame->GetFirstMesh();
	while( pMesh )
	{
		bool bSameTexture = false;
		bool bSameMaterial = false;
		bool bSameProperty = false;

		if( stricmp(szTextureName, pMesh->GetTexture()->m_szMapFile) == 0 )
			bSameTexture = true;

		A3DMaterial * pMaterial = pMesh->GetMaterial();
		if( 0 == memcmp(&pMaterial->m_clAmbientValue, &clAmbient, sizeof(A3DCOLORVALUE)) &&
			0 == memcmp(&pMaterial->m_clDiffuseValue, &clDiffuse, sizeof(A3DCOLORVALUE)) &&
			0 == memcmp(&pMaterial->m_clEmissiveValue, &clEmissive, sizeof(A3DCOLORVALUE)) &&
			0 == memcmp(&pMaterial->m_clSpecularValue, &clSpecular, sizeof(A3DCOLORVALUE)) &&
			pMaterial->m_fShininess == fShiness &&
			pMaterial->m_b2Sided == b2Sided )
			bSameMaterial = true;

		if (pMesh->GetProperty() == prop)
			bSameProperty = true;
		
		if (bSameTexture && bSameMaterial && bSameProperty)
			return pMesh;

		pMesh = pFrame->GetNextMesh();
	}

	A3DFrame * pChild = pFrame->GetFirstChildFrame();
	while( pChild )
	{
		pMesh = FindSameMesh(pChild, szTextureName, clAmbient, clDiffuse, clEmissive, clSpecular, fShiness, b2Sided, prop);
		if( pMesh )
			return pMesh;

		pChild = pFrame->GetNextChildFrame();
	}

	return NULL;
}

bool MOXExport::GetAbsoluteTM(INode * pNode, TimeValue t, A3DMATRIX4 * pMatrix)
{
	A3DMATRIX4 matTM = IdentityMatrix();
	Matrix3	thisTM;
	Point3	row0, row1, row2, row3;
	
	GetNodeUnitTM(t, pNode, &thisTM);

	//Convert it into A3DMatrix, from right-handed into left-handed;
	row0 = thisTM.GetRow(0);
	row1 = thisTM.GetRow(1);
	row2 = thisTM.GetRow(2);
	row3 = thisTM.GetRow(3);

	matTM.m[0][0] = row0.x;
	matTM.m[0][1] = row0.z;
	matTM.m[0][2] = row0.y;
	
	matTM.m[1][0] = row2.x;
	matTM.m[1][1] = row2.z;
	matTM.m[1][2] = row2.y;
	
	matTM.m[2][0] = row1.x;
	matTM.m[2][1] = row1.z;
	matTM.m[2][2] = row1.y;

	matTM.m[3][0] = row3.x;
	matTM.m[3][1] = row3.z;
	matTM.m[3][2] = row3.y;

	*pMatrix = matTM;
	return true;
}

bool MOXExport::GetRelativeTM(INode * pNode, INode * pParentNode, TimeValue t, A3DMATRIX4 * pMatrix)
{
	A3DMATRIX4 matTM = IdentityMatrix();
	Matrix3	   thisTM, parentTM, relativeTM;
	Point3	   row0, row1, row2, row3;
	
	GetNodeUnitTM(t, pNode, &thisTM);
	GetNodeUnitTM(t, pParentNode, &parentTM);
	parentTM.Invert();
	relativeTM = thisTM * parentTM;
	row0 = relativeTM.GetRow(0);
	row1 = relativeTM.GetRow(1);
	row2 = relativeTM.GetRow(2);
	row3 = relativeTM.GetRow(3);

	matTM.m[0][0] = row0.x;
	matTM.m[0][1] = row0.z;
	matTM.m[0][2] = row0.y;
	
	matTM.m[1][0] = row2.x;
	matTM.m[1][1] = row2.z;
	matTM.m[1][2] = row2.y;
	
	matTM.m[2][0] = row1.x;
	matTM.m[2][1] = row1.z;
	matTM.m[2][2] = row1.y;

	matTM.m[3][0] = row3.x;
	matTM.m[3][1] = row3.z;
	matTM.m[3][2] = row3.y;

	*pMatrix = matTM;
	return true;
}

bool MOXExport::AddNodeToFrame(HWND hD3DWnd,IDirect3DDevice8 * pDevice, A3DFrame * pParentFrame, INode * pNode, INode * pParentNode, bool bAnimatedParent, MOX_OPTION * pOption)
{
	TriObject	*objTri;

	Point3		*pVerts = NULL;
	WORD		*pIndices = NULL;
	Point3		*pTVVerts = NULL;
	WORD		*pTVIndices = NULL;
	
	Point3		*pNormals = NULL;
	int			*pSmoothNum = NULL;

	int			nVertCount, nTVVertCount, nA3DVertCount, nNormalCount;
	int			nIndexCount;

	A3DCOLORVALUE clAmbient, clDiffuse, clEmissive, clSpecular;
	FLOAT		fShiness;
	bool		b2Sided = false;

	A3DVERTEX	*pA3DVerts = NULL;
	WORD		*pA3DIndices = NULL;

	char		szTextureName[256];
	char		szDetailTextureName[256];
	bool		bHasDetail = false;
	bool		bHasUV = false;
	A3DMATRIX4	matDetailTM;

	A3DMesh		*pThisMesh = NULL;
	A3DFrame	*pThisFrame = NULL;

	int			nFrameCount;
	Interval	animRange;

	animRange = ip->GetAnimRange();
	nFrameCount = m_nExportEnd - m_nExportStart + 1;//(animRange.End() - animRange.Start()) / GetTicksPerFrame();

	BeginPerformanceRecord(9);
	ObjectState os = pNode->EvalWorldState(ip->GetTime());
	EndPerformanceRecord(9);

	BeginPerformanceRecord(10);
	bool bAnimated = false;
	if( pOption->bForceAnimation )
		bAnimated = true;
	else if( !pOption->bPutIntoOneFrame && (bAnimatedParent || IsNodeMatrixAnimated(pNode, 0, nFrameCount)) )
		bAnimated = true;
	else
		bAnimated = false;
	
	char * szName = pNode->GetName();

	bool bNewMesh = true;

	static int nCallCount = 0;
	static int nAddCount = 0;

	nCallCount ++;
	EndPerformanceRecord(10);
	if( pNode->Selected() && (!pNode->IsGroupHead()) && os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0) )
	{
		char szName[256];
		bool bFlip = false;

		int nPos = nAddCount * 100 / ip->GetSelNodeCount();
		ip->ProgressUpdate(nPos);
		nAddCount ++;

		//All Bounding Box Dummy begin with "BB_";
		strncpy(szName, pNode->GetName(), 256);
		if( strstr(szName, "HH_") == szName )
		{
			//Output HH Dummy Object;
			if( pNode->GetParentNode() == NULL || pNode->GetParentNode() == ip->GetRootNode() )
			{
				char szInfo[256];
				sprintf(szInfo, "要导出的悬挂位置(%s)必须链接到某个物体上！", szName);
				MessageBox(hD3DWnd, szInfo, "失败", MB_OK);
				return false;
			}

			int nHHFrameCount = 1;
			if( bAnimated )
				nHHFrameCount = nFrameCount;

			//This is a hanging helper, so output it into the frame;
			pThisFrame = new A3DFrame();
			pThisFrame->SetName(pNode->GetName());

			if( !pThisFrame->Init(pDevice, nHHFrameCount) )
				goto Failure;
			pThisFrame->SetParent(pParentFrame);
			pParentFrame->AddChild(pThisFrame);

			A3DMATRIX4 matTM;
			for(int i=0; i<nHHFrameCount; i++)
			{
				GetRelativeTM(pNode, pParentNode, /*animRange.Start() + */(m_nExportStart + i) * GetTicksPerFrame(), &matTM);
				pThisFrame->SetTM(matTM, true, i);
			}
			goto Exit;
		}
		else if( strstr(szName, "CC_") == szName )
		{
			// Output CC Dummy Object(Connection Center)
			// For a Connection Center, the relative TM is in fact the inverse transform matrix
			// For world space;
			pThisFrame = new A3DFrame();
			pThisFrame->SetName(pNode->GetName());

			int nCCFrameCount = 1;
			if( IsNodeAbsoluteMatrixAnimated(pNode, 0, nFrameCount) && !pOption->bPutIntoOneFrame )
				nCCFrameCount = nFrameCount;

			if( !pThisFrame->Init(pDevice, nCCFrameCount) )
				goto Failure;

			pThisFrame->SetParent(pParentFrame);
			pParentFrame->AddChild(pThisFrame);

			A3DMATRIX4 matTM;
			for(int i=0; i<nCCFrameCount; i++)
			{
				GetAbsoluteTM(pNode, /*animRange.Start() + */(m_nExportStart + i) * GetTicksPerFrame(), &matTM);
				matTM = InverseTM(matTM);
				pThisFrame->SetTM(matTM, true, i);
			}
			goto Exit;
		}
		else if( strstr(szName, "BB_") == szName )
		{
			// Do something for BB_
			if( pNode->GetParentNode() == NULL || pNode->GetParentNode() == ip->GetRootNode() )
			{
				char szInfo[256];
				sprintf(szInfo, "要导出的包围盒(%s)必须链接到某个物体上！", szName);
				MessageBox(hD3DWnd, szInfo, "失败", MB_OK);
				return false;
			}

			if( bAnimated )	// Missing BB_ parent, we need add a empty parent for the bb
			{
				int nBBFrameCount = 1;
				nBBFrameCount = nFrameCount;
				//This is a hanging helper, so output it into the frame;
				pThisFrame = new A3DFrame();
				char szName[256];

				sprintf(szName, "parent_%s", pNode->GetName());
				pThisFrame->SetName(szName);

				if( !pThisFrame->Init(pDevice, nBBFrameCount) )
					goto Failure;

				pThisFrame->SetParent(pParentFrame);
				pParentFrame->AddChild(pThisFrame);

				A3DMATRIX4 matTM;
				for(int i=0; i<nBBFrameCount; i++)
				{
					GetRelativeTM(pNode, pParentNode, /*animRange.Start() + */(m_nExportStart + i) * GetTicksPerFrame(), &matTM);
					pThisFrame->SetTM(matTM, true, i);
				}
				// Descend into the node;
				pParentNode = pNode;
				pParentFrame = pThisFrame;
			}
		}
		else
		{
			char szInfo[256];
			sprintf(szInfo, "不允许非包围盒的Dummy物体(%s)输出！", szName);
			MessageBox(hD3DWnd, szInfo, "警告", MB_OK);
			return true;
		}
		
		Box3 box = ((DummyObject *)os.obj)->GetBox();
		Point3  pos, x, y, z, o;
		Matrix3 tm, matObject, parentTM;
			
		tm = pNode->GetObjTMAfterWSM(0);
		//Now we must tranform into pParentNode;
		GetNodeUnitTM(0, pParentNode, &parentTM);
		parentTM.Invert();
		matObject = tm * parentTM;
		
		//Transform box into parent's frame;
		box.pmin = box.pmin * matObject;
		box.pmax = box.pmax * matObject;

		//	Transform axis;
		o = Point3::Origin * matObject;
		x = Point3::XAxis * matObject - o;
		y = Point3::ZAxis * matObject - o;
		z = Point3::YAxis * matObject - o;

		x = x.Normalize();
		y = y.Normalize();
		z = z.Normalize();

		if (fabs(DotProd(CrossProd(x, y), z)) < 1.0f - 1e-3)
		{
			char szInfo[256];
			sprintf(szInfo, "作为包围盒的物体 %s 已经不是长方体了，发生了侧切变形。请检查!", pNode->GetName());
			MessageBox(hD3DWnd, szInfo, "失败", MB_ICONSTOP | MB_OK);
			return false;
		}

		//	Calculate OBB;
		A3DFRAMEOBB obb;
		pos = (box.pmin + box.pmax) / 2.0f;
		obb.a3dOBB.Center.x = pos.x; obb.a3dOBB.Center.y = pos.z; obb.a3dOBB.Center.z = pos.y; 
		obb.a3dOBB.XAxis.x = x.x; obb.a3dOBB.XAxis.y = x.z; obb.a3dOBB.XAxis.z = x.y;
		obb.a3dOBB.YAxis.x = y.x; obb.a3dOBB.YAxis.y = y.z; obb.a3dOBB.YAxis.z = y.y; 
		obb.a3dOBB.ZAxis.x = z.x; obb.a3dOBB.ZAxis.y = z.z; obb.a3dOBB.ZAxis.z = z.y; 
		
		pos = (box.pmax - box.pmin) / 2.0f;
		obb.a3dOBB.Extents.x = fabs(DotProd(pos, x)); 
		obb.a3dOBB.Extents.y = fabs(DotProd(pos, y));
		obb.a3dOBB.Extents.z = fabs(DotProd(pos, z));

		//	Now get the property sets;
		obb.dwProps = 0;
		int	nval;
		BOOL bval;

		if (pNode->GetUserPropBool("可射穿", bval) && bval)
			obb.SetShootThroughFlag(true);

		if (pNode->GetUserPropBool("不留痕", bval) && bval)
			obb.SetNoMarkFlag(true);

		if (pNode->GetUserPropBool("碰不到", bval) && bval)
			obb.SetPickThroughFlag(true);

		if (pNode->GetUserPropBool("不可摧", bval) && bval)
			obb.SetCanNotDestroyFlag(true);

		if (pNode->GetUserPropInt("材料", nval))
			obb.MtlType = (A3DMATERIALTYPE)nval;

		if (!pParentFrame->AddBoundingBox(obb))
		{
			MessageBox(hD3DWnd, "保存包围盒失败！", "失败", MB_OK);
			return false;
		}

		goto Exit;
	}
	else if( pNode->Selected() && os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
	{
		Matrix3		tm;
		Matrix3		tmTexture;
		float		UTile = 1.0f;
		float		VTile = 1.0f;
		bool		bFlip;
		int			i, n0, n1, n2;
		int			nFrame, nNodeFrameCount, nObjectFrameCount;
		Interval	validRange;
		
		tmTexture.IdentityMatrix();
		int nPos = nAddCount * 100 / ip->GetSelNodeCount();
		ip->ProgressUpdate(nPos);
		nAddCount ++;

		BeginPerformanceRecord(0);
		objTri = (TriObject *)os.obj->ConvertToType(ip->GetTime(), Class_ID(TRIOBJ_CLASS_ID, 0));
		if( !objTri )
			goto Exit;

		if( bAnimated )	  
		{
			nNodeFrameCount = nFrameCount;
		}
		else
		{
			nNodeFrameCount = 1;
		}
		validRange = objTri->ObjectValidity(ip->GetTime());
		if( (validRange & animRange) == animRange )
			nObjectFrameCount = 1;
		else
			nObjectFrameCount = nFrameCount;

		if( pOption->bPutIntoOneFrame )
		{
			nNodeFrameCount = 1;
			if( !pOption->bAllowObjectAnim )
				nObjectFrameCount = 1;
		}

		if( nNodeFrameCount == 1 && nObjectFrameCount == 1 )
			nFrameCount = 1;

		//Now find out the texture name;
		bHasUV = false;
		szTextureName[0] = '\0';
		szDetailTextureName[0] = '\0';
		Mtl * pMtl = pNode->GetMtl();
		if( pMtl )
		{
			if( pMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0) )
			{
				StdMat * pStdMtl = (StdMat *) pMtl;
				TimeValue t = ip->GetTime();
				
				clAmbient = A3DCOLORVALUE(pStdMtl->GetAmbient(t).r, pStdMtl->GetAmbient(t).g, pStdMtl->GetAmbient(t).b, pStdMtl->GetOpacity(t));
				clDiffuse = A3DCOLORVALUE(pStdMtl->GetDiffuse(t).r, pStdMtl->GetDiffuse(t).g, pStdMtl->GetDiffuse(t).b, pStdMtl->GetOpacity(t));
				clEmissive = A3DCOLORVALUE(pStdMtl->GetSelfIllumColor(t).r, pStdMtl->GetSelfIllumColor(t).g, pMtl->GetSelfIllumColor().b, pStdMtl->GetOpacity(t));
				clSpecular = A3DCOLORVALUE(pStdMtl->GetSpecular(t).r, pStdMtl->GetSpecular(t).g, pStdMtl->GetSpecular(t).b, pStdMtl->GetOpacity(t));
				
				clSpecular.r *= pStdMtl->GetShinStr(t);
				clSpecular.g *= pStdMtl->GetShinStr(t);
				clSpecular.b *= pStdMtl->GetShinStr(t);

				fShiness = pStdMtl->GetShininess(t) * 100.0f;
				b2Sided = pStdMtl->GetTwoSided() ? true : false;
				
				Texmap * pTexmap = pStdMtl->GetSubTexmap(ID_DI);
				if( pTexmap )
				{
					if( 0 == objTri->mesh.getNumTVerts() ) //If not uvw maping
						bHasUV = false;
					else
					{
						if( pTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
						{
							BitmapTex * pBitmapTex = (BitmapTex *)pTexmap;
							sprintf(szTextureName, pBitmapTex->GetMapName());
							bHasUV = true;
							//For max 3.1 and 4.0 we must ignore the material diffuse color if there is a maps;
							clAmbient = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, clAmbient.a);
							clDiffuse = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, clDiffuse.a);
						}
						pTexmap->GetUVTransform(tmTexture);
					}
				}

				bHasDetail = false;
				// Now try to find the detail texture of this material;
				Texmap * pDetailTexmap = pStdMtl->GetSubTexmap(ID_BU);
				if( pDetailTexmap && bHasUV )
				{
					Matrix3 tmInv = tmTexture;
					Matrix3	tmDetailTM;

					if( pDetailTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
					{
						BitmapTex * pBitmapTex = (BitmapTex *)pDetailTexmap;
						sprintf(szDetailTextureName, pBitmapTex->GetMapName());

						pDetailTexmap->GetUVTransform(tmDetailTM);

						bHasDetail = true;
						tmInv.Invert();
						tmDetailTM = tmInv * tmDetailTM;
						matDetailTM = IdentityMatrix();
						matDetailTM._11 = tmDetailTM.GetRow(0).x;
						matDetailTM._12 = tmDetailTM.GetRow(0).y;
						matDetailTM._13 = tmDetailTM.GetRow(0).z;

						matDetailTM._21 = tmDetailTM.GetRow(1).x;
						matDetailTM._22 = tmDetailTM.GetRow(1).y;
						matDetailTM._23 = tmDetailTM.GetRow(1).z;

						matDetailTM._31 = tmDetailTM.GetRow(2).x;
						matDetailTM._32 = tmDetailTM.GetRow(2).y;
						matDetailTM._33 = tmDetailTM.GetRow(2).z;
					}
				}
			}
			else if( pMtl->ClassID() == Class_ID(MULTI_CLASS_ID, 0) )
			{
				//For multi material, we do not support now;
				bool bHasSetMaterial = false;
				int	nSubMtlNum = pMtl->NumSubMtls();
				if( nSubMtlNum )
				{
					Mtl * pSubMtl;
					for(int i=0; i<nSubMtlNum; i++)
					{
						pSubMtl = pMtl->GetSubMtl(i);
						if( pSubMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0) )
						{
							StdMat * pStdMtl = (StdMat *) pSubMtl;
							TimeValue t = ip->GetTime();
							
							clAmbient = A3DCOLORVALUE(pStdMtl->GetAmbient(t).r, pStdMtl->GetAmbient(t).g, pStdMtl->GetAmbient(t).b, pStdMtl->GetOpacity(t));
							clDiffuse = A3DCOLORVALUE(pStdMtl->GetDiffuse(t).r, pStdMtl->GetDiffuse(t).g, pStdMtl->GetDiffuse(t).b, pStdMtl->GetOpacity(t));
							clEmissive = A3DCOLORVALUE(pStdMtl->GetSelfIllumColor(t).r, pStdMtl->GetSelfIllumColor(t).g, pMtl->GetSelfIllumColor().b, pStdMtl->GetOpacity(t));
							clSpecular = A3DCOLORVALUE(pStdMtl->GetSpecular(t).r, pStdMtl->GetSpecular(t).g, pStdMtl->GetSpecular(t).b, pStdMtl->GetOpacity(t));
							
							clSpecular.r *= pStdMtl->GetShinStr(t);
							clSpecular.g *= pStdMtl->GetShinStr(t);
							clSpecular.b *= pStdMtl->GetShinStr(t);

							fShiness = pStdMtl->GetShininess(t) * 100.0f;
							b2Sided = pStdMtl->GetTwoSided() ? true : false;
							
							Texmap * pTexmap = pStdMtl->GetSubTexmap(ID_DI);
							if( pTexmap )
							{
								if( 0 == objTri->mesh.getNumTVerts() ) //If not uvw maping
									bHasUV = false;
								else
								{
									if( pTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
									{
										BitmapTex * pBitmapTex = (BitmapTex *)pTexmap;
										sprintf(szTextureName, pBitmapTex->GetMapName());
										bHasUV = true;
										//For max 3.1 and 4.0 we must ignore the material diffuse color if there is a maps;
										clAmbient = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
										clDiffuse = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
									}
								}
							}

							bHasDetail = false;
							// Now try to find the detail texture of this material;
							Texmap * pDetailTexmap = pStdMtl->GetSubTexmap(ID_BU);
							if( pDetailTexmap && bHasUV )
							{
								Matrix3 tmInv = tmTexture;
								Matrix3	tmDetailTM;

								if( pDetailTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
								{
									BitmapTex * pBitmapTex = (BitmapTex *)pDetailTexmap;
									sprintf(szDetailTextureName, pBitmapTex->GetMapName());

									pDetailTexmap->GetUVTransform(tmDetailTM);

									bHasDetail = true;
									tmInv.Invert();
									tmDetailTM = tmInv * tmDetailTM;
									matDetailTM = IdentityMatrix();
									matDetailTM._11 = tmDetailTM.GetRow(0).x;
									matDetailTM._12 = tmDetailTM.GetRow(0).y;
									matDetailTM._13 = tmDetailTM.GetRow(0).z;

									matDetailTM._21 = tmDetailTM.GetRow(1).x;
									matDetailTM._22 = tmDetailTM.GetRow(1).y;
									matDetailTM._23 = tmDetailTM.GetRow(1).z;

									matDetailTM._31 = tmDetailTM.GetRow(2).x;
									matDetailTM._32 = tmDetailTM.GetRow(2).y;
									matDetailTM._33 = tmDetailTM.GetRow(2).z;
								}
							}
							bHasSetMaterial = true;
							break;
						}
					}
				}
				if( !bHasSetMaterial )
				{
					Color wireColor(pNode->GetWireColor());
					clAmbient = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
					clDiffuse = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
					clSpecular = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
					clEmissive = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
					fShiness = 0.0f;	
					b2Sided = false;
				}
			}
			else
			{
				//Unkown material, so set it to white;
				clAmbient = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
				clDiffuse = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
				clSpecular = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
				clEmissive = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
				fShiness = 0.0f;
				b2Sided = false;
			}
		}
		else
		{
			//Not any material, so we use the iNode wirecolor;
			Color wireColor(pNode->GetWireColor());
			clAmbient = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
			clDiffuse = A3DCOLORVALUE(wireColor.r, wireColor.g, wireColor.b, 1.0f);
			clSpecular = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
			clEmissive = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
			fShiness = 0.0f;
			b2Sided = false;
		}

		// get rid of / in the file path
		A3DRepairFilePath(szTextureName);
		A3DRepairFilePath(szDetailTextureName);

		if( !pDevice && pOption->bCopyMap )
		{
			//Saving duration;
			//Copy the map file
			char szFileTitle[MAX_PATH];
			char szFullPath[MAX_PATH];

			if( strlen(szTextureName) )
			{
				if( !AGetFileTitle(szTextureName, szFileTitle, MAX_PATH) )
					goto Failure;
				sprintf(szFullPath, "%s\\%s", pOption->szMapDir, szFileTitle);
				FILE * fileDest = fopen(szFullPath, "rb");
				if( fileDest )
				{
					char szInfo[1024];
					fclose(fileDest);

					sprintf(szInfo, "Map file [%s] already exist in dir [%s].\n\tDo you want to REPLACE it?", 
						szFileTitle, pOption->szMapDir);
					
					if( g_nLastAsk4Answer == ASK4_YESALL )
					{
						Util_CopyFile(szTextureName, szFullPath, FALSE);
					}
					else if( g_nLastAsk4Answer == ASK4_NOALL )
					{
					}
					else
					{
						DlgAsk4 dlgAsk4;
						dlgAsk4.Init(hInstance, theMOXExport.hPanel);
						dlgAsk4.SetTitleInfo("Overwrite Confirm", szInfo);
						g_nLastAsk4Answer = dlgAsk4.DoModal();
						if( ASK4_YES == g_nLastAsk4Answer || ASK4_YESALL == g_nLastAsk4Answer )
						{
							Util_CopyFile(szTextureName, szFullPath, FALSE);
						}
					}
				}
				else
				{
					Util_CopyFile(szTextureName, szFullPath, FALSE);
				}
				strcpy(szTextureName, szFileTitle);
			}

			// Copy detail texture also.
			if( strlen(szDetailTextureName) )
			{
				if( !AGetFileTitle(szDetailTextureName, szFileTitle, MAX_PATH) )
					goto Failure;
				sprintf(szFullPath, "%s\\%s", pOption->szMapDir, szFileTitle);
				Util_CopyFile(szDetailTextureName, szFullPath, FALSE);
				strcpy(szDetailTextureName, szFileTitle);
			}
		}
		//Determine Material Section;
		EndPerformanceRecord(0);

		BeginPerformanceRecord(1);
		//Determine vertex and face count;
		nVertCount = objTri->mesh.getNumVerts();
		nIndexCount = objTri->mesh.getNumFaces() * 3;
		if( nVertCount < 3 || nIndexCount < 3 )
			return true;

		if( nIndexCount > 0xffff )
		{
			MessageBox(hD3DWnd, "Index Beyond the range", "FAIL", MB_ICONSTOP | MB_OK);
			return false;
		}
		
		//Init verts and tvVerts and A3DVerts table;
		pVerts = (Point3 *) malloc(sizeof(Point3) * nVertCount);
		pIndices = (WORD *) malloc(sizeof(WORD) * nIndexCount);
		if( bHasUV )
		{
			nTVVertCount = objTri->mesh.getNumTVerts();

			pTVVerts = (Point3 *) malloc(sizeof(Point3) * nTVVertCount);
			pTVIndices = (WORD *) malloc(sizeof(WORD) * nIndexCount);
			pA3DVerts = (A3DVERTEX *) malloc(sizeof(A3DVERTEX) * nTVVertCount);
			ZeroMemory(pA3DVerts, sizeof(A3DVERTEX) * nTVVertCount);
			pA3DIndices = (WORD *) malloc(sizeof(WORD) * nIndexCount);
			nA3DVertCount = nTVVertCount;
		}
		else
		{
			nTVVertCount = 0;
			pA3DVerts = (A3DVERTEX *) malloc(sizeof(A3DVERTEX) * nVertCount);
			ZeroMemory(pA3DVerts, sizeof(A3DVERTEX) * nVertCount);
			pA3DIndices = (WORD *) malloc(sizeof(WORD) * nIndexCount);
			nA3DVertCount = nVertCount;
		}

		//Init normal table;
		if( !bHasUV || pOption->bNormalKeep )
			nNormalCount = nVertCount;
		else
			nNormalCount = nTVVertCount;

		pNormals = (Point3 *) malloc(sizeof(Point3) * nNormalCount);
		ZeroMemory(pNormals, sizeof(Point3) * nNormalCount);
		pSmoothNum = (int *) malloc(sizeof(int) * nNormalCount);
		ZeroMemory(pSmoothNum, sizeof(int) * nNormalCount);

		A3DMESHPROP prop(0, A3DMTL_CONCRETE);
		int	nval;
		BOOL bval;

		if (pNode->GetUserPropBool("双面", bval) && bval)
			prop.Set2SidesFlag(true);
		if (pNode->GetUserPropBool("可射穿", bval) && bval)
			prop.SetShootThroughFlag(true);
		if (pNode->GetUserPropBool("不留痕", bval) && bval)
			prop.SetNoMarkFlag(true);
		if (pNode->GetUserPropBool("碰不到", bval) && bval)
			prop.SetPickThroughFlag(true);
		if (pNode->GetUserPropBool("地表面", bval) && bval)
			prop.SetOnGroundFlag(true);
		if (pNode->GetUserPropBool("分割面", bval) && bval)
			prop.SetBSPPVSFlag(true);
		if (pNode->GetUserPropBool("不渲染", bval) && bval)
			prop.SetNotRenderFlag(true);
		if (pNode->GetUserPropBool("忽略行走", bval) && bval)
			prop.SetWalkIgnoreFlag(true);
		if (pNode->GetUserPropBool("行走", bval) && bval)
			prop.SetOnlyForWalkFlag(true);

		if (pNode->GetUserPropInt("材料", nval))
			prop.MtlType = (A3DMATERIALTYPE)nval;

		//Now init the mesh objects;
		pThisMesh = NULL;
		if( pOption->bPutIntoOneFrame )
		{
			//Now we should test if the mesh's material has appeared before;
			//If it is we just add more verts to that mesh;
			bNewMesh = false;
			pThisMesh = FindSameMesh(g_pScene, szTextureName, clAmbient, clDiffuse, clEmissive, clSpecular, fShiness, b2Sided, prop);
		}

		if( !pThisMesh )
		{
			pThisMesh = new A3DMesh();
			bNewMesh = true;
			if( !pThisMesh->Init(pDevice, nObjectFrameCount, nA3DVertCount, nIndexCount) )
				goto Failure;
		}

		if( !pOption->bPutIntoOneFrame )
		{
			pThisFrame = new A3DFrame();
			pThisFrame->SetName(pNode->GetName());

			if( !pThisFrame->Init(pDevice, nNodeFrameCount) )
				goto Failure;
			pThisFrame->SetParent(pParentFrame);
		}

		if( bNewMesh )
		{
			if( strlen(szTextureName) && !pThisMesh->SetTexture(szTextureName) )
				goto Failure;
			if( bHasDetail )
			{
				if( strlen(szDetailTextureName) && !pThisMesh->SetDetailTexture(szDetailTextureName, matDetailTM) )
					goto Failure;
			}
			if( !pThisMesh->SetMaterial(clAmbient, clDiffuse, clEmissive, clSpecular, fShiness, b2Sided) )
				goto Failure;
		}

		//Now determine the mesh transform matrix and if flipped;
		tm = pNode->GetNodeTM(ip->GetTime());
		
		if( DotProd(CrossProd(tm.GetRow(0), tm.GetRow(1)), tm.GetRow(2)) > 0 )
			bFlip = false;
		else
			bFlip = true;

		bool bFlip2;
		tm = pNode->GetObjTMAfterWSM(ip->GetTime());
		if( DotProd(CrossProd(tm.GetRow(0), tm.GetRow(1)), tm.GetRow(2)) > 0 )
			bFlip2 = false;
		else
			bFlip2 = true;

		//Now build the indices list;
		for(i=0; i<nIndexCount / 3; i++)
		{
			if( !bFlip && !bFlip2)
			{
				pIndices[i * 3 + 0] = objTri->mesh.faces[i].v[1];
				pIndices[i * 3 + 1] = objTri->mesh.faces[i].v[0];
				pIndices[i * 3 + 2] = objTri->mesh.faces[i].v[2];
				if( bHasUV )
				{
					pTVIndices[i * 3 + 0] = objTri->mesh.tvFace[i].t[1];
					pTVIndices[i * 3 + 1] = objTri->mesh.tvFace[i].t[0];
					pTVIndices[i * 3 + 2] = objTri->mesh.tvFace[i].t[2];
				}
			}
			else
			{
				pIndices[i * 3 + 0] = objTri->mesh.faces[i].v[0];
				pIndices[i * 3 + 1] = objTri->mesh.faces[i].v[1];
				pIndices[i * 3 + 2] = objTri->mesh.faces[i].v[2];
				if( bHasUV )
				{
					pTVIndices[i * 3 + 0] = objTri->mesh.tvFace[i].t[0];
					pTVIndices[i * 3 + 1] = objTri->mesh.tvFace[i].t[1];
					pTVIndices[i * 3 + 2] = objTri->mesh.tvFace[i].t[2];
				}
			}
		}

		if( bHasUV )
			memcpy(pA3DIndices, pTVIndices, sizeof(WORD) * nIndexCount);
		else
			memcpy(pA3DIndices, pIndices, sizeof(WORD) * nIndexCount);

		if( bNewMesh )
		{
			if( !pThisMesh->SetIndices(pA3DIndices, nIndexCount) )
				goto Failure;
		}
		
		if( os.obj != objTri )
			objTri->DeleteMe();
		//Init memory and fill index buffer;
		EndPerformanceRecord(1);

		//Now loop to set all frame's verts;
		for(nFrame=0; nFrame<nFrameCount; nFrame++)
		{
			int nPos = ((nAddCount - 1) * nFrameCount + nFrame) * 100 / (ip->GetSelNodeCount() * nFrameCount);
			ip->ProgressUpdate(nPos);

			TimeValue t = /*animRange.Start() + */(m_nExportStart + nFrame) * GetTicksPerFrame();

			if( pOption->bPutIntoOneFrame && !pOption->bAllowObjectAnim )
			{
				t = ip->GetTime();
			}
			
			if( nFrame < nObjectFrameCount )
			{
				Matrix3 matObject;
				Matrix3 NodeTM;

				BeginPerformanceRecord(2);
				ObjectState frameOS = pNode->EvalWorldState(t);
				//<== Eval;
				EndPerformanceRecord(2);
			
				BeginPerformanceRecord(3);
				if( !frameOS.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
					goto Failure;
				//Can Convert
				EndPerformanceRecord(3);

				BeginPerformanceRecord(4);
				TriObject * frameTriObj = (TriObject *)frameOS.obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));
				if( !frameTriObj )
					goto Failure;
				//Convert
				EndPerformanceRecord(4);

				BeginPerformanceRecord(5);
				Mesh& mesh = frameTriObj->mesh;

				if( pOption->bPutIntoOneFrame )
				{
					//Transform into world coordinates;
					tm = pNode->GetObjTMAfterWSM(t);
					matObject = tm;
				}
				else
				{
					tm = pNode->GetObjTMAfterWSM(t);
					GetNodeUnitTM(t, pNode, &NodeTM);
					NodeTM.Invert();
					matObject = tm * NodeTM;
				}
				//Get Frame TM;
				EndPerformanceRecord(5);

				BeginPerformanceRecord(6);
				if( mesh.getNumVerts() != nVertCount || mesh.getNumFaces() * 3 != nIndexCount ||
					(bHasUV && mesh.getNumTVerts() != nTVVertCount) )
					goto Failure;

				//Now build the vertex list of original mesh;
				for(i=0; i<nVertCount; i++)
				{
					//Note that we have exchange the z and y part for RH to LH;
					Point3 vert = mesh.getVert(i) * matObject;
					pVerts[i].x = vert.x;
					pVerts[i].y = vert.z;
					pVerts[i].z = vert.y;
				}

				//Build pTVVerts here, if need;
				if( bHasUV )
				{
					for(i=0; i<nIndexCount; i++)
						pTVVerts[pTVIndices[i]] = pVerts[pIndices[i]];
				}

				ZeroMemory(pNormals, sizeof(Point3) * nNormalCount);
				ZeroMemory(pSmoothNum, sizeof(int) * nNormalCount);

				//Calculate the vertex normals;
				for(i=0; i<nIndexCount / 3; i++)
				{
					Point3 edge01, edge12;
					Point3 faceNormal;

					if( bHasUV && !pOption->bNormalKeep )
					{
						n0 = pTVIndices[i * 3 + 0];
						n1 = pTVIndices[i * 3 + 1];
						n2 = pTVIndices[i * 3 + 2];
						edge01 = pTVVerts[n1] - pTVVerts[n0];
						edge12 = pTVVerts[n2] - pTVVerts[n1];
						faceNormal = CrossProd(edge01, edge12);
					}
					else
					{
						n0 = pIndices[i * 3 + 0];
						n1 = pIndices[i * 3 + 1];
						n2 = pIndices[i * 3 + 2];
						edge01 = pVerts[n1] - pVerts[n0];
						edge12 = pVerts[n2] - pVerts[n1];
						faceNormal = CrossProd(edge01, edge12);				
					}
					
					//For all verts are in local frame, so the flip is not allowed here;
					if( bFlip && bFlip2 && !pOption->bPutIntoOneFrame )
						faceNormal *= -1.0f;

					pNormals[n0] += faceNormal;
					pNormals[n1] += faceNormal;
					pNormals[n2] += faceNormal;
					pSmoothNum[n0] ++;
					pSmoothNum[n1] ++;
					pSmoothNum[n2] ++;
				}

				//Average the face normals to create proper vertex normal;
				for(i=0; i<nNormalCount; i++)
				{
					if( pSmoothNum[i] == 0 )
						continue;//Isoleted vertex;
					pNormals[i] /= pSmoothNum[i];
					pNormals[i] = pNormals[i].Normalize();
				}

				//Build A3DVERTEX List here, include pos, normal and tu, tv;
				for(i=0; i<nIndexCount; i++)
				{
					int n;
					if( bHasUV )
					{
						n = pTVIndices[i];
						pA3DVerts[n].x = pTVVerts[n].x;
						pA3DVerts[n].y = pTVVerts[n].y;
						pA3DVerts[n].z = pTVVerts[n].z;
						
						//Now use the uv Transform matrix got from texmap;
						UVVert uv = mesh.tVerts[n];
						uv = uv * tmTexture;
						pA3DVerts[n].tu = uv.x;
						pA3DVerts[n].tv = 1.0f - uv.y;
					}
					else
					{
						n = pIndices[i];
						pA3DVerts[n].x = pVerts[n].x;
						pA3DVerts[n].y = pVerts[n].y;
						pA3DVerts[n].z = pVerts[n].z;
						pA3DVerts[n].tu = pA3DVerts[n].tv = 0.0f;
					}

					if( bHasUV )
					{
						if( pOption->bNormalKeep )
						{
							pA3DVerts[pTVIndices[i]].nx = pNormals[pIndices[i]].x;
							pA3DVerts[pTVIndices[i]].ny = pNormals[pIndices[i]].y;
							pA3DVerts[pTVIndices[i]].nz = pNormals[pIndices[i]].z;
						}
						else
						{
							pA3DVerts[pTVIndices[i]].nx = pNormals[pTVIndices[i]].x;
							pA3DVerts[pTVIndices[i]].ny = pNormals[pTVIndices[i]].y;
							pA3DVerts[pTVIndices[i]].nz = pNormals[pTVIndices[i]].z;
						}
					}
					else
					{
						pA3DVerts[pIndices[i]].nx = pNormals[pIndices[i]].x;
						pA3DVerts[pIndices[i]].ny = pNormals[pIndices[i]].y;
						pA3DVerts[pIndices[i]].nz = pNormals[pIndices[i]].z;
					}
				}

				if( bNewMesh )
				{
					pThisMesh->SetProperty(prop);
					pThisMesh->SetVerts(nFrame, pA3DVerts, nA3DVertCount);
				}
				else
				{
					pThisMesh->AddMoreVertex(pA3DVerts, nA3DVertCount, pA3DIndices, nIndexCount);
				}

				pThisMesh->BuildOBBs();

				if( frameOS.obj != frameTriObj )
					frameTriObj->DeleteMe();
				EndPerformanceRecord(6);
			}
			
			if( !pOption->bPutIntoOneFrame && nFrame < nNodeFrameCount )
			{
				A3DMATRIX4 matTM = IdentityMatrix();
				Matrix3	   thisTM, parentTM, relativeTM;
				Point3	   row0, row1, row2, row3;
				
				GetNodeUnitTM(t, pNode, &thisTM);
				GetNodeUnitTM(t, pParentNode, &parentTM);
				parentTM.Invert();
				relativeTM = thisTM * parentTM;
				row0 = relativeTM.GetRow(0);
				row1 = relativeTM.GetRow(1);
				row2 = relativeTM.GetRow(2);
				row3 = relativeTM.GetRow(3);

				matTM.m[0][0] = row0.x;
				matTM.m[0][1] = row0.z;
				matTM.m[0][2] = row0.y;
				
				matTM.m[1][0] = row2.x;
				matTM.m[1][1] = row2.z;
				matTM.m[1][2] = row2.y;
				
				matTM.m[2][0] = row1.x;
				matTM.m[2][1] = row1.z;
				matTM.m[2][2] = row1.y;

				matTM.m[3][0] = row3.x;
				matTM.m[3][1] = row3.z;
				matTM.m[3][2] = row3.y;

				pThisFrame->SetTM(matTM, true, nFrame);
			}
		}
		//Add Frame Matrix;
		EndPerformanceRecord(7);

		free(pVerts);
		free(pIndices);
		if( bHasUV )
		{
			free(pTVVerts);
			free(pTVIndices);
		}
		free(pNormals);
		free(pSmoothNum);
		free(pA3DVerts);
		free(pA3DIndices);

		if( !pOption->bPutIntoOneFrame )
		{
			pParentFrame->AddChild(pThisFrame);
			pThisFrame->AddMesh(pThisMesh);
		}
		else
		{
			if( bNewMesh )
				pParentFrame->AddMesh(pThisMesh);
		}
	}
	
Exit:
	bool bAnimatedForce;
	bAnimatedForce = false;
	if( pThisFrame == NULL )
	{
		//This Frame will not be included in the model;
		//If this frame is animated, the childs my inherit this property;
		bAnimatedForce = bAnimated;
	}
	else
	{
		pParentNode = pNode;
		pParentFrame = pThisFrame;
	}

	int nChildNodeNum;
	INode * pChildNode;
	int i;

	nChildNodeNum = pNode->NumberOfChildren();
	for(i=0; i<nChildNodeNum; i++)
	{
		pChildNode = pNode->GetChildNode(i);
		AddNodeToFrame(hD3DWnd, pDevice, pParentFrame, pChildNode, pParentNode, bAnimatedForce, pOption);
	}

	return true;

Failure:
	MessageBox(NULL, "Add Node To Frame Fail", "FAIL", MB_OK);
	return false;
}

bool MOXExport::IsNodeMatrixAnimated(INode * pNode, int nStart, int nEnd)
{
	Matrix3 thisTM, parentTM, relativeTM, lastRelativeTM;

	int nFrame;
	Interval animRange = ip->GetAnimRange();
	char * szNodeName = pNode->GetName();
	for(nFrame=nStart; nFrame<nEnd; nFrame++)
	{
		TimeValue t = /*animRange.Start() + */nFrame * GetTicksPerFrame();
		BeginPerformanceRecord(11);
		thisTM = pNode->GetNodeTM(t);
		EndPerformanceRecord(11);
		BeginPerformanceRecord(12);
		parentTM = pNode->GetParentTM(t);
		EndPerformanceRecord(12);
		BeginPerformanceRecord(13);
		parentTM.Invert();
		relativeTM = thisTM * parentTM;
		if( nFrame != nStart )
		{
			if( !relativeTM.Equals(lastRelativeTM, 1e-4f) )
			{
				EndPerformanceRecord(13);
				return true;
			}
		}
		EndPerformanceRecord(13);
		lastRelativeTM = relativeTM;
	}

	return false;
}

bool MOXExport::IsNodeAbsoluteMatrixAnimated(INode * pNode, int nStart, int nEnd)
{
	Matrix3 absoluteTM, lastAbsoluteTM;

	int nFrame;
	Interval animRange = ip->GetAnimRange();
	char * szNodeName = pNode->GetName();
	for(nFrame=nStart; nFrame<nEnd; nFrame++)
	{
		TimeValue t = /*animRange.Start() + */nFrame * GetTicksPerFrame();
		absoluteTM = pNode->GetNodeTM(t);
		if( nFrame != nStart )
		{
			if( !absoluteTM.Equals(lastAbsoluteTM, 1e-4f) )
			{
				return true;
			}
		}
		lastAbsoluteTM = absoluteTM;
	}

	return false;
}

bool MOXExport::GetNodeUnitTM(TimeValue t, INode * pNode, Matrix3 * pMatrix)
{
	Matrix3 tm = pNode->GetNodeTM(t);
	Point3 row0, row1, row2;
	
	row0 = tm.GetRow(0);
	row1 = tm.GetRow(1);
	row2 = tm.GetRow(2);

	row0 = row0.Normalize();
	row1 = row1.Normalize();
	row2 = row2.Normalize();

	pMatrix->SetRow(0, row0);
	pMatrix->SetRow(1, row1);
	pMatrix->SetRow(2, row2);
	pMatrix->SetRow(3, tm.GetRow(3));
	return true;
}

bool MOXExport::BeginPerformanceRecord(int n)
{
	return true;
	if( !QueryPerformanceCounter(m_TickBegin + n) )
		return false; 

	return true;
}

bool MOXExport::EndPerformanceRecord(int n)
{
	return true;
	LARGE_INTEGER tickEnd;

	if( !QueryPerformanceCounter(&tickEnd) )
		return false;

	m_TickUsed[n].QuadPart += (tickEnd.QuadPart - m_TickBegin[n].QuadPart);
	return true;
}

bool MOXExport::ResetPerformanceRecord()
{
	memset(m_TickBegin, 0, sizeof(LARGE_INTEGER) * COUNTER_NUM);
	memset(m_TickUsed, 0, sizeof(LARGE_INTEGER) * COUNTER_NUM);

	return true;
}

bool AGetFileTitle(LPCTSTR lpszFile, LPTSTR lpszTitle, WORD cbBuf)
{
    char * pszTemp;
    
    pszTemp = (char *)lpszFile + lstrlen(lpszFile);

    --pszTemp;
    if ('\\' == (* pszTemp)) return false;
    while (true) {
        if ('\\' == (* pszTemp)) {
            ++pszTemp;
            break;
        }
        if (pszTemp == lpszFile) break;
        --pszTemp;
    }
    lstrcpy(lpszTitle, pszTemp);
    return true;
}

bool AGetFilePath(LPCTSTR lpszFile, LPTSTR lpszPath, WORD cbBuf)
{
    char * pszTemp;
    
	strncpy(lpszPath, lpszFile, cbBuf);
    pszTemp = (char *)lpszPath + lstrlen(lpszPath);

    --pszTemp;
    while (true) {
        if ('\\' == (* pszTemp)) {
            break;
        }
        if (pszTemp == lpszPath) break;
        --pszTemp;
    }
    *pszTemp = '\0';
    return true;
}
