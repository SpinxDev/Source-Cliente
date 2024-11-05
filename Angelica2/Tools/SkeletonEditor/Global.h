/*
 * FILE: Global.h
 *
 * DESCRIPTION: Global variables and definitions
 *
 * CREATED BY: Duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stdafx.h"
#include "Resource.h"

#include "ALog.h"
#include "AMemory.h"
#include "AString.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define	WM_INPUTWNDEND		WM_USER + 100		//	w: 1 - cancel; l: owner window's ID
#define WM_COMBOWNDEND		WM_USER + 101		//	w: 1 - cancel; l: owner window's ID
#define WM_ACTIVATEPAGE		WM_USER + 102		//	w: 1 - activate;

#define ACTFILE_VERSION		3

//	Window IDs

//	Color table references
enum
{
	COL_BONEBOX = 0,		//	Bone box
	COL_SELBONEBOX,			//	Selected bone box
	COL_BONESTAFF,			//	Staff connect bones	
	COL_HOOKBOX,			//	Hook box
	COL_AXISX,				//	Axis X
	COL_AXISY,				//	Axis Y
	COL_AXISZ,				//	Axis Z
	COL_BOUNDBOX,			//	Bounding box
	COL_MODELAABB,			//	Model AABB
	COL_SELRECT,			//	Selecting rectangle
	COL_UNSELRECT,			//	Unselecting rectangle
	LEN_COLORTABLE,			//	Length of color table
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	Editing config
struct EDITCONFIG
{
	bool	bSkeletonOnly;		//	true, render skeleton only. false, render skin mesh
	bool	bShowBoundBox;		//	true, show model's bounding box
	float	fBoneBoxSize;		//	Bone box size
	float	fStaffRadius;		//	Radius of staff which connect bones
	int		iStaffSegment;		//	Segment of staff which connect bones
	int		iCameraMoveSpeed;	//	Camera move speed
	bool	bShowPhyShape;		//	true, show physique shape
	int		iEditPhyShape;		//	Index of physique shape which is being edited
	bool	bShowBoneCoord;		//	true, show bnoe local coordinates
	int		iCameraRotSpeed;	//	Camera rotate speed
	int		iMouseDragSpeed;	//	Mouse drag speed
	bool	bShowGround;		//	true, show ground plane
	bool	bShowNormal;		//	true, show vertex normal
};

extern DWORD		g_aColTable[];	//	Color table
extern ALog			g_Log;
extern EDITCONFIG	g_Configs;
extern char			g_szWorkDir[];
extern char			g_szIniFile[];	//	.ini file name

extern A3DVECTOR3	g_vOrigin;
extern A3DVECTOR3	g_vAxisX;
extern A3DVECTOR3	g_vAxisY;
extern A3DVECTOR3	g_vAxisZ;

class CMainFrame;
class CSkeletonEditorApp;
class A3DSkinModel;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

CSkeletonEditorApp* AUX_GetApp();
CMainFrame*	AUX_GetMainFrame();
int AUX_MessageBox(int iType, const char* szMsg, ...);
bool AUX_WriteStringToFile(FILE* fp, const char* szStr);
bool AUX_ReadStringFromFile(FILE* fp, char* szBuf);
bool AUX_ReadStringFromFile(FILE* fp, AString& szStr);
void AUX_SaveConfigs();

bool AUX_FileExist(const char* szFile);
bool AUX_ChangeFileExt(char* szFile, int iBufLen, const char* szNewExt);
bool AUX_CreateDirectory(const char* szDir);
bool AUX_DirectoryExist(const char* szDir);
bool AUX_GetModelTrackDir(A3DSkinModel* pModel, char* szTrackDir, bool bAbs);

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_GLOBAL_H_



