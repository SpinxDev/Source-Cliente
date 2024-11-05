#if !defined(AFX_COMMONFILEDLG_H__A89FDAF9_1CF3_4564_B43B_D31BFFC9E205__INCLUDED_)
#define AFX_COMMONFILEDLG_H__A89FDAF9_1CF3_4564_B43B_D31BFFC9E205__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommonFileDlg.h : header file
//

#include "StdAfx.h"
#include "ItemInfo.h"
#include "resource.h"
#include "RenderWnd.h"

class CRenderableObject;

#define AFILE_ALLOWMULFILES		1
/////////////////////////////////////////////////////////////////////////////
// CCommonFileDlg dialog

class CCommonFileDlg : public CDialog
{
// Construction
public:
	CCommonFileDlg(CWnd* pParent = NULL);   // standard constructor
	CCommonFileDlg(A3DEngine* pA3DEngine, DWORD dwmfileFlag, CString sRelPath = "", CWnd* pParent = NULL);

private:
	
	CString m_curDir;				// 打开的文件所在文件夹的完整路径
	CString m_curRoot;				// 当前工程（文件）的工作路径
	CString m_relRoot;				// relative root path, m_curDir - m_curRoot
	CString m_ext;					// extension of file
	CString m_workRoot;				// work root path, m_curRoot + sRelPath
	CString m_relworkRoot;			// m_curDir - m_workRoot
	
	CString difRel;					// record sRelPath, if different sRelPath is defined, so don't open from save log!
	CString m_strExt;

	CPckTreeNode  m_tnRoot;			// package tree
	CPckTreeNode* m_pCurNode;		// current package node
	CPckTreeNode* m_pRootNode;		// package root node

	CBitmap bmp[2];
	CBitmap m_bmpPrev;				// bitmap for preview
	CBitmap bmpbutton[3];

	CImageList m_imageList;			// icon of lookin and file list
	CImageList m_dirImageList;		// icon of dir list

	CStringArray m_arrDir;			// folder array for disk files
	CStringArray m_arrFiles;		// files array for disk files

	CStringArray m_arrFilelist;		// for multiple selected files
	CStringArray m_backDir;			// restore past directory
	CStringArray m_arrCurDir;		// save the decomposed current directory

	CStringArray m_arrWorkRoot;

	bool bChanged;					
	int OpenMode;					// from harddisk or from package
	DWORD flags;					// multiple selection or single selection
	bool bsaved;
	bool set_null;

	A3DEngine*		m_pA3DEngine;
	AFilePackage*	m_pPackage;
	AMSoundBufferMan* m_pSoundBufMan;
	AMSoundBuffer*	m_pSoundBuf;
	HINSTANCE m_hHostResourceHandle;
	
	CPoint			m_ptLast;

	CString			m_strLastPath; //last filepath clicked
	// Dialog Data
	//{{AFX_DATA(CCommonFileDlg)
	enum { IDD = IDD_COMMONFILE_DIALOG };
	CButton	m_sortBut;
	CRenderWnd	m_sPrev;
	CButton	m_upBut;
	CStatic	m_strImageInfo;
	CStatic	m_strFileInfo;
	CListCtrl	m_lstFile;
	CListCtrl	m_lstDir;
	CComboBoxEx	m_cboLookIn;
	CComboBox	m_cboFileType;
	CButton	m_preview;
	CButton	m_prevBut;
	CString	m_fileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommonFileDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	bool OpenSavedLog();
	
	bool InitDir();
	void InitExt();

	bool OpenFromDisk(CString sPath);
	bool UpdateDiskDirCtrl(CString sPath);
	bool UpdateDiskListCtrl(CString sPath);

	bool OpenFromPackage(CString sPath);
	bool UpdatePckListCtrl(CPckTreeNode* pNode);
	bool UpdatePckDirCtrl(CPckTreeNode* pNode);
	void ClosePckListCtrl();

	void UpdateLookinCombo();
	void UpdateArrCurDir(CString sDir);
	void UpdatePrevRegion();
	
	bool LoadPrev(CString& sFile);

	void InsertFiles(CString file);
	void SortItemsDescending();

	bool GetMulFiles();

	void Open();

