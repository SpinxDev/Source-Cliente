/*
 * FILE: VTechDemo.h
 *
 * DESCRIPTION: An tech demo application object for Angelica Game Engine
 *
 * CREATED BY: Hedi, 2003/1/10
 *
 * HISTORY:
 *
 * Copyright (c) 2002-2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _VTECHDEMO_H_
#define _VTECHDEMO_H_

#include "VGame.h"

class VTechDemo : public VGame
{
private:
	//AMVideoClip		m_videoClip;

protected:

public:
	virtual bool Tick();
	virtual bool RenderContent(A3DViewport * pCurrentViewport);

	virtual bool InitWorld();
	virtual bool ReleaseWorld();

	virtual bool ShowSplash();
	virtual bool ShowEnd();

	VTechDemo();
	~VTechDemo();
};

typedef VTechDemo * PVTechDemo;
#endif

