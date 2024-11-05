/*
* FILE: AFileItem.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/18
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AFileItem_H_
#define _AFileItem_H_

#include "AListCtrl2.h"

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

class AFileItemType;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AFileItem
//	
///////////////////////////////////////////////////////////////////////////

class AFileItem : public AListItem
{

public:		//	Types

public:		//	Constructor and Destructor

	AFileItem(const AFileItemType* pType
		, const TCHAR* szTitle
		, const CTime& kModifyTime
		, const UINT uFileSize
		, const TCHAR* szFullPath);
	virtual ~AFileItem(void);

public:		//	Attributes

public:		//	Operations

	const AFileItemType* GetItemType() const { return m_pType; }
	const TCHAR* GetItemTitle() const { return m_strTitle; }
	const TCHAR* GetFullPath() const { return m_strFullPath; }
	const TCHAR* GetImageSize() const { return m_ImageSize; }
	bool LoadFileImage(unsigned char* pBuf, long lDataLen);
	CBitmap* GetBitmap(bool bAlphaBlend);
	void ChangeSizeText(DWORD ImageHeight, DWORD ImageWidth);

protected:	//	Attributes

	AFileImageWrapper* m_pImg;
	const AFileItemType* m_pType;
	const CString m_strTitle;
	const CTime m_kModifyTime;
	const UINT m_uFileSize;
	CString m_strFullPath;
	CString m_ImageSize;

protected:	//	Operations

	virtual bool Less(const AListItem* pRhs, int iPropIdx) const;
	static CString FileTime2String(const CTime& ft);
	static CString FileSize2String(const UINT uFileSize);
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class DirectoryItem
//	
///////////////////////////////////////////////////////////////////////////

class DirectoryItem : public AFileItem
{
public:		//	Types

public:		//	Constructor and Destructor

	DirectoryItem(const AFileItemType* pType
		, const TCHAR* szTitle
		, const CTime& kModifyTime
		, const UINT uFileSize
		, const TCHAR* szFullPath);
	virtual ~DirectoryItem(void);

public:		//	Attributes

public:		//	Operations

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AFileItem_H_


