// PageSkeletonFile.cpp : implementation file
//

#include "Global.h"
#include "SkeletonExportor.h"
#include "PageSkeletonFile.h"
#include "SkeletonCandidate.h"

/////////////////////////////////////////////////////////////////////////////
// CPageSkeletonFile property page

IMPLEMENT_DYNCREATE(CPageSkeletonFile, CPropertyPage)

CPageSkeletonFile::CPageSkeletonFile() : CPropertyPage(CPageSkeletonFile::IDD)
{
	//{{AFX_DATA_INIT(CPageSkeletonFile)
	m_strSkeletonFile = _T("");
	m_strStep = _T("");
	//}}AFX_DATA_INIT
	
	m_iStepIndex = 0;
	m_iStepTotal = 0;

	m_pSkeletonCandidate = NULL;
}

CPageSkeletonFile::~CPageSkeletonFile()
{
}

void CPageSkeletonFile::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageSkeletonFile)
	DDX_Text(pDX, IDC_EDIT_SKELETONFILE, m_strSkeletonFile);
	DDX_Text(pDX, IDC_STATIC_STEP, m_strStep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageSkeletonFile, CPropertyPage)
	//{{AFX_MSG_MAP(CPageSkeletonFile)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageSkeletonFile message handlers

void CPageSkeletonFile::SetStep(int iIndex, int iTotal)
{
	m_iStepIndex = iIndex;
	m_iStepTotal = iTotal;

	m_strStep.Format("Step %d of %d", iIndex, iTotal);
}

void CPageSkeletonFile::OnBtnBrowse() 
{
	//	Let user select a skeleton (.skl) file
	char szFilter[] = "骨骼文件(*.bon)|*.bon||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
	CFileDialog	FileDia(TRUE, "*.bon", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDCANCEL)
		return;

	m_strSkeletonFile = FileDia.GetPathName();

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_SKELETONFILE, m_strSkeletonFile);
}

LRESULT CPageSkeletonFile::OnWizardNext() 
{
	//	Cleanup page data
	if (!EndPage())
		return -1;
	
	return CPropertyPage::OnWizardNext();
}

BOOL CPageSkeletonFile::OnWizardFinish() 
{
	//	Cleanup page data
	if (!EndPage())
		return FALSE;
	
	return CPropertyPage::OnWizardFinish();
}

BOOL CPageSkeletonFile::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPageSkeletonFile::OnSetActive() 
{
	//	Get property sheet
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();

	//	Set wizard buttons and text
	DWORD dwButtons = PSWIZB_NEXT;

	if (m_iStepIndex == m_iStepTotal)
		dwButtons |= PSWIZB_FINISH;

	if (m_iStepIndex > 1)
		dwButtons |= PSWIZB_BACK;

	pSheet->SetWizardButtons(dwButtons);

	// initialize page data
	if (!BeginPage())
		return FALSE;
	
	return CPropertyPage::OnSetActive();
}

bool CPageSkeletonFile::BeginPage()
{
	return true;
}

bool CPageSkeletonFile::EndPage()
{
	UpdateData(TRUE);

	if (m_strSkeletonFile == "")
	{
		MessageBox("请选择一个骨骼文件(.bon) !");
		return false;
	}

	a_SendLogToDebugWnd(0, "Load skeleton from file ...");

	//	Load skeleton from file
	if (!m_pSkeletonCandidate->InitFromFile(m_strSkeletonFile))
	{
		MessageBox("加载骨骼文件(.bon)失败 !");
		return false;
	}

	return true;
}


