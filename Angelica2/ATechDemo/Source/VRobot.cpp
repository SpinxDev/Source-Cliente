#include "VRobot.h"
#include <VGame.h>

VRobot::VRobot()
{
	m_pModelBody = NULL;
	m_vecVelocity = A3DVECTOR3(0.0f, 0.0f, 0.0f);
}
			
VRobot::~VRobot()
{
}	   

void VRobot::GroundTrace()
{
	FLOAT		vTerrainHeight;
	A3DVECTOR3	vecDelta = A3DVECTOR3(0.0f, -0.1f, 0.0f);
	A3DVECTOR3  vecPos = GetPos();
	A3DVECTOR3	vecCenterPos = GetCenterPos();
	
	//Check ground if we are falling;
	if( m_vecVelocity.y > 0.0f )
	{
		m_move.GroundType = TRACE_OBJECT_NULL;
		return;
	}

	vTerrainHeight = g_pGame->GetA3DEngine()->GetA3DWorld()->GetA3DTerrain()->GetHeight(vecPos);
	if( vTerrainHeight >= vecPos.y )
	{
		m_groundTrace.vNormal = m_pA3DEngine->GetA3DWorld()->GetA3DTerrain()->GetFaceNormal(vecPos);
		m_move.GroundTrace = m_groundTrace;
		m_move.GroundType = TRACE_OBJECT_TERRAIN;
	}
	else
	{
		if( g_pGame->GetA3DEngine()->GetA3DWorld()->AABBTrace(GetCenterPos(), m_obbShape.vExtents, vecDelta, 1.0f, &m_groundTrace, GetModelBody()) )
		{
			m_move.GroundTrace = m_groundTrace;
			m_move.GroundType = TRACE_OBJECT_ESPMODEL;
		}
		else
		{
			m_move.GroundTrace = m_groundTrace;
			m_move.GroundType = TRACE_OBJECT_NULL;
		}
	}
}

void VRobot::WalkMove(A3DVECTOR3& vecDelta)
{
	A3DMOVE move;
	A3DVECTOR3 vecDeltaClip;

	if( 1 )//m_move.GroundType == TRACE_OBJECT_NULL )
	{
		vecDeltaClip = vecDelta;
	}
	else
		TRA_ClipVelocity(vecDelta, m_groundTrace.vNormal, vecDeltaClip, 1.0f);
	
	ZeroMemory(&move, sizeof(move));
	move.vStart			= GetCenterPos();
	move.vVelocity		= vecDeltaClip;
	move.fTime			= 1.0f;
	move.Shape			= m_obbShape;
	move.fStepSize		= 1.0f;
	move.GroundTrace	= m_move.GroundTrace;
	move.GroundType		= m_move.GroundType;
	move.pWorld			= m_pA3DEngine->GetA3DWorld();
	move.pModel			= GetModelBody();

	TRA_StepSlideMove(&move, false);

	m_move = move;
}

void VRobot::AirMove(A3DVECTOR3& vecDelta)
{
	A3DMOVE move;		 

	ZeroMemory(&move, sizeof(move));
	move.vStart			= GetCenterPos();
	move.vVelocity		= vecDelta;
	move.fTime			= 1.0f;
	move.Shape			= m_obbShape;
	move.fStepSize		= 0.3f;
	move.GroundTrace	= m_move.GroundTrace;
	move.GroundType		= m_move.GroundType;
	move.pWorld			= m_pA3DEngine->GetA3DWorld();
	move.pModel			= GetModelBody();
		
	TRA_SlideMove(&move, false);

	m_move = move;
}

