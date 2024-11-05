/*
 * FILE: SkinMeshExportDesc.h
 *
 * DESCRIPTION: Skin mesh export desc class
 *
 * CREATED BY: duyuxin, 2003/9/15
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _SKINMESHEXPORTDESC_H_
#define _SKINMESHEXPORTDESC_H_

#include "max.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define SKINMESH_EXPORT_ID Class_ID(0x674a2aca, 0x7cff5931)

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
//	Class CSkinMeshExportDesc
//
///////////////////////////////////////////////////////////////////////////

class CSkinMeshExportDesc : public ClassDesc2
{
public:		//	Constructions and Destructions

	CSkinMeshExportDesc() {}
	virtual ~CSkinMeshExportDesc() {}

public:		//	Attributes

public:		//	Operaitons

	void SetInstance(HINSTANCE hInstance) { m_hInstance = hInstance; }
	HINSTANCE GetInstance() { return m_hInstance; }

	void* Create(BOOL loading=FALSE);
	int IsPublic() { return 1; }
	const TCHAR* ClassName() { return _T("A3D Skin Export"); }
	SClass_ID SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID ClassID() { return SKINMESH_EXPORT_ID; }
	const TCHAR* Category() { return _T("Game Utilities"); }
	const TCHAR* InternalName() { return _T("A3D_Skin_Export"); }

protected:	//	Attributes

	HINSTANCE m_hInstance;

protected:	//	Operations
	
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_SKINMESHEXPORTDESC_H_

