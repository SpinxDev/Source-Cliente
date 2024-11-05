#include "LightExtra3DWindow.h"

LightExtra3DWindow::LightExtra3DWindow()
{
	m_ListIBLMesh.Init();
	m_pIBLScene = NULL;
	m_pModelBall = NULL;
}

LightExtra3DWindow::~LightExtra3DWindow()
{
}

bool LightExtra3DWindow::Release()
{
	ALISTELEMENT * pIBLElement = m_ListIBLMesh.GetFirst();

	while(pIBLElement != m_ListIBLMesh.GetTail())
	{
		A3DIBLMesh * pIBLMesh = (A3DIBLMesh *) pIBLElement->pData;

		pIBLMesh->Release();
		delete pIBLMesh;

		pIBLElement = pIBLElement->pNext;
	}

	m_ListIBLMesh.Release();

	return Engine3DWindow::Release();
}
bool LightExtra3DWindow::BeforeRenderScene(A3DViewport * pCurrentViewport)
{
	return Engine3DWindow::BeforeRenderScene(pCurrentViewport);
}

bool LightExtra3DWindow::AfterRenderScene(A3DViewport * pCurrentViewport)
{
	ALISTELEMENT * pIBLElement = m_ListIBLMesh.GetFirst();

	while(pIBLElement != m_ListIBLMesh.GetTail())
	{
		A3DIBLMesh * pIBLMesh = (A3DIBLMesh *) pIBLElement->pData;

		pIBLMesh->Render(pCurrentViewport);
		
		pIBLElement = pIBLElement->pNext;
	}

	if( m_pIBLScene )
	{
		m_pIBLScene->Render(pCurrentViewport, A3DSCENE_RENDER_SOLID);
		m_pIBLScene->RenderSort(pCurrentViewport, A3DSCENE_RENDER_ALPHA);
	}

	if( m_pModelBall && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) )
	{
		static float vZPos = 0.6f;
		static float vXPos = 0.0f;
		static float vYPos = -0.2f;

		if( GetAsyncKeyState(VK_PRIOR) & 0x8000 )
			vYPos += 0.01f;
		if( GetAsyncKeyState(VK_NEXT) & 0x8000 )
			vYPos -= 0.01f;
		if( GetAsyncKeyState(VK_UP) & 0x8000 )
			vZPos += 0.01f;
		if( GetAsyncKeyState(VK_DOWN) & 0x8000 )
			vZPos -= 0.01f;
		if( GetAsyncKeyState(VK_LEFT) & 0x8000 )
			vXPos -= 0.01f;
		if( GetAsyncKeyState(VK_RIGHT) & 0x8000 )
			vXPos += 0.01f;

		A3DCameraBase * pCamera = pCurrentViewport->GetCamera();
		A3DVECTOR3 vecCamPos = pCamera->GetPos();
		A3DVECTOR3 vecCamDir = pCamera->GetDir();
		A3DVECTOR3 vecCamUp  = pCamera->GetUp();
		A3DVECTOR3 vecCamRight  = pCamera->GetRight();

		m_pModelBall->SetPos(vecCamPos + vecCamDir * vZPos + vecCamRight * vXPos - vecCamUp * vYPos);
		m_pModelBall->SetDirAndUp(vecCamDir, vecCamUp);
		m_pModelBall->TickAnimation();

		if( m_pIBLScene )
		{
			A3DVECTOR3		vecLightDir;
			A3DCOLOR		clAmbient;
			A3DCOLORVALUE	clDirectional;

			m_pIBLScene->GetLightGrid()->GetEquivalentLightInfo(m_pModelBall->GetModelAABB().Center, &vecLightDir, &clDirectional, &clAmbient);

			A3DLIGHTPARAM dirParam = m_pA3DDirLight->GetLightparam();
			dirParam.Direction = vecLightDir;
			dirParam.Diffuse = clDirectional;
			m_pA3DDirLight->SetLightParam(dirParam);

			m_pA3DDevice->SetAmbient(clAmbient);
			m_pA3DDevice->SetTextureColorOP(0, m_pIBLScene->GetLightGrid()->GetColorOP());

			m_pModelBall->Render(pCurrentViewport, false);

			m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
		}
	}

	return Engine3DWindow::AfterRenderScene(pCurrentViewport);
}

bool LightExtra3DWindow::AddIBLMesh(A3DIBLMesh * pIBLMesh)
{
	m_ListIBLMesh.Append(pIBLMesh, NULL);
	return true;
}

bool LightExtra3DWindow::LoadAllLightMaps()
{
	ALISTELEMENT * pIBLElement = m_ListIBLMesh.GetFirst();
	while(pIBLElement != m_ListIBLMesh.GetTail())
	{
		A3DIBLMesh * pIBLMesh = (A3DIBLMesh *) pIBLElement->pData;

		pIBLMesh->LoadLightMaps();
		
		pIBLElement = pIBLElement->pNext;
	}

	if( m_pIBLScene )
	{
		m_pIBLScene->LoadAllMaps();
	}
	return true;
}

bool LightExtra3DWindow::SetIBLScene(A3DIBLScene * pIBLScene)
{
	m_pIBLScene = pIBLScene;
	return true;
}

bool LightExtra3DWindow::InitEngine()
{
	SetCurrentDirectory("c:\\3dsmax4\\plugcfg\\LightExtra");
	Engine3DWindow::InitEngine();
	m_pA3DEngine->GetA3DModelMan()->LoadModelFile("Ball.mod", &m_pModelBall);
	return true;
}

bool LightExtra3DWindow::ReleaseEngine()
{
	if( m_pModelBall )
		m_pA3DEngine->GetA3DModelMan()->ReleaseModel(m_pModelBall);

	Engine3DWindow::ReleaseEngine();
	return true;
}