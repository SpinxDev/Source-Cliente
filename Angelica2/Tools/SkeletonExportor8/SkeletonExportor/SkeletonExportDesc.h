/*
 * FILE: SkeletonExportDesc.h
 *
 * DESCRIPTION: Skeleton export desc class
 *
 * CREATED BY: duyuxin, 2003/7/5
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _SKELETONEXPORTDESC_H_
#define _SKELETONEXPORTDESC_H_

#include "max.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define SKELETON_EXPORT_ID Class_ID(0x2ea5727d, 0xe4a671a)

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CSkeletonExportDesc
//
///////////////////////////////////////////////////////////////////////////

class CSkeletonExportDesc : public ClassDesc2
{
public:		//	Constructions and Destructions

	CSkeletonExportDesc() {}
	virtual ~CSkeletonExportDesc() {}

public:		//	Attributes

public:		//	Operaitons

	void SetInstance(HINSTANCE hInstance) { m_hInstance = hInstance; }
	HINSTANCE GetInstance() { return m_hInstance; }

	void* Create(BOOL loading=FALSE);
	int IsPublic() { return 1; }
	const TCHAR* ClassName() { return _T("A3D Skeleton Export"); }
	SClass_ID SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID ClassID() { return SKELETON_EXPORT_ID; }
	const TCHAR* Category() { return _T("Game Utilities"); }
	const TCHAR* InternalName() { return _T("A3D_Skeleton_Export"); }

protected:	//	Attributes

	HINSTANCE m_hInstance;

protected:	//	Operations
	
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_SKELETONEXPORTDESC_H_

