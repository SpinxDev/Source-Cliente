/*
 * FILE: PhysiqueExportDesc.h
 *
 * DESCRIPTION: Physique export desc class
 *
 * CREATED BY: duyuxin, 2004/5/24
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _PHYSIQUEEXPORTDESC_H_
#define _PHYSIQUEEXPORTDESC_H_

#include "max.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define PHYSIQUE_EXPORT_ID Class_ID(0x6f174020, 0x147c3334)

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
//	Class CPhysiqueExportDesc
//
///////////////////////////////////////////////////////////////////////////

class CPhysiqueExportDesc : public ClassDesc2
{
public:		//	Constructions and Destructions

	CPhysiqueExportDesc() {}
	virtual ~CPhysiqueExportDesc() {}

public:		//	Attributes

public:		//	Operaitons

	void SetInstance(HINSTANCE hInstance) { m_hInstance = hInstance; }
	HINSTANCE GetInstance() { return m_hInstance; }

	void* Create(BOOL loading=FALSE);
	int IsPublic() { return 1; }
	const TCHAR* ClassName() { return _T("A3D SkinModel physique Export"); }
	SClass_ID SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID ClassID() { return PHYSIQUE_EXPORT_ID; }
	const TCHAR* Category() { return _T("Game Utilities"); }
	const TCHAR* InternalName() { return _T("A3D_physique_Export"); }

protected:	//	Attributes

	HINSTANCE m_hInstance;

protected:	//	Operations
	
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_PHYSIQUEEXPORTDESC_H_

