/*
 * FILE: AMImmEffect.h
 *
 * DESCRIPTION: wrapper class for force feedback effect
 *
 * CREATED BY: Hedi, 2002/2/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMIMMEFFECT_H_
#define _AMIMMEFFECT_H_

#include "AMTypes.h"
#include "AMPlatform.h"

#ifndef IMMWRAPPER_EXPORTS
#define CLASS_DESLSPEC __declspec(dllimport)
#else
#define CLASS_DESLSPEC __declspec(dllexport)
#endif

class CImmCompoundEffect;

class CLASS_DESLSPEC AMImmEffect
{
private:
	CImmCompoundEffect *	m_pImmEffect;

protected:
public:
	AMImmEffect();
	~AMImmEffect();

	bool Init(CImmCompoundEffect * pImmEffect);
	bool Release();

	bool Start();
	bool Stop();

	inline CImmCompoundEffect * GetImmEffect() { return m_pImmEffect; }
};

typedef class AMImmEffect * PAMImmEffect;
#endif//_AMIMMEFFECT_H_