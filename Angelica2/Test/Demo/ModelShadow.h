    /*
 * FILE: ModelShadow.h
 *
 * DESCRIPTION: Shadow map for demo
 *
 * CREATED BY: Hedi, 2004/4/19
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _MODELSHADOW_H_
#define _MODELSHADOW_H_

bool Shadow_Init(const A3DVECTOR3& vecLightDir);
bool Shadow_Release();

bool Shadow_Render(A3DSkinModel * pModel, A3DViewport * pViewport, float vDis, float vFov, const char * szFrameName);

#endif//_MODELSHADOW_H_
