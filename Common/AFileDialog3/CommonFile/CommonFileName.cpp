#include "stdafx.h"
#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
#include "CommonFileName.h"
#include "CommonFileDlg.h"

CCommonFileName::CCommonFileName(A3DEngine* pA3DEngine, DWORD dwmfileFlag, const char* sRelPath /* = "" */, const char* szExt, CWnd* pParent /* = NULL */)
{
	dwmfileFlag = AFILE_ALLOWMULFILES;
	CString str = (const char*)sRelPath;
	m_pOfn = new CCommonFileDlg(pA3DEngine, dwmfileFlag, str, pParent);
	if (szExt != "")
		SetFileExtension(szExt);
}

CCommonFileName::~CCommonFileName()
{
	if(m_pOfn)
	{
		delete m_pOfn;
		m_pOfn = NULL;
	}
}

int CCommonFileName::DoModal()
{
	return m_pOfn->DoModal();
}

const char* CCommonFileName::GetFullFileName()
{
	memset(m_aStr, 0, sizeof(m_aStr));
	strcpy(m_aStr, AC2AS((LPCTSTR)m_pOfn->GetFullFileName()));
	return m_aStr;
}

const char* CCommonFileName::GetRelativeFileName()
{
	memset(m_aStr, 0, sizeof(m_aStr));
	strcpy(m_aStr, AC2AS((LPCTSTR)m_pOfn->GetRelativeFileName()));
	return m_aStr;
}

const char* CCommonFileName::GetFirstSelectedFullName()
{
	memset(m_aStr, 0, sizeof(m_aStr));
	strcpy(m_aStr, AC2AS((LPCTSTR)m_pOfn->GetFirstSelectedFullName()));
	return m_aStr;
}

const char* CCommonFileName::GetFirstSelectedRelName()
{
	memset(m_aStr, 0, sizeof(m_aStr));
	strcpy(m_aStr, AC2AS((LPCTSTR)m_pOfn->GetFirstSelectedRelName()));
	return m_aStr;
}

int CCommonFileName::GetSelectedNum()
{
	return m_pOfn->GetSelectedNum();
}

const char* CCommonFileName::GetNextSelectedFullName(int nIndex)
{
	memset(m_aStr, 0, sizeof(m_aStr));
	strcpy(m_aStr, AC2AS((LPCTSTR)m_pOfn->GetNextSelectedFullName(nIndex)));
	return m_aStr;
}

const char* CCommonFileName::GetNextSelectedRelName(int nIndex)
{
	memset(m_aStr, 0, sizeof(m_aStr));
	strcpy(m_aStr, AC2AS((LPCTSTR)m_pOfn->GetNextSelectedRelName(nIndex)));
	return m_aStr;
}

void CCommonFileName::SetFileExtension(const char* szExt)
{
	m_pOfn->SetFileExtension(szExt);
}

#endif