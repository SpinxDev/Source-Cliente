/*
* FILE: AImagePreviewWrapper.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/08/31
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AImagePreviewWrapper_H_
#define _AImagePreviewWrapper_H_

#include "AFileExpDecl.h"

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

class AImagePreviewImpl;
class AFilePreviewWnd;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AImagePreviewWrapper
//	
///////////////////////////////////////////////////////////////////////////

class AImagePreviewWrapper
{

public:		//	Types

	class _AFILE_EXP_ Listener
	{
	public:
		virtual void OneFrameRendered(DWORD * pdwBuffer, DWORD dwWndSize) = 0;
	};

private:		//	Constructor and Destructor

	AImagePreviewWrapper(void);
	virtual ~AImagePreviewWrapper(void);

public:		//	Attributes

public:		//	Operations

	_AFILE_EXP_ static AImagePreviewWrapper* GetInstance();

	//	Set listener callback function when one frame is rendered.
	_AFILE_EXP_ void SetListener(Listener* pListener);
	//	Set preview file's path, if succeed, return true.
	//	After successfully set the preview file, user would get the callback
	//	Set NULL to stop from preview
	_AFILE_EXP_ bool SetPreviewFile(const TCHAR* szFile);

	//	Below functions are only used inside this DLL.
	bool Init(AFilePreviewWnd* pPreviewWnd);
	void Release(AFilePreviewWnd* pPreviewWnd);

protected:	//	Attributes

	AImagePreviewImpl* m_pImpl;

protected:	//	Operations


};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AImagePreviewWrapper_H_


