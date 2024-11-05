/*
 * FILE: AMImmWrapper.cpp
 *
 * DESCRIPTION: wrapper class of immersion corporation's ifc
 *
 * CREATED BY: Hedi, 2002/2/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AMTypes.h"
#include "AMPlatform.h"
#include "AMImmEffect.h"
#include "AMImmWrapper.h"
#include "AMImmPI.h"

#define DIRECTINPUT_VERSION 0x0800
#include <ifc.h>

ALog * g_pAMIMMErrLog = NULL;

AMImmWrapper::AMImmWrapper()
{
	m_pImmDevice = NULL;
	m_pImmProject = NULL;
}

AMImmWrapper::~AMImmWrapper()
{
}

bool AMImmWrapper::Init(HINSTANCE hInstance, HWND hWnd, char * szProjectName, char * szLogDir)
{
	g_pAMIMMErrLog = new ALog();
	if( NULL == g_pAMIMMErrLog )
		return false; // Not enough memory!

	g_pAMIMMErrLog->SetLogDir(szLogDir);
	g_pAMIMMErrLog->Init("Imm.log", "Angelica Media ImmWrapper Log");

	strncpy(m_szProjectName, szProjectName, MAX_PATH);

	m_pImmDevice = CImmDevice::CreateDevice(GetModuleHandle(NULL), hWnd);
	if( !m_pImmDevice )
	{
		AMIMMERRLOG(("AMImmWrapper::Init(), Error Creating ImmDevice"));
		return false;
	}
	
	// Open force resource file
	m_pImmProject = new CImmProject();
	if( NULL == m_pImmProject )
	{
		AMIMMERRLOG(("AMImmWrapper::Init(), Not enough memory!"));
		if( m_pImmDevice )
		{
			delete m_pImmDevice;
			m_pImmDevice = NULL;
		}
		return false;
	}

	if( !m_pImmProject->OpenFile(szProjectName, m_pImmDevice) )
	{
		AMIMMERRLOG(("AMImmWrapper::Init(), Error Opening the project file [%s]", szProjectName));
		if( m_pImmDevice )
		{
			delete m_pImmDevice;
			m_pImmDevice = NULL;
		}
		if( m_pImmProject )
		{
			delete m_pImmProject;
			m_pImmProject = NULL;
		}
		return false;
	}

	return true;
}

bool AMImmWrapper::Release()
{
	if( m_pImmProject )
	{
		m_pImmProject->Close();
		delete m_pImmProject;
		m_pImmProject = NULL;
	}
	if( m_pImmDevice )
	{
		delete m_pImmDevice;
		m_pImmDevice = NULL;
	}

	if( g_pAMIMMErrLog )
	{
		g_pAMIMMErrLog->Release();
		delete g_pAMIMMErrLog;
		g_pAMIMMErrLog = NULL;
	}
	return true;
}

bool AMImmWrapper::LoadImmEffect(char * szEffectName, AMImmEffect ** ppImmEffect)
{
	*ppImmEffect = NULL;

	if( !m_pImmDevice )
		return true;

	CImmCompoundEffect * pNewCompoundEffect;
	pNewCompoundEffect = m_pImmProject->CreateEffect(szEffectName, m_pImmDevice);
	if( NULL == pNewCompoundEffect )
	{
		// Not using an force feedback effect;
		return true;
	}
	
	AMImmEffect * pNewEffect;
	pNewEffect = new AMImmEffect();
	if( NULL == pNewEffect )
	{
		AMIMMERRLOG(("AMImmWarpper::LoadImmEffect(), Not enough memory!"));
		delete pNewCompoundEffect;
		return false;
	}

	if( !pNewEffect->Init(pNewCompoundEffect) )
	{
		AMIMMERRLOG(("AMImmWrapper::LoadImmEffect(), Init an AMImmEffect object fail!"));
		delete pNewCompoundEffect;
		return false;
	}

	*ppImmEffect = pNewEffect;
	return true;
}

bool AMImmWrapper::ReleaseImmEffect(AMImmEffect *& pImmEffect)
{
	if( !m_pImmDevice )
		return true;

	m_pImmProject->DestroyEffect(pImmEffect->GetImmEffect());

	pImmEffect->Release();
	delete pImmEffect;
	pImmEffect = NULL;

	return true;
}

bool AMImmWrapper::PlayImmEffect(char * szEffectName)
{
	if( !m_pImmDevice )
		return true;

	m_pImmProject->Start(szEffectName);

	return true;
}
