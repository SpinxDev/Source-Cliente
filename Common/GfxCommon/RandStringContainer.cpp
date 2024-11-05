/*
* FILE: RandStringContainer.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/05/13
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "RandStringContainer.h"

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
//	Implement RandStringContainer
//	
///////////////////////////////////////////////////////////////////////////

RandStringContainer::RandStringContainer(int iMaxNum)
: m_iMaxNum(iMaxNum)
{
}

RandStringContainer::RandStringContainer(const RandStringContainer& rhs)
: m_iMaxNum(rhs.m_iMaxNum)
{
	for (int iIdx = 0; iIdx < rhs.m_aItems.GetSize(); ++iIdx)
	{
		if (iIdx >= m_aItems.GetSize())
			m_aItems.Add(rhs.m_aItems[iIdx]);
		else
			m_aItems[iIdx] = rhs.m_aItems[iIdx];
	}
}

void RandStringContainer::swap(RandStringContainer& rhs)
{
	a_Swap(m_iMaxNum, rhs.m_iMaxNum);
	a_Swap(m_aItems, rhs.m_aItems);
}

RandStringContainer& RandStringContainer::operator = (const RandStringContainer& rhs)
{
	if (&rhs == this)
		return *this;

	m_iMaxNum = rhs.m_iMaxNum;
	m_aItems.RemoveAll(false);
	for (int iIdx = 0; iIdx < rhs.m_aItems.GetSize(); ++iIdx)
	{
		m_aItems.Add(rhs.m_aItems[iIdx]);
	}
	return *this;
}

RandStringContainer::~RandStringContainer(void)
{
}

bool RandStringContainer::UniqueAdd(const char* szString)
{
	if (!szString || !szString[0])
		return false;

	if (m_aItems.GetSize() >= m_iMaxNum)
		return false;

	return m_aItems.UniquelyAdd(AString(szString)) >= 0;	
}

const char* RandStringContainer::GetRandString() const
{
	if (!m_aItems.GetSize())
		return NULL;

	return m_aItems[a_Random(0, m_aItems.GetSize() - 1)];
}

bool RandStringContainer::SetString(int iIdx, const char* szString)
{
	if (!szString || !szString[0])
		return false;

	if (m_aItems.Find(AString(szString)) >= 0)
		return false;

	m_aItems[iIdx] = szString;
	return true;
}

bool RandStringContainer::IsExist(const AString& strFind) 
{
	return m_aItems.Find(strFind) >= 0;
}

bool RandStringContainer::operator == (const RandStringContainer& rhs) const
{
	if (m_iMaxNum != rhs.m_iMaxNum)
		return false;

	if (m_aItems.GetSize() != rhs.m_aItems.GetSize())
		return false;

	for (int iIdx = 0; iIdx < m_aItems.GetSize(); ++iIdx)
	{
		if (m_aItems[iIdx] != rhs.m_aItems[iIdx])
			return false;
	}

	return true;
}