/*
 * FILE: Portal.h
 *
 * DESCRIPTION: Portal class
 *
 * CREATED BY: Duyuxin, 2003/7/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_PORTAL_H_
#define _PORTAL_H_

#include "A3DTypes.h"
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
class CSector;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CPortal
//
///////////////////////////////////////////////////////////////////////////

class CPortal : public CEditObject
{
public:		//	Types

	//	Link relation between portal and sector
	enum
	{
		LINK_NONE = 0,	//	No link
		LINK_FROM,		//	Link from
		LINK_TO,		//	Link to
		LINK_BIDIR,		//	Bidirection
	};

	struct SECTOR
	{
		CSector*	pSector;	//	Sector
		int			iLinkMode;	//	Link mode
	};

	//	Data struct used to load and save
	struct DATA
	{
		int		iMajorAxis;
		int		iSector1;
		int		iLinkMode1;
		int		iSector2;
		int		iLinkMode2;
	};

public:		//	Constructor and Destructor

	CPortal();
	virtual ~CPortal() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize portal
	bool Init(CRender* pRender, A3DVECTOR3* aVerts, int iMajorAxis);
	//	Release portal
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
	//	Bound portal
	void Bound(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, bool bWindow);
	//	Check sector is visible
	bool IsVisible();

	//	Set vertices of portal
	void SetVertices(A3DVECTOR3* aVerts);
	//	Get portal vertices
	void GetVertices(A3DVECTOR3* aVerts)
	{
		aVerts[0] = m_aVerts[0];
		aVerts[1] = m_aVerts[1];
		aVerts[2] = m_aVerts[2];
		aVerts[3] = m_aVerts[3];
	}

	//	Set/get major axis
	void SetMajorAxis(int iAxis) { m_iMajorAxis = iAxis; }
	int GetMajorAxis() { return m_iMajorAxis; }

	//	Link a sector to this portal
	bool LinkSector(CSector* pSector);
	//	Unlink all sectors
	void UnlinkSectors();
	//	Get number of linked sector
	int GetLinkedSectorNum();
	//	Get linked sector
	CSector* GetLinkedSector(int iIndex, int* piLinkMode);
	//	Change link mode
	void ChangeLinkMode();

	//	Set/Get name
	void SetName(const char* szName) { strcpy(m_szName, szName); }
	const char* GetName() { return m_szName; }

protected:	//	Attributes

	CRender*	m_pRender;			//	Render object
	char		m_szName[32];		//	Name of portal

	A3DVECTOR3	m_aVerts[4];
	int			m_iMajorAxis;		//	Major axis of portal
	DWORD		m_dwColor;			//	Portal's color

	A3DVECTOR3	m_aRVerts[8];		//	Render vertices
	static WORD	m_aIndices[24];		//	Indices

	SECTOR		m_aSectors[2];		//	Sectors linked with this portal

protected:	//	Operations

	//	Set sector's color according to link mode.
	void SetSectorCol(CSector* pSector, int iLinkMode);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_PORTAL_H_