void VRobot::Move(FLOAT vTime)
{	
	A3DVECTOR3 vecDelta = m_vecVelocity * vTime;

	if( 0 )//m_move.GroundType == TRACE_OBJECT_NULL )
	{
		//vecDelta.y -= 9.8f / 60.0f;
		AirMove(vecDelta);	
	}
	else
	{
		WalkMove(vecDelta);
	}

	vecDelta = m_move.vDest - GetCenterPos();
	m_pModelBody->SetPos(m_pModelBody->GetPos() + vecDelta);

	A3DVECTOR3 vecPos = m_pModelBody->GetPos();
	if( m_vecVelocity.y <= 0.0f )
	{
		FLOAT vTerrainHeight = g_pGame->GetA3DEngine()->GetA3DWorld()->GetA3DTerrain()->GetHeight(vecPos);
		if( vTerrainHeight > vecPos.y )
		{
			vecPos.y = vTerrainHeight;
			m_pModelBody->SetPos(vecPos);
			m_vecVelocity.y = 0.0f;
		}
		else
		{
			vecDelta = A3DVECTOR3(0.0f, -0.1f, 0.0f);
			if( g_pGame->GetA3DEngine()->GetA3DWorld()->AABBTrace(GetCenterPos(), m_obbShape.vExtents, vecDelta, 1.0f, &m_groundTrace, GetModelBody()) )
			{
				m_move.GroundTrace = m_groundTrace;
				m_move.GroundType = TRACE_OBJECT_ESPMODEL;
				m_vecVelocity.y = 0.0f;
			}
			else
			{
				m_move.GroundTrace = m_groundTrace;
				m_move.GroundType = TRACE_OBJECT_NULL;
				m_vecVelocity.y -= 9.8f * vTime;
			}
		}
	}
	else
	{
		m_vecVelocity.y -= 9.8f * vTime;
	}

	m_vecVelocity.x *= 0.8f;
	m_vecVelocity.z *= 0.8f;

	RAYTRACE rayTrace;
	if( g_pGame->GetA3DEngine()->GetA3DWorld()->RayTrace(m_pModelBody->GetPos() + A3DVECTOR3(0.0f, BODY_HEIGHT / 2.0f, 0.0f), 
		A3DVECTOR3(0.0f, -50.0f, 0.0f), 1.0f, &rayTrace, m_pModelBody) )
	{
		m_pShadowGFX->SetPos(rayTrace.vPoint);
		m_pShadowGFX->SetDir(rayTrace.vNormal);
		A3DAABB aabb = m_pModelBody->GetModelAABB();
		m_pShadowGFX->SetScaleAll(1.5f, 1.5f, 1.5f);
		m_pShadowGFX->RecalculateAllFX();
	}
	else
	{
		m_pShadowGFX->SetPos(A3DVECTOR3(0.0f, -1000.0f, 0.0f));
		m_pShadowGFX->RecalculateAllFX();
	}
}					  

bool VRobot::Init(A3DDevice * pA3DDevice, char * szRobot)
{
	m_pA3DDevice = pA3DDevice;
	m_pA3DEngine = m_pA3DDevice->GetA3DEngine();
	
	m_obbShape.vExtents = A3DVECTOR3(BODY_WIDTH / 2.0f, BODY_HEIGHT / 2.0f, BODY_WIDTH / 2.0f);
	m_move.GroundType = TRACE_OBJECT_NULL;

	//First load it from file;
	if( !m_pA3DEngine->GetA3DModelMan()->LoadModelFile(szRobot, &m_pModelBody) )
		return false;
	
	m_bVisible = true;
	
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->LoadGFXFromFile("人的影子.gfx", NULL, NULL, false, &m_pShadowGFX) )
		return false;

	m_pShadowGFX->SetPos(m_pModelBody->GetPos());
	m_pShadowGFX->SetDirAndUp(m_pModelBody->GetDir(), m_pModelBody->GetUp());
	m_pShadowGFX->SetScaleAll(2.0f, 1.0f, 2.0f);
	m_pShadowGFX->RecalculateAllFX();
	m_pShadowGFX->Start(false);
	return true;
}

bool VRobot::Release()
{
	if( m_pModelBody )
	{
		//First remove the model from the A3DWorld;
		//m_pA3DEngine->GetA3DWorld()->DeleteObjectModel(m_pModelBody);
		//Then release this model;
		m_pA3DEngine->GetA3DModelMan()->ReleaseModel(m_pModelBody);
	}

	return true;
}

