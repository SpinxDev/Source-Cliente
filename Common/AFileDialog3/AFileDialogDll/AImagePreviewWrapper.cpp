/*
* FILE: AImagePreviewWrapper.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/08/31
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"

#ifdef _ANGELICA31

#include "SyncWrapper.h"
#include "AImagePreviewWrapper.h"
#include "AImagePreviewImpl.h"

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

static AImagePreviewWrapper* s_pInstance;
CCriticalSection s_cs;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AImagePreviewWrapper
//	
///////////////////////////////////////////////////////////////////////////

AImagePreviewWrapper* AImagePreviewWrapper::GetInstance()
{
	if (s_pInstance)
		return s_pInstance;

	{
		SyncWrapper<CCriticalSection> locker(s_cs);
		if (!s_pInstance)
		{
			static AImagePreviewWrapper _inst;
			s_pInstance = &_inst;
		}

		return s_pInstance;
	}
}

AImagePreviewWrapper::AImagePreviewWrapper(void)
: m_pImpl(new AImagePreviewImpl)
{
}

AImagePreviewWrapper::~AImagePreviewWrapper(void)
{
	delete m_pImpl;
}

//	Set listener callback function when one frame is rendered.
void AImagePreviewWrapper::SetListener(Listener* pListener)
{
	m_pImpl->SetListener(pListener);
}

//	Set preview file's path, if succeed, return true.
//	After successfully set the preview file, user would get the callback
//	Set NULL to stop from preview
bool AImagePreviewWrapper::SetPreviewFile(const TCHAR* szFile)
{
	if (m_pImpl)
		return m_pImpl->SetPreviewFile(szFile);

	return false;
}


#endif