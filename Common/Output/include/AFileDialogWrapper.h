// AFileDialogWrapper.h: interface for the AFileDialogWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFILEDIALOGWRAPPER_H__6A8FB80C_12F5_41F1_838C_1CBDA9FE1D99__INCLUDED_)
#define AFX_AFILEDIALOGWRAPPER_H__6A8FB80C_12F5_41F1_838C_1CBDA9FE1D99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AFileExpDecl.h"

class A3DDevice;
class IAFileDialog;

//	From 2009.8 on this Dll will initialize an engine inside of it
//	In order to render the gfx / ecm in a preview window / to a preview image
//	Call below two functions in the InitInstance / ExitInstance of the host application.
extern "C" _AFILE_EXP_ bool AFD_DllInitialize();
extern "C" _AFILE_EXP_ void AFD_DllFinalize();
extern "C" _AFILE_EXP_ bool AFD_IsInitialized();

class _AFILE_EXP_ AFileDialogWrapper
{
public:
	//	The Listview style shown when dialog is created
	enum ListStyle {
		AFD_THUMBNAIL,
		AFD_ICON,
		AFD_LIST,
		AFD_REPORT,
		AFD_USEDEFAULT,		// Use default, or the value set at last time by user
	};

	//	--------
	//	0) pDevice			: A3DDevice from Angelica engine (notice the version of Angelica and the AFileDialog used are compatible)
	//	1) bIsOpenFile		: Create an open file dialog / save file dialog, TRUE for an open file dialog
	//	2) szRelativePath	: Specifies the relative path based on af_GetBaseDir(), eg. "Models\\2Sticks\\"
	//	3) szCaption		: AFileDilaog's window text
	//	4) szExt			: Custom Extensions eg. ".gfx .psd .smd"
	//	6) iListStyle		: List style shown when dialog is created
	//	5) pParent			: Parent window pointer
	//	--------
	AFileDialogWrapper(A3DDevice* pDevice
		, BOOL bIsOpenFile
		, LPCTSTR szRelativePath = _T("")
		, LPCTSTR szCaption = _T("AFileDialog")
		, LPCTSTR szExt = _T("")
		, CWnd* pParent = NULL);

	AFileDialogWrapper(A3DDevice* pDevice
		, BOOL bIsOpenFile
		, LPCTSTR szRelativePath = _T("")
		, LPCTSTR szCaption = _T("AFileDialog")
		, LPCTSTR szExt = _T("")
		, int iListStyle = AFD_USEDEFAULT
		, CWnd* pParent = NULL);

	virtual ~AFileDialogWrapper();

public:
	INT_PTR DoModal();
	const TCHAR * GetFullPathName() const;
	const TCHAR * GetRelativePathName() const;
	const TCHAR * GetFileName() const;
	int GetSelectedCount() const;
	const TCHAR * GetSelectedFileByIdx(int nIdx) const;
	void SetInitSubDir(const TCHAR* szSubDir);
private:
	IAFileDialog* m_pFdImpl;

private:
	AFileDialogWrapper(const AFileDialogWrapper&);
	AFileDialogWrapper& operator = (const AFileDialogWrapper&);
};

#endif // !defined(AFX_AFILEDIALOGWRAPPER_H__6A8FB80C_12F5_41F1_838C_1CBDA9FE1D99__INCLUDED_)
