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

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////


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
bool PT_AddPortal(A3DVECTOR3* aVerts, int iNumVert, int iFront, int iBack, int iType);
bool PT_Build();
bool PT_Save(const char* szFile);

#endif	//	_PT_INTERFACE_H_

