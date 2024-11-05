/*
 * FILE: PhysObjectMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/3/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "PhysObjectMan.h"
#include "PhysObject.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CPhysObjectMan
//	
///////////////////////////////////////////////////////////////////////////

CPhysObjectMan::CPhysObjectMan()
{
}

CPhysObjectMan::~CPhysObjectMan()
{
}

//	Initialize object
bool CPhysObjectMan::Init()
{
	return true;
}

//	Release object
void CPhysObjectMan::Release()
{
	//	Release all objects
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Release();
		delete pObject;
	}

	m_aObjects.RemoveAll(true);
}

//	Reset object
bool CPhysObjectMan::Reset()
{
	//	Release all objects
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Release();
		delete pObject;
	}

	m_aObjects.RemoveAll(true);

	return true;
}

//	Tick routine
bool CPhysObjectMan::Tick(DWORD dwDeltaTime)
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Tick(dwDeltaTime);
	}

	return true;
}

//	Sync physical data to graphics data
bool CPhysObjectMan::Sync()
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Sync();
	}

	return true;
}

//	Render routine
bool CPhysObjectMan::Render(A3DViewport* pViewport)
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->Render(pViewport);
	}

	return true;
}

//	Add physical object
bool CPhysObjectMan::AddPhysObject(CPhysObject* pObject)
{
	m_aObjects.Add(pObject);
	return true;
}

//	Change physical state
void CPhysObjectMan::ChangePhysicalState(int iPhysState)
{
	for (int i=0; i < m_aObjects.GetSize(); i++)
	{
		CPhysObject* pObject = m_aObjects[i];
		pObject->ChangePhysicalState(iPhysState);
	}
}
