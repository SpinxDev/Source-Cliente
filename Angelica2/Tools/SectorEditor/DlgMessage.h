#if !defined(AFX_DLGMESSAGE_H__28094B58_3CA4_4E83_BB60_CBFC9EB35479__INCLUDED_)
#define AFX_DLGMESSAGE_H__28094B58_3CA4_4E83_BB60_CBFC9EB35479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMessage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMessage dialog

class CDlgMessage : public CDialogBar
{
// Construction
public:
	CDlgMessage();   // standard constructor

	DECLARE_DYNCREATE(CDlgMessage)

// Dialog Data
	//{{AFX_DATA(CDlgMessage)
	//}}AFX_DATA

	//	Clear all message
	void ClearAllMessages();
	//	Add a message
	void AddMessage(char* szMsg);

	//	Get message list
	CListBox* GetMessageList();

	//	Set window text
	void SetWindowText(LPCTSTR szText);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMessage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMessage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMESSAGE_H__28094B58_3CA4_4E83_BB60_CBFC9EB35479__INCLUDED_)
