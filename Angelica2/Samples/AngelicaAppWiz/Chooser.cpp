// chooser.cpp : Implements the CDialogChooser class
//

#include "stdafx.h"
#include "AngelicaAppWiz.h"
#include "chooser.h"
#include "cstm1dlg.h"
#include "cstm2dlg.h"
#include "cstm3dlg.h"
#include "AngelicaAppWizaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Remember where the custom steps begin, so we can delete them in
//  the destructor
#define FIRST_CUSTOM_STEP 1
#define LAST_CUSTOM_STEP 3

int CDialogChooser::m_aWin32Track[CDialogChooser::MAXSTEP_WIN32] = {0, 1, 2};
int CDialogChooser::m_aMFCTrack[CDialogChooser::MAXSTEP_MFC] = {0, 1, 3};

// On construction, set up internal array with pointers to each step.
CDialogChooser::CDialogChooser()
{
	m_pDlgs[0]	= NULL;
	m_pDlgs[1]	= new CCustom1Dlg;
	m_pDlgs[2]	= new CCustom2Dlg;
	m_pDlgs[3]	= new CCustom3Dlg;

	m_nCurrDlg	= 0;
	m_pCurTrack	= m_aWin32Track;
	m_iMaxStep	= MAXSTEP_WIN32;
}

// The destructor deletes entries in the internal array corresponding to
//  custom steps.
CDialogChooser::~CDialogChooser()
{
	for (int i = FIRST_CUSTOM_STEP; i <= LAST_CUSTOM_STEP; i++)
	{
		ASSERT(m_pDlgs[i] != NULL);
		delete m_pDlgs[i];
	}
}

// Use the internal array to determine the next step.
CAppWizStepDlg* CDialogChooser::Next(CAppWizStepDlg* pDlg)
{
	ASSERT(0 <= m_nCurrDlg && m_nCurrDlg < m_iMaxStep);
	ASSERT(pDlg == m_pDlgs[m_pCurTrack[m_nCurrDlg]]);

	//	If the current step is the "application type" step, 
	//	the AppWizard "track" may have changed.
	if (m_nCurrDlg == 1)
	{
		CString strTemp;
		if (AngelicaAppWizaw.m_Dictionary.Lookup(_T("APP_TYPE_WIN32"), strTemp))
		{
			m_pCurTrack	= m_aWin32Track;
			m_iMaxStep	= MAXSTEP_WIN32;
		}
		else
		{
			m_pCurTrack	= m_aMFCTrack;
			m_iMaxStep	= MAXSTEP_MFC;
		}

		SetNumberOfSteps(m_iMaxStep-1);
	}

	m_nCurrDlg++;

	//	If the new step is the "application type" step, don't display the 
	//	max number of steps.
	if (m_nCurrDlg == 1)
		SetNumberOfSteps(-1);

	return m_pDlgs[m_pCurTrack[m_nCurrDlg]];
}

// Use the internal array to determine the previous step.
CAppWizStepDlg* CDialogChooser::Back(CAppWizStepDlg* pDlg)
{
	ASSERT(1 <= m_nCurrDlg && m_nCurrDlg <= m_iMaxStep);
	ASSERT(pDlg == m_pDlgs[m_pCurTrack[m_nCurrDlg]]);

	m_nCurrDlg--;

	//	If the new step is the "project type" step, don't display the 
	//	max number of steps.
	if (m_nCurrDlg == 1)
		SetNumberOfSteps(-1);

	return m_pDlgs[m_pCurTrack[m_nCurrDlg]];
}
