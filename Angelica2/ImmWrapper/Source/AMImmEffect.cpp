/*
 * FILE: AMImmEffect.cpp
 *
 * DESCRIPTION: wrapper class for force feedback effect
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

#define DIRECTINPUT_VERSION 0x0800
#include <ifc.h>

AMImmEffect::AMImmEffect()
{
	m_pImmEffect = NULL;
}

AMImmEffect::~AMImmEffect()
{
}

bool AMImmEffect::Init(CImmCompoundEffect * pImmEffect)
{
	m_pImmEffect = pImmEffect;

	return true;
}

bool AMImmEffect::Release()
{
	m_pImmEffect = NULL;
	
	return true;
}

bool AMImmEffect::Start()
{
	if( m_pImmEffect )
	{
		m_pImmEffect->Start();
	}

	return true;
}

bool AMImmEffect::Stop()
{
	if( m_pImmEffect )
	{
		m_pImmEffect->Stop();
	}

	return true;
}