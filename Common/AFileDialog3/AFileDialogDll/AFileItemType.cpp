/*
* FILE: AFileItemType.cpp
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
#include "AFileItemType.h"
#include "AListCtrl2.h"

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

extern void _SetBitmapTransparent(CBitmap* pBitmap, COLORREF colorTrans, COLORREF colorDest);

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
//	Implement AFileItemType
//	
///////////////////////////////////////////////////////////////////////////

AFileItemType::AFileItemType(void)
: m_iImageIdx(-1)
, m_iThumbImgIdx(-1)
{
}

AFileItemType::~AFileItemType(void)
{
}

void AFileItemType::OnUseDefaultImage(AListCtrl2* pListCtrl)
{
	if (!m_DefaultImage.GetSafeHandle())
		OnLoadBitmap();

	m_iImageIdx = pListCtrl->AddCommonImage(&m_DefaultImage);
}

void AFileItemType::OnPrepareDefaultThumbImage(CImageList* pImgLst)
{
	if (!m_DefaultImage.GetSafeHandle())
		OnLoadBitmap();

	m_iThumbImgIdx = pImgLst->Add(&m_DefaultImage, RGB(0, 0, 0));
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement AFIImageType
//
///////////////////////////////////////////////////////////////////////////


AFIImageType::AFIImageType()
{
	m_strTypeName = _T("图片文件");
	m_strTypeFilter = _T("*.bmp, *.tga, *.jpg, *.png, *.tif, *.dds");
	m_iTypeId = AFI_IMAGE;
}

bool AFIImageType::IsCurrentType(const TCHAR* szFileName) const
{
	if (AFCheckFileExt(szFileName, _T(".jpg"))
		|| AFCheckFileExt(szFileName, _T(".bmp"))
		|| AFCheckFileExt(szFileName, _T(".tga"))
		|| AFCheckFileExt(szFileName, _T(".png"))
		|| AFCheckFileExt(szFileName, _T(".tif"))
		|| AFCheckFileExt(szFileName, _T(".dds")))
		return true;

	return false;
}

void AFIImageType::OnLoadBitmap()
{
	m_DefaultImage.LoadBitmap(IDB_PICTURE);
	_SetBitmapTransparent(&m_DefaultImage, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));
}


///////////////////////////////////////////////////////////////////////////
//
//	Implement AFIEngineType
//
///////////////////////////////////////////////////////////////////////////

AFIEngineType::AFIEngineType()
{
	m_strTypeName = _T("Angelica引擎文件");
#ifdef _ANGELICA31
	m_strTypeFilter = _T("*.ecm3, *.gfx3, *.SMD, *.umd, *.umds");
#endif
	m_iTypeId = AFI_ENGINE;
}

bool AFIEngineType::IsCurrentType(const TCHAR* szFileName) const
{
	if (AFCheckFileExt(szFileName, _T(".ecm3"))
		||AFCheckFileExt(szFileName, _T(".gfx3"))
		||AFCheckFileExt(szFileName, _T(".SMD"))
		||AFCheckFileExt(szFileName, _T(".umd"))
		||AFCheckFileExt(szFileName, _T(".umds")))
		return true;

	return false;
}

bool AFIEngineType::IsValidFileType() const
{
#ifdef _ANGELICA31
	return true;
#else
	return false;
#endif
}

void AFIEngineType::OnLoadBitmap()
{
	m_DefaultImage.LoadBitmap(IDB_PICTURE);
	_SetBitmapTransparent(&m_DefaultImage, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));
}
///////////////////////////////////////////////////////////////////////////
//
//	Implement AFISoundType
//
///////////////////////////////////////////////////////////////////////////

AFISoundType::AFISoundType()
{
	m_strTypeName = _T("音频文件");
	m_strTypeFilter = _T("*.wav, *.mp3, *.ogg");
	m_iTypeId = AFI_SOUND;
}

bool AFISoundType::IsCurrentType(const TCHAR* szFileName) const
{
	if (AFCheckFileExt(szFileName, _T(".wav"))
		|| AFCheckFileExt(szFileName, _T(".mp3"))
		|| AFCheckFileExt(szFileName, _T(".ogg")))
		return true;

	return false;
}

void AFISoundType::OnLoadBitmap()
{
	m_DefaultImage.LoadBitmap(IDB_SOUND);
	_SetBitmapTransparent(&m_DefaultImage, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));
}
///////////////////////////////////////////////////////////////////////////
//
//	Implement AFIDirType
//
///////////////////////////////////////////////////////////////////////////

AFIDirType::AFIDirType()
{
	m_strTypeName = _T("文件夹");
	m_iTypeId = AFI_DIR;
}

bool AFIDirType::IsValidFileType() const
{
	return false;
}

bool AFIDirType::IsCurrentType(const TCHAR* szFileName) const
{
	return PathIsDirectory(szFileName) == TRUE;
}

void AFIDirType::OnLoadBitmap()
{
	m_DefaultImage.LoadBitmap(IDB_DIRECTORY);
	_SetBitmapTransparent(&m_DefaultImage, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement AFICustomType
//
///////////////////////////////////////////////////////////////////////////

static bool checkIsInExt(const TCHAR ch)
{
	return isalpha(ch) || isdigit(ch);
}

AFICustomType::AFICustomType(const TCHAR* szCustomExts)
{
	m_strTypeName = _T("自定义文件");
	m_iTypeId = AFI_CUSTOM;

	CString strExt(szCustomExts);
	strExt.TrimLeft();
	strExt.TrimRight();
	if (strExt.IsEmpty())
		return;

	int iCurIdx = 0;
	CString strCur;
	for (; iCurIdx != strExt.GetLength(); ++iCurIdx)
	{
		if (checkIsInExt(strExt[iCurIdx]))
			strCur += strExt[iCurIdx];
		else if (!strCur.IsEmpty())
		{
			if (strCur.Left(1) != _T("."))
				strCur = _T(".") + strCur;

			m_aTypeExts.Add(strCur);
			strCur.Empty();
		}
	}

	if (!strCur.IsEmpty())
	{		
		if (strCur.Left(1) != _T("."))
			strCur = _T(".") + strCur;

		m_aTypeExts.Add(strCur);
		strCur.Empty();
	}

	for (iCurIdx = 0; iCurIdx < m_aTypeExts.GetSize(); ++iCurIdx)
	{
		m_strTypeFilter += m_aTypeExts[iCurIdx];
		if (iCurIdx != m_aTypeExts.GetSize() - 1)
			m_strTypeFilter += _T(" ");
	}
}

bool AFICustomType::IsCurrentType(const TCHAR* szFileName) const
{
	for (int iExtIdx = 0; iExtIdx < m_aTypeExts.GetSize(); ++iExtIdx)
	{
		if (AFCheckFileExt(szFileName, m_aTypeExts[iExtIdx]))
			return true;
	}

	return false;
}

bool AFICustomType::IsValidFileType() const
{
	return m_aTypeExts.GetSize() != 0;
}

void AFICustomType::OnLoadBitmap()
{
	m_DefaultImage.LoadBitmap(IDB_OTHER);
	_SetBitmapTransparent(&m_DefaultImage, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement AFIDefaultType
//
///////////////////////////////////////////////////////////////////////////

AFIDefaultType::AFIDefaultType()
{
	m_strTypeName = _T("其他文件");
	m_strTypeFilter = _T("*.*");
	m_iTypeId = AFI_DEFAULT;
}

bool AFIDefaultType::IsCurrentType(const TCHAR* szFileName) const
{
	return true;
}

void AFIDefaultType::OnLoadBitmap()
{
	m_DefaultImage.LoadBitmap(IDB_OTHER);
	_SetBitmapTransparent(&m_DefaultImage, RGB(255, 0, 255), GetSysColor(COLOR_WINDOW));
}