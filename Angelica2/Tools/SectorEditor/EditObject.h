/*
 * FILE: EditObject.h
 *
 * DESCRIPTION: Base class of edit object
 *
 * CREATED BY: Duyuxin, 2003/7/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_EDITOBJECT_H_
#define _EDITOBJECT_H_

#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	ID edit objects
enum
{
	EOID_UNKNOWN = 0,	//	Unknown object
	EOID_SECTOR,		//	CSector object
	EOID_PORTAL,		//	CPortal object
};

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
//	Class CEditObject
//
///////////////////////////////////////////////////////////////////////////

class CEditObject
{
public:		//	Types

public:		//	Constructor and Destructor

	CEditObject()
	{
		m_iID		= EOID_UNKNOWN;
		m_bSelected = false;
		m_bInBound	= true;
	}

public:		//	Attributes

public:		//	Operations

	//	Get ID
	int GetID() const { return m_iID; }

	//	Select/Deselect object
	virtual void Select(bool bSel) { m_bSelected = bSel; }
	//	Move object
	virtual void Move(const A3DVECTOR3& vDelta) {}

	//	Load data from file
	virtual bool Load(FILE* fp) { return true; }
	//	Save data from file
	virtual bool Save(FILE* fp) { return true; }

	//	Get select flag
	bool IsSelected() { return m_bSelected; }

	//	Set/Get in-bound flag
	void SetInBoundFlag(bool bIn) { m_bInBound = bIn; }
	bool GetInBoundFlag() { return m_bInBound; }

protected:	//	Attributes

	int		m_iID;			//	Object ID
	bool	m_bSelected;	//	Selected flag
	bool	m_bInBound;		//	In bound flag

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_EDITOBJECT_H_
