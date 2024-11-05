/*
* FILE: ASoundPlayerWrapper.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/08/25
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "ASoundPlayerWrapper.h"
#include "ASoundPlayerImpl.h"

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
//	Implement ASoundPlayerWrapper
//	
///////////////////////////////////////////////////////////////////////////


ASoundPlayerWrapper* ASoundPlayerWrapper::CreateSoundPlayer()
{
	return new ASoundPlayerWrapper();
}

void ASoundPlayerWrapper::ReleaseSoundPlayer(ASoundPlayerWrapper* pSoundPlayer)
{
	delete pSoundPlayer;
}

ASoundPlayerWrapper::ASoundPlayerWrapper(void)
: m_pImpl(new ASoundPlayerImpl)
{
}

ASoundPlayerWrapper::~ASoundPlayerWrapper(void)
{
	delete m_pImpl;
	m_pImpl = NULL;
}

//	Only support .mp3 .wav .ogg file
bool ASoundPlayerWrapper::Init(A3DEngine* pA3DEngine, const TCHAR* szSoundFile)
{
	return m_pImpl->Init(pA3DEngine, szSoundFile);
}

void ASoundPlayerWrapper::Release()
{
	m_pImpl->Release();
}

bool ASoundPlayerWrapper::Play()
{
	return m_pImpl->Play();
}

void ASoundPlayerWrapper::Stop()
{
	m_pImpl->Stop();
}