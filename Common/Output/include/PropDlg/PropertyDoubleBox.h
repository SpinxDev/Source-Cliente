// PropertyDoubleBox.h: interface for the CPropertyDoubleBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTYDOUBLEBOX_H__6B440784_A8A9_4D49_9722_F18B073D7358__INCLUDED_)
#define AFX_PROPERTYDOUBLEBOX_H__6B440784_A8A9_4D49_9722_F18B073D7358__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PropertyBox.h"

class CPropertyDoubleBox : public CPropertyBox  
{
public:
	CPropertyDoubleBox();
	virtual ~CPropertyDoubleBox();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyFloatBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void DrawSelf(CDC * pDC);
	virtual BOOL IsModified();
	virtual void SetReadOnly(BOOL bReadOnly);

	// Generated message map functions
protected:

	afx_msg void OnEditChanged();
	CEdit m_wnd_edit;
	//{{AFX_MSG(CPropertyFloatBox)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PROPERTYDOUBLEBOX_H__6B440784_A8A9_4D49_9722_F18B073D7358__INCLUDED_)
