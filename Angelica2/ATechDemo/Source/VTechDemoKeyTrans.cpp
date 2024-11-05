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

#include "VTechDemoKeyTrans.h"

#include "VGame.h"
#include "VMsgQueue.h"
#include "VTechDemoMsg.h"
#include "VKeyBoardDevice.h"

VTechDemoKeyTranslator::VTechDemoKeyTranslator()
{
}

VTechDemoKeyTranslator::~VTechDemoKeyTranslator()
{
}

bool VTechDemoKeyTranslator::Translate(float vDeltaTime)
{
	// First translate common messages;
	if( !VKeyBoardTranslator::Translate(vDeltaTime) )
		return false;

	if( !m_pKeyBoardDevice || !m_pMsgQueue )
		return true;

	POINT			ptCurPos = m_pKeyBoardDevice->GetMousePosNow();
	POINT			ptOldPos = m_pKeyBoardDevice->GetMousePosLast();

	m_pKeyBoardDevice->SetCursorPosition(512, 384);
	GAMEMSG			msg;
	
	// First rotate the camera;
	if( 1 ) // !bFreeCursor
	{
		float vParam;

		msg.dwMsgID = GAMEMSG_DEGDELTA;
		vParam = (ptCurPos.x - ptOldPos.x) * 0.2f;
		msg.dwParam1 = *(DWORD *)&vParam;
		m_pMsgQueue->MsgQueueIn(msg);

		msg.dwMsgID = GAMEMSG_PITCHDELTA;
		vParam = (ptOldPos.y - ptCurPos.y) * 0.2f;
		msg.dwParam1 = *(DWORD *)&vParam;
		m_pMsgQueue->MsgQueueIn(msg);
	}

	if( m_pKeyBoardDevice->IsKeyPressedNow(VK_UP) || m_pKeyBoardDevice->IsKeyPressedNow('W') )
	{
		float	vParam = 4.0f;

		if( m_pKeyBoardDevice->IsKeyPressedNow(VK_SHIFT) )
			vParam *= 3.0f;

		msg.dwMsgID	= GAMEMSG_FORWARD;
		msg.dwParam1 = *(DWORD *)&vParam;
		m_pMsgQueue->MsgQueueIn(msg);
	}

	if( m_pKeyBoardDevice->IsKeyPressedNow(VK_DOWN) || m_pKeyBoardDevice->IsKeyPressedNow('S') )
	{
		float	vParam = 4.0f;

		if( m_pKeyBoardDevice->IsKeyPressedNow(VK_SHIFT) )
			vParam *= 3.0f;

		msg.dwMsgID	= GAMEMSG_BACK;
		msg.dwParam1 = *(DWORD *)&vParam;
		m_pMsgQueue->MsgQueueIn(msg);
	}

	if( m_pKeyBoardDevice->IsKeyPressedNow(VK_LEFT) || m_pKeyBoardDevice->IsKeyPressedNow('A') )
	{
		float	vParam = 4.0f;

		if( m_pKeyBoardDevice->IsKeyPressedNow(VK_SHIFT) )
			vParam *= 3.0f;

		msg.dwMsgID	= GAMEMSG_LEFT;
		msg.dwParam1 = *(DWORD *)&vParam;
		m_pMsgQueue->MsgQueueIn(msg);
	}

	if( m_pKeyBoardDevice->IsKeyPressedNow(VK_RIGHT) || m_pKeyBoardDevice->IsKeyPressedNow('D') )
	{
		float	vParam = 4.0f;

		if( m_pKeyBoardDevice->IsKeyPressedNow(VK_SHIFT) )
			vParam *= 3.0f;

		msg.dwMsgID	= GAMEMSG_RIGHT;
		msg.dwParam1 = *(DWORD *)&vParam;
		m_pMsgQueue->MsgQueueIn(msg);
	}

	if( m_pKeyBoardDevice->IsKeyPressedNow(VK_PRIOR) )
	{
		float	vParam = 0.1f;

		if( m_pKeyBoardDevice->IsKeyPressedNow(VK_SHIFT) )
			vParam *= 10.0f;

		msg.dwMsgID	= GAMEMSG_CLIBM;
		msg.dwParam1 = *(DWORD *)&vParam;
		m_pMsgQueue->MsgQueueIn(msg);
	}

	if( m_pKeyBoardDevice->IsKeyPressedNow(VK_NEXT) )
	{
		float	vParam = 0.1f;

		if( m_pKeyBoardDevice->IsKeyPressedNow(VK_SHIFT) )
			vParam *= 10.0f;

		msg.dwMsgID	= GAMEMSG_DIVE;
		msg.dwParam1 = *(DWORD *)&vParam;
		m_pMsgQueue->MsgQueueIn(msg);
	}

	if( m_pKeyBoardDevice->IsKeyPressedNow(VK_RETURN) && !
		m_pKeyBoardDevice->IsKeyPressedLast(VK_RETURN) )
	{
		msg.dwMsgID = GAMEMSG_EFFECT_EXPLODE;
		m_pMsgQueue->MsgQueueIn(msg);
	}

	if( m_pKeyBoardDevice->IsKeyPressedNow(VK_SPACE) )
	{
		msg.dwMsgID = GAMEMSG_JUMP;
		m_pMsgQueue->MsgQueueIn(msg);
	}
	
	if( m_pKeyBoardDevice->IsKeyPressedNow(VK_LBUTTON) )
	{
		msg.dwMsgID = GAMEMSG_FIRE;
		m_pMsgQueue->MsgQueueIn(msg);
	}

	return true;
}