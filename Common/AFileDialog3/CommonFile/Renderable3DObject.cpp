// Renderable3DObject.cpp: implementation of the Renderable3DObject class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
#include "Renderable3DObject.h"

#include "EC_Model.h"
#include "A3DGFXExMan.h"
#include "Render.h"
#include "afi.h"


extern char g_szAppPath[];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderable3DObject::CRenderable3DObject()
{

	m_pGfxEx = 0;
	m_pECModel = 0;
	//m_pELBuilding = 0;
	m_nObjectType = RENDERABLE_OBJECT_3D;
}

CRenderable3DObject::~CRenderable3DObject()
{

}

A3DMATRIX4 CRenderable3DObject::GetAbsoluteTM()
{
	A3DMATRIX4 matX,matY,matZ;
	matX.RotateX(m_vRotate.x);
	matY.RotateY(m_vRotate.y);
	matZ.RotateZ(m_vRotate.z);
	return matZ * matY * matX;
}

A3DVECTOR3 CRenderable3DObject::GetDefaultCamaraPos()
{
	float fRadius = 0.0f;
	if(m_pECModel)
	{
		A3DAABB aabb = m_pECModel->GetA3DSkinModel()->GetModelAABB();
		if(aabb.Extents.x > fRadius) fRadius = aabb.Extents.x;
		if(aabb.Extents.y > fRadius) fRadius = aabb.Extents.y;
		if(aabb.Extents.z > fRadius) fRadius = aabb.Extents.z;
		
		return A3DVECTOR3(aabb.Center.x,aabb.Center.y,fRadius*3);
	}

	if(m_pGfxEx)
	{
		A3DAABB aabb = m_pGfxEx->GetAABB();
		if(aabb.Extents.x > fRadius) fRadius = aabb.Extents.x;
		if(aabb.Extents.y > fRadius) fRadius = aabb.Extents.y;
		if(aabb.Extents.z > fRadius) fRadius = aabb.Extents.z;
		
		return A3DVECTOR3(aabb.Center.x,aabb.Center.y,fRadius*3);
	}
	/*
	if(m_pELBuilding)
	{
		A3DAABB aabb = m_pELBuilding->GetModelAABB();
		if(aabb.Extents.x > fRadius) fRadius = aabb.Extents.x;
		if(aabb.Extents.y > fRadius) fRadius = aabb.Extents.y;
		if(aabb.Extents.z > fRadius) fRadius = aabb.Extents.z;
		
		return A3DVECTOR3(aabb.Center.x,aabb.Center.y,fRadius*5);
	}*/

	return A3D::g_vOrigin;
}

void CRenderable3DObject::Render(A3DViewport *pA3dViewPort)
{
	
	if(FALSE)
	{
		CRender::GetInstance()->GetA3DDevice()->SetAlphaBlendEnable(false);
		CRender::GetInstance()->GetA3DDevice()->SetAlphaTestEnable(false);
	}else 
	{
		CRender::GetInstance()->GetA3DDevice()->SetAlphaBlendEnable(true);
		CRender::GetInstance()->GetA3DDevice()->SetAlphaTestEnable(true);
	}
	
	if(m_pECModel)
	{
		A3DLIGHTPARAM lightParams = CRender::GetInstance()->GetDirLight()->GetLightparam();
		A3DSkinModel::LIGHTINFO LightInfo;
		LightInfo.colAmbient	= CRender::GetInstance()->GetA3DDevice()->GetAmbientColor();
		LightInfo.vLightDir		= lightParams.Direction;//Direction of directional light
		LightInfo.colDirDiff	= lightParams.Diffuse;//Directional light's diffuse color
		LightInfo.colDirSpec	= lightParams.Specular;//	Directional light's specular color
		LightInfo.bPtLight		= false;//	false, disable dynamic point light,
		m_pECModel->GetA3DSkinModel()->SetLightInfo(LightInfo);
		m_pECModel->Render(pA3dViewPort);	
		//A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		//pWireCollector->AddSphere(m_pECModel->GetPos(),2,A3DCOLORRGB(255,0,0));
	}
	
	/*
	if(m_pELBuilding)
	{
		//A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		//pWireCollector->AddSphere(m_pELBuilding->GetPos(),10,A3DCOLORRGB(255,0,0));
		m_pELBuilding->Render(pA3dViewPort);
	}*/

	if(m_pGfxEx && CRender::GetInstance()->GetA3DEngine()->GetActiveCamera())
		AfxGetGFXExMan()->RegisterGfx(m_pGfxEx);
}

