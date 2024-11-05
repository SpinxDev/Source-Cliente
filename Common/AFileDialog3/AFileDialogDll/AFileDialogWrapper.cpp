// AFileDialogWrapper.cpp: implementation of the AFileDialogWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AListCtrl.h"
#include "AFileDialog2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int get_list_ctrl_style_from_afd_style(int iStyle)
{
	switch(iStyle)
	{
	case AFileDialogWrapper::AFD_THUMBNAIL:
		return FLS_THUMBNAIL;
	case AFileDialogWrapper::AFD_ICON:
		return FLS_ICON;
	case AFileDialogWrapper::AFD_LIST:
		return FLS_LIST;
	case AFileDialogWrapper::AFD_REPORT:
		return FLS_DETAILS;
	default:
		return -1;
	}
}

int get_list_ctrl_style_from_afd_style2(int iStyle)
{
	switch (iStyle)
	{
	case AFileDialogWrapper::AFD_THUMBNAIL:
		return AListCtrl2::LV_THUMBVIEW;
	case AFileDialogWrapper::AFD_ICON:
		return AListCtrl2::LV_ICON;
	case AFileDialogWrapper::AFD_LIST:
		return AListCtrl2::LV_LIST;
	case AFileDialogWrapper::AFD_REPORT:
		return AListCtrl2::LV_DETAIL;
	default:
		return -1;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AFileDialogWrapper::AFileDialogWrapper(A3DDevice* pDevice
	, BOOL bIsOpenFile
	, LPCTSTR szRelativePath
	, LPCTSTR szCaption
	, LPCTSTR szExt
	, CWnd* pParent)
{
	//m_pFdImpl = new AFileDialog(pDevice, bIsOpenFile, szRelativePath, szCaption, szExt, FLS_THUMBNAIL, pParent);
#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
	m_pFdImpl = new AFileDialog2(pDevice, bIsOpenFile, szRelativePath, szCaption, szExt, AListCtrl2::LV_THUMBVIEW, pParent);
#elif defined(_ANGELICA31)
	m_pFdImpl = new AFileDialog2(bIsOpenFile, szRelativePath, szCaption, szExt, AListCtrl2::LV_THUMBVIEW, pParent);
#endif
}

AFileDialogWrapper::AFileDialogWrapper(A3DDevice* pDevice
	, BOOL bIsOpenFile
	, LPCTSTR szRelativePath
	, LPCTSTR szCaption
	, LPCTSTR szExt
	, int iListStyle
	, CWnd* pParent)
{
	//m_pFdImpl = new AFileDialog(pDevice, bIsOpenFile, szRelativePath, szCaption, szExt, get_list_ctrl_style_from_afd_style(iListStyle), pParent);
#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
	m_pFdImpl = new AFileDialog2(pDevice, bIsOpenFile, szRelativePath, szCaption, szExt, get_list_ctrl_style_from_afd_style2(iListStyle), pParent);
#elif defined(_ANGELICA31)
	m_pFdImpl = new AFileDialog2(bIsOpenFile, szRelativePath, szCaption, szExt, get_list_ctrl_style_from_afd_style2(iListStyle), pParent);
#endif
}

AFileDialogWrapper::~AFileDialogWrapper()
{
	delete m_pFdImpl;
}

INT_PTR AFileDialogWrapper::DoModal()
{
	return m_pFdImpl->DoModal();
}

const TCHAR * AFileDialogWrapper::GetFullPathName() const
{
	return m_pFdImpl->GetFullFileName();
}

const TCHAR * AFileDialogWrapper::GetRelativePathName() const
{
	return m_pFdImpl->GetRelativeFileName();
}

const TCHAR * AFileDialogWrapper::GetFileName() const
{
	return PathFindFileName(m_pFdImpl->GetFullFileName());
}

int AFileDialogWrapper::GetSelectedCount() const
{
	return static_cast<int>(m_pFdImpl->GetSelFiles().size());
}

const TCHAR * AFileDialogWrapper::GetSelectedFileByIdx(int nIdx) const
{
	if (nIdx < 0 || nIdx > static_cast<int>(m_pFdImpl->GetSelFiles().size()))
		return NULL;
	return m_pFdImpl->GetSelFiles()[nIdx];
}

void AFileDialogWrapper::SetInitSubDir(const TCHAR* szSubDir)
{
	m_pFdImpl->SetInitSubDir(szSubDir);
}