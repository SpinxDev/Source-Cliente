/*
 * FILE: SkeletonCandidate.cpp
 *
 * DESCRIPTION: Skeleton candidate class
 *
 * CREATED BY: duyuxin, 2003/8/26
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "SkeletonCandidate.h"
#include "BoneCandidate.h"
#include "MAXMesh.h"
#include "MAXNode.h"
#include "MAXInterface.h"
#include "InterfaceWrapper.h"

#include <A3DBone.h>

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

static int _BoneCompare(const void* elem1, const void* elem2)
{
	CBoneCandidate* pBone1 = *(CBoneCandidate**)elem1;
	CBoneCandidate* pBone2 = *(CBoneCandidate**)elem2;
	return strcmp(pBone1->GetName(), pBone2->GetName());
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CSkeletonCandidate
//
///////////////////////////////////////////////////////////////////////////

CSkeletonCandidate::CSkeletonCandidate()
{
	m_bUpdateFile	= false;
	m_iNumSelBone	= 0;
	m_iStartFrame	= 0;
	m_iEndFrame		= 100;
	m_iAnmiFPS		= 30;
}

CSkeletonCandidate::~CSkeletonCandidate()
{
	CSkeletonCandidate::Release();
}

//	Initialize object
bool CSkeletonCandidate::Init()
{
	//	Release current content
	Release();

	//	Loop through all selected nodes
	CMAXNode* pRootNode = g_pMAXInterface->GetRootNode();
	int i, iNumChild = pRootNode->GetChildCount();

	for (i=0; i < iNumChild; i++)
	{
		CMAXNode* pNode = pRootNode->GetChild(i);

		//	Recursively add the node to the skeleton candidate
		if (!AddNode(pNode, -1, (pNode->GetINode()->Selected() == TRUE) ? true : false))
		{
			delete pNode;
			return false;
		}
	}

	delete pRootNode;

	a_SendLogToDebugWnd(0, "%d bone node. %d root bone node", m_aBones.GetSize(), m_RootBoneList.GetCount());

	return true;
}

//	Initialize from file
bool CSkeletonCandidate::InitFromFile(const char* szSkeletonFile)
{
	//	Release current content
	Release();

	m_strSkeletonFile = szSkeletonFile;

	//	Create a A3D skeleton object
	CSkeletonWrapper Skeleton;

	if (!Skeleton.Init())
	{
		a_SendLogToDebugWnd(1, "CSkeletonCandidate::InitFromFile, Failed to create A3D skeleton object");
		return false;
	}

	//	Load the skeleton file
	if (!Skeleton.Load(szSkeletonFile))
	{
		a_SendLogToDebugWnd(1, "CSkeletonCandidate::InitFromFile, Failed to load skeleton file %s", szSkeletonFile);
		Skeleton.Release();
		return false;
	}

	m_iStartFrame	= g_pMAXInterface->GetStartFrame();
	m_iEndFrame		= g_pMAXInterface->GetEndFrame();
	m_iAnmiFPS		= g_pMAXInterface->GetFps();

	int i;

	//	Allocate bone array
	m_aBones.SetSize(Skeleton.GetBoneNum(), 10);
	for (i=0; i < Skeleton.GetBoneNum(); i++)
		m_aBones[i] = NULL;

	//	Loop through all root bones
	for (i=0; i < Skeleton.GetBoneNum(); i++)
	{
		A3DBone* pBone = Skeleton.GetBone(i);
		if (pBone->GetParent() >= 0)
			continue;

		//	Recursively add the bone to the skeleton candidate
		if (!AddNode(&Skeleton, pBone, -1))
		{
			a_SendLogToDebugWnd(1, "CSkeletonCandidate::InitFromFile, Failed to add bone");
			Skeleton.Release();
			return false;
		}
	}

	//	Release skeleton object
	Skeleton.Release();

	a_SendLogToDebugWnd(0, "%d bone. %d root bone", m_aBones.GetSize(), m_RootBoneList.GetCount());

	//	Select all bones
	for (i=0; i < m_aBones.GetSize(); i++)
		m_aBones[i]->SetSelected(true);

	BuildSelectedId();

	return true;
}

//	Release object
void CSkeletonCandidate::Release()
{
	int i;

	//	Destroy all bone candidates stored in this skeleton candidate
	for (i=0; i < m_aBones.GetSize(); i++)
		delete m_aBones[i];

	m_aBones.RemoveAll();

	//	Clear root bone candidate id list
	m_RootBoneList.RemoveAll();

	for (i=0; i < m_aHooks.GetSize(); i++)
		delete m_aHooks[i];

	m_aHooks.RemoveAll();
}

//	Check name collision of bone node
bool CSkeletonCandidate::CheckNameCollision(const char* szName)
{
	//	Empty name will be treated as invalid name
	if (!szName || !szName[0])
		return true;

	int i;

	//	Check bones
	for (i=0; i < m_aBones.GetSize(); i++)
	{
		CBoneCandidate* pBone = m_aBones[i];
		if (!stricmp(pBone->GetName(), szName))
			return true;
	}

	//	Check hooks
	for (i=0; i < m_aHooks.GetSize(); i++)
	{
		CBoneCandidate* pBone = m_aHooks[i];
		if (!stricmp(pBone->GetName(), szName))
			return true;
	}

	return false;
}

bool CSkeletonCandidate::AddNode(CMAXNode* pNode, int iParent, bool bSelected)
{
	if (!pNode)
		return false;

	MAXNODETYPE Type = pNode->GetType();

	if (Type == NODE_HITBOX || Type == NODE_PHYSHAPE)
		return true;

	bool bFakeBone = false;
	if (Type == NODE_MESH && !g_pMAXInterface->HasPhysiqueOrSkinModifier(pNode->GetINode()))
		bFakeBone = true;

	//	Check if the node is a candidate
	if (bSelected && (Type == NODE_BONE || bFakeBone || Type == NODE_DUMMY || Type == NODE_HOOK))
	{
		//	Check name collision at first
		if (CheckNameCollision(pNode->GetName()))
		{
			AUX_OutputMessage("¹Ç÷ÀÃüÃû³åÍ» [%s]", pNode->GetName());
			return false;
		}

		//	Allocate a new bone candidate
		CBoneCandidate* pBoneCandidate = new CBoneCandidate;
		if (!pBoneCandidate)
			return false;

		if (Type != NODE_HOOK)
		{
			//	Create the bone candidate
			if (!pBoneCandidate->Create(m_aBones.GetSize(), iParent, pNode, bFakeBone))
			{
				delete pBoneCandidate;
				return false;
			}

			//	Insert node element into hierarchy
			m_aBones.Add(pBoneCandidate);

			//	Insert node element id into parent node element
			if (iParent == -1)
			{
				//	no parent -> this is a root node
				m_RootBoneList.AddTail(pBoneCandidate->GetId());
			}
			else
			{
				//	Valid parent -> this is a child node
				CBoneCandidate* pParent = m_aBones[iParent];
				pParent->AddChildId(pBoneCandidate->GetId());
			}
		}
		else	//	if Type == NODE_HOOK
		{
			//	Create the bone candidate
			if (!pBoneCandidate->Create(m_aHooks.GetSize(), iParent, pNode, bFakeBone))
			{
				delete pBoneCandidate;
				return false;
			}

			m_aHooks.Add(pBoneCandidate);

			if (iParent != -1)
			{
				CBoneCandidate* pParent = m_aBones[iParent];
				pParent->AddHookId(pBoneCandidate->GetId());
			}

			//	Is hook, stop here
			pBoneCandidate->SetHookFlag(true);
			return true;
		}

		//	Set parent id for the children
		iParent = pBoneCandidate->GetId();
	}

	//	Handle all children of the node
	for (int i=0; i < pNode->GetChildCount(); i++)
	{
		CMAXNode* pChild = pNode->GetChild(i);
		bool bSel;

		if (bSelected)
			bSel = true;
		else
			bSel = (pChild->GetINode()->Selected() == TRUE) ? true : false;

		if (!AddNode(pChild, iParent, bSel))
		{
			delete pChild;
			return false;
		}
	}

	return true;
}

bool CSkeletonCandidate::AddNode(A3DSkeleton* pA3DSkeleton, A3DBone* pBone, int iParent)
{
	if (!pBone)
	{
		a_SendLogToDebugWnd(1, "CSkeletonCandidate::AddNode, Invalid handle!");
		return false;
	}

	//	Find the node with the name of the core bone
	CMAXNode* pNode = g_pMAXInterface->GetNode(pBone->GetName());
	if (!pNode)
	{
		a_SendLogToDebugWnd(1, "CSkeletonCandidate::AddNode, Failed to find node %s !", pBone->GetName());
		return false;
	}

	bool bMatchBone = true;
	int iNodeType = pNode->GetType();

	//	Check if the node is a candidate
	if (!pBone->IsFakeBone())
	{
		if (iNodeType != NODE_BONE && iNodeType != NODE_DUMMY)
			bMatchBone = false;
	}
	else	//	Is fake bone
	{
		if (iNodeType != NODE_MESH || g_pMAXInterface->HasPhysiqueOrSkinModifier(pNode->GetINode()))
			bMatchBone = false;
	}

	if (!bMatchBone)
	{
		delete pNode;
		a_SendLogToDebugWnd(1, "CSkeletonCandidate::AddNode, Failed find matched bone %s in skeleton!", pBone->GetName());
		return false;
	}

	//	Allocate a new bone candidate
	CBoneCandidate* pBoneCandidate = new CBoneCandidate;
	if (!pBoneCandidate)
	{
		delete pNode;
		a_SendLogToDebugWnd(1, "CSkeletonCandidate::AddNode, Not enough memory !");
		return false;
	}

	//	Get bone's position in the A3D skeleton
	int iSkeletonBone;
	pA3DSkeleton->GetBone(pBone->GetName(), &iSkeletonBone);

	//	Create the bone candidate
	if (!pBoneCandidate->Create(iSkeletonBone, iParent, pNode, pBone->IsFakeBone()))
	{
		delete pNode;
		delete pBoneCandidate;
		a_SendLogToDebugWnd(1, "CSkeletonCandidate::AddNode, Failed to create bone candidate !");
		return false;
	}

	//	Insert bone candidate in the same order as it's in the A3D skeleton
	m_aBones[iSkeletonBone] = pBoneCandidate;

	//	Insert node element id into parent node element
	if (iParent < 0)
	{
		//	no parent -> this is a root node
		m_RootBoneList.AddTail(pBoneCandidate->GetId());
	}
	else
	{
		//	Valid parent -> this is a child node
		CBoneCandidate* pParent = m_aBones[iParent];
		pParent->AddChildId(pBoneCandidate->GetId());
	}

	//	Handle all children of the bone
	for (int i=0; i < pBone->GetChildNum(); i++)
	{
		if (!AddNode(pA3DSkeleton, pBone->GetChildPtr(i), pBoneCandidate->GetId()))
			return false;
	}

	return true;
}

//	Get selected parent of specified bone
int CSkeletonCandidate::GetParentSelectedId(bool bBone, int iIndex)
{
	CBoneCandidate* pBoneCandidate;

	if (bBone)
	{
		//	Check if the bone candidate id is valid
		if (iIndex < 0 || iIndex >= m_aBones.GetSize())
			return -1;

		//	Get the bone candidate
		pBoneCandidate = m_aBones[iIndex];
	}
	else
	{
		//	Check if the bone candidate id is valid
		if (iIndex < 0 || iIndex >= m_aHooks.GetSize())
			return -1;

		//	Get the bone candidate
		pBoneCandidate = m_aHooks[iIndex];
	}

	//	Get the parent id
	int iParent = pBoneCandidate->GetParentId();

	//	Loop until we found the selected parent
	while (iParent != -1)
	{
		//	Get the parent bone candidate
		pBoneCandidate = m_aBones[iParent];

		//	Check id we found a selected parent
		if (pBoneCandidate->IsSelected())
			return pBoneCandidate->GetSelectedId();

		//	Get the parent id
		iParent = pBoneCandidate->GetParentId();
	}

	//	Bone candidate is at root within selected ones
	return -1;
}

//	Build selected ID of bones. Return number of selected bone
int CSkeletonCandidate::BuildSelectedId()
{
	m_iNumSelBone = 0;

	for (int iBone=0; iBone < m_aBones.GetSize(); iBone++)
	{
		CBoneCandidate* pBoneCandidate = m_aBones[iBone];

		if (pBoneCandidate->IsSelected())
			pBoneCandidate->SetSelectedId(m_iNumSelBone++);
	}

	return m_iNumSelBone;
}

//	Get specified node's transform matrix relative to parent node
void CSkeletonCandidate::GetTranslationAndRotation(bool bBone, int iIndex, float fTime, A3DMATRIX4& mat)
{
	CBoneCandidate* pBoneCandidate;

	if (bBone)
	{
		//	Check if the bone candidate id is valid
		if (iIndex < 0 || iIndex >= m_aBones.GetSize())
			return;

		//	Get the bone candidate
		pBoneCandidate = m_aBones[iIndex];
	}
	else
	{
		//	Check if the bone candidate id is valid
		if (iIndex < 0 || iIndex >= m_aHooks.GetSize())
			return;

		//	Get the bone candidate
		pBoneCandidate = m_aHooks[iIndex];
	}

	//	Get the node of the bone candidate
	CMAXNode* pNode = pBoneCandidate->GetNode();

	//	Get the parent id
	int iParent = pBoneCandidate->GetParentId();

	//	Get the node of the parent bone candidate
	CMAXNode* pParentNode = iParent < 0 ? NULL : m_aBones[iParent]->GetNode();

	//	Get the translation and rotation of the node
	g_pMAXInterface->GetTranslationAndRotation(pNode, pParentNode, fTime, mat);
}

//	Get specified node's transform matrix in bone space
void CSkeletonCandidate::GetTranslationAndRotationBoneSpace(bool bBone, int iIndex, float fTime, A3DMATRIX4& mat)
{
	CBoneCandidate* pBoneCandidate;

	if (bBone)
	{
		//	Check if the bone candidate id is valid
		if (iIndex < 0 || iIndex >= m_aBones.GetSize())
			return;

		//	Get the bone candidate
		pBoneCandidate = m_aBones[iIndex];
	}
	else
	{
		//	Check if the bone candidate id is valid
		if (iIndex < 0 || iIndex >= m_aHooks.GetSize())
			return;

		//	Get the bone candidate
		pBoneCandidate = m_aHooks[iIndex];
	}

	//	Get the node of the bone candidate
	CMAXNode* pNode = pBoneCandidate->GetNode();

	//	Get the bone space translation and rotation of the node
	g_pMAXInterface->GetTranslationAndRotationBoneSpace(pNode, fTime, mat);
}

//	Check bone name collision
bool CSkeletonCandidate::CheckBoneNameCollision()
{
	int i, j;

	for (i=0; i < m_aBones.GetSize()-1; i++)
	{
		CBoneCandidate* pBoneCandidate1 = m_aBones[i];
		if (!pBoneCandidate1->IsSelected())
			continue;

		for (j=i+1; j < m_aBones.GetSize(); j++)
		{
			CBoneCandidate* pBoneCandidate2 = m_aBones[j];
			if (!pBoneCandidate2->IsSelected())
				continue;

			if (!stricmp(pBoneCandidate1->GetName(), pBoneCandidate2->GetName()))
				return true;
		}
	}

	return false;
}

//	Get bone's ID through it's name
int CSkeletonCandidate::GetBoneId(const char* szName)
{
	//	Loop through all bone candidates
	for (int i=0; i < m_aBones.GetSize(); i++)
	{
		CBoneCandidate* pBone = m_aBones[i];
		if (!strcmp(pBone->GetName(), szName))
			return i;
	}

	return -1;
}

//	Get bone's selelcted ID through it's name
int CSkeletonCandidate::GetBoneSelectedId(const char* szName)
{
	int iBone = GetBoneId(szName);
	if (iBone < 0)
		return -1;

	CBoneCandidate* pBone = m_aBones[iBone];
	if (!pBone->IsSelected())
		return -1;

	return pBone->GetSelectedId();
}

//	Get bone by selected id
CBoneCandidate* CSkeletonCandidate::GetBoneCandidateBySelectedID(int idSelected)
{
	for (int i=0; i < m_aBones.GetSize(); i++)
	{
		CBoneCandidate* pBone = m_aBones[i];
		if (pBone->GetSelectedId() == idSelected)
			return pBone;
	}

	return NULL;
}

//	Set bone init matrices
bool CSkeletonCandidate::SetBoneInitMatrices()
{
	APtrArray<CMAXNode*> aMeshNodes;
	APtrArray<CMAXMesh*> aMeshes;

	CMAXNode* pRootNode = g_pMAXInterface->GetRootNode();
	int i, iNumChild = pRootNode->GetChildCount();

	for (i=0; i < iNumChild; i++)
	{
		CMAXNode* pNode = pRootNode->GetChild(i);

		if (!AddMeshNode(pNode, aMeshNodes))
			delete pNode;
	}

	delete pRootNode;

	//	Check if one (and only one!) node/mesh is selected
	if (!aMeshNodes.GetSize())
	{
		a_SendLogToDebugWnd(0, "CSkeletonCandidate::SetBoneInitMatrices, No mesh was selected !");
		return false;
	}

	//	Create meshes form mesh node
	for (i=0; i < aMeshNodes.GetSize(); i++)
	{
		CMAXNode* pNode = aMeshNodes[i];
		CMAXMesh* pMesh = new CMAXMesh;
		ASSERT(pMesh);

		pMesh->CreateSimple(pNode->GetINode());
		delete pNode;

		aMeshes.Add(pMesh);
	}

	//	Set bone init matrix
	for (i=0; i < aMeshes.GetSize(); i++)
	{
		CMAXMesh* pMesh = aMeshes[i];
		pMesh->SetBoneInitTM(this);
		delete pMesh;
	}

	return true;
}

//	Add mesh nodes. Return true if pMMAXNode is a mesh node
bool CSkeletonCandidate::AddMeshNode(CMAXNode* pMAXNode, APtrArray<CMAXNode*>& aMeshes)
{
	bool bRet = false;

	if (pMAXNode->GetType() == NODE_MESH)
	{
		aMeshes.Add(pMAXNode);
		bRet = true;
	}
	
	int i, iNumChild = pMAXNode->GetChildCount();

	for (i=0; i < iNumChild; i++)
	{
		CMAXNode* pChild = pMAXNode->GetChild(i);

		//	Recursively add the node to the skeleton candidate
		if (!AddMeshNode(pChild, aMeshes))
			delete pChild;
	}

	return bRet;
}

//	Sort all bones
void CSkeletonCandidate::SortBones()
{
	APtrArray<CBoneCandidate*> aTempBnoes;
	AArray<int, int> aMaps;

	aTempBnoes.SetSize(m_aBones.GetSize(), 10);
	aMaps.SetSize(m_aBones.GetSize(), 10);

	int i;
	for (i=0; i < m_aBones.GetSize(); i++)
		aTempBnoes[i] = m_aBones[i];

	//	Sort bones in increasing order base on their name
	qsort(aTempBnoes.GetData(), aTempBnoes.GetSize(), sizeof (CBoneCandidate*), _BoneCompare);

	for (i=0; i < m_aBones.GetSize(); i++)
	{
		CBoneCandidate* pBone = aTempBnoes[i];
		aMaps[pBone->GetId()] = i;
	}

	//	Remap bone id, parent id and children ids
	for (i=0; i < m_aBones.GetSize(); i++)
	{
		CBoneCandidate* pBone = aTempBnoes[i];
		pBone->SetId(i);

		if (pBone->GetParentId() >= 0)
			pBone->SetParentId(aMaps[pBone->GetParentId()]);

		for (int j=0; j < pBone->GetChildNum(); j++)
		{
			int iChild = pBone->GetChildID(j);
			pBone->SetChildID(j, aMaps[iChild]);
		}
	}

	//	Remap hook's parent id
	for (i=0; i < m_aHooks.GetSize(); i++)
	{
		CBoneCandidate* pHook = m_aHooks[i];
		if (pHook->GetParentId() >= 0)
			pHook->SetParentId(aMaps[pHook->GetParentId()]);
	}

	//	Set new bone array
	for (i=0; i < m_aBones.GetSize(); i++)
		m_aBones[i] = aTempBnoes[i];
}

