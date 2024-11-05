/*
 * FILE: AMImmPI.h
 *
 * DESCRIPTION: private interface for wrapper class for force feedback effect
 *
 * CREATED BY: Hedi, 2002/2/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMIMMPI_H_
#define _AMIMMPI_H_

#include <AC.h>

extern ALog * g_pAMIMMErrLog;
#define AMIMMERRLOG(fmt) {if(g_pAMIMMErrLog) g_pAMIMMErrLog->Log fmt;}

#endif//_AMIMMPI_H_
