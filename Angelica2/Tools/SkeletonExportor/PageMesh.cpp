// PageMesh.cpp : implementation file
//

#include "Global.h"
#include "SkeletonExportor.h"
#include "PageMesh.h"
#include "SkeletonCandidate.h"
#include "SkinCandidate.h"
#include "AAssist.h"

/////////////////////////////////////////////////////////////////////////////
// CPageMesh property page

IMPLEMENT_DYNCREATE(CPageMesh, CPropertyPage)

CPageMesh::CPageMesh() : CPropertyPage(CPageMesh::IDD)
{
	//{{AFX_DATA_INIT(CPageMesh)
	m_iMaxBoneNum = 4;
	m_fMinWeight = 0.0f;
	m_strStep = _T("");
	m_bUpdateSkeleton = FALSE;
	m_bExtraTexDir = FALSE;
	//}}AFX_DATA_INIT

	m_iStepIndex	= 0;
	m_iStepTotal	= 0;
	
	m_pSkeletonCandidate	= NULL;
	m_pSkinCandidate		= NULL;
}

CPageMesh::~CPageMesh()
{
}

void CPageMesh::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMesh)
	DDX_Text(pDX, IDC_EDIT_MAXBONE, m_iMaxBoneNum);
	DDX_Text(pDX, IDC_EDIT_MINWEIGHT, m_fMinWeight);
	DDX_Text(pDX, IDC_STATIC_STEP, m_strStep);
	DDX_Check(pDX, IDC_CHECK_UPDATESKELETON, m_bUpdateSkeleton);
	DDX_Check(pDX, IDC_CHECK_EXTRATEXDIR, m_bExtraTexDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageMesh, CPropertyPage)
	//{{AFX_MSG_MAP(CPageMesh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageMesh message handlers

void CPageMesh::SetStep(int iIndex, int iTotal)
{
	m_iStepIndex = iIndex;
	m_iStepTotal = iTotal;

	m_strStep.Format("Step %d of %d", iIndex, iTotal);
}

BOOL CPageMesh::OnWizardFinish() 
{
	//	Cleanup page data
	if (!EndPage())
		return FALSE;
	
	return CPropertyPage::OnWizardFinish();
}

LRESULT CPageMesh::OnWizardNext() 
{
	//	Cleanup page data
	if (!EndPage())
		return -1;
	
	return CPropertyPage::OnWizardNext();
}

BOOL CPageMesh::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPageMesh::OnSetActive() 
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

bool CPageMesh::BeginPage()
{
	return true;
}

bool CPageMesh::EndPage()
{
	UpdateData(TRUE);

	m_pSkeletonCandidate->SetUpdateFileFlag(m_bUpdateSkeleton ? true : false);
	m_pSkinCandidate->SetExtraTexDirFlag(m_bExtraTexDir ? true : false);

	a_Clamp(m_iMaxBoneNum, 1, 4);
	a_Clamp(m_fMinWeight, 0.0f, 1.0f);

	a_SendLogToDebugWnd(0, "Process all mesh nodes ...");

	//	Process all mesh nodes
	if (!m_pSkinCandidate->ProcessAllMeshNodes(m_pSkeletonCandidate, m_iMaxBoneNum, m_fMinWeight))
	{
		MessageBox("´´½¨ Skin Candidate Ê§°Ü !");
		return false;
	}

	return true;
}


