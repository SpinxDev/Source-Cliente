/**********************************************************************
 *<
	FILE: SkinMap.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __SKINMAP__H
#define __SKINMAP__H

#pragma once

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "modstack.h"

#include "utilapi.h"

#if MAX_RELEASE >= 4000
#include "iskin.h"
#endif

extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

#endif // __SKINMAP__H
