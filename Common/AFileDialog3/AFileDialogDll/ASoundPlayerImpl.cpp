/*
* FILE: ASoundPlayerImpl.cpp
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
#include "ASoundPlayerImpl.h"
#include "AFTaskPlaySound.h"

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
//	Implement ASoundPlayerImpl
//	
///////////////////////////////////////////////////////////////////////////

ASoundPlayerImpl::ASoundPlayerImpl(void)
: m_pAMSoundEngine(NULL)
, m_pAMSoundBufferMan(NULL)
, m_pAFSoundPlayItem(new AFSoundPlayItem)
{
}

ASoundPlayerImpl::~ASoundPlayerImpl(void)
{
	Release();

	delete m_pAFSoundPlayItem;
	m_pAFSoundPlayItem = NULL;
}


bool ASoundPlayerImpl::Init(A3DEngine* pA3DEngine, const TCHAR* szSoundFile)
{
	if (!pA3DEngine || !szSoundFile || !szSoundFile[0])
		return false;

	m_pAMSoundEngine = pA3DEngine->GetAMSoundEngine();
	m_pAMSoundBufferMan = m_pAMSoundEngine->GetAMSoundBufferMan();

	m_strSoundFile = AC2AS(szSoundFile);
	
	return true;
}

void ASoundPlayerImpl::Release()
{
	m_pAFSoundPlayItem->Release(m_pAMSoundBufferMan);
}

bool ASoundPlayerImpl::Play()
{
	int iSoundType = is_support_sound(AS2AC(m_strSoundFile));
	if (iSoundType == AF_SUPPORT_SOUND_UNKNOWN)
		return false;

	if (iSoundType == AF_SUPPORT_SOUND_WAV)
	{
		m_pAFSoundPlayItem->LoadWavSound(m_pAMSoundBufferMan, m_strSoundFile);
	}
	else if (iSoundType == AF_SUPPORT_SOUND_OGG
		|| iSoundType == AF_SUPPORT_SOUND_MP3)
	{
		m_pAFSoundPlayItem->LoadStreamSound(m_pAMSoundEngine, m_strSoundFile);
	}

	return true;
}

void ASoundPlayerImpl::Stop()
{
	m_pAFSoundPlayItem->StopSound();
}

