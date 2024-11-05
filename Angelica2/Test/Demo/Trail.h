 /*
 * FILE: Trail.h
 *
 * DESCRIPTION: Trail for knife and something like that
 *
 * CREATED BY: Hedi, 2004/4/17
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _TRAIL_H_
#define _TRAIL_H_

#define MAX_LIFE		1000

bool Trail_Init(const char * szSegTex, A3DVECTOR3 vecPt1, A3DVECTOR3 vecPt2);
bool Trail_Release();
bool Trail_Add(const A3DMATRIX4& matNow);
bool Trail_Tick(int nDeltaTime);
bool Trail_Render(A3DViewport * pCurrentViewport);

#endif//_TRAIL_H_

