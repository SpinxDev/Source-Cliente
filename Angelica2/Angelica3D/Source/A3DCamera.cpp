#include "A3DCamera.h"
#include "A3DDevice.h"
#include "A3DFuncs.h"

A3DCamera::A3DCamera()
{ 
	m_dwClassID = A3D_CID_CAMERA;
}

//	fov is in radian!
bool A3DCamera::Init(A3DDevice* pA3DDevice, FLOAT vFov, FLOAT vFront, FLOAT vBack, FLOAT vRatio)
{
	if (!A3DCameraBase::Init(pA3DDevice, vFront, vBack))
		return false;

	m_vFOV		= vFov;
	m_vFOVSet	= vFov;
	m_vRatio	= vRatio;

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, vFov, vRatio, vFront, vBack);
	m_matProjectTM = *(A3DMATRIX4*) &matProj;
	a3d_InverseTM(m_matProjectTM, &m_matInvProjectTM);

	m_matPostProjectTM.Identity();
	
	//	Create frustums
	m_ViewFrustum.Init(6);
	m_WorldFrustum.Init(6);
	CreateViewFrustum();

	return true;
}

void A3DCamera::Release()
{
	A3DCameraBase::Release();
}

bool A3DCamera::UpdateProjectTM()
{
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, m_vFOV, m_vRatio, m_vFront, m_vBack);
	m_matProjectTM = *(A3DMATRIX4*) &matProj;
	a3d_InverseTM(m_matProjectTM, &m_matInvProjectTM);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseTM(m_matVPTM, &m_matInvVPTM);

	CreateViewFrustum();
	return true;
}

bool A3DCamera::Active()
{
	Update();
	return A3DCameraBase::Active();
}

bool A3DCamera::Update()
{
	if (m_bMirrored)
	{
		return UpdateForMirroredCamera();
	}

	if (m_vFOVSet != m_vFOV)
	{
		if (m_vFOV > m_vFOVSet)
		{
			m_vFOV -= 0.01f;	//	Zoom in;
			if (m_vFOV < m_vFOVSet)
				m_vFOV = m_vFOVSet;
		}
		else
		{
			m_vFOV += 0.01f;	//	Zoom out;
			if (m_vFOV > m_vFOVSet)
				m_vFOV = m_vFOVSet;
		}

		UpdateProjectTM();
	}

	return true;
}

//	Create view frustum in view coordinates
bool A3DCamera::CreateViewFrustum()
{
	A3DPLANE *p1, *p2;

	//	Top and bottom clip planes
	float c, s, fAngle = m_vFOV * 0.5f;

	c = (float)cos(fAngle);
	s = (float)sin(fAngle);

	p1 = m_ViewFrustum.GetPlane(A3DFrustum::VF_TOP);
	p2 = m_ViewFrustum.GetPlane(A3DFrustum::VF_BOTTOM);

	p1->vNormal.Set(0.0f, -c, s);
	p2->vNormal.Set(0.0f, c, s);

	p1->fDist = p2->fDist = 0.0f;

	//	Left and right clip planes
	fAngle = (float)(tan(fAngle) * m_vRatio);

	c = (float)cos(atan(fAngle));
	s = (float)sin(atan(fAngle));

	p1 = m_ViewFrustum.GetPlane(A3DFrustum::VF_LEFT);
	p2 = m_ViewFrustum.GetPlane(A3DFrustum::VF_RIGHT);

	p1->vNormal.Set(c, 0.0f, s);
	p2->vNormal.Set(-c, 0.0f, s);

	p1->fDist = p2->fDist = 0.0f;

	//	Near and Far clipping plane
	p1 = m_ViewFrustum.GetPlane(A3DFrustum::VF_NEAR);
	p2 = m_ViewFrustum.GetPlane(A3DFrustum::VF_FAR);

	p1->vNormal.Set(0.0f, 0.0f, 1.0f);
	p2->vNormal.Set(0.0f, 0.0f, -1.0f);

	p1->fDist = p2->fDist = 0.0f;

	//	Update frustum in world coordinates
	UpdateWorldFrustum();

	return true;
}

//	Update view frustum and translate it into world coordinates
bool A3DCamera::UpdateWorldFrustum()
{
	A3DSPLANE *pw, *pv;

	//	Translate left plane
	pw = m_WorldFrustum.GetPlane(A3DFrustum::VF_LEFT);
	pv = m_ViewFrustum.GetPlane(A3DFrustum::VF_LEFT);

	pw->vNormal	= a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist	= DotProduct(pw->vNormal, m_vecPos);
	pw->byType	= A3DSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate right plane
	pw = m_WorldFrustum.GetPlane(A3DFrustum::VF_RIGHT);
	pv = m_ViewFrustum.GetPlane(A3DFrustum::VF_RIGHT);

	pw->vNormal	= a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist	= DotProduct(pw->vNormal, m_vecPos);
	pw->byType	= A3DSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate top plane
	pw = m_WorldFrustum.GetPlane(A3DFrustum::VF_TOP);
	pv = m_ViewFrustum.GetPlane(A3DFrustum::VF_TOP);

	pw->vNormal	= a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist	= DotProduct(pw->vNormal, m_vecPos);
	pw->byType	= A3DSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate bottom plane
	pw = m_WorldFrustum.GetPlane(A3DFrustum::VF_BOTTOM);
	pv = m_ViewFrustum.GetPlane(A3DFrustum::VF_BOTTOM);

	pw->vNormal	= a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist	= DotProduct(pw->vNormal, m_vecPos);
	pw->byType	= A3DSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate near plane
	pw = m_WorldFrustum.GetPlane(A3DFrustum::VF_NEAR);

	A3DVECTOR3 vPos = m_vecPos + m_vecDir * m_vFront;

	pw->vNormal	= m_vecDir;
	pw->fDist	= DotProduct(pw->vNormal, vPos);
	pw->byType	= A3DSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Transform far plane
	pw = m_WorldFrustum.GetPlane(A3DFrustum::VF_FAR);

	vPos = m_vecPos + m_vecDir * m_vBack;

	pw->vNormal	= -m_vecDir;
	pw->fDist	= DotProduct(pw->vNormal, vPos);
	pw->byType	= A3DSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	return true;
}

//	Set / Get projection parameters
bool A3DCamera::SetProjectionParam(FLOAT fFOV, FLOAT fFront, FLOAT fBack, FLOAT fRatio)
{
	if (fFOV > 0.0f)
	{
		m_vFOV = fFOV;
		m_vFOVSet = m_vFOV; // we will lock this when force set projection param
	}

	if (fRatio > 0.0f)
		m_vRatio = fRatio;

	return SetZFrontAndBack(fFront, fBack, true);
}
