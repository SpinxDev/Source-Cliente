/*
 * FILE: PT_Interface.h
 *
 * DESCRIPTION: Global definitions
 *
 * CREATED BY: Duyuxin, 2003/6/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_PT_INTERFACE_H_
#define _PT_INTERFACE_H_

#include "A3DTypes.h"
#include "A3DPortalFile.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

//	Portal type
enum
{
	PTTYPE_BIDIRECTIONAL = 0,	//	Bidirectional portal
	PTTYPE_BACKTOFRONT,			//	From back to front
	PTTYPE_FRONTTOBACK,			//	From front to back
};

/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////

//	Building parameters
struct PTBUILDPARAMS
{
	int		aSectorDiv[3];	//	Recommended part number on x, y, z axis, 0 means no division
	float	fPartMax;		//	Maximum limit of part size
	float	fPartMin;		//	Minimum limit of part size
};

//	Structure used to add portal
struct PTADDPORTALDATA
{
	A3DVECTOR3	aVerts[4];	//	aVerts: vertices array
	int			iNumVert;	//	iNumVert: number of vertices
	int			iFront;		//	iFront: index of front sector.
	int			iBack;		//	iBack: index of back sector
	int			iType;		//	iType: portal type
	
	char		szName[PTNAME_LENGTH];
};

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////

bool PT_Init();
bool PT_Reset();
void PT_Release();

/*	Call below functions on this order:

	1. PT_AddRenderSurfs
	2. PT_AddSector
	3. PT_AddPortal
	4. PT_Build
	5. PT_Save
*/
bool PT_AddRenderSurfs(A3DVECTOR3* aVerts, WORD* aIndices, int iNumSurf);
bool PT_AddRenderSurfs(A3DVECTOR3* aVerts, int iNumSurf);
bool PT_AddSector(A3DVECTOR3* aVerts, WORD* aIndices, int iNumFace);
bool PT_AddPortal(const PTADDPORTALDATA& Data);
bool PT_Build(const PTBUILDPARAMS& Params);
bool PT_Save(const char* szFile);

#endif	//	_PT_INTERFACE_H_

