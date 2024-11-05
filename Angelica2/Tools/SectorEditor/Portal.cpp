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

#include "Global.h"
#include "A3D.h"
#include "Render.h"
#include "Portal.h"
#include "WireCollector.h"
#include "Utility.h"
#include "Sector.h"
#include "SectorEditorDoc.h"
#include "MainFrm.h"

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	Indices
WORD CPortal::m_aIndices[24] =
{
//	0, 1, 1, 2, 2, 3, 3, 0,
	0, 1, 2, 3, 4, 5, 6, 7,
	1, 3, 3, 5, 5, 7, 7, 1,
	0, 2, 2, 4, 4, 6, 6, 0,
};

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CPortal
//
///////////////////////////////////////////////////////////////////////////

CPortal::CPortal()
{
	m_iID			= EOID_PORTAL;
	m_pRender		= NULL;
	m_iMajorAxis	= 0;
	m_dwColor		= g_aColTable[COL_PORTAL];
	m_szName[0]		= '\0';

	m_aSectors[0].pSector	= NULL;
	m_aSectors[1].pSector	= NULL;
}

//	Initialize portal
bool CPortal::Init(CRender* pRender, A3DVECTOR3* aVerts, int iMajorAxis)
{
	m_pRender		= pRender;
	m_iMajorAxis	= iMajorAxis;

	SetVertices(aVerts);
	return true;
}

//	Release portal
void CPortal::Release()
{
	m_pRender	= NULL;
	m_bSelected	= false;
}

//	Render sector
bool CPortal::Render()
{
	if (!IsVisible())
		return true;

	ASSERT(m_pRender);

	//	Push to render buffer
	m_pRender->GetWireCollector()->AddRenderData(m_aRVerts, 8, m_aIndices, 24, m_dwColor);

	return true;
}

void CPortal::Select(bool bSel) 
{
	CEditObject::Select(bSel);
	m_dwColor = bSel ? g_aColTable[COL_SELPORTAL] : g_aColTable[COL_PORTAL];

	if (bSel)
	{
		SetSectorCol(m_aSectors[0].pSector, m_aSectors[0].iLinkMode);
		SetSectorCol(m_aSectors[1].pSector, m_aSectors[1].iLinkMode);

		if (m_aSectors[0].pSector)	m_aSectors[0].pSector->SetDelayRenderFlag(true);
		if (m_aSectors[1].pSector)	m_aSectors[1].pSector->SetDelayRenderFlag(true);
	}
	else
	{
		//	Restore sector's color
		SetSectorCol(m_aSectors[0].pSector, LINK_NONE);
		SetSectorCol(m_aSectors[1].pSector, LINK_NONE);

		if (m_aSectors[0].pSector)	m_aSectors[0].pSector->SetDelayRenderFlag(false);
		if (m_aSectors[1].pSector)	m_aSectors[1].pSector->SetDelayRenderFlag(false);
	}
}

//	Set vertices of portal
void CPortal::SetVertices(A3DVECTOR3* aVerts)
{
	for (int i=0; i < 4; i++)
	{
		SnapVector(&aVerts[i], &m_aVerts[i], MIN_GRID);

		//	Adjust pin's vertices
		int n = i * 2 ;

		m_aRVerts[n]	= m_aVerts[i];
		m_aRVerts[n+1]	= m_aVerts[i];

		m_aRVerts[n].m[m_iMajorAxis]	= m_aVerts[i].m[m_iMajorAxis] - MIN_GRID;
		m_aRVerts[n+1].m[m_iMajorAxis]	= m_aVerts[i].m[m_iMajorAxis] + MIN_GRID;
	}
}

//	Move object
void CPortal::Move(const A3DVECTOR3& vDelta)
{
	A3DVECTOR3 aVerts[4];

	for (int i=0; i < 4; i++)
		aVerts[i] = m_aVerts[i] + vDelta;
	
	SetVertices(aVerts);
}

//	Link a sector to this portal
bool CPortal::LinkSector(CSector* pSector)
{
	if (!pSector)
		return false;

	if (m_aSectors[0].pSector == pSector || m_aSectors[1].pSector == pSector)
		return true;	//	Sector has been linked with this portal

	bool bLinked = false;

	if (!m_aSectors[0].pSector)
	{
		m_aSectors[0].pSector	= pSector;
		m_aSectors[0].iLinkMode = LINK_BIDIR;
		bLinked = true;
	}
	else if (!m_aSectors[1].pSector)
	{
		m_aSectors[1].pSector	= pSector;
		m_aSectors[1].iLinkMode = LINK_BIDIR;
		bLinked = true;
	}

	//	This portal is being selected ?
	if (bLinked && m_bSelected)
	{
		SetSectorCol(pSector, LINK_BIDIR);
		pSector->SetDelayRenderFlag(true);
	}

	return bLinked;
}

//	Unlink all sectors
void CPortal::UnlinkSectors()
{
	//	This portal is being selected ?
	if (m_bSelected)
	{
		//	Restore sector's color
		SetSectorCol(m_aSectors[0].pSector, LINK_NONE);
		SetSectorCol(m_aSectors[1].pSector, LINK_NONE);

		if (m_aSectors[0].pSector)	m_aSectors[0].pSector->SetDelayRenderFlag(false);
		if (m_aSectors[1].pSector)	m_aSectors[1].pSector->SetDelayRenderFlag(false);
	}

	m_aSectors[0].pSector = NULL;
	m_aSectors[1].pSector = NULL;
}

