/*
* FILE: PropertyInterface.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2011/4/15
*
* HISTORY: 
*
* Copyright (c) 2011 Archosaur Studio, All Rights Reserved.
*/

#ifndef _PropertyInterface_H_
#define _PropertyInterface_H_

#include "A3DVector.h"
#include "AString.h"

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

namespace AudioEngine
{
	class EventSystem;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class APropertyInterface
//	说明：本类用于让PropDlg控件增加一些扩展功能
//	如果需要PropDlg控件支持诸如音频引擎的Project和Event浏览选择功能(AProperty中的WAY_AUDIOEVENT)
//	需要从本类派生，实现GetEventSystem接口，并将指针通过SetPropertyInterface进行设置
//	
///////////////////////////////////////////////////////////////////////////

class APropertyInterface
{

public:		//	Types

public:		//	Constructor and Destructor

	virtual ~APropertyInterface(void) = 0 {}

public:		//	Attributes

public:		//	Operations

	static APropertyInterface* GetPropertyInterface();
	static void SetPropertyInterface(APropertyInterface* pInterface);

	virtual AudioEngine::EventSystem* GetEventSystem() const = 0;
	//	Camera position is used
	virtual A3DVECTOR3 GetCamPos() const = 0;

protected:	//	Attributes

	static APropertyInterface* s_Interface;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	class ADefaultPIImpl
//
///////////////////////////////////////////////////////////////////////////

class ADefaultPIImpl : public APropertyInterface
{
public:
	ADefaultPIImpl(AudioEngine::EventSystem* pEventSystem)
		: m_pEventSystem(pEventSystem)
	{

	}

	virtual AudioEngine::EventSystem* GetEventSystem() const { return m_pEventSystem; }

private:
	AudioEngine::EventSystem* m_pEventSystem;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_PropertyInterface_H_

