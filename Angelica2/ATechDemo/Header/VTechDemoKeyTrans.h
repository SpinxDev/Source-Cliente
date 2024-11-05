/*
 * FILE: VTechDemoKeyTrans.h
 *
 * DESCRIPTION: class which take translate a keyboard message into technique demo message
 *
 * CREATED BY: Hedi, 2003/1/10
 *
 * HISTORY:
 *
 * Copyright (c) 2002-2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _VTECHDEMOKEYTRANS_H_
#define _VTECHDEMOKEYTRANS_H_

#include <Windows.h>
#include "VKeyBoardTranslator.h"

class VTechDemoKeyTranslator : public VKeyBoardTranslator
{
private:
protected:
public:
	VTechDemoKeyTranslator();
	~VTechDemoKeyTranslator();

	bool Translate(float vDeltaTime);
};

#endif//_VTECHDEMOKEYTRANS_H_
