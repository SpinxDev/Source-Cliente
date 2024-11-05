/*
 * FILE: Model.cpp
 *
 * DESCRIPTION: Model class
 *
 * CREATED BY: Duyuxin, 2003/9/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "MainFrm.h"
#include "Model.h"
#include "Render.h"

#include "A3D.h"
#include "AFile.h"

#define new A_DEBUG_NEW

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

//	Get skinmodel file (.smd) version
DWORD AUX_GetSkinModelFileVer(const char* szFile)
{
	AFile file;

	if (!file.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_Log.Log("AUX_GetSkinModelFileVer, Failed to open smd file %s", szFile);
		return 0;
	}

	DWORD dwRead, dwVersion = 0;

	//	Skip file flag
	if (!file.Seek(sizeof (DWORD), AFILE_SEEK_CUR))
		return 0;
	
	//	Version
	if (!file.Read(&dwVersion, sizeof (DWORD), &dwRead))
		return 0;
	
	file.Close();

	return dwVersion;
}

//	Get skeleton file (.bon) version
DWORD AUX_GetSkeletonFileVer(const char* szFile)
{
	AFile file;

	if (!file.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_Log.Log("AUX_GetSkeletonFileVer, Failed to open skeleton file %s", szFile);
		return 0;
	}

	DWORD dwRead, dwVersion = 0;

	//	Skip file flag
	if (!file.Seek(sizeof (DWORD), AFILE_SEEK_CUR))
		return 0;
	
	//	Version
	if (!file.Read(&dwVersion, sizeof (DWORD), &dwRead))
		return 0;
	
	file.Close();

	return dwVersion;
}

//	Convert .bon file to new version
bool AUX_ConvertSkeletonFile(const char* szFile)
{
	DWORD dwVer = AUX_GetSkeletonFileVer(szFile);
	if (!dwVer)
		return false;

	if (dwVer >= 6)
		return true;

	A3DSkinMan* pA3DSkinMan = g_Render.GetA3DEngine()->GetA3DSkinMan();
	A3DSkeleton* pSkeleton = pA3DSkinMan->LoadSkeletonFile(szFile);
	if (!pSkeleton)
	{
		g_Log.Log("AUX_ConvertSkeletonFile, Failed to get load file %s", szFile);
		return false;
	}

	//	Get track set
	A3DSklTrackSet* pTrackSet = A3D::g_A3DTrackMan.GetSkeletonTrackSet(pSkeleton->GetSkeletonID(), pSkeleton->GetFileName());
	if (!pTrackSet)
	{
		pA3DSkinMan->ReleaseSkeleton(&pSkeleton);
		g_Log.Log("AUX_ConvertSkeletonFile, Failed to get track set data");
		return false;
	}

	//	Export track set data
	char szTckFile[MAX_PATH];
	strcpy(szTckFile, szFile);
	AUX_ChangeFileExt(szTckFile, MAX_PATH, ".stck");
	
	if (!pTrackSet->Save(szTckFile))
	{
		pA3DSkinMan->ReleaseSkeleton(&pSkeleton);
		g_Log.Log("AUX_ConvertSkeletonFile, Failed to save track set data to file %s", szTckFile);
		return false;
	}

	//	Re-save skeleton file
	if (!pSkeleton->Save(szFile))
	{
		pA3DSkinMan->ReleaseSkeleton(&pSkeleton);
		g_Log.Log("AUX_ConvertSkeletonFile, Failed to save skeleton file %s", szFile);
		return false;
	}

	pA3DSkinMan->ReleaseSkeleton(&pSkeleton);

	return true;
}

//	Read skeleton file info
bool AUX_ReadSkeletonInfo(const char* szFile, SKELETONINFO& info)
{
	AFile file;

	if (!file.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_Log.Log("AUX_ReadSkeletonInfo, Failed to open skeleton file %s", szFile);
		return false;
	}

	DWORD dwRead;

	//	Skip file flag
	file.Seek(sizeof (DWORD), AFILE_SEEK_CUR);
	
	//	Version
	if (!file.Read(&info.dwVersion, sizeof (DWORD), &dwRead))
	{
		file.Close();
		return false;
	}

	//	Bone number
	if (!file.Read(&info.iNumBone, sizeof (int), &dwRead))
	{
		file.Close();
		return false;
	}

	//	Skip joint and hook number
	file.Seek(sizeof (int) * 2, AFILE_SEEK_CUR);

	//	Start frame
	if (!file.Read(&info.iStartFrame, sizeof (int), &dwRead))
	{
		file.Close();
		return false;
	}

	//	End frame
	if (!file.Read(&info.iEndFrame, sizeof (int), &dwRead))
	{
		file.Close();
		return false;
	}

	//	FPS
	if (!file.Read(&info.iAnimFPS, sizeof (int), &dwRead))
	{
		file.Close();
		return false;
	}

	file.Close();

	return true;
}

//	Read track file info
bool AUX_ReadTrackInfo(const char* szFile, SKELETONINFO& info)
{
	AFile file;

	if (!file.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_Log.Log("AUX_ReadTrackInfo, Failed to open track file %s", szFile);
		return false;
	}

	DWORD dwRead;

	//	Skip file flag
	file.Seek(sizeof (DWORD), AFILE_SEEK_CUR);
	
	//	Version
	if (!file.Read(&info.dwVersion, sizeof (DWORD), &dwRead))
	{
		file.Close();
		return false;
	}

	//	Bone number
	if (!file.Read(&info.iNumBone, sizeof (int), &dwRead))
	{
		file.Close();
		return false;
	}

	//	Start frame
	if (!file.Read(&info.iStartFrame, sizeof (int), &dwRead))
	{
		file.Close();
		return false;
	}

	//	End frame
	if (!file.Read(&info.iEndFrame, sizeof (int), &dwRead))
	{
		file.Close();
		return false;
	}

	//	FPS
	if (!file.Read(&info.iAnimFPS, sizeof (int), &dwRead))
	{
		file.Close();
		return false;
	}

	file.Close();

	return true;
}

