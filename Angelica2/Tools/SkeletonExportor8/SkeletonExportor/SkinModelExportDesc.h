/*
 * FILE: SkinModelExportDesc.h
 *
 * DESCRIPTION: Skin model export desc class
 *
 * CREATED BY: duyuxin, 2003/10/14
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _SKINMODELEXPORTDESC_H_
#define _SKINMODELEXPORTDESC_H_

#include "max.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define SKINMODEL_EXPORT_ID Class_ID(0x35a22e3, 0x4a9d7e5d)

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
//	Class CSkinModelExportDesc
//
///////////////////////////////////////////////////////////////////////////

class CSkinModelExportDesc : public ClassDesc2
{
public:		//	Constructions and Destructions

	CSkinModelExportDesc() {}
	virtual ~CSkinModelExportDesc() {}

public:		//	Attributes

public:		//	Operaitons

	void SetInstance(HINSTANCE hInstance) { m_hInstance = hInstance; }
	HINSTANCE GetInstance() { return m_hInstance; }

	void* Create(BOOL loading=FALSE);
	int IsPublic() { return 1; }
	const TCHAR* ClassName() { return _T("A3D SkinModel Export"); }
	SClass_ID SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID ClassID() { return SKINMODEL_EXPORT_ID; }
	const TCHAR* Category() { return _T("Game Utilities"); }
	const TCHAR* InternalName() { return _T("A3D_SkinModel_Export"); }

protected:	//	Attributes

	HINSTANCE m_hInstance;

protected:	//	Operations
	
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_SKINMODELEXPORTDESC_H_

