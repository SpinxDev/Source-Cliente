/*
 * FILE: Model.cpp
 *
 * DESCRIPTION: Model class
 *
 * CREATED BY: Duyuxin, 2003/9/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "Render.h"
#include "MainFrm.h"
#include "Box3D.h"
#include "Hook.h"
#include "Staff.h"
#include "Model.h"
#include "SkeletonEditor.h"
#include "ToolTabWnd.h"
#include "InterfaceWrapper.h"

#define new A_DEBUG_NEW

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

MODELSTOPCALLBACK	g_ModelSCBData = {SCB_UNKNOWN, 0};

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

//	Use specified blend matrices to calculate vertices' positions and normals through software rendering method
//	One of aVerts and aNormals can be NULL
static bool _GetBlendedVertices(A3DSkin* pSkin, A3DSkinMesh* pSkinMesh, const A3DMATRIX4* aBlendMats, 
					A3DVECTOR3* aVerts, A3DVECTOR3* aNormals)
{
	if (!aVerts && !aNormals)
		return false;

	A3DVECTOR3 vPos, vNormal;
	float fTotalWeight;
	SKIN_VERTEX* aSrcVerts = pSkinMesh->GetOriginVertexBuf();
	int iNumVert = pSkinMesh->GetVertexNum();

	for (int i=0; i < iNumVert; i++)
	{
		SKIN_VERTEX* pSrc = &aSrcVerts[i];
		A3DVECTOR3 vSrcPos(pSrc->vPos[0], pSrc->vPos[1], pSrc->vPos[2]);
		A3DVECTOR3 vSrcNormal(pSrc->vNormal[0], pSrc->vNormal[1], pSrc->vNormal[2]);

		vPos.Clear();
		vNormal.Clear();

		fTotalWeight = 0;
		int j;

		for (j=0; j < 3; j++)
		{
			if (!pSrc->aWeights[j])
				break;

			//	Map bone index
			int iMat = (pSrc->dwMatIndices >> (j << 3)) & 0x000000ff;
			iMat = pSkin->MapBoneIndex(iMat);

			vPos	+= (aBlendMats[iMat] * vSrcPos) * pSrc->aWeights[j];
			vNormal += a3d_VectorMatrix3x3(vSrcNormal, aBlendMats[iMat]) * pSrc->aWeights[j];

			fTotalWeight += pSrc->aWeights[j];
		}

		if (j >= 3)
		{
			fTotalWeight = 1.0f - fTotalWeight;
			if (fTotalWeight > 0.0f)
			{
				//	Map bone index
				int iMat = (pSrc->dwMatIndices & 0xff000000) >> 24;
				iMat = pSkin->MapBoneIndex(iMat);

				vPos	+= (aBlendMats[iMat] * vSrcPos) * fTotalWeight;
				vNormal += a3d_VectorMatrix3x3(vSrcNormal, aBlendMats[iMat]) * fTotalWeight;
			}
		}

		if (aVerts)
			aVerts[i] = vPos;

		if (aNormals)
		{
			vNormal.Normalize();
			aNormals[i] = vNormal;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CModel
//
///////////////////////////////////////////////////////////////////////////

CModel::CModel()
{
	m_pRender		= NULL;
	m_pA3DModel		= NULL;
	m_fModelRadius	= 0.0f;
	m_fRotateX		= 0.0f;
	m_fRotateY		= 0.0f;

	m_vModelCenter.Clear();

	m_matRotate.Identity();
}

//	Initialize model
bool CModel::Init(CRender* pRender)
{
	m_pRender = pRender;

	if (!(m_pA3DModel = new CSkinModel))
	{
		g_Log.Log("CModel::Init, Not enough memory");
		return false;
	}

	if (!m_pA3DModel->Init(pRender->GetA3DEngine()))
	{
		g_Log.Log("CModel::Init, Failed to initialize A3D skin model");
		return false;
	}

	if (!m_ModelBox.Init(pRender, 10, false))
		return false;

	m_ModelBox.SetColor(g_aColTable[COL_MODELAABB]);

	//	Set model's light information
	const A3DLIGHTPARAM& lp = g_Render.GetDirectionalLight()->GetLightparam();

	A3DSkinModel::LIGHTINFO LightInfo;
	memset(&LightInfo, 0, sizeof (LightInfo));

	LightInfo.colAmbient	= g_Render.GetA3DDevice()->GetAmbientValue();
	LightInfo.vLightDir		= lp.Direction;
	LightInfo.colDirDiff	= lp.Diffuse;
	LightInfo.colDirSpec	= lp.Specular;
	LightInfo.bPtLight		= false;

	m_pA3DModel->SetLightInfo(LightInfo);

	return true;
}

//	Release model
void CModel::Release()
{
	ReleaseBoneNodes();
	ReleaseHooks();

	if (m_pA3DModel)
	{
		m_pA3DModel->Release();
		delete m_pA3DModel;
		m_pA3DModel = NULL;
	}

	m_ModelBox.Release();

	m_pRender = NULL;
}

//	Frame move
bool CModel::FrameMove(DWORD dwTime)
{
	m_pA3DModel->SetAbsoluteTM(m_matRotate);

	m_pA3DModel->FrameUpdate(dwTime);

	//	Update bone nodes
	UpdateBoneNodes();

	//	Update hooks
	UpdateHooks();

	//	If mesh obb > skeleton aabb, use mesh obb as model's radius
	const A3DOBB& obb = m_pA3DModel->GetInitMeshOBB();
	float fRadius = sqrt(DotProduct(obb.Extents, obb.Extents));
	if (fRadius > m_fModelRadius)
		m_fModelRadius = fRadius;

	return true;
}

//	Update bone nodes
bool CModel::UpdateBoneNodes()
{
	A3DSkeleton* pSkeleton = m_pA3DModel->GetSkeleton();
	if (!g_Configs.bSkeletonOnly || !pSkeleton)
		return true;

	//	Update all boxes and staffs
	int i, j, iNumBone = pSkeleton->GetBoneNum();

	for (i=0; i < iNumBone; i++)
	{
		A3DBone* pBone = pSkeleton->GetBone(i);
		BONENODE* pNode = m_aBoneNodes[i];

		A3DMATRIX4 mat = pBone->GetAbsoluteTM();
		A3DVECTOR3 vParent	= mat.GetRow(3);
		A3DVECTOR3 vBoxZ	= mat.GetRow(2);
		A3DVECTOR3 vBoxY	= mat.GetRow(1);
		A3DVECTOR3 vBoxX	= mat.GetRow(0);

		pNode->pBox->SetPos(vParent);
		pNode->pBox->SetDirAndUp(vBoxZ, vBoxY);
	//	pNode->pBox->SetTM(m_matRotate);

		//	Update staffs which link parent and children
		for (j=0; j < pBone->GetChildNum(); j++)
		{
			CStaff* pStaff = pNode->aLinkStaffs[j];
			A3DBone* pChildBone = pBone->GetChildPtr(j);

			A3DMATRIX4 mat1 = pChildBone->GetAbsoluteTM();
			A3DVECTOR3 vChild = mat1.GetRow(3);

			A3DMATRIX4 matView;
			D3DXMatrixLookAtLH((D3DXMATRIX*)&matView, (D3DXVECTOR3*)&vParent, (D3DXVECTOR3*)&vChild, (D3DXVECTOR3*)&g_vAxisY);

			vChild -= vParent;
			float fLen = vChild.Magnitude();

			pStaff->SetLength(fLen);
			pStaff->SetPos(vParent + vChild * 0.5f);
			pStaff->SetDirAndUp(matView.GetCol(2), matView.GetCol(1));
		//	pStaff->SetTM(m_matRotate);
		}

		//	Update coordinates staffs
		CStaff* pStaff = pNode->aCoorStaffs[0];
		pStaff->SetPos(vParent + vBoxX * (pStaff->GetLength() * 0.5f));
		pStaff->SetDirAndUp(vBoxX, vBoxY);

		pStaff = pNode->aCoorStaffs[1];
		pStaff->SetPos(vParent + vBoxY * (pStaff->GetLength() * 0.5f));
		pStaff->SetDirAndUp(vBoxY, vBoxZ);

		pStaff = pNode->aCoorStaffs[2];
		pStaff->SetPos(vParent + vBoxZ * (pStaff->GetLength() * 0.5f));
		pStaff->SetDirAndUp(vBoxZ, vBoxY);
	}	

	return true;
}

//	Update hook nodes
bool CModel::UpdateHooks()
{
	A3DSkeleton* pSkeleton = m_pA3DModel->GetSkeleton();
	if (!pSkeleton)
		return true;

	for (int i=0; i < m_aHooks.GetSize(); i++)
	{
		CHook* pHook = m_aHooks[i];
		A3DSkeletonHook* pA3DHook = pHook->GetA3DHook();

		A3DMATRIX4 mat;
		int iBone = pA3DHook->GetBone();
		if (iBone < 0)
			mat = pSkeleton->GetAbsoluteTM();
		else
		//	mat = pSkeleton->GetBone(iBone)->GetNoScaleAbsTM();
			mat = pSkeleton->GetBone(iBone)->GetAbsoluteTM();
		
		pHook->FrameMove(mat);
	}

	return true;
}

//	Render model
bool CModel::Render(A3DViewport* pViewport)
{
	if (!m_pA3DModel || !m_pA3DModel->GetSkeleton())
		return true;

	m_matRotate.RotateAxis(m_vModelCenter, g_vAxisY, DEG2RAD(m_fRotateY));
	m_matRotate *= RotateAxis(m_vModelCenter, g_vAxisX, DEG2RAD(m_fRotateX));

	if (g_Configs.bSkeletonOnly)
	{
		RenderBoneNodes();

		//	Draw bounding box
		RenderModelBox();
	}
	else
	{
	//	m_pA3DModel->Render(pViewport, false);
		m_pA3DModel->Render(pViewport);

		//	Render vertex normals
		if (g_Configs.bShowNormal)
			RenderNormals();
	}

	int i;

	//	Render hooks
	for (i=0; i < m_aHooks.GetSize(); i++)
	{
		CHook* pHook = m_aHooks[i];
		pHook->Render();
	}

	//	Draw physique shapes
	RenderPhysiqueShapes();

	return true;
}

//	Render physique shapes
bool CModel::RenderPhysiqueShapes()
{
	A3DWireCollector* pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();

	for (int i=0; i < m_pA3DModel->GetSkinPhyShapeNum(); i++)
	{
		if (g_Configs.bShowPhyShape && i != g_Configs.iEditPhyShape)
		{
			A3DSkinPhyShape* pPhyShape = m_pA3DModel->GetSkinPhyShape(i);
			pWireCollector->AddCapsule(pPhyShape->GetWorldCapsule(), A3DCOLORRGB(200, 0, 64));
		}
	}

	//	Draw the physique shape which is being edited
	if (g_Configs.iEditPhyShape >= 0)
	{
		A3DSkinPhyShape* pPhyShape = m_pA3DModel->GetSkinPhyShape(g_Configs.iEditPhyShape);
		pWireCollector->AddCapsule(pPhyShape->GetWorldCapsule(), A3DCOLORRGB(255, 0, 255));
	}

	return true;
}

//	Render model's bounding box
bool CModel::RenderModelBox()
{
	if (!g_Configs.bShowBoundBox)
		return true;

	A3DWireCollector* pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();

	//	Render hit boxes
	for (int i=0; i < m_pA3DModel->GetSkinHitBoxNum(); i++)
	{
		const A3DOBB& obb = m_pA3DModel->GetSkinHitBox(i)->GetWorldOBB();
		pWireCollector->AddOBB(obb, A3DCOLORRGB(0, 160, 0));
	}

	//	Render model's AABB
	const A3DAABB& aabb = m_pA3DModel->GetModelAABB();
	pWireCollector->AddAABB(aabb, A3DCOLORRGB(0, 255, 0));

	return true;
}

//	Render bone nodes
bool CModel::RenderBoneNodes()
{
	m_pRender->GetA3DDevice()->SetWorldMatrix(m_matRotate);

	int i, j;

	for (i=0; i < m_aBoneNodes.GetSize(); i++)
	{
		BONENODE* pNode = m_aBoneNodes[i];

		//	Render box
		pNode->pBox->Render();

		//	Render staffs which link parent and children
		for (j=0; j < pNode->aLinkStaffs.GetSize(); j++)
			pNode->aLinkStaffs[j]->Render();

		if (pNode->bSelected && g_Configs.bShowBoneCoord)
		{
			//	Render coordinates staffs
			for (j=0; j < 3; j++)
				pNode->aCoorStaffs[j]->Render();
		}
	}	

	return true;
}

//	Draw model's normal
bool CModel::RenderNormals()
{
	int i, j, m;

	A3DWireCollector* pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	A3DVECTOR3* aVerts = NULL;
	A3DVECTOR3* aNormals = NULL;

	//	Build blend matrices
	AArray<A3DMATRIX4> aBlendMats;
	aBlendMats.SetSize(m_pA3DModel->GetBlendMatrixNum(), 4);
	for (i=0; i < aBlendMats.GetSize(); i++)
		aBlendMats[i] = m_pA3DModel->GetBlendMatrix(i);
	
	//	Draw normals
	int iNumSkin = m_pA3DModel->GetSkinNum();
	float fLen = m_fModelRadius * 0.03f;

	for (i=0; i < iNumSkin; i++)
	{
		A3DSkin* pSkin = m_pA3DModel->GetA3DSkin(i);
		if (!pSkin)
			continue;

		int iNumMesh = pSkin->GetSkinMeshNum();
		for (j=0; j < iNumMesh; j++)
		{
			A3DSkinMesh* pSkinMesh = pSkin->GetSkinMesh(j);

			int iNumVert = pSkinMesh->GetVertexNum();

			if (!(aNormals = new A3DVECTOR3[iNumVert]) || 
				!(aVerts = new A3DVECTOR3[iNumVert]))
				return false;

			_GetBlendedVertices(pSkin, pSkinMesh, aBlendMats.GetData(), aVerts, aNormals);

			for (m=0; m < iNumVert; m++)
			{
				A3DVECTOR3 v2 = aVerts[m] + aNormals[m] * fLen;
				pWireCollector->Add3DLine(aVerts[m], v2, 0xffff8080);
			}

			delete [] aVerts;
			delete [] aNormals;
		}

		iNumMesh = pSkin->GetRigidMeshNum();
		for (j=0; j < iNumMesh; j++)
		{
			A3DRigidMesh* pRigidMesh = pSkin->GetRigidMesh(j);

			int iNumVert = pRigidMesh->GetVertexNum();
			int iBoneIdx = pRigidMesh->GetBoneIndex();
			const A3DMATRIX4& mat = aBlendMats[iBoneIdx];

			const A3DVERTEX* aVerts = pRigidMesh->GetOriginVertexBuf();

			for (m=0; m < iNumVert; m++)
			{
				const A3DVERTEX& src = aVerts[m];
				A3DVECTOR3 v1(src.x, src.y, src.z);
				A3DVECTOR3 vNormal(src.nx, src.ny, src.nz);

				v1 = v1 * mat;
				a3d_VectorMatrix3x3(vNormal, mat);

				A3DVECTOR3 v2 = v1 + vNormal * fLen;
				pWireCollector->Add3DLine(v1, v2, 0xffff8080);
			}
		}
	}

	pWireCollector->Flush();

	return true;
}

//	Build bone nodes
bool CModel::BuildBoneNodes()
{
	ASSERT(m_pA3DModel);

	//	Release old bone nodes
	ReleaseBoneNodes();

	A3DSkeleton* pSkeleton = m_pA3DModel->GetSkeleton();
	if (!pSkeleton)
		return true;

	int i, j, iNumBone = pSkeleton->GetBoneNum();

	for (i=0; i < iNumBone; i++)
	{
		BONENODE* pNode = new BONENODE;
		ASSERT(pNode);

		pNode->pBox = new CBox3D;
		ASSERT(pNode);

		pNode->bSelected = false;

		pNode->pBox->Init(m_pRender, g_Configs.fBoneBoxSize);
		pNode->pBox->SetColor(g_aColTable[COL_BONEBOX]);

		//	Create staffs which link parent and children
		A3DBone* pBone = pSkeleton->GetBone(i);
		for (j=0; j < pBone->GetChildNum(); j++)
		{
			CStaff* pStaff = new CStaff;
			ASSERT(pStaff);

			pStaff->Init(m_pRender, g_Configs.iStaffSegment, 1.0f, g_Configs.fStaffRadius);
			pStaff->SetColor(g_aColTable[COL_BONESTAFF]);
			pNode->aLinkStaffs.Add(pStaff);
		}

		//	Create coordinates staffs
		for (j=0; j < 3; j++)
		{
			CStaff* pStaff = new CStaff;
			ASSERT(pStaff);

			pStaff->Init(m_pRender, g_Configs.iStaffSegment, g_Configs.fStaffRadius * 20.0f, g_Configs.fStaffRadius * 0.5f);
			pStaff->SetColor(g_aColTable[COL_AXISX+j]);
			pNode->aCoorStaffs[j] = pStaff;
		}

		m_aBoneNodes.Add(pNode);
	}

	return true;
}

//	Release bone nodes
void CModel::ReleaseBoneNodes()
{
	int i, j;

	for (i=0; i < m_aBoneNodes.GetSize(); i++)
	{
		BONENODE* pNode = m_aBoneNodes[i];

		pNode->pBox->Release();
		delete pNode->pBox;

		for (j=0; j < pNode->aLinkStaffs.GetSize(); j++)
		{
			CStaff* pStaff = pNode->aLinkStaffs[j];
			pStaff->Release();
			delete pStaff;
		}

		pNode->aLinkStaffs.RemoveAll();

		for (j=0; j < 3; j++)
		{
			CStaff* pStaff = pNode->aCoorStaffs[j];
			pStaff->Release();
			delete pStaff;
		}

		delete pNode;
	}

	m_aBoneNodes.RemoveAll();
}

//	Build hooks
bool CModel::BuildHooks()
{
	ASSERT(m_pA3DModel);

	//	Release old hooks
	ReleaseHooks();

	A3DSkeleton* pSkeleton = m_pA3DModel->GetSkeleton();
	if (!pSkeleton)
		return true;

	for (int i=0; i < pSkeleton->GetHookNum(); i++)
	{
		A3DSkeletonHook* pA3DHook = pSkeleton->GetHook(i);

		CHook* pHook = new CHook;
		ASSERT(pHook);

		if (!pHook->Init(m_pRender, pA3DHook))
			return false;

		m_aHooks.Add(pHook);
	}

	return true;
}

//	Release all hooks
void CModel::ReleaseHooks()
{
	for (int i=0; i < m_aHooks.GetSize(); i++)
	{
		CHook* pHook = m_aHooks[i];

		pHook->Release();
		delete pHook;
	}

	m_aHooks.RemoveAll();
}

//	Set box size and staff radius
void CModel::SetBoxAndStaffSize(float fBoxSize, float fStaffRadius)
{
	int i, j;

	//	Update bones
	for (i=0; i < m_aBoneNodes.GetSize(); i++)
	{
		BONENODE* pNode = m_aBoneNodes[i];

		pNode->pBox->SetSize(fBoxSize);

		for (j=0; j < pNode->aLinkStaffs.GetSize(); j++)
		{
			CStaff* pStaff = pNode->aLinkStaffs[j];
			pStaff->SetRadius(fStaffRadius);
		}

		for (j=0; j < 3; j++)
		{
			CStaff* pStaff = pNode->aCoorStaffs[j];
			pStaff->SetLength(fStaffRadius * 20.0f);
			pStaff->SetRadius(fStaffRadius * 0.5f);
		}
	}

	//	Update hooks
	for (i=0; i < m_aHooks.GetSize(); i++)
	{
		CHook* pHook = m_aHooks[i];
		pHook->SetBoxAndStaffSize(fBoxSize, fStaffRadius);
	}
}

//	Get skeleton
A3DSkeleton* CModel::GetSkeleton()
{
	ASSERT(m_pA3DModel);
	return m_pA3DModel->GetSkeleton();
}

//	Get skin
A3DSkin* CModel::GetSkin(int i)
{
	ASSERT(m_pA3DModel);
	return m_pA3DModel->GetA3DSkin(i);
}

/*	Change model's skin

	iIndex: skin's index. -1 means add a new skin
*/
bool CModel::ChangeSkin(int iIndex, const char* szFile)
{
	ASSERT(m_pA3DModel);

	A3DSkeleton* pSkeleton = m_pA3DModel->GetSkeleton();
	if (!pSkeleton)
	{
		g_Log.Log("CModel::ChangeSkin, No skeleton is found");
		return false;
	}

	if (iIndex < 0)
	{
		ASSERT(szFile);
		iIndex = m_pA3DModel->AddSkinFile(szFile, true);
		if (iIndex < 0)
		{
			g_Log.Log("CModel::ChangeSkin, Failed to load skin from file %s", szFile);
			return false;
		}
	}
	else if (szFile)
	{
		if (!m_pA3DModel->ReplaceSkinFile(iIndex, szFile, true))
		{
			g_Log.Log("CModel::ChangeSkin, Failed to load skin from file %s", szFile);
			return false;
		}
	}
	else	//	Remove skin
	{
		m_pA3DModel->ReplaceSkin(iIndex, NULL, true);
	}

	return true;
}

