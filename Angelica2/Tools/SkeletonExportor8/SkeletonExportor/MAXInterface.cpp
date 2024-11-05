/*
 * FILE: MAXInterface.cpp
 *
 * DESCRIPTION: 3DMAX interface class
 *
 * CREATED BY: duyuxin, 2003/8/25
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "MAXInterface.h"
#include "MAXNode.h"
#include "MAXMesh.h"
#include "MAXNullView.h"

#include "A3DMaterial.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

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

CMAXInterface*	g_pMAXInterface = NULL;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CMAXInterface
//
///////////////////////////////////////////////////////////////////////////

CMAXInterface::CMAXInterface()
{
	m_ei = NULL;
	m_i	 = NULL;
}

bool CMAXInterface::Init(ExpInterface* pExpInterface, Interface* pInterface)
{
	m_ei = pExpInterface;
	m_i  = pInterface;
	return true;
}

int CMAXInterface::GetCurrentFrame()
{
	return m_i->GetTime() / GetTicksPerFrame();
}

float CMAXInterface::GetCurrentTime()
{
	return TicksToSec(m_i->GetTime());
}

int CMAXInterface::GetEndFrame()
{
	return m_i->GetAnimRange().End() / GetTicksPerFrame();
}

int CMAXInterface::GetStartTime()
{
	return m_i->GetAnimRange().Start();
}

int CMAXInterface::GetEndTime()
{
	return m_i->GetAnimRange().End();
}

int CMAXInterface::GetFps()
{
	return GetFrameRate();
}

CWnd* CMAXInterface::GetMainWnd()
{
	return CWnd::FromHandle(m_i->GetMAXHWnd());
}

CMAXNode* CMAXInterface::GetNode(const char* szName)
{
	//	Allocate a new max node instance
	CMAXNode *pNode = new CMAXNode;
	if (!pNode)
		return NULL;
	
	//	Create the max node
	if (!pNode->Create(m_i->GetINodeByName(szName)))
	{
		delete pNode;
		return NULL;
	}

	return pNode;
}

Matrix3 CMAXInterface::GetNodeTM(CMAXNode* pNode, float fTime)
{
	//	Initialize matrix with the identity
	Matrix3 tm;
	tm.IdentityMatrix();

	//	Only do this for valid nodes
	if (pNode)
	{
		//	Get the node transformation
		tm = pNode->GetINode()->GetNodeTM(SecToTicks(fTime));

		//	Make the transformation uniform
		tm.NoScale();
	}

	return tm;
}

int CMAXInterface::GetSelectedNodeCount()
{
	//	Get the number of selected nodes
	int iNodeCount = m_i->GetSelNodeCount();

	//	If nothing is selected, there is always the scene root node
//	if (!iNodeCount)
//		return 1;

	return iNodeCount;
}

CMAXNode* CMAXInterface::GetSelectedNode(int iNodeId)
{
	//	Get the number of selected nodes
	int iNodeCount = m_i->GetSelNodeCount();

	//	If nothing is selected, we go with the scene root node
	if (!iNodeCount)
	{
		//	Check if the given node id is valid
		if (iNodeId)
			return NULL;
		
		//	Allocate a new max node instance
		CMAXNode* pNode = new CMAXNode();
		if (!pNode)
			return NULL;

		//	Create the max node
		if (!pNode->Create(m_i->GetRootNode()))
		{
			delete pNode;
			return NULL;
		}

		return pNode;
	}

	//	Check if the given node id is valid
	if ((iNodeId < 0) || (iNodeId >= m_i->GetSelNodeCount()))
		return NULL;

	//	Allocate a new max node instance
	CMAXNode* pNode = new CMAXNode();
	if (!pNode)
		return NULL;

	//	Create the max node
	if (!pNode->Create(m_i->GetSelNode(iNodeId)))
	{
		delete pNode;
		return NULL;
	}

	return pNode;
}

CMAXNode* CMAXInterface::GetRootNode()
{
	INode* pINode = m_i->GetRootNode();

	CMAXNode* pMAXNode = new CMAXNode();
	if (!pMAXNode)
		return NULL;

	//	Create the max node
	if (!pMAXNode->Create(pINode))
	{
		a_SendLogToDebugWnd(1, "CMAXInterface::GetRootNode, Failed to create MAX node!");
		delete pMAXNode;
		return NULL;
	}

	return pMAXNode;

}

int CMAXInterface::GetStartFrame()
{
	return m_i->GetAnimRange().Start() / GetTicksPerFrame();
}

void CMAXInterface::GetTranslationAndRotation(CMAXNode* pNode, CMAXNode* pParentNode, float fTime, A3DMATRIX4& mat)
{
	//	Check for invalid nodes
	if (!pNode)
		return;

	//	The initial node pose (time == -1.0) is at (time == 0.0) for 3ds max
	if (fTime < 0.0f)
		fTime = 0.0f;

	//	Calculate the relative transformation
	Matrix3 tm = GetNodeTM(pNode, fTime) * Inverse(GetNodeTM(pParentNode, fTime));

	//	Convert to A3D matrix
	MatrixFromMAXToA3D(tm, mat);
}

void CMAXInterface::GetTranslationAndRotationBoneSpace(CMAXNode* pNode, float fTime, A3DMATRIX4& mat)
{
	//	Check for invalid nodes
	if (!pNode)
		return;

	//	The initial node pose (time == -1.0) is at (time == 0.0) for 3ds max
	if (fTime < 0.0f) 
		fTime = 0.0f;

	//	Calculate the inverse transformation
	Matrix3 tm = Inverse(GetNodeTM(pNode, fTime));

	//	Convert to A3D matrix
	MatrixFromMAXToA3D(tm, mat);
}

//	Convert 3DMAX matrix to A3D matrix
void CMAXInterface::MatrixFromMAXToA3D(const Matrix3& matMAX, A3DMATRIX4& matA3D)
{
	matA3D.Identity();
	Point3 row0, row1, row2, row3;
	
	row0 = matMAX.GetRow(0);
	row1 = matMAX.GetRow(1);
	row2 = matMAX.GetRow(2);
	row3 = matMAX.GetRow(3);

	matA3D.m[0][0] = row0.x;
	matA3D.m[0][1] = row0.z;
	matA3D.m[0][2] = row0.y;
	
	matA3D.m[1][0] = row2.x;
	matA3D.m[1][1] = row2.z;
	matA3D.m[1][2] = row2.y;
	
	matA3D.m[2][0] = row1.x;
	matA3D.m[2][1] = row1.z;
	matA3D.m[2][2] = row1.y;

	matA3D.m[3][0] = row3.x;
	matA3D.m[3][1] = row3.z;
	matA3D.m[3][2] = row3.y;
}

bool CMAXInterface::IsBipedBone(INode *pNode)
{
	//	Check for invalid nodes
	if (!pNode)
		return false;

	//	Check for root node
	if (pNode->IsRootNode())
		return false;

	//	Check for bone node
	ObjectState os = pNode->EvalWorldState(0);
	if (os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
		return false;

	//	Check for biped node
	Control* pControl = pNode->GetTMController();
	if (pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID || 
		pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)
		return true;

	return false;
}

bool CMAXInterface::IsBone(CMAXNode* pNode)
{
	//	Check for invalid nodes
	if (!pNode)
		return false;

	//	Get internal max node
	return IsBoneINode(pNode->GetINode());
}

bool CMAXInterface::IsDummy(CMAXNode* pNode)
{
	//	Check for invalid nodes
	if (!pNode)
		return false;

	//	Get internal max node
	return IsDummyINode(pNode->GetINode());
}

bool CMAXInterface::IsMesh(CMAXNode* pNode)
{
	//	Check for invalid nodes
	if (!pNode)
		return false;

	//	Get internal max node
	return IsMeshINode(pNode->GetINode());
}

bool CMAXInterface::IsHook(CMAXNode* pNode)
{
	//	Check for invalid nodes
	if (!pNode)
		return false;

	//	Get internal max node
	return IsHookINode(pNode->GetINode());
}

bool CMAXInterface::IsHitBox(CMAXNode* pNode)
{
	if (!pNode)
		return false;

	return IsHitBoxINode(pNode->GetINode());
}

//	Is physique-shape node ?
bool CMAXInterface::IsPhyShape(CMAXNode* pNode)
{
	if (!pNode)
		return false;

	return IsPhyShapeINode(pNode->GetINode());
}

bool CMAXInterface::IsBoneINode(INode* pINode)
{
	if (!pINode)
		return false;

	//	Check for root node
	if (pINode->IsRootNode())
		return false;

	//	Check for bone node
	ObjectState os = pINode->EvalWorldState(0);
	if (os.obj->ClassID() == Class_ID(BONE_CLASS_ID, 0))
		return true;

#if MAX_RELEASE >= 4000
	if (os.obj->ClassID() == BONE_OBJ_CLASSID)
		return true;
#endif

	if (os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
		return false;

	//	Check for biped node
	Control* pControl = pINode->GetTMController();
	if (pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID || 
		pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)
		return true;

	return false;
}

bool CMAXInterface::IsDummyINode(INode* pINode)
{
	if (!pINode)
		return false;

	//	Check for root node
	if (pINode->IsRootNode())
		return false;

	//	Check for dummy node
	ObjectState os = pINode->EvalWorldState(0);
	if (os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
		return true;

	return false;
}

bool CMAXInterface::IsMeshINode(INode* pINode)
{
	if (!pINode)
		return false;

	//	Check for root node
	if (pINode->IsRootNode())
		return false;

	//	Check for mesh node
	ObjectState os = pINode->EvalWorldState(0);
	if (os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID &&
		os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
		return true;

	return false;
}

bool CMAXInterface::IsHookINode(INode* pINode)
{
	//	To be a hook, it first should be a dummy
	if (!IsDummyINode(pINode))
		return false;
	
	const char* szName = pINode->GetName();
	if ((szName[0] == 'H' && szName[1] == 'H' && szName[2] == '_') ||
		(szName[0] == 'C' && szName[1] == 'C' && szName[2] == '_'))
		return true;

	return false;
}

bool CMAXInterface::IsHitBoxINode(INode* pINode)
{
	//	To be a hit box, it first should be a dummy
	if (!IsDummyINode(pINode))
		return false;
	
	const char* szName = pINode->GetName();
	if (szName[0] == 'B' && szName[1] == 'B' && szName[2] == '_')
		return true;

	return false;
}

bool CMAXInterface::IsPhyShapeINode(INode* pINode)
{
	if (!pINode)
		return false;

	//	Check for root node
	if (pINode->IsRootNode())
		return false;

	//	Check for mesh node
	ObjectState os = pINode->EvalWorldState(0);
	if (os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
		return false;
	
	if (!os.obj->CanConvertToType(Class_ID(CYLINDER_CLASS_ID, 0)) &&
		!os.obj->CanConvertToType(Class_ID(SPHERE_CLASS_ID, 0)))
		return false;

	const char* szName = pINode->GetName();
	if (szName[0] == 'P' && szName[1] == 'H' && szName[2] == 'Y' && szName[3] == '_')
		return true;

	return false;
}

DWORD WINAPI CMAXInterface::ProgressFunction(LPVOID arg)
{
	return 0;
}

void CMAXInterface::SetProgressInfo(int percentage)
{
	m_i->ProgressUpdate(percentage);
}

void CMAXInterface::StartProgressInfo(const char* szText)
{
	m_i->ProgressStart((char*)szText, true, ProgressFunction, 0);
}

void CMAXInterface::StopProgressInfo()
{
	m_i->ProgressEnd();
}

CMAXMesh* CMAXInterface::GetMesh(CMAXNode* pNode)
{
	//	Check for invalid nodes
	if (!pNode)
	{
		a_SendLogToDebugWnd(1, "CMAXInterface::GetMesh, Invalid node handle.");
		return NULL;
	}
	
	//	Allocate a new max mesh instance
	CMAXMesh* pMaxMesh = new CMAXMesh;
	if (!pMaxMesh)
	{
		a_SendLogToDebugWnd(1, "CMAXInterface::GetMesh, Not enough memory.");
		return NULL;
	}

	//	Create the max mesh
	if (!pMaxMesh->Create(pNode->GetINode()))
	{
		a_SendLogToDebugWnd(1, "CMAXInterface::GetMesh, Failed to create mesh.");
		delete pMaxMesh;
		return NULL;
	}

	return pMaxMesh;
}

//	Convert MAX material to A3D material
void CMAXInterface::MaterialFromMAXToA3D(StdMat* pMAXMtl, A3DMaterial* pA3DMtl)
{
	A3DMATERIALPARAM Params;

	//	Get the current time
	TimeValue t = m_i->GetTime();

	Params.Ambient	= A3DCOLORVALUE(pMAXMtl->GetAmbient(t).r, pMAXMtl->GetAmbient(t).g, pMAXMtl->GetAmbient(t).b, pMAXMtl->GetOpacity(t));
	Params.Diffuse	= A3DCOLORVALUE(pMAXMtl->GetDiffuse(t).r, pMAXMtl->GetDiffuse(t).g, pMAXMtl->GetDiffuse(t).b, pMAXMtl->GetOpacity(t));
	Params.Emissive = A3DCOLORVALUE(pMAXMtl->GetSelfIllumColor(t).r, pMAXMtl->GetSelfIllumColor(t).g, pMAXMtl->GetSelfIllumColor().b, pMAXMtl->GetOpacity(t));
	Params.Specular = A3DCOLORVALUE(pMAXMtl->GetSpecular(t).r, pMAXMtl->GetSpecular(t).g, pMAXMtl->GetSpecular(t).b, pMAXMtl->GetOpacity(t));
	
	Params.Specular.r *= pMAXMtl->GetShinStr(t);
	Params.Specular.g *= pMAXMtl->GetShinStr(t);
	Params.Specular.b *= pMAXMtl->GetShinStr(t);

	Params.Power = pMAXMtl->GetShininess(t) * 100.0f;
	bool b2Sided = pMAXMtl->GetTwoSided() ? true : false;

	pA3DMtl->Init(NULL, Params);
	pA3DMtl->Set2Sided(b2Sided);
}

/*	Check a node contain physique or skin modifier

	Return value:
	
		0, if non-modifier was found.
		1, if physique modifier was found.
		2, if skin modifier was found
*/
int CMAXInterface::HasPhysiqueOrSkinModifier(INode* pINode)
{
	//	Get the object reference of the node
	Object* pObject = pINode->GetObjectRef();
	if (!pObject)
		return NULL;

	//	Loop through all derived objects
	while (pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* pDerivedObject = (IDerivedObject*)pObject;

		//	Loop through all modifiers
		for (int i=0; i < pDerivedObject->NumModifiers(); i++)
		{
			//	Get the modifier
			Modifier* pModifier = pDerivedObject->GetModifier(i);

			//	Check if we found the physique modifier
			if (pModifier->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
				return 1;
			else if (pModifier->ClassID() == SKIN_CLASSID)
				return 2;
		}

		//	Continue with next derived object
		pObject = pDerivedObject->GetObjRef();
	}

	return 0;
}

//	Normalize matrix
Matrix3 CMAXInterface::NormalizeTM(const Matrix3& mat)
{
	Matrix3 matOut;

	Point3 v0 = mat.GetRow(0);
	Point3 v1 = mat.GetRow(1);
	Point3 v2 = mat.GetRow(2);

	v0 = v0.Normalize();
	v1 = v1.Normalize();
	v2 = v2.Normalize();

	matOut.SetRow(0, v0);
	matOut.SetRow(1, v1);
	matOut.SetRow(2, v2);
	matOut.SetRow(3, mat.GetRow(3));

	return matOut;
}

//	Get matrix scale
Point3 CMAXInterface::GetMatrixScale(const Matrix3& mat)
{
	Point3 v0 = mat.GetRow(0);
	Point3 v1 = mat.GetRow(1);
	Point3 v2 = mat.GetRow(2);
	
	float sx = v0.Length();
	float sy = v1.Length();
	float sz = v2.Length();

	return Point3(sx, sy, sz);
}


