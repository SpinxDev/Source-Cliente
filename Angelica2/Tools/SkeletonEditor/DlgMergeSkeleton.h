#if !defined(AFX_DLGMERGESKELETON_H__D3BFD463_3D7E_4331_9A78_1C8AF1031061__INCLUDED_)
#define AFX_DLGMERGESKELETON_H__D3BFD463_3D7E_4331_9A78_1C8AF1031061__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMergeSkeleton.h : header file
//

#include "AArray.h"

class A3DSkeleton;
class A3DSkinModelActionCore;
class A3DSklTrackSet;
class CSkeletonWrapper;
class A3DSkinModel;

struct SKELETONINFO;

/////////////////////////////////////////////////////////////////////////////
// CDlgMergeSkeleton dialog

class CDlgMergeSkeleton : public CDialog
{
public:		//	Types

// Construction
public:
	CDlgMergeSkeleton(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMergeSkeleton)
	enum { IDD = IDD_MERGESKELETON };
	CListCtrl	m_SkeletonList;
	CString	m_strSMDFile;
	CString	m_strTrackDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMergeSkeleton)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

	char	m_szTempBone[MAX_PATH];		//	Temporary bone file
	char	m_szTempModel[MAX_PATH];	//	Temporary skinmodel file
	char	m_szSMDDir[MAX_PATH];		//	SMD file directory

	int		m_iBadActCnt;		//	Bad action counter

	APtrArray<A3DSkeleton*>		m_aSkeletons;
	APtrArray<A3DSklTrackSet*>	m_aTracks;

protected:	//	Operations

	//	Check skeleton file collision
	bool CheckFileCollision(const char* szFile);
	//	Add skeleton candidate
	bool AddSkeleton(const char* szFile);
	//	Release all skeletions
	void ReleaseAllSkeletons();
	//	Add skeleton to list
	void AddSkeletonToList(const char* szFile, const SKELETONINFO& info);
	//	Clear skeleton and track array
	void ClearSkeletonsAndTracks();
	//	Check whether bones of skeletons match
	bool CheckBoneMatch(A3DSkeleton* pNewSkeleton);
	//	Move skeleton item's position in skeleton list
	void MoveSkeletonItemPos(int iOldPos, int iNewPos, bool bSelNew);

	//	Merge skeletons
	bool MergeSkeleton();
	//	Convert all candidate skeleton to new version
	bool ConvertSkeletonFiles();
	//	Load all candidate skeletons and tracks
	bool LoadSkeletonAndTracks();
	//	Emit bones' data
	bool EmitBones(CSkeletonWrapper* pNewSkeleton);
	//	Emit joints' data
	bool EmitJoints(CSkeletonWrapper* pNewSkeleton);
	//	Emit hooks' data
	bool EmitHooks(CSkeletonWrapper* pNewSkeleton);

	//	Update action info in skinmodel file
	bool UpdateModelActions();
	//	Remove all actions in skin model
	void RemoveAllActions(A3DSkinModel* pSkinModel);

	//	Copy completed skeleton and track files
	bool FinishCopyFiles();

	// Generated message map functions
	//{{AFX_MSG(CDlgMergeSkeleton)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnAddskeleton();
	afx_msg void OnBtnDelskeleton();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnMerge();
	afx_msg void OnBtnUp();
	afx_msg void OnDestroy();
	virtual void OnCancel();
	afx_msg void OnBtnBrowse();
	afx_msg void OnBtnTop();
	afx_msg void OnBtnBottom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMERGESKELETON_H__D3BFD463_3D7E_4331_9A78_1C8AF1031061__INCLUDED_)
