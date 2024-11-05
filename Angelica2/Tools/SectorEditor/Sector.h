/*
 * FILE: Sector.h
 *
 * DESCRIPTION: Sector class
 *
 * CREATED BY: Duyuxin, 2003/7/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_SECTOR_H_
#define _SECTOR_H_

#include "Box3D.h"
#include "EditObject.h"

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

class CRender;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CSector
//
///////////////////////////////////////////////////////////////////////////

class CSector : public CEditObject
{
public:		//	Types

public:		//	Constructor and Destructor

	CSector();
	virtual ~CSector() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize sector
	bool Init(CRender* pRender, const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);
	//	Release sector
	void Release();

	//	Select/Deselect object
	virtual void Select(bool bSel);
	//	Move object
	virtual void Move(const A3DVECTOR3& vDelta);

	//	Load data from file
	virtual bool Load(FILE* fp);
	//	Save data from file
	virtual bool Save(FILE* fp);

	//	Render sector
	bool Render();
	//	Bound sector
	void Bound(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, bool bWindow);
	//	Check sector is visible
	bool IsVisible();

	A3DVECTOR3 GetMins() { return m_vMins; }
	A3DVECTOR3 GetMaxs() { return m_vMaxs; }
	A3DVECTOR3 GetCenter() { return (m_vMins + m_vMaxs) * 0.5f; }

	//	Resize sector
	void SetMinsAndMaxs(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);
	//	Check sector collision
	bool Collision(CSector* pSector) { return Collision(pSector->GetMins(), pSector->GetMaxs()); }
	bool Collision(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);

	//	Set/Get color
	void SetColor(DWORD dwCol) { m_Box3D.SetColor(dwCol); }
	DWORD GetColor() { return m_Box3D.GetColor(); }

	//	Set/Get delay render flag
	void SetDelayRenderFlag(bool bDelay) { m_bDelayRender = bDelay; }
	bool GetDelayRenderFlag() { return m_bDelayRender; }

protected:	//	Attributes

	CBox3D		m_Box3D;

	A3DVECTOR3	m_vMins;		//	Sector volume
	A3DVECTOR3	m_vMaxs;

	bool		m_bDelayRender;	//	Delay render flag

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_SECTOR_H_
