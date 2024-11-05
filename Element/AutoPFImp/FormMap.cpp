// FormMap.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPFImp.h"
#include "FormMap.h"


#include "Global.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormMap

IMPLEMENT_DYNCREATE(CFormMap, CFormView)

CFormMap::CFormMap()
	: CFormView(CFormMap::IDD)
{
	//{{AFX_DATA_INIT(CFormMap)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CFormMap::~CFormMap()
{
}

void CFormMap::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormMap)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormMap, CFormView)
	//{{AFX_MSG_MAP(CFormMap)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_MAPCFG, OnButtonLoadMapcfg)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_LAYER, OnButtonViewLayer)
	ON_BN_CLICKED(IDC_BUTTON_ADD_JOINT, OnButtonAddJoint)
	ON_BN_CLICKED(IDC_BUTTON_DEL_JOINT, OnButtonDelJoint)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_MAPBIN, OnButtonLoadMapbin)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_MAPCFG2, OnButtonLoadMapcfg2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormMap diagnostics

#ifdef _DEBUG
void CFormMap::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormMap::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFormMap message handlers

void CFormMap::OnButtonLoadMapcfg() 
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDlg(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("txt files (*.txt)|*.txt||"));
	TCHAR szBuffer[MAX_PATH];
	szBuffer[0] = _T('\0');
	FileDlg.m_ofn.lpstrFile = szBuffer;
	FileDlg.m_ofn.nMaxFile  = MAX_PATH;

	if(FileDlg.DoModal()!=IDOK) return;	
	CString file_path = FileDlg.GetPathName();

	char drive[MAX_PATH];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];

	_splitpath(AC2AS(file_path), drive, dir, fname, ext);
	
	CString file_dir(drive);
	file_dir += dir;
	AUX_SetWorkDir(AC2AS(file_dir));
	g_szCfgPath = AC2AS(file_path);
	if (AUX_GetMainFrame()->GetView()->LoadMapCfg(AC2AS(file_path))){
		SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_VIEW_LAYER, BN_CLICKED));
	}
}

void CFormMap::OnButtonLoadMapcfg2() 
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDlg(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("txt files (*.txt)|*.txt||"));
	TCHAR szBuffer[MAX_PATH];
	szBuffer[0] = _T('\0');
	FileDlg.m_ofn.lpstrFile = szBuffer;
	FileDlg.m_ofn.nMaxFile  = MAX_PATH;
	
	if(FileDlg.DoModal()!=IDOK) return;	
	CString file_path = FileDlg.GetPathName();
	
	char drive[MAX_PATH];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];
	
	_splitpath(AC2AS(file_path), drive, dir, fname, ext);
	
	CString file_dir(drive);
	file_dir += dir;
	AUX_SetWorkDir(AC2AS(file_dir));
	g_szCfgPath = AC2AS(file_path);
	if (AUX_GetMainFrame()->GetView()->LoadMapCfg(AC2AS(file_path), true)){
		SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_VIEW_LAYER, BN_CLICKED));
	}
}

void CFormMap::OnButtonViewLayer() 
{
	// TODO: Add your control notification handler code here
	
	int iLayer = GetDlgItemInt(IDC_EDIT_LAYER);

	AUX_GetMainFrame()->GetView()->ViewLayer(iLayer);
}

void CFormMap::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	_ResetJoints();
}

