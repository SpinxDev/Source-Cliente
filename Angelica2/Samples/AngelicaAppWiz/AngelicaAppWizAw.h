#if !defined(AFX_ANGELICAAPPWIZAW_H__2B9CA51E_F4BC_49C2_AFCF_7021B6D11D3E__INCLUDED_)
#define AFX_ANGELICAAPPWIZAW_H__2B9CA51E_F4BC_49C2_AFCF_7021B6D11D3E__INCLUDED_

// AngelicaAppWizaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see AngelicaAppWiz.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CAngelicaAppWizAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);

	void SetBoolVar(LPCTSTR lpszName, BOOL bVal);
	void SetStringVar(LPCTSTR lpszName, LPCTSTR lpszVal);

protected:
	CDialogChooser* m_pChooser;

	//	Build document symbol
	void BuildDocumentSymbol();
};

// This declares the one instance of the CAngelicaAppWizAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global AngelicaAppWizaw.  (Its definition is in AngelicaAppWizaw.cpp.)
extern CAngelicaAppWizAppWiz AngelicaAppWizaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANGELICAAPPWIZAW_H__2B9CA51E_F4BC_49C2_AFCF_7021B6D11D3E__INCLUDED_)
