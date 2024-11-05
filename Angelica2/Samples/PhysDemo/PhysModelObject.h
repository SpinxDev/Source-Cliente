/*
 * FILE: PhysModelObject.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/5/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _PHYSMODELOBJECT_H_
#define _PHYSMODELOBJECT_H_

#include "PhysObject.h"
#include <A3DVector.h>
#include <AArray.h>
#include <A3DMatrix.h>
#include <AString.h>

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

class CSkinModel;
class A3DViewport;
class A3DModelPhysSyncData;
class A3DModelPhysics;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CPhysModelObject
//	
///////////////////////////////////////////////////////////////////////////

class CPhysModelObject : public CPhysObject
{
public:		//	Types

	//	Object type
	enum
	{
		TYPE_UNKNOWN = 0,
		TYPE_2STICKS = 1,
	};

public:		//	Constructor and Destructor

	CPhysModelObject();
	virtual ~CPhysModelObject();

public:		//	Attributes

	//	Initialize object
	bool Init(int iType);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Sync physical data to graphics data
	virtual bool Sync();
	//	Render routine
	virtual bool Render(A3DViewport* pViewport);
	//	Render proxy
	virtual bool RenderProxy(A3DViewport* pViewport);

	//	Set position
	virtual void SetPos(const A3DVECTOR3& vPos);
	//	Set orientation
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Setup object
	virtual bool Setup(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);

	//	Change physical state
	virtual bool ChangePhysicalState(int iState);

public:		//	Operations

protected:	//	Attributes

	CSkinModel*				m_pSkinModel;		//	Skin model object
	A3DModelPhysSyncData*	m_pPhysSyncData;	//	Synchronizer data
	A3DModelPhysics*		m_pModelPhys;		//	Model physics proxy

	int		m_iType;		//	Object type

protected:	//	Operations

	//	Load skin model from file
	CSkinModel* LoadSkinModel(const char* szFile);

	//	Setup 2-stick object
	bool Setup2Sticks(const A3DVECTOR3& vPos);
	//	Add cloth skin
	bool AddClothSkin();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_PHYSMODELOBJECT_H_
