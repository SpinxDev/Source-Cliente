/*
 * FILE: AMImmWrapper.h
 *
 * DESCRIPTION: wrapper class of immersion corporation's ifc
 *
 * CREATED BY: Hedi, 2002/2/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMIMMWRAPPER_H_
#define _AMIMMWRAPPER_H_

#include "AMTypes.h"
#include "AMPlatform.h"
#include "AMImmEffect.h"

#ifndef IMMWRAPPER_EXPORTS
#define CLASS_DESLSPEC __declspec(dllimport)
#else
#define CLASS_DESLSPEC __declspec(dllexport)
#endif

class CImmDevice;
class CImmProject;
					 
class CLASS_DESLSPEC AMImmWrapper
{
private:
	char			m_szProjectName[MAX_PATH];
	CImmDevice *	m_pImmDevice;
	CImmProject *	m_pImmProject;

protected:
public:
	AMImmWrapper();
	~AMImmWrapper();

	bool Init(HINSTANCE hInstance, HWND hWnd, char * szProjectName, char * szLogDir);
	bool Release();
	
	bool LoadImmEffect(char * szEffectName, AMImmEffect ** ppImmEffect);
	bool ReleaseImmEffect(AMImmEffect *& pImmEffect);

	bool PlayImmEffect(char * szEffectName);
};

typedef class AMImmWrapper * PAMImmWrapper;
#endif//_AMIMMWRAPPER_H_