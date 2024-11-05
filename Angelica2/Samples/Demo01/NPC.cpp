//	NPC.cpp

#include <windows.h>
#include "NPC.h"
#include "Game.h"
#include "Render.h"

#include "A3D.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CNPC
//	
///////////////////////////////////////////////////////////////////////////

CNPC::CNPC()
{
	m_pSkinModel	= NULL;
	m_iState		= STATE_NONE;
	m_iStTimeCnt	= 0;
}

CNPC::~CNPC()
{
}

//	Initialize object
bool CNPC::Init(const char* szModelFile, const A3DVECTOR3& vPos, const A3DVECTOR3& vDir)
{
	if (!LoadModel(szModelFile))
		return false;

	SetPos(vPos);
	SetDirAndUp(vDir, CGame::m_vAxisY);

	m_pSkinModel->Update(1);

	//	Enter stand state
	EnterState(STATE_STAND);

	return true;
}

//	Release object
void CNPC::Release()
{
	A3DRELEASE(m_pSkinModel);
}

//	Load model
bool CNPC::LoadModel(const char* szFile)
{
	A3DSkinModel* pModel = new A3DSkinModel;
	if (!pModel)
	{
		a_LogOutput(1, "CNPC::LoadModel, not enough memory");
		return false;
	}

	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		a_LogOutput(1, "CNPC::LoadModel, Failed to initialize skin model");
		return false;
	}

	if (!pModel->Load(szFile, 0))
	{
		delete pModel;
		a_LogOutput(1, "CNPC::LoadModel, Failed to load model %s", szFile);
		return false;
	}

	//	Set light info.
	A3DSkinModel::LIGHTINFO li;
	memset(&li, 0, sizeof (li));
	const A3DLIGHTPARAM& lp = g_Render.GetDirectionalLight()->GetLightparam();

	li.colAmbient	= g_Render.GetA3DDevice()->GetAmbientValue();
	li.vLightDir	= lp.Direction;
	li.colDirDiff	= lp.Diffuse;
	li.colDirSpec	= lp.Specular;
	li.bPtLight		= false;

	pModel->SetLightInfo(li);

	m_pSkinModel = pModel;

	return true;
}

//	Set position
void CNPC::SetPos(const A3DVECTOR3& vPos)
{
	if (m_pSkinModel)
		m_pSkinModel->SetPos(vPos);
}

//	Set absolute forward and up direction
void CNPC::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if (m_pSkinModel)
		m_pSkinModel->SetDirAndUp(vDir, vUp);
}

//	Tick routine
bool CNPC::Tick(DWORD dwDeltaTime)
{
	if (GetKeyState(VK_F1) & 0x8000)
		dwDeltaTime = 0;

	switch (m_iState)
	{
	case STATE_STAND:	Tick_StateStand(dwDeltaTime);	break;
	case STATE_WALK:	Tick_StateWalk(dwDeltaTime);	break;
	}

	if (m_pSkinModel)
		m_pSkinModel->Update(dwDeltaTime);

	return true;
}

//	Render routine
bool CNPC::Render(A3DViewport* pViewport)
{
//	if (m_pSkinModel)
//		m_pSkinModel->Render(pViewport);
	RenderWithEdge(pViewport);

	return true;
}

//	Render with bright edge
bool CNPC::RenderWithEdge(A3DViewport* pViewport)
{
	if (!m_pSkinModel)
		return false;

	A3DVertexShader* pvsSMEdge = g_Render.GetSMEdgeVS();
	A3DVertexShader* pvsRMEdge = g_Render.GetRMEdgeVS();
	m_pSkinModel->SetVertexShader(pvsSMEdge, pvsRMEdge);

	if (!pvsSMEdge || !pvsRMEdge)
		return false;

	A3DMaterial CurMtl = m_pSkinModel->GetUniformMaterial();
	int iCurMtlMode = m_pSkinModel->GetMaterialMethod();
	A3DSkinModel::LIGHTINFO CurLightInfo = m_pSkinModel->GetLightInfo();

	A3DMaterial mtl;
	mtl.SetDiffuse(1.0f, 0.0f, 0.0f, 0.05f);
	mtl.SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);
	mtl.SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	mtl.SetEmissive(0.0f, 0.0f, 0.0f, 0.0f);
	m_pSkinModel->SetUniformMaterial(mtl);
	m_pSkinModel->SetMaterialMethod(A3DSkinModel::MTL_UNIFORM);
		
	A3DSkinModel::LIGHTINFO li;
	li.colDirDiff.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_pSkinModel->SetLightInfo(li);

	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	pA3DDevice->SetZWriteEnable(false);

	m_pSkinModel->RenderAtOnce(pViewport, A3DSkinModel::RAO_NOTEXTURE);

	pA3DDevice->SetZWriteEnable(true);

	m_pSkinModel->SetUniformMaterial(CurMtl);
	m_pSkinModel->SetMaterialMethod(iCurMtlMode);
	m_pSkinModel->SetLightInfo(CurLightInfo);
	m_pSkinModel->RestoreVertexShader();

	m_pSkinModel->RenderAtOnce(pViewport, 0);

	return true;
}