//	Set sector's color according to link mode.
void CPortal::SetSectorCol(CSector* pSector, int iLinkMode)
{
	if (!pSector)
		return;

	switch (iLinkMode)
	{
	case LINK_NONE:		pSector->SetColor(g_aColTable[COL_SECTOR]);			break;
	case LINK_TO:		pSector->SetColor(g_aColTable[COL_HALFLINKSECTOR]);	break;
	case LINK_FROM:
	case LINK_BIDIR:	pSector->SetColor(g_aColTable[COL_LINKSECTOR]);		break;
	}
}

//	Get number of linked sector
int CPortal::GetLinkedSectorNum()
{
	int i, iNum = 0;

	for (i=0; i < 2; i++)
	{
		if (m_aSectors[i].pSector)
			iNum++;
	}

	return iNum;
}

//	Get linked sector
CSector* CPortal::GetLinkedSector(int iIndex, int* piLinkMode)
{
	ASSERT(iIndex >= 0 && iIndex < 2);
	
	if (piLinkMode)
		*piLinkMode = m_aSectors[iIndex].iLinkMode;

	return m_aSectors[iIndex].pSector;
}

//	Change link mode
void CPortal::ChangeLinkMode()
{
	if (!m_aSectors[0].pSector || !m_aSectors[1].pSector)
		return;

	int iMode = m_aSectors[0].iLinkMode + 1;
	if (iMode > LINK_BIDIR)
		iMode = LINK_FROM;

	m_aSectors[0].iLinkMode = iMode;

	switch (iMode)
	{
	case LINK_FROM:		m_aSectors[1].iLinkMode = LINK_TO;		break;
	case LINK_TO:		m_aSectors[1].iLinkMode = LINK_FROM;	break;
	case LINK_BIDIR:	m_aSectors[1].iLinkMode = LINK_BIDIR;	break;
	}

	if (m_bSelected)
	{
		SetSectorCol(m_aSectors[0].pSector, m_aSectors[0].iLinkMode);
		SetSectorCol(m_aSectors[1].pSector, m_aSectors[1].iLinkMode);
	}
}

/*	Bound sector

	vMins, vMaxs: bound's volume
	bWindow: true, window mode; false, crossing mode
*/
void CPortal::Bound(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, bool bWindow)
{
	A3DVECTOR3 vThisMins, vThisMaxs;
	ClearAABB(vThisMins, vThisMaxs);

	for (int i=0; i < 8; i++)
		AddVertexToAABB(vThisMins, vThisMaxs, m_aRVerts[i]);

	bool bIn;

	if (bWindow)
		bIn = AABBInAABB(vThisMins, vThisMaxs, vMins, vMaxs);
	else
		bIn = AABBToAABB(vThisMins, vThisMaxs, vMins, vMaxs);

	SetInBoundFlag(bIn);
}

//	Load data from file
bool CPortal::Load(FILE* fp)
{
	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();
	int iFileVer = pDoc->GetCurFileVersion();

	//	Load name
	if (iFileVer >= 2)
		fread(m_szName, 1, sizeof (m_szName), fp);
	else
		m_szName[0] = '\0';

	//	Load vertices
	A3DVECTOR3 aVerts[4];
	fread(aVerts, 1, sizeof (aVerts), fp);

	DATA Data;
	fread(&Data, 1, sizeof (Data), fp);
	
	m_iMajorAxis = Data.iMajorAxis;

	//	Set vertices after major axis has been set.
	SetVertices(aVerts);

	m_aSectors[0].pSector	= pDoc->GetSectorByIndex(Data.iSector1);
	m_aSectors[0].iLinkMode	= Data.iLinkMode1;
	m_aSectors[1].pSector	= pDoc->GetSectorByIndex(Data.iSector2);
	m_aSectors[1].iLinkMode	= Data.iLinkMode2;

	return true;
}

//	Save data from file
bool CPortal::Save(FILE* fp)
{
	//	Write name
	if (SEDFILE_VERSION >= 2)
		fwrite(m_szName, 1, sizeof (m_szName), fp);

	//	Write vertices
	fwrite(m_aVerts, 1, sizeof (m_aVerts), fp);
	
	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();

	DATA Data;
	Data.iMajorAxis = m_iMajorAxis;
	Data.iSector1	= m_aSectors[0].pSector ? pDoc->GetSectorIndex(m_aSectors[0].pSector) : -1;
	Data.iLinkMode1	= m_aSectors[0].iLinkMode;
	Data.iSector2	= m_aSectors[1].pSector ? pDoc->GetSectorIndex(m_aSectors[1].pSector) : -1;
	Data.iLinkMode2	= m_aSectors[1].iLinkMode;

	fwrite(&Data, 1, sizeof (Data), fp);

	return true;
}

//	Check sector is visible
bool CPortal::IsVisible()
{
	//	Selected object always is visible
	if (m_bSelected)
		return true;

	if (!g_Configs.bShowPortal)
		return false;

	if (!g_Configs.bForceRender && !m_bInBound)
		return false;

	return true;
}


