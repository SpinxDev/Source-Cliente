// AngelicaAppWizaw.cpp : implementation file
//

#include "stdafx.h"
#include "AngelicaAppWiz.h"
#include "AngelicaAppWizaw.h"
#include "chooser.h"

#include <atlbase.h>
#include <ObjModel\\bldauto.h>

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void CAngelicaAppWizAppWiz::InitCustomAppWiz()
{
	// Create a new dialog chooser; CDialogChooser's constructor initializes
	//  its internal array with pointers to the steps.
	m_pChooser = new CDialogChooser;

	// Set the maximum number of steps.
	SetNumberOfSteps(LAST_DLG);

	//	Set default values. Symbol replace error may occurs without default
	//	values, because some steps may be skipped if user press "Finish" button
	//	directly.
	AngelicaAppWizaw.SetBoolVar(_T("APP_TYPE_WIN32"), true);
}

// This is called just before the custom AppWizard is unloaded.
void CAngelicaAppWizAppWiz::ExitCustomAppWiz()
{
	// Deallocate memory used for the dialog chooser
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;

	// TODO: Add code here to deallocate resources used by the custom AppWizard
}

// This is called when the user clicks "Create..." on the New Project dialog
//  or "Next" on one of the custom AppWizard's steps.
CAppWizStepDlg* CAngelicaAppWizAppWiz::Next(CAppWizStepDlg* pDlg)
{
	if (!m_pChooser->GetCurStep())
	{
		//	Use just clicks "Create..."
		BuildDocumentSymbol();
	}

	// Delegate to the dialog chooser
	return m_pChooser->Next(pDlg);
}

// This is called when the user clicks "Back" on one of the custom
//  AppWizard's steps.
CAppWizStepDlg* CAngelicaAppWizAppWiz::Back(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Back(pDlg);
}

void CAngelicaAppWizAppWiz::CustomizeProject(IBuildProject* pProject)
{
	// TODO: Add code here to customize the project.  If you don't wish
	//  to customize project, you may remove this virtual override.
	
	// This is called immediately after the default Debug and Release
	//  configurations have been created for each platform.  You may customize
	//  existing configurations on this project by using the methods
	//  of IBuildProject and IConfiguration such as AddToolSettings,
	//  RemoveToolSettings, and AddCustomBuildStep. These are documented in
	//  the Developer Studio object model documentation.

	// WARNING!!  IBuildProject and all interfaces you can get from it are OLE
	//  COM interfaces.  You must be careful to release all new interfaces
	//  you acquire.  In accordance with the standard rules of COM, you must
	//  NOT release pProject, unless you explicitly AddRef it, since pProject
	//  is passed as an "in" parameter to this function.  See the documentation
	//  on CCustomAppWiz::CustomizeProject for more information.
	CComPtr<IConfigurations> pConfigs;
	if (FAILED(pProject->get_Configurations(&pConfigs)))
	{
		AfxMessageBox("An error occurred while obtaining the IConfigurations interface pointer");
		return;
	}

	CComPtr<IConfiguration> pConfig;
	VARIANT dummy = {0};
	CComVariant index;
	CComBSTR Name;
	CString strTemp;

	long Count = 0;
	pConfigs->get_Count(&Count);

	// Iterate through all the configurations of the project
	for (long i=1; i <= Count; i++)
	{
		index = i;
		if (FAILED(pConfigs->Item(index, &pConfig)))
		{
			AfxMessageBox("An error occurred while obtaining the IConfiguration pointer");
			return;
		}

		//	Check debug / release version
		bool bDebug = true;
		pConfig->get_Name(&Name);
		strTemp = Name;
		if (strTemp.Find("Debug") == -1)
			bDebug = false;

		//	Decide how to use MFC libs
		if (m_Dictionary.Lookup(_T("APP_TYPE_WIN32"), strTemp))
			pConfig->AddToolSettings(L"mfc", L"0", dummy);
		else if (m_Dictionary.Lookup(_T("APP_TYPE_MFC"), strTemp))
		{
			if (m_Dictionary.Lookup(_T("MFC_LIB_STATIC"), strTemp))
				pConfig->AddToolSettings(L"mfc", L"1", dummy);
			else	//	if (m_Dictionary.Lookup(_T("MFC_LIB_DLL"), strTemp))
				pConfig->AddToolSettings(L"mfc", L"2", dummy);
		}

		//	Add our own libs
		if (bDebug)
		{
			pConfig->AddToolSettings(L"link.exe", L"zliblib.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"mpg123lib.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"Immwrapper.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"AngelicaCommon_d.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"Angelica3D_d.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"AngelicaFile_d.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"AngelicaMedia_d.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"dxguid.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"d3d8.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"d3dx8.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"dsound.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"shlwapi.lib", dummy);

			//	Set code generation type to 'Debug Multithread DLL'
			pConfig->RemoveToolSettings(L"cl.exe", L"/MTd", dummy);
			pConfig->RemoveToolSettings(L"cl.exe", L"/MLd", dummy);
			pConfig->AddToolSettings(L"cl.exe", L"/MDd", dummy);
		}
		else	//	Release version
		{
			pConfig->AddToolSettings(L"link.exe", L"zliblib.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"mpg123lib.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"Immwrapper.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"AngelicaCommon.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"Angelica3D.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"AngelicaFile.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"AngelicaMedia.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"dxguid.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"d3d8.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"d3dx8.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"dsound.lib", dummy);
			pConfig->AddToolSettings(L"link.exe", L"shlwapi.lib", dummy);

			//	Set code generation type to 'Debug Multithread DLL'
			pConfig->RemoveToolSettings(L"cl.exe", L"/MT", dummy);
			pConfig->RemoveToolSettings(L"cl.exe", L"/ML", dummy);
			pConfig->AddToolSettings(L"cl.exe", L"/MD", dummy);
		}

		//	Change 'Use precompiled file' to 'Auto-use precompiled file'
		pConfig->RemoveToolSettings(L"cl.exe", L"/Yu\"stdafx.h\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/YX", dummy);
	}
}

void CAngelicaAppWizAppWiz::SetBoolVar(LPCTSTR lpszName, BOOL bVal)
{
	if (bVal)
		m_Dictionary.SetAt(lpszName, _T("1"));
	else
		m_Dictionary.RemoveKey(lpszName);
}

void CAngelicaAppWizAppWiz::SetStringVar(LPCTSTR lpszName, LPCTSTR lpszVal)
{
	m_Dictionary.SetAt(lpszName, lpszVal);
}

//	Build document symbol
void CAngelicaAppWizAppWiz::BuildDocumentSymbol()
{
	CString strRoot;
	m_Dictionary.Lookup(_T("root"), strRoot);

	CString strDocID, strDoc1, strDoc2;
	strDoc2 = strRoot;

	char ch = strDoc2[0];
	if (ch >= '0' && ch <= '9')		//	The first char is a number
		strDoc2 = "My" + strDoc2;

	strDoc1	= strDoc2.Left(6);
	strDocID = strDoc1;
	strDocID.MakeUpper();

	m_Dictionary.SetAt(_T("APP_DOC_ID"), strDocID);
	m_Dictionary.SetAt(_T("APP_DOC_1"), strDoc1);
	m_Dictionary.SetAt(_T("APP_DOC_2"), strDoc2);
}

// Here we define one instance of the CAngelicaAppWizAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global AngelicaAppWizaw.
CAngelicaAppWizAppWiz AngelicaAppWizaw;

