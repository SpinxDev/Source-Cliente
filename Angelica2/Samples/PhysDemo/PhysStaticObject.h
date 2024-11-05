/*
 * FILE: PhysStaticObject.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _PHYSSTATICOBJECT_H_
#define _PHYSSTATICOBJECT_H_

#include "PhysObject.h"
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

class A3DFrame;
class A3DLitModel;
class APhysXObject;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CPhysStaticObject
//	
///////////////////////////////////////////////////////////////////////////

class CPhysStaticObject : public CPhysObject
{
public:		//	Types

public:		//	Constructor and Destructor

	CPhysStaticObject();
	virtual ~CPhysStaticObject();

public:		//	Attributes

public:		//	Operations

	//	Load object from file
	bool Load(const char* szFile);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
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

	//	Get model object
	A3DLitModel* GetModel() { return m_pModel; }

protected:	//	Attributes

	A3DLitModel*	m_pModel;			//	Model object
	APhysXObject*	m_pPhysXRBObject;	//	Physical rigid body
	AString			m_strPhysFile;		//	Physical data file

protected:	//	Operations

	//	Load model from file
	bool LoadModel(const char* szFile);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_PHYSSTATICOBJECT_H_
