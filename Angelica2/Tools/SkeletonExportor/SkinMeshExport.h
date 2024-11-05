/*
 * FILE: SkinMeshExport.h
 *
 * DESCRIPTION: Skin mesh export class
 *
 * CREATED BY: duyuxin, 2003/9/15
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _SKINMESHEXPORT_H_
#define _SKINMESHEXPORT_H_

#include "max.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DSkin;
class CSkinCandidate;
class CSkeletonCandidate;
class CMeshCandidate;
class A3DSkinMesh;
class A3DRigidMesh;
class A3DMorphMesh;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CSkinMeshExport
//
///////////////////////////////////////////////////////////////////////////

class CSkinMeshExport : public SceneExport
{
public:		//	Types

public:		//	Constructions and Destructions

	CSkinMeshExport();
	virtual ~CSkinMeshExport() {}

public:		//	Attributes

public:		//	Operaitons

	const TCHAR* AuthorName();
	const TCHAR* CopyrightMessage();
	int DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options);
	const TCHAR* Ext(int n);
	int ExtCount();
	const TCHAR* LongDesc();
	const TCHAR* OtherMessage1();
	const TCHAR* OtherMessage2();
	const TCHAR* ShortDesc();
	void ShowAbout(HWND hWnd);
	unsigned int Version();

protected:	//	Attributes

	ExpInterface*	m_ei;			//	Export interface
	Interface*		m_i;			//	Interface

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_SKINMESHEXPORT_H_