	CString GetCurDir()		{ return m_curDir; }
	CString GetRootDir()	{ return m_curRoot;	}
//	CString GetRelRoot()	{ return m_relRoot; }
//	CString GetRelWorkRoot()	{ return m_relworkRoot; }
	CString GetWorkRoot()	{ return m_workRoot; }
	
	CString GetRelCurRoot()
	{
		int dirlen = m_curDir.GetLength();
		int rootlen = m_curRoot.GetLength();

		DWORD nCount = dirlen - rootlen -1;
		m_relRoot = m_curDir.Right(nCount);
		
		return m_relRoot;
	}
	CString GetRelWorkRoot()
	{

		int dirlen = m_curDir.GetLength();
		int rootlen = m_workRoot.GetLength();

		DWORD nCount = dirlen- rootlen -1;
		m_relworkRoot = m_curDir.Right(nCount);

		return m_relworkRoot;
	}

	void GetAllWorkRoot(const CString& csRoots)
	{
		int iLastIdx = 0;
		for (int i=0; i<csRoots.GetLength(); ++i)
		{
			if(csRoots.GetAt(i) == '|')
			{
				m_arrWorkRoot.Add(csRoots.Mid(iLastIdx, i-iLastIdx));
				iLastIdx = i+1;
			}
		}
		m_arrWorkRoot.Add(csRoots.Mid(iLastIdx, csRoots.GetLength()-iLastIdx));		
	}
	
// Implementation
public:

	CString GetFullFileName()
	{
		if (set_null)
			return "";

		GetRelCurRoot();
		
		if(m_relRoot == _T(""))
			return m_fileName;
		else
			return (m_relRoot+_T("\\")+m_fileName);
	}

	CString GetRelativeFileName()
	{
		if (set_null)
			return _T("");

		GetRelWorkRoot();

		if(m_relworkRoot == _T(""))
			return m_fileName;
		else
			return (m_relworkRoot+_T("\\")+m_fileName);
	}

	CString GetFirstSelectedFullName()
	{
		GetRelCurRoot();
		
		if(m_arrFilelist.GetSize() == 0)
			return "";
		else
		{
			CString pItem = m_arrFilelist.GetAt(0);

			if(m_relRoot == "")
				return pItem;
			else
				return (m_relRoot+_T("\\")+pItem);
		}		
	}

	CString GetFirstSelectedRelName()
	{
		GetRelWorkRoot();

		if(m_arrFilelist.GetSize() == 0)
			return "";
		else
		{
			CString pItem = m_arrFilelist.GetAt(0);

			if(m_relworkRoot == _T(""))
				return pItem;
			else
				return (m_relworkRoot+_T("\\")+pItem);
		}
	}

	int GetSelectedNum()
	{
		return (int)m_arrFilelist.GetSize();
	}

	CString GetNextSelectedFullName(int nIndex)
	{
		GetRelCurRoot();

		if(GetSelectedNum() != 0 && nIndex < GetSelectedNum())
		{
			if(m_relRoot == _T(""))
				return m_arrFilelist.GetAt(nIndex);
			else
				return (m_relRoot+_T("\\")+m_arrFilelist.GetAt(nIndex));
		}
		else
			return "";
	}
	
	CString GetNextSelectedRelName(int nIndex)
	{
		GetRelWorkRoot();

		if(GetSelectedNum() != 0 && nIndex <= GetSelectedNum())
		{
			if(m_relworkRoot == _T(""))
				return m_arrFilelist.GetAt(nIndex-1);
			else
				return (m_relworkRoot+_T("\\")+m_arrFilelist.GetAt(nIndex-1));
		}
		else
			return "";
	}

	void SetFileExtension(const char* szExt) { m_strExt = szExt; }

	// Generated message map functions
	//{{AFX_MSG(CCommonFileDlg)
	afx_msg void OnChangeFilename();
	afx_msg void OnClickDirlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnPreview();
	afx_msg void OnSelchangeComboFiletype();
	afx_msg void OnSelchangeComboboxexLookin();
	afx_msg void OnbtnUp();
	afx_msg void OnbtnBack();
	afx_msg void OnPaint();
	afx_msg void OnOpen();
	virtual void OnCancel();
	afx_msg void OnSort();
	afx_msg void OnItemchangedFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetNull();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMONFILEDLG_H__A89FDAF9_1CF3_4564_B43B_D31BFFC9E205__INCLUDED_)
