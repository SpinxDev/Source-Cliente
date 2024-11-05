/*
* FILE: AFileItem.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/18
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "AFileItem.h"
#include "AFileItemType.h"

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
//	Implement AFileItem
//	
///////////////////////////////////////////////////////////////////////////


CString AFileItem::FileTime2String(const CTime& ft)
{
	CString ftStr;
	ftStr.Format(_T("%d-%d-%d %d:%d")
		, ft.GetYear()
		, ft.GetMonth()
		, ft.GetDay()
		, ft.GetHour()
		, ft.GetMinute());
	return ftStr;
}

CString AFileItem::FileSize2String(const UINT uFileSize)
{
	UINT calcFileSize(uFileSize);

	if (0 == uFileSize)
		return CString();

	calcFileSize = uFileSize / 1024 + ((uFileSize % 1024 == 0) ? 0 : 1);
	CString strFileSize;
	strFileSize.Format(_T("%d"), calcFileSize);
	for (int j = strFileSize.GetLength() % 3; j < strFileSize.GetLength(); j += 3)
	{
		if (j == 0 || j == (strFileSize.GetLength() - 1))
			continue;

		strFileSize.Insert(j, _T(','));
		++j;
	}

	strFileSize += _T(" KB");

	return strFileSize;
}

AFileItem::AFileItem(const AFileItemType* pType
					 , const TCHAR* szTitle
					 , const CTime& kModifyTime
					 , const UINT uFileSize
					 , const TCHAR* szFullPath)
: m_pType(pType)
, m_strTitle(szTitle)
, m_kModifyTime(kModifyTime)
, m_uFileSize(uFileSize)
, m_strFullPath(szFullPath)
, m_pImg(new AFileImageWrapper)
{
	int iIndex = 0;
	
	SetItemText(iIndex++, szTitle);
	SetItemText(iIndex++, FileSize2String(uFileSize));
	SetItemText(iIndex++, pType->GetTypeName());
	m_ImageSize =_T("");
	SetItemText(iIndex++, m_ImageSize);
	SetItemText(iIndex++, kModifyTime.Format(_T("%Y-%m-%d %H:%M")));

	m_iCommonImgIdx = pType->GetDefImageIdx();
	m_iThumbImgIdx[0] = pType->GetDefThumbImageIdx();
	m_iThumbImgIdx[1] = pType->GetDefThumbImageIdx();
}

AFileItem::~AFileItem(void)
{
	delete m_pImg;
}

bool AFileItem::LoadFileImage(unsigned char* pBuf, long lDataLen)
{
	return m_pImg->ReadFromMemory(pBuf, lDataLen);
}

CBitmap* AFileItem::GetBitmap(bool bAlphaBlend)
{
	return m_pImg->GetAsBitmap(bAlphaBlend);
}

bool AFileItem::Less(const AListItem* pRhs, int iPropIdx) const
{
	ASSERT( pRhs );
	const AFileItem* pCompDst = (const AFileItem*)(pRhs);
	if (!pCompDst)
		return false;

	if (pCompDst->m_pType->GetTypeId() == AFileItemType::AFI_DIR
		&& m_pType->GetTypeId() != AFileItemType::AFI_DIR)
		return false;
	if (pCompDst->m_pType->GetTypeId() != AFIImageType::AFI_DIR
		&& m_pType->GetTypeId() == AFIImageType::AFI_DIR)
		return true;

	if (iPropIdx == 0)
	{
		return m_strTitle.CompareNoCase(pCompDst->m_strTitle) < 0;
	}
	else if (iPropIdx == 1)
	{
		return m_uFileSize < pCompDst->m_uFileSize;
	}
	else if (iPropIdx == 2)
	{
		return _tcscmp(m_pType->GetTypeName(), pCompDst->m_pType->GetTypeName()) < 0;
	}
	else if (iPropIdx == 3)
	{
		return m_kModifyTime < pCompDst->m_kModifyTime;
	}
	else
		return false;
}

void AFileItem::ChangeSizeText(DWORD ImageHeight, DWORD ImageWidth)
{
	if (m_pType->GetTypeId() != AFileItemType::AFI_IMAGE)
		return;

	m_ImageSize.Format(_T("%u * %u"), ImageWidth, ImageHeight);
	SetItemText(3, m_ImageSize);
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement DirectoryItem
//
//////////////////////////////////////////////////////////////////////////


DirectoryItem::DirectoryItem(const AFileItemType* pType
							 , const TCHAR* szTitle
							 , const CTime& kModifyTime
							 , const UINT uFileSize
							 , const TCHAR* szFullPath)
: AFileItem(pType
			, szTitle
			, kModifyTime
			, uFileSize
			, szFullPath)
{
	if (m_strFullPath.Right(1) != '\\' && m_strFullPath.Right(1) != '/')
		m_strFullPath += '\\';
}

DirectoryItem::~DirectoryItem(void)
{

}
