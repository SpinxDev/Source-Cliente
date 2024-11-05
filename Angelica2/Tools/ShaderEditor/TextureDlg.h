#if !defined(AFX_TEXTUREDLG_H__A9BBF14B_F55D_4B93_9F7A_3CB1D05A959D__INCLUDED_)
#define AFX_TEXTUREDLG_H__A9BBF14B_F55D_4B93_9F7A_3CB1D05A959D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextureDlg.h : header file
//

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

struct TEXTUREDLGPARAMS
{
	int		iType;			//	Texture type
	int		iFormat;		//	Texture format
	char	szFile[256];	//	Texture file name
	int		iNumFrame;		//	Number of frame
	int		dwFrameTime;	//	Frame time (ms)
	int		iD3DSlot;		//	D3D texture slot

	//	Set only by CTextureDlg
	bool	bTexChanged;	//	Texture changed
};

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	CTextureDlg dialog
//
///////////////////////////////////////////////////////////////////////////

class CTextureDlg : public CDialog
{
// Construction
public:
	CTextureDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
public:

// Operations
public:

	//	Initialize data
	bool Init(TEXTUREDLGPARAMS& Params);
	//	Get data
	void GetData(TEXTUREDLGPARAMS& Params);	

// Dialog Data
	//{{AFX_DATA(CTextureDlg)
	enum { IDD = IDD_TEXTUREDLG };
	CComboBox	m_FormatCombo;
	CComboBox	m_TypeCombo;
	int		m_iFrameTime;
	int		m_iNumFrame;
	CString	m_strTexFile;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextureDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeTextypelist();
	afx_msg void OnBtnBrowse();
	afx_msg void OnSelchangeTexformatlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:	//	Attributes

	TEXTUREDLGPARAMS	m_Params;

protected:	//	Operations

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTUREDLG_H__A9BBF14B_F55D_4B93_9F7A_3CB1D05A959D__INCLUDED_)