void CRenderable3DObject::Tick(DWORD dwTimeDelta)
{
	
	A3DMATRIX4 matRotate,matX,matY,matZ,matTrans;
	matX.RotateX(m_vRotate.x);
	matY.RotateY(m_vRotate.y);
	matZ.RotateZ(m_vRotate.z);
	matRotate = matZ * matY * matX;
	A3DVECTOR3 vDir = A3D::g_vAxisZ;
	A3DVECTOR3 vUp = A3D::g_vAxisY;
	vDir = matRotate * vDir;
	vUp = matRotate * vUp;
	
	if(m_pGfxEx && CRender::GetInstance()->GetA3DEngine()->GetActiveCamera())
	{
		m_pGfxEx->SetParentTM(matRotate);
		m_pGfxEx->TickAnimation(dwTimeDelta);
		
		GFX_STATE states = m_pGfxEx->GetState();
		if(1 != states) 
			m_pGfxEx->Start(true);		
	}
	
	if(m_pECModel)
	{
		m_pECModel->SetPos(A3DVECTOR3(0,0,0));
		m_pECModel->SetDirAndUp(vDir,vUp);
		m_pECModel->Tick(dwTimeDelta);		
	}

	/*	
	if(m_pELBuilding)
	{
		m_pELBuilding->SetCollideOnly(false);
		m_pELBuilding->SetPos(A3DVECTOR3(0,0,0));
		m_pELBuilding->SetDirAndUp(vDir,vUp);
		m_pELBuilding->Tick(dwTimeDelta);
	}*/
}

void CRenderable3DObject::ResetScale()
{
	m_vRotate = A3DVECTOR3(0,0,0);
}

void CRenderable3DObject::Release()
{
	
	if(m_pGfxEx)
	{
		m_pGfxEx->Release();
		delete m_pGfxEx;
		m_pGfxEx = 0;
	}

	if(m_pECModel)
	{
		m_pECModel->Release();
		delete m_pECModel;
		m_pECModel = 0;
	}

// 	if(m_pELBuilding)
// 	{
// 		m_pELBuilding->Release();
// 		delete m_pELBuilding;
// 		m_pELBuilding = 0;
// 	}
}

bool CRenderable3DObject::Load(CString& path)
{
#ifdef _DEBUG	
	DWORD oldTime = timeGetTime();
#endif
	
	Release();
	CString strPath = path;
	int pos = strPath.ReverseFind('.');
	if(pos == -1) return false;
	strPath = strPath.Right(strPath.GetLength() - pos);
	strPath.MakeLower();
	SetCurrentDirectoryA(g_szAppPath);
	af_SetBaseDir(g_szAppPath);
	if(strPath == ".gfx")
	{
		CString  temp(af_GetBaseDir());
		temp.MakeLower();
		temp += "\\gfx\\";
		strPath = path;
		strPath.Replace(temp,_T(""));
		
		m_pGfxEx = AfxGetGFXExMan()->LoadGfx(CRender::GetInstance()->GetA3DDevice(),AString(AC2AS(strPath)));
		if(m_pGfxEx==NULL)
		{
			return false;
		}

		m_pGfxEx->GetParentTM();
				
	}else if(strPath == ".ecm")
	{
		m_pECModel = new CECModel;
		bool bResult = m_pECModel->Load(AString(AC2AS(path)));
		if(!bResult)
		{
			delete m_pECModel;
			m_pECModel = NULL;
			return false;
		}
		m_pECModel->GetA3DSkinModel()->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);
	}
// 	else if(strPath == ".mox")
// 	{
// 		m_pELBuilding = new CELBuilding;
// 		CString strHullName = path;
// 		strHullName.Replace(".mox",".chf");
// 		if(!m_pELBuilding->LoadFromMOXAndCHF(g_Render.GetA3DDevice(),path,strHullName))
// 		{
// 			g_Log.Log("CRenderable3DObject::ReloadModel(), Failed load %s", path);
// 			delete m_pELBuilding;
// 			m_pELBuilding = NULL;
// 			return false;
// 		}
// 	}



#ifdef _DEBUG
	DWORD escape = timeGetTime() - oldTime;
	CString test;
	test.Format(_T("\nLoadding 3d model times = %d"), escape);
	TRACE(test);
#endif

	return true;
}


void CRenderable3DObject::GetAABB(A3DAABB *aabb)
{
	if (!aabb) return;
	aabb->Clear();

	if(m_pGfxEx) 
		*aabb = m_pGfxEx->GetAABB();
	else if(m_pECModel)
		if (m_pECModel->GetA3DSkinModel())
			*aabb = m_pECModel->GetA3DSkinModel()->GetModelAABB();
		else
			*aabb = m_pECModel->GetModelAABB();
	else
		aabb->Maxs = aabb->Mins = aabb->Extents = aabb->Center = 0;
}


A3DVECTOR3 CRenderable3DObject::GetPos()
{
	if (m_pGfxEx)
		return m_pGfxEx->GetPos();
	if (m_pECModel)
		if (m_pECModel->GetA3DSkinModel())
			return m_pECModel->GetA3DSkinModel()->GetPos();
		else
			return m_pECModel->GetPos();

	return A3DVECTOR3(0.f);
}

#endif