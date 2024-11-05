#if !defined(AFX_SHADERFORM_H__80BF8CF4_6E33_4A93_B221_2CB82D06F067__INCLUDED_)
#define AFX_SHADERFORM_H__80BF8CF4_6E33_4A93_B221_2CB82D06F067__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShaderForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShaderForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include <A3DShader.h>

struct SHADERINFO;

class CShaderForm : public CFormView
{
public:
	CShaderForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CShaderForm)

// Form Data
public:
	//{{AFX_DATA(CShaderForm)
	enum { IDD = IDD_SHADERFORM };
	CListBox	m_ShaderList;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

	bool		Initialize();					//	Own initial routine
	void		Release();						//	Release all resources

	bool		NewShader(char* szName, char* szFile);	//	Create a new shader
	bool		AddShader(const char* szFileName);		//	Add a shader
	SHADERINFO*	GetCurrentShader();						//	Get current shader's informatoin
	bool		SaveAllShaders(bool bAsk);				//	Save all shaders

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShaderForm)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShaderForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CShaderForm)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeShaderlist();
	afx_msg void OnShaderRename();
	afx_msg void OnShaderRemove();
	afx_msg void OnShaderAddstage();
	afx_msg void OnShaderSave();
	afx_msg void OnShaderSaveas();
	afx_msg void OnShaderCopy();
	afx_msg void OnShaderPaste();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attributes
protected:

	HMENU		m_hShaderMenu;	

// Operations
protected:

	void		RemoveShader(int iIndex);		//	Remove a shader
	void		RemoveAllShaders();				//	Remove all shaders

	bool		SaveOneShader(int iIndex);		//	Save one shader
	bool		SaveShaderGeneral(FILE* fp, A3DShader* pShader); 	//	Save shader's general properties
	bool		SaveShaderStage(FILE* fp, int iStage, SHADERSTAGE& Stage);	//	Save one stage in shader
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHADERFORM_H__80BF8CF4_6E33_4A93_B221_2CB82D06F067__INCLUDED_)