bool VRobot::Stand()
{
	return m_pModelBody->PlayActionByName("[原地站立静]", true);
}

bool VRobot::Run()
{
	return m_pModelBody->PlayActionByName("[前跑循环]", true);
}

bool VRobot::Stop()
{
	return m_pModelBody->PlayActionByName("[前跑到站]", true);
}

bool VRobot::Jump()
{
	return m_pModelBody->PlayActionByName("[前站到跳]", true);
}

bool VRobot::Crush()
{
	return m_pModelBody->PlayActionByName("[前跳到站]", true);
}

bool VRobot::TickAnimation()
{
	m_pModelBody->TickAnimation();
	return true;
}

bool VRobot::StepLeft()
{
	return m_pModelBody->PlayActionByName("[左横移]", true);
}

bool VRobot::StepRight()
{
	return m_pModelBody->PlayActionByName("[右横移]", true);
}

bool VRobot::RunBack()
{
	return m_pModelBody->PlayActionByName("[后跑循环]", true);
}

bool VRobot::Fire()
{
	m_pModelBody->PlayActionByName("[冷光剑]", true, false);
	return true;
}

bool VRobot::Render(A3DViewport * pCurrentViewport, bool bFirstView)
{
	/*
	A3DVECTOR3		vecLightDir;
	A3DCOLOR		clAmbient;
	A3DCOLORVALUE	clDirectional;

	if( !bFirstView )
	{
		if( g_pIBLScene )
		{
			A3DVECTOR3 vecPos = m_pModelBody->GetModelAABB().Center;
			A3DLIGHTPARAM dynamicLightParam;
			g_pIBLScene->GetLightGrid()->GetEquivalentLightInfo(vecPos, &vecLightDir, &clDirectional, &clAmbient, &dynamicLightParam);

			A3DLIGHTPARAM dirParam = g_pDirLight->GetLightparam();
			dirParam.Direction = vecLightDir;
			dirParam.Diffuse = clDirectional;
			g_pDirLight->SetLightParam(&dirParam);

			g_pA3DEngine->GetA3DDevice()->SetAmbient(clAmbient);
			g_pA3DEngine->GetA3DDevice()->SetTextureColorOP(0, g_pIBLScene->GetLightGrid()->GetColorOP());

			if( dynamicLightParam.Type != A3DLIGHT_FORCE_DWORD )
			{
				g_pDynamicLight->SetLightParam(&dynamicLightParam);
				g_pDynamicLight->TurnOn();
			}
			else
				g_pDynamicLight->TurnOff();
		}

		m_pModelBody->Render(pCurrentViewport, false);
		g_pA3DEngine->GetA3DDevice()->SetTextureColorOP(0, A3DTOP_MODULATE);
	}
	else // g_nCameraMode == 0 
	{
		if( g_pIBLScene )
		{
			A3DVECTOR3 vecPos = g_pModelGun->GetModelAABB().Center;
			A3DLIGHTPARAM dynamicLightParam;
			g_pIBLScene->GetLightGrid()->GetEquivalentLightInfo(vecPos, &vecLightDir, &clDirectional, &clAmbient, &dynamicLightParam);

			A3DLIGHTPARAM dirParam = g_pDirLight->GetLightparam();
			dirParam.Direction = vecLightDir;
			dirParam.Diffuse = clDirectional;
			g_pDirLight->SetLightParam(&dirParam);

			g_pA3DEngine->GetA3DDevice()->SetAmbient(clAmbient);
			g_pA3DEngine->GetA3DDevice()->SetTextureColorOP(0, g_pIBLScene->GetLightGrid()->GetColorOP());

			if( dynamicLightParam.Type != A3DLIGHT_FORCE_DWORD )
			{
				g_pDynamicLight->SetLightParam(&dynamicLightParam);
				g_pDynamicLight->TurnOn();
			}
			else
				g_pDynamicLight->TurnOff();
		}

		g_pModelGun->Render(pCurrentViewport, false);
		
		g_pA3DEngine->GetA3DDevice()->SetTextureColorOP(0, A3DTOP_MODULATE);
	}
	*/

	return true;
}
