/*
 * FILE: Global.cpp
 *
 * DESCRIPTION: Global variables and definitions
 *
 * CREATED BY: Duyuxin, 2002/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "SkeletonEditor.h"

#include <afi.h>
#include <A3DSkinModel.h>

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

ALog		g_Log;
EDITCONFIG	g_Configs;
char		g_szWorkDir[MAX_PATH];
char		g_szIniFile[MAX_PATH];	//	.ini file name

A3DVECTOR3	g_vOrigin(0.0f);
A3DVECTOR3	g_vAxisX(1.0f, 0.0f, 0.0f);
A3DVECTOR3	g_vAxisY(0.0f, 1.0f, 0.0f);
A3DVECTOR3	g_vAxisZ(0.0f, 0.0f, 1.0f);

//	Color table
DWORD g_aColTable[LEN_COLORTABLE] = 
{
	0xffff8000,		//	COL_BONEBOX,	Bone box
	0xffffff00,		//	COL_SELBONEBOX,	Selected bone box
	0xff8080ff,		//	COL_BONESTAFF,	Staff connect bones	
	0xff000080,		//	COL_HOOKBOX,	Hook box
	0xffff0000,		//	COL_AXISX,		Axis X
	0xff00ff00,		//	COL_AXISY,		Axis Y
	0xff0000ff,		//	COL_AXISZ,		Axis Z
	0xff00ffff,		//	COL_BOUNDBOX,	Bounding box
	0xff00a0a0,		//	COL_MODELAABB,	Model AABB
	0xffffff00,		//	COL_SELRECT,	Selecting rectangle
	0xffff8000,		//	COL_UNSELRECT,	Unselecting rectangle
};

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

CSkeletonEditorApp* AUX_GetApp()
{
	return (CSkeletonEditorApp*)AfxGetApp();
}

CMainFrame*	AUX_GetMainFrame()
{	
	return ((CSkeletonEditorApp*)AfxGetApp())->GetMainFrame();
}

//	Output message with window's message box
int AUX_MessageBox(int iType, const char* szMsg, ...)
{
	static char szText[1024];
	
	va_list vaList;
	va_start(vaList, szMsg);
	vsprintf(szText, szMsg, vaList);
	va_end(vaList);

	return ((CWnd*)AUX_GetMainFrame())->MessageBox(szText, "ÌבÊ¾", iType);
}

//	Write string to file
bool AUX_WriteStringToFile(FILE* fp, const char* szStr)
{
	int iLen = 0;
		
	if (szStr && szStr[0])
		iLen = strlen(szStr);

	fwrite(&iLen, 1, sizeof (int), fp);

	if (iLen)
		fwrite(szStr, 1, iLen, fp);

	return true;
}

//	Read string from file
bool AUX_ReadStringFromFile(FILE* fp, char* szBuf)
{
	int iLen;
	fread(&iLen, 1, sizeof (int), fp);

	if (iLen)
		fread(szBuf, 1, iLen, fp);

	szBuf[iLen] = '\0';

	return true;
}

bool AUX_ReadStringFromFile(FILE* fp, AString& szStr)
{
	//	Read length of string
	int iLen;
	fread(&iLen, 1, sizeof (int), fp);

	//	Read string data
	if (iLen)
	{
		char* szBuf = new char [iLen+1];
		if (!szBuf)
			return false;

		fread(szBuf, 1, iLen, fp);
		szBuf[iLen] = '\0';
		szStr = szBuf;

		delete [] szBuf;
	}
	else
		szStr = "";

	return true;
}

//	Save configs to .ini file
void AUX_SaveConfigs()
{
	char szStr[256];
	sprintf(szStr, "%f", g_Configs.fBoneBoxSize);
	WritePrivateProfileString("Configs", "BoxSize", szStr, g_szIniFile);

	sprintf(szStr, "%f", g_Configs.fStaffRadius);
	WritePrivateProfileString("Configs", "StaffRadius", szStr, g_szIniFile);

	sprintf(szStr, "%d", g_Configs.iCameraMoveSpeed);
	WritePrivateProfileString("Configs", "CameraMove", szStr, g_szIniFile);
	
	sprintf(szStr, "%d", g_Configs.iCameraRotSpeed);
	WritePrivateProfileString("Configs", "CameraRot", szStr, g_szIniFile);

	sprintf(szStr, "%d", g_Configs.iMouseDragSpeed);
	WritePrivateProfileString("Configs", "MouseDrag", szStr, g_szIniFile);
}

bool AUX_FileExist(const char* szFile)
{
	if (!szFile || !szFile[0])
		return false;

	FILE* fp = fopen(szFile, "rb");
	if (!fp)
		return false;

	fclose(fp);
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

bool AUX_DirectoryExist(const char* szDir)
{
	//	Save current directory
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurDir);

	bool bRet = false;
	if (SetCurrentDirectory(szDir))
		bRet = true;

	//	Restore current directory
	SetCurrentDirectory(szCurDir);

	return bRet;
}

bool AUX_GetModelTrackDir(A3DSkinModel* pModel, char* szTrackDir, bool bAbs)
{
	if (!pModel)
	{
		ASSERT(pModel);
		szTrackDir[0] = '\0';
		return false;
	}

	char szRelFile[MAX_PATH], szRelPath[MAX_PATH], szResult[MAX_PATH];
	
	af_GetRelativePath(pModel->GetFileName(), szRelFile);
	af_GetFilePath(szRelFile, szRelPath, MAX_PATH);

	const char* szTckDir = pModel->GetTrackDataDir();
	if (szRelPath[0] && szTckDir[0])
		sprintf(szResult, "%s\\%s", szRelPath, szTckDir);
	else if (szRelPath[0])
		strcpy(szResult, szRelPath);
	else
		strcpy(szResult, szTckDir);

	if (bAbs)
	{
		if (szResult[0])
		{
			sprintf(szTrackDir, "%s%s", g_szWorkDir, szResult);
		}
		else
		{
			strcpy(szTrackDir, g_szWorkDir);
			int iLen = strlen(szTrackDir);
			szTrackDir[iLen] = '\0';
		}
	}
	else
		strcpy(szTrackDir, szResult);

	return true;
}