void CFormMap::AddJoint(const char * txt)
{
	Joint joint;
	CString msg;
	char line[1000];
	char * token;
	char seps[]   = " ,\t";
	strcpy(line, txt);
	token = strtok(line, seps);
	if (!token)
	{
		msg.Format(_T("%s unknown format"), txt);
		AfxMessageBox(msg);
		return;
	}
	joint.layer1 = atoi(token);
	token = strtok(NULL, seps);
	if (!token)
	{
		msg.Format(_T("%s unknown format"), txt);
		AfxMessageBox(msg);
		return ;
	}
	joint.layer2 = atoi(token);
	token = strtok(NULL, seps);
	if (!token)
	{
		msg.Format(_T("%s unknown format"), txt);
		AfxMessageBox(msg);
		return ;
	}
	joint.ptMap.x = atoi(token);
	token = strtok(NULL, seps);
	if (!token)
	{
		msg.Format(_T("%s unknown format"), txt);
		AfxMessageBox(msg);
		return ;
	}
	joint.ptMap.y = atoi(token);

	
	CLayerMap * pLayerMap1 = g_AutoMoveAgent.GetMoveMap()->GetLayer(joint.layer1);
	if (!pLayerMap1)
	{
		msg.Format(_T("no layer %d"), joint.layer1);
		AfxMessageBox(msg);
		return;
	}

	if (!pLayerMap1->GetRMap()->GetPixel(joint.ptMap.x, joint.ptMap.y))
	{
		msg.Format(_T("layer %d pt(%d %d) unpassable"), joint.layer1, joint.ptMap.x, joint.ptMap.y);
		AfxMessageBox(msg);
		return;
	}

	CLayerMap * pLayerMap2 = g_AutoMoveAgent.GetMoveMap()->GetLayer(joint.layer2);
	if(!pLayerMap2)
	{
		msg.Format(_T("no layer %d"), joint.layer2);
		AfxMessageBox(msg);
		return;
	}

	if (!pLayerMap2->GetRMap()->GetPixel(joint.ptMap.x, joint.ptMap.y))
	{
		msg.Format(_T("layer %d pt(%d %d) unpassable"), joint.layer2, joint.ptMap.x, joint.ptMap.y);
		AfxMessageBox(msg);
		return;
	}

	CJointList * pJointList = g_AutoMoveAgent.GetMoveMap()->GetJointList();

	if (!pJointList->Check(joint))
	{
		msg.Format(_T("fail to add joint %d %d (%d %d), may be too close to other joints"));
		AfxMessageBox(msg);
		return;
	}

	pJointList->Add(joint);
	_ResetJoints();
}


void CFormMap::OnButtonAddJoint() 
{
	// TODO: Add your control notification handler code here
	CString txt;
	GetDlgItemText(IDC_EDIT_JOINT, txt);

	AddJoint(AC2AS(txt));

}

void CFormMap::_ResetJoints()
{
	CListBox * pListBox = (CListBox *)GetDlgItem(IDC_LIST_JOINT);
	pListBox->ResetContent();

	CJointList * pJointList = g_AutoMoveAgent.GetMoveMap()->GetJointList();
	if (!pJointList)
	{
		return;
	}
	int i;
	CString txt;
	for (i = 0; i < pJointList->GetCount(); ++i)
	{
		const	Joint * pJoint = pJointList->GetJoint(i);
		txt.Format(_T("%d %d %d %d"), pJoint->layer1, pJoint->layer2, pJoint->ptMap.x, pJoint->ptMap.y);
		pListBox->AddString(txt);
	}
}

void CFormMap::OnButtonDelJoint() 
{
	// TODO: Add your control notification handler code here
	CListBox * pListBox = (CListBox *)GetDlgItem(IDC_LIST_JOINT);
	int iSel = pListBox->GetCurSel();
	
	if (iSel != CB_ERR && pListBox->GetCount()>= 1)
	{
		CJointList * pJointList = g_AutoMoveAgent.GetMoveMap()->GetJointList();				
		pJointList->Del(iSel);
		_ResetJoints();
	}
}

void CFormMap::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	AUX_GetMainFrame()->GetView()->Save();
}

void CFormMap::OnButtonLoadMapbin() 
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDlg(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("cfg files (*.cfg)|*.cfg||"));
	TCHAR szBuffer[MAX_PATH];
	szBuffer[0] = '\0';
	FileDlg.m_ofn.lpstrFile = szBuffer;
	FileDlg.m_ofn.nMaxFile  = MAX_PATH;

	if(FileDlg.DoModal()!=IDOK) return;	
	CString file_path = FileDlg.GetPathName();

	char drive[MAX_PATH];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];

	_splitpath(AC2AS(file_path), drive, dir, fname, ext);
	
	CString file_dir(drive);
	file_dir += dir;
	AUX_SetWorkDir(AC2AS(file_dir));

	g_szCfgPath = AC2AS(file_dir + fname + _T(".txt"));

	if (AUX_GetMainFrame()->GetView()->LoadMapBin(AC2AS(file_path))){
		SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_VIEW_LAYER, BN_CLICKED));
	}
	
}
