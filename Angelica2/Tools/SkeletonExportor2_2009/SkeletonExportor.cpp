// SkeletonExportor.cpp : Defines the initialization routines for the DLL.
//

#include "Global.h"
#include "SkeletonExportor.h"
#include "SkeletonExportDesc.h"
#include "SkinMeshExportDesc.h"
#include "SkinModelExportDesc.h"
#include "PhysiqueExportDesc.h"

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

CSkeletonExportorApp theDll;

CSkeletonExportDesc CSkeletonExportorApp::m_SkeletonExportDesc;
CSkinMeshExportDesc	CSkeletonExportorApp::m_SkinMeshExportDesc;
CSkinModelExportDesc CSkeletonExportorApp::m_SkinModelExportDesc;
CPhysiqueExportDesc CSkeletonExportorApp::m_PhysiqueExportDesc;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

__declspec (dllexport) ClassDesc *LibClassDesc(int id)
{
	return theDll.GetClassDesc(id);
}

__declspec (dllexport) const TCHAR *LibDescription()
{
	return "Angelica 3D Skeleton Exporter";
}

__declspec (dllexport) int LibNumberClasses()
{
	return theDll.GetClassDescCount();
}

__declspec (dllexport) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

void AUX_OutputMessage(const char* szMsg, ...)
{
	static char szBuf[1024];

	va_list vaList;
	va_start(vaList, szMsg);
	vsprintf(szBuf, szMsg, vaList);
	va_end(vaList);

	AfxMessageBox(szBuf, MB_OK | MB_ICONEXCLAMATION);
}

//	Create a directory
bool AUX_CreateDirectory(const char* szDir)
{
	CString strDir = szDir;
	int iLen = strDir.GetLength();

	if (iLen <= 3)
		return true;

	//	Clear the last '\\'
	if (strDir.GetAt(iLen-1) == '\\')
		strDir.SetAt(iLen-1, '\0');

	//	Save current directory
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurDir);

	CString strTemp;

	int iPos = strDir.Find('\\', 0);
	while (iPos > 0)
	{
		strTemp = strDir.Mid(0, iPos);
		CreateDirectory(strTemp, NULL);
		iPos = strDir.Find('\\', iPos+1);
	}

	CreateDirectory(szDir, NULL);

	//	Restore current directory
	SetCurrentDirectory(szCurDir);

	return true;
}

bool AUX_ChangeFileExt(char* szFile, int iBufLen, const char* szNewExt)
{
	char szFileName[MAX_PATH];
	strcpy(szFileName, szFile);

	char* pTemp = strrchr(szFileName, '.');
	if (pTemp)
		strcpy(pTemp, szNewExt);
	else
		strcat(szFileName, szNewExt);

	int iLen = strlen(szFileName);
	if (iLen >= iBufLen)
	{
		ASSERT(iLen < iBufLen);
		return false;
	}

	strcpy(szFile, szFileName);
	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CSkeletonExportorApp
//
///////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkeletonExportorApp, CWinApp)
	//{{AFX_MSG_MAP(CSkeletonExportorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkeletonExportorApp construction

CSkeletonExportorApp::CSkeletonExportorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSkeletonExportorApp object

ClassDesc* CSkeletonExportorApp::GetClassDesc(int id)
{
	switch (id)
	{
	case 0:

		m_SkeletonExportDesc.SetInstance(m_hInstance);
		return &m_SkeletonExportDesc;

	case 1:

		m_SkinMeshExportDesc.SetInstance(m_hInstance);
		return &m_SkinMeshExportDesc;

	case 2:

		m_SkinModelExportDesc.SetInstance(m_hInstance);
		return &m_SkinModelExportDesc;

	case 3:

		m_PhysiqueExportDesc.SetInstance(m_hInstance);
		return &m_PhysiqueExportDesc;
	}

	return NULL;
}

int CSkeletonExportorApp::GetClassDescCount()
{
	return 4;
}