//	Get NPC aabb
const A3DAABB& CNPC::GetAABB()
{
	return m_pSkinModel->GetModelAABB();
}

//	Enter state
void CNPC::EnterState(int iState)
{
	int iStandActNum = 6;

	static char* szStandActs[] = 
	{
		"技能攻击",
		"普通攻击1",
		"普通攻击2",
		"休闲",
		"战斗站立",
		"站立",
	};

	switch (iState)
	{
	case STATE_STAND:

		m_iStTimeCnt = 4000 + a_Random(100, 2000);
		m_pSkinModel->PlayActionByNameDC(szStandActs[a_Random(0, 5)], -1);
		break;

	case STATE_WALK:
	{
		m_iStTimeCnt = 4000 + a_Random(100, 2000);
		m_pSkinModel->PlayActionByNameDC("行走", -1);

		//	Select a random direction
		A3DVECTOR3 vDir = a3d_RandDirH();
		SetDirAndUp(vDir, CGame::m_vAxisY);
		break;
	}
	default:
		return;
	}

	m_iState = iState;
}

//	State tick
void CNPC::Tick_StateStand(DWORD dwDeltaTime)
{
	m_iStTimeCnt -= dwDeltaTime;
	if (m_iStTimeCnt <= 0)
	{
		//	Turn to walk state
		EnterState(STATE_WALK);
		return;
	}

	//	Do stand work here
}

void CNPC::Tick_StateWalk(DWORD dwDeltaTime)
{
	m_iStTimeCnt -= dwDeltaTime;
	if (m_iStTimeCnt <= 0)
	{
		//	Turn to stand state
		EnterState(STATE_STAND);
		return;
	}

	A3DVECTOR3 vDir = m_pSkinModel->GetDir();
	A3DVECTOR3 vPos = m_pSkinModel->GetPos();
	
	float fDist = 1.0f * dwDeltaTime * 0.001f;
	vPos += vDir * fDist;

	//	Make sure npc move on ground
	float fTerrainHei = g_Game.GetTerrain()->GetPosHeight(vPos);
	vPos.y = fTerrainHei;
	SetPos(vPos);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CNPCMan
//	
///////////////////////////////////////////////////////////////////////////

CNPCMan::CNPCMan()
{
}

CNPCMan::~CNPCMan()
{
}

//	Initialize object
bool CNPCMan::Init()
{
	return true;
}

//	Release object
void CNPCMan::Release()
{
	//	Release all NPCs
	ALISTPOSITION pos = m_NPCList.GetHeadPosition();
	while (pos)
	{
		CNPC* pNPC = m_NPCList.GetNext(pos);
		pNPC->Release();
		delete pNPC;
	}

	m_NPCList.RemoveAll();
}

//	Tick routine
bool CNPCMan::Tick(DWORD dwDeltaTime)
{
	ALISTPOSITION pos = m_NPCList.GetHeadPosition();
	while (pos)
	{
		CNPC* pNPC = m_NPCList.GetNext(pos);
		pNPC->Tick(dwDeltaTime);
	}

	return true;
}

//	Render routine
bool CNPCMan::Render(A3DViewport* pViewport)
{
	ALISTPOSITION pos = m_NPCList.GetHeadPosition();
	while (pos)
	{
		CNPC* pNPC = m_NPCList.GetNext(pos);
		pNPC->Render(pViewport);
	}

	return true;
}

//	Create a NPC at specified position
bool CNPCMan::CreateNPC(const A3DVECTOR3& vPos, const char* szModelFile)
{
	CNPC* pNPC = new CNPC;
	if (!pNPC)
		return false;

	A3DVECTOR3 vDir = a3d_RandDirH();

	if (!pNPC->Init(szModelFile, vPos, vDir))
	{
		a_LogOutput(1, "CNPCMan::CreateNPC, Failed to init NPC");
		return false;
	}

	//	Add NPC to list
	m_NPCList.AddTail(pNPC);

	return true;
}

