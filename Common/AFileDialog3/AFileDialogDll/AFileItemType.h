/*
* FILE: AFileItemType.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/18
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AFileItemType_H_
#define _AFileItemType_H_


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

class AListCtrl2;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AFileItemType
//	
///////////////////////////////////////////////////////////////////////////

class AFileItemType
{

public:		//	Types

	enum 
	{
	AFI_IMAGE	= 0,		// .jpg .png .tga .dds
	AFI_ENGINE,				// .smd .ecm3 .umd .umds
	AFI_SOUND,				// .ogg .mp3 .wav
	AFI_DIR,				// directory
	AFI_CUSTOM,				// custom types
	AFI_DEFAULT,			// default types (other than all above)
	AFI_NUM,
	};

public:		//	Constructor and Destructor

	AFileItemType(void);
	virtual ~AFileItemType(void);

public:		//	Attributes

public:		//	Operations

	virtual bool IsCurrentType(const TCHAR* szFileName) const = 0;
	virtual bool IsValidFileType() const { return true; }
	
	//virtual int GetPropertyCount() const = 0;
	const CBitmap* GetBitmap() const { return &m_DefaultImage; }
	void OnUseDefaultImage(AListCtrl2* pListCtrl);
	void OnPrepareDefaultThumbImage(CImageList* pImgLst);
	const TCHAR* GetFilterStr() const { return m_strTypeFilter; }
	const TCHAR* GetTypeName() const { return m_strTypeName; }
	const int GetTypeId() const { return m_iTypeId; }
	int GetDefImageIdx() const { return m_iImageIdx; }
	int GetDefThumbImageIdx() const { return m_iThumbImgIdx; }


protected:	//	Attributes

	CString m_strTypeName;
	CString m_strTypeFilter;
	CBitmap m_DefaultImage;
	int m_iImageIdx;
	int m_iThumbImgIdx;
	int m_iTypeId;

protected:	//	Operations

	virtual void OnLoadBitmap() = 0;
};

class AFIImageType : public AFileItemType
{
public:
	AFIImageType();

protected:
	virtual bool IsCurrentType(const TCHAR* szFileName) const;
	virtual void OnLoadBitmap();
};

class AFIEngineType : public AFileItemType
{
public:
	AFIEngineType();

protected:
	virtual bool IsValidFileType() const;
	virtual bool IsCurrentType(const TCHAR* szFileName) const;
	virtual void OnLoadBitmap();
};

class AFISoundType : public AFileItemType
{
public:
	AFISoundType();

protected:
	virtual bool IsCurrentType(const TCHAR* szFileName) const;
	virtual void OnLoadBitmap();
};

class AFIDirType : public AFileItemType
{
public:
	AFIDirType();

protected:
	virtual bool IsValidFileType() const;
	virtual bool IsCurrentType(const TCHAR* szFileName) const;
	virtual void OnLoadBitmap();
};

class AFICustomType : public AFileItemType
{
public:
	AFICustomType(const TCHAR* szCustomExts);

private:
	AArray<CString> m_aTypeExts;

protected:
	virtual bool IsCurrentType(const TCHAR* szFileName) const;
	virtual bool IsValidFileType() const;
	virtual void OnLoadBitmap();
};

class AFIDefaultType : public AFileItemType
{
public:
	AFIDefaultType();

protected:
	virtual bool IsCurrentType(const TCHAR* szFileName) const;
	virtual void OnLoadBitmap();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AFileItemType_H_


