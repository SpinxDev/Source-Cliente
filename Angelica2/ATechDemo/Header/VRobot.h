#include <A3D.h>
#include <AM.h>

#ifndef _VROBOT_H_
#define _VROBOT_H_

#define HUMAN_SIGHT

#ifdef HUMAN_SIGHT
	#define BODY_HEIGHT		1.8f
	#define BODY_WIDTH		0.5f
#else
	#define BODY_HEIGHT		6.0f
	#define BODY_WIDTH		3.0f
#endif

class VRobot
{
private:
	OBBSHAPE	m_obbShape;
	
	A3DDevice * m_pA3DDevice;
	A3DEngine * m_pA3DEngine;

	A3DGraphicsFX * m_pShadowGFX;

	//Moving control;
	AABBTRACE	m_groundTrace;
	A3DMOVE		m_move;
	A3DVECTOR3	m_vecVelocity;

	FLOAT		m_vFallSpeed;
public:
	//Visual control;
	bool		m_bVisible;
	A3DModel *  m_pModelBody;
	
public:
	VRobot();
	~VRobot();

	bool Init(A3DDevice * pA3DDevice, char * szRobot);
	bool Release();

	bool Render(A3DViewport * pCurrentViewport, bool bFirstView);

	inline void SetPos(A3DVECTOR3 vecPos) { m_pModelBody->SetPos(vecPos); }
	inline void SetDirAndUp(A3DVECTOR3 vecDir, A3DVECTOR3 vecUp) { m_pModelBody->SetDirAndUp(vecDir, vecUp); }
	inline A3DVECTOR3 GetPos() { return m_pModelBody->GetPos(); }
	inline A3DVECTOR3 GetCenterPos() { return m_pModelBody->GetPos() + A3DVECTOR3(0.0f, BODY_HEIGHT / 2.0f, 0.0f); }
	inline A3DVECTOR3 GetEyePos() { return m_pModelBody->GetPos() + A3DVECTOR3(0.0f, BODY_HEIGHT, 0.0f); }
	inline A3DVECTOR3 GetDir() { return m_pModelBody->GetDir(); }					    
	inline void SetVisibility(bool bVisible) { m_pModelBody->SetVisibility(bVisible); }
	inline void Turn(FLOAT vDeltaRad) { m_pModelBody->RotateYRelative(vDeltaRad); }
	inline A3DModel * GetModelBody() { return m_pModelBody; }
	
	//Moving control;
	inline void SetVelocity(const A3DVECTOR3& vecVelocity) { m_vecVelocity = vecVelocity; }
	inline void SetVelocityXZ(float vX, float vZ) { m_vecVelocity.x = vX; m_vecVelocity.z = vZ; }
	inline void SetVelocityY(float vY) { m_vecVelocity.y = vY; }
	void Move(FLOAT vTime);
	void GroundTrace();
	void AirMove(A3DVECTOR3& vecDelta);
	void WalkMove(A3DVECTOR3& vecDelta);

	bool TickAnimation();
	
	//Animation Actions;
	bool Stand();
	bool Walk();
	bool WalkBack();
	bool WalkStop();

	bool Run();
	bool RunBack();
	bool Stop();
	bool Jump();
	bool Crush();
	bool Fire();
	
	bool StepLeft();
	bool StepRight();

	bool IsOnGround() { return m_move.GroundType != TRACE_OBJECT_NULL; }
	bool IsOnTerrain() { return m_move.GroundType == TRACE_OBJECT_TERRAIN; }
	void JumpOffGround() { m_move.GroundType = TRACE_OBJECT_NULL; }
};

#endif