//	Load model from file
bool CModel::Load(const char* szFile)
{
	//	Use AUTOAABB_INITMESH before model loading, so m_obbInitMesh can be calculated.
	//	We need it to calculate model's radius
//	m_pA3DModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);

	if (!m_pA3DModel->Load(szFile))
	{
		g_Log.Log("CModel::Load, Failed to load skin model %s", szFile);
		return false;
	}

	//	Test code
//	m_pA3DModel->SetAutoUpdateChildFlag(false);
//	m_pA3DModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);
//	m_pA3DModel->SetAutoAABBType(A3DSkinModel::AUTOAABB_SKELETON);
//	m_pA3DModel->EnableSpecular(true);
	
	//	Set all bones to initial state
	m_pA3DModel->Update(10);

	//	Calculate model approximate size
	CalcModelSize();

	//	Build bone node
	if (!BuildBoneNodes())
	{
		g_Log.Log("CModel::ChangeSkeleton, Failed to build bone nodes");
		return NULL;
	}

	//	Build hooks
	BuildHooks();

	//	test code
/*	A3DBone* pBone = m_pA3DModel->GetSkeleton()->GetBone("Bip02 L UpperArm", NULL);
	if (pBone)
	{
		pBone->SetScaleType(A3DBone::SCALE_WHOLE);
		pBone->SetScaleFactor(A3DVECTOR3(2.0f, 2.0f, 2.0f));
	}
*/
	return true;
}

//	Save model to file
bool CModel::Save(const char* szFile)
{
	if (m_pA3DModel->GetVersion() < 7)
	{
		AUX_MessageBox(MB_OK, "无法直接保存老版本的模型数据，请先使用 '编辑->动作加工厂' 对模型文件进行升级。");
		return false;
	}

	if (!m_pA3DModel->Save(szFile))
	{
		g_Log.Log("CModel::Save, Failed to save skin model %s", szFile);
		return false;
	}

	return true;
}

//	Get action number
int CModel::GetActionNum()
{
	ASSERT(m_pA3DModel);
	return m_pA3DModel->GetActionNum();
}

//	Get hanger number
int CModel::GetHangerNum()
{
	ASSERT(m_pA3DModel);
	return m_pA3DModel->GetHangerNum();
}

//	Calculate model center and radius
void CModel::CalcModelSize()
{
	A3DSkeleton* pSkeleton = m_pA3DModel->GetSkeleton();
	if (!pSkeleton)
		return;

	m_fModelRadius = 0.0f; 
	m_vModelCenter.Clear();

	int i;

	//	Calculate center
	for (i=0; i < pSkeleton->GetBoneNum(); i++)
	{
		const A3DMATRIX4 mat = pSkeleton->GetBone(i)->GetAbsoluteTM();
		m_vModelCenter += mat.GetRow(3);
	}

	m_vModelCenter /= (float)pSkeleton->GetBoneNum();

	//	Calculate radius
	if (pSkeleton->GetBoneNum() == 1)
	{
		m_fModelRadius = g_Configs.fBoneBoxSize;
	}
	else
	{
		for (i=0; i < pSkeleton->GetBoneNum(); i++)
		{
			const A3DMATRIX4 mat = pSkeleton->GetBone(i)->GetAbsoluteTM();
			A3DVECTOR3 v = mat.GetRow(3);

			float fRadius = Magnitude(v - m_vModelCenter);
			if (fRadius > m_fModelRadius)
				m_fModelRadius = fRadius;
		}

		//	Change box and staff size base on model
		g_Configs.fBoneBoxSize = m_fModelRadius * 0.01;
		g_Configs.fStaffRadius = g_Configs.fBoneBoxSize * 0.5f;

		//	Save configs
		AUX_SaveConfigs();
	}
}

//	Add rotate degree arounding Y axis
void CModel::AddRotateY(float fDeg)
{
	m_fRotateY += fDeg;
	while (m_fRotateY > 360.0f)
		m_fRotateY -= 360.0f;

	while (m_fRotateY < 0.0f)
		m_fRotateY += 360.0f;
}

//	Add rotate degree arounding X axis
void CModel::AddRotateX(float fDeg)
{
	m_fRotateX += fDeg;
	while (m_fRotateX > 360.0f)
		m_fRotateX -= 360.0f;

	while (m_fRotateX < 0.0f)
		m_fRotateX += 360.0f;
}

//	Selected bone
void CModel::SelectBone(int iBone, bool bSelect)
{
	if (!m_pA3DModel->GetSkeleton())
		return;

	if (iBone < 0 || iBone >= m_aBoneNodes.GetSize())
		return;

	BONENODE* pNode = m_aBoneNodes[iBone];
	pNode->bSelected = bSelect;

	if (bSelect)
		pNode->pBox->SetColor(g_aColTable[COL_SELBONEBOX]);
	else
		pNode->pBox->SetColor(g_aColTable[COL_BONEBOX]);
}

void CModel::SelectBone(const char* szBone, bool bSelect)
{
	A3DSkeleton* pSkeleton = m_pA3DModel->GetSkeleton();
	if (!pSkeleton)
		return;

	int iBone;

	if (pSkeleton->GetBone(szBone, &iBone))
	{
		BONENODE* pNode = m_aBoneNodes[iBone];
		pNode->bSelected = bSelect;

		if (bSelect)
			pNode->pBox->SetColor(g_aColTable[COL_SELBONEBOX]);
		else
			pNode->pBox->SetColor(g_aColTable[COL_BONEBOX]);
	}
}

//	Clear all bones' selection state
void CModel::ClearBoneSelection()
{
	A3DSkeleton* pSkeleton = m_pA3DModel->GetSkeleton();
	if (!pSkeleton)
		return;

	for (int i=0; i < m_aBoneNodes.GetSize(); i++)
	{
		BONENODE* pNode = m_aBoneNodes[i];
		pNode->bSelected = false;
		pNode->pBox->SetColor(g_aColTable[COL_BONEBOX]);
	}
}

//	Bone is selected ?
bool CModel::BoneIsSelected(int iBone)
{
	if (!m_pA3DModel->GetSkeleton())
		return false;

	if (iBone < 0 || iBone >= m_aBoneNodes.GetSize())
		return false;

	BONENODE* pNode = m_aBoneNodes[iBone];
	return pNode->bSelected;
}

bool CModel::BoneIsSelected(const char* szBone)
{
	A3DSkeleton* pSkeleton = m_pA3DModel->GetSkeleton();
	if (!m_pA3DModel->GetSkeleton())
		return false;

	int iBone;

	if (pSkeleton->GetBone(szBone, &iBone))
	{
		BONENODE* pNode = m_aBoneNodes[iBone];
		return pNode->bSelected;
	}

	return false;
}

//	Select bones
void CModel::SelectBone(const RECT& rcSel, A3DViewport* pViewport, bool bAdd)
{
	if (!g_Configs.bSkeletonOnly || !m_aBoneNodes.GetSize())
		return;

	for (int i=0; i < m_aBoneNodes.GetSize(); i++)
	{
		BONENODE* pNode = m_aBoneNodes[i];
		if (pNode->bSelected == bAdd)
			continue;	//	Bone has been selected or unselected

		A3DVECTOR3 vPos = pNode->pBox->GetPos();// * m_matRotate;
		if (pViewport->Transform(vPos, vPos))
			continue;	//	Vertex should be clipped

		POINT pt = {(int)vPos.x, (int)vPos.y};
		if (PtInRect(&rcSel, pt))
			SelectBone(i, bAdd);
	}

	//	Update bone select states
	AUX_GetMainFrame()->GetToolTabWnd()->UpdateBoneSelection();
}


