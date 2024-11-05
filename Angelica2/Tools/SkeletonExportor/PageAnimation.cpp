// PageAnimation.cpp : implementation file
//

#include "Global.h"
#include "SkeletonExportor.h"
#include "PageAnimation.h"
#include "SkeletonCandidate.h"
#include "MAXInterface.h"
#include "SheetSkeleton.h"

/////////////////////////////////////////////////////////////////////////////
// CPageAnimation property page

IMPLEMENT_DYNCREATE(CPageAnimation, CPropertyPage)

CPageAnimation::CPageAnimation() : CPropertyPage(CPageAnimation::IDD)
{
	//{{AFX_DATA_INIT(CPageAnimation)
	m_iEndFrame = 0;
	m_iStartFrame = 0;
	m_strStep = _T("");
	//}}AFX_DATA_INIT

	m_iStepIndex	= 0;
	m_iStepTotal	= 0;
	m_iStartFrame	= 0;
	m_iEndFrame		= 100;
	
	m_pSkeletonCandidate	= NULL;
}

CPageAnimation::~CPageAnimation()
{
}

void CPageAnimation::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageAnimation)
	DDX_Text(pDX, IDC_EDIT_END, m_iEndFrame);
	DDX_Text(pDX, IDC_EDIT_START, m_iStartFrame);
	DDX_Text(pDX, IDC_STATIC_STEP, m_strStep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageAnimation, CPropertyPage)
	//{{AFX_MSG_MAP(CPageAnimation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageAnimation message handlers

void CPageAnimation::SetStep(int iIndex, int iTotal)
{
	m_iStepIndex = iIndex;
	m_iStepTotal = iTotal;

	m_strStep.Format("Step %d of %d", iIndex, iTotal);
}

BOOL CPageAnimation::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_iStartFrame	= g_pMAXInterface->GetStartFrame();
	m_iEndFrame		= g_pMAXInterface->GetEndFrame();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CPageAnimation::BeginPage()
{
	return true;
}

bool CPageAnimation::EndPage()
{
	UpdateData(TRUE);

	m_pSkeletonCandidate->SetStartFrame(m_iStartFrame);
	m_pSkeletonCandidate->SetEndFrame(m_iEndFrame);
	m_pSkeletonCandidate->SetAnimFPS(g_pMAXInterface->GetFps());

	return true;
}

LRESULT CPageAnimation::OnWizardNext() 
{
	//	Cleanup page data
	if (!EndPage())
		return -1;
	
	return CPropertyPage::OnWizardNext();
}

BOOL CPageAnimation::OnWizardFinish() 
{
	//	Cleanup page data
	if (!EndPage())
		return FALSE;
	
	return CPropertyPage::OnWizardFinish();
}

BOOL CPageAnimation::OnSetActive() 
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

