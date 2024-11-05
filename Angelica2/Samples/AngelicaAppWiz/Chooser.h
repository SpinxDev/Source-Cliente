#if !defined(AFX_CHOOSER_H__4C894B74_7E22_4EAD_81B9_711894273409__INCLUDED_)
#define AFX_CHOOSER_H__4C894B74_7E22_4EAD_81B9_711894273409__INCLUDED_

// chooser.h : declaration of the CDialogChooser class
//             This class keeps track of what dialogs to pop up when.

#define LAST_DLG 3

class CDialogChooser
{
public:

	enum
	{
		MAXSTEP_WIN32	= 3,
		MAXSTEP_MFC		= 3,
	};

public:
	CDialogChooser();
	~CDialogChooser();

	// All calls by mfcapwz.dll to CAngelicaAppWizAppWiz::Next
	//  & CAngelicaAppWizAppWiz::Back are delegated to these member
	//  functions, which keep track of what dialog is up
	//  now, and what to pop up next.
	CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);

	//	Get current step
	int GetCurStep() const { return m_nCurrDlg; }

protected:

	static int	m_aWin32Track[MAXSTEP_WIN32];	//	Win32 app track
	static int	m_aMFCTrack[MAXSTEP_MFC];		//	MFC app track

	int*	m_pCurTrack;	//	Current track
	int		m_iMaxStep;		//	Current track's maximum step

	//	Current step's index into the internal array m_pDlgs
	int m_nCurrDlg;

	// Internal array of pointers to the steps
	CAppWizStepDlg* m_pDlgs[LAST_DLG + 1];
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSER_H__4C894B74_7E22_4EAD_81B9_711894273409__INCLUDED_)
