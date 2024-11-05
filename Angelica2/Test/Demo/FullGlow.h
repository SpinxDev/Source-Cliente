/*
 * FILE: FullGlow.cpp
 *
 * DESCRIPTION: Fullscreen glow for the demo
 *
 * CREATED BY: Hedi, 2004/5/28
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _FULLGLOW_H_
#define _FULLGLOW_H_

#include "Global.h"
#include "Render.h"

bool FullGlow_Init();
bool FullGlow_Release();
bool FullGlow_BeginRender();
bool FullGlow_EndRender();
bool FullGlow_GlowPresent();

#endif//_FULLGLOW_H_