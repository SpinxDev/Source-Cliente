/*
* FILE: APhysXCharCtrlerCallBackAgnet.h
*
* DESCRIPTION: 
*	This is a agent class used for the callback of character controller.
*
*
* CREATED BY: Yang Liu, 2009/11/17
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_CALLBACK_AGENT_H_
#define _APHYSX_CHARACTER_CONTROLLER_CALLBACK_AGENT_H_

#include "..\..\APhysXNxCharacter_Public\include\NxController.h"

class APhysXCCHitReportAgent : public NxUserControllerHitReport
{
//----------------------------------------------------------
// singleton object
	APHYSX_DECLARE_SINGLETON(APhysXCCHitReportAgent)
//----------------------------------------------------------

public:
	virtual NxControllerAction onShapeHit(const NxControllerShapeHit& hit);
	virtual NxControllerAction onControllerHit(const NxControllersHit& hit);
};

#endif