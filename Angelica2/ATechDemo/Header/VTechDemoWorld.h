/*
 * FILE: VTechDemoWorld.h
 *
 * DESCRIPTION: class representing the technique demo world
 *
 * CREATED BY: Hedi, 2003/1/10
 *
 * HISTORY:
 *
 * Copyright (c) 2002-2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _VTECHDEMOWORLD_H_
#define _VTECHDEMOWORLD_H_

#include "VWorld.h"
#include "VRobot.h"

class VTechDemoWorld : public VWorld
{
private:
	A3DEngine *			m_pA3DEngine;
	A3DWorld *			m_pA3DWorld;
	A3DIBLScene *		m_pIBLScene;
	A3DModel *			m_pModelWeapon;
	A3DModel *			m_pModelGrenade;

	A2DSpriteBuffer *	m_pSpriteBuffers;
	A2DSprite *			m_pSpriteBullets;

	VRobot				m_theRobot;

protected:
	bool RenderMainWeapon(A3DViewport * pCurrentViewport);

public:
	VTechDemoWorld();
	~VTechDemoWorld();
			  
	virtual bool Restart();

	virtual bool Tick();
	virtual bool TickAnimation();

	virtual bool RenderBeforeEngineScene(A3DViewport * pCurrentViewport);
	virtual bool RenderAfterEngineScene(A3DViewport * pCurrentViewport);

	virtual bool Init(VGame * pGame);
	virtual bool Release();

	virtual bool OnMsg(const GAMEMSG& msg, DWORD dwUserData);

	bool Move(const A3DVECTOR3& vecDelta);
	bool TickControls();
};

#endif//_VTECHDEMOWORLD_H_