// DlgModelInfo.cpp : implementation file
//

#include "Global.h"
#include "DlgModelInfo.h"
#include "SkeletonEditor.h"
#include "Model.h"

#include <A3D.h>
#include <afi.h>

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgModelInfo dialog

CDlgModelInfo::CDlgModelInfo(CModel* pModel, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModelInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgModelInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pModel = pModel;
}


void CDlgModelInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgModelInfo)
	DDX_Control(pDX, IDC_EDIT_PROP, m_PropEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgModelInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgModelInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgModelInfo message handlers

BOOL CDlgModelInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();

	A3DSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();

	char szMsg[1024];
	int i, j;

	AddLine("Model\n");

	A3DVECTOR3 v = m_pModel->GetModelCenter();
	sprintf(szMsg, "+-- Center: %f, %f, %f\n", v.x, v.y, v.z);
	AddLine(szMsg);

	sprintf(szMsg, "+-- Radius: %f\n", m_pModel->GetModelRadius());
	AddLine(szMsg);

	//	Track data path
	AString strPath, strTckDir;
	af_GetFilePath(pSkinModel->GetFileName(), strPath);
	strTckDir = pSkinModel->GetTrackDataDir();
	if (strPath.GetLength() && strTckDir.GetLength())
		sprintf(szMsg, "+-- Track data path: %s\n", strPath + "\\" + strTckDir);
	else if (strPath.GetLength())
		sprintf(szMsg, "+-- Track data path: %s\n", strPath);
	else
		sprintf(szMsg, "+-- Track data path: %s\n", strTckDir);

	AddLine(szMsg);

	AddLine("\n");
	AddLine("===================================================================\n");
	AddLine("\n");

	//	Skeleton information
	A3DSkeleton* pSkeleton = m_pModel->GetSkeleton();
	if (pSkeleton)
	{
		sprintf(szMsg, "Skeleton: %s\n", pSkeleton->GetFileName());
		AddLine(szMsg);

		sprintf(szMsg, "+-- Bone number: %d\n", pSkeleton->GetBoneNum());
		AddLine(szMsg);

		sprintf(szMsg, "+-- Joint number: %d\n", pSkeleton->GetJointNum());
		AddLine(szMsg);

		sprintf(szMsg, "+-- Hook number: %d\n", pSkeleton->GetHookNum());
		AddLine(szMsg);

		AddLine("\n");
	}

	AddLine("===================================================================\n");
	AddLine("\n");

	//	Skin information
	sprintf(szMsg, "Skin number: %d\n", pSkinModel->GetSkinNum());
	AddLine(szMsg);
	
	for (j=0; j < pSkinModel->GetSkinNum(); j++)
	{
		AddLine("\n");
		AddLine("--------------------------------------------------------------------\n");
		AddLine("\n");

		A3DSkin* pSkin = pSkinModel->GetA3DSkin(j);
		if (!pSkin)
		{
			sprintf(szMsg, "Skin # %d: null\n");
			AddLine(szMsg);
			continue;
		}

		sprintf(szMsg, "Skin # %d: %s\n", j, pSkin->GetFileName());
		AddLine(szMsg);

		sprintf(szMsg, "+-- Material number: %d\n", pSkin->GetMaterialNum());
		AddLine(szMsg);

		//	Textures
		sprintf(szMsg, "+-- Texture number: %d\n", pSkin->GetTextureNum());
		AddLine(szMsg);

		for (i=0; i < pSkin->GetTextureNum(); i++)
		{
			A3DTexture* pTexture = pSkin->GetTexture(i);

			sprintf(szMsg, "+------ # %d: %s\n", i, pTexture->GetMapFile());
			AddLine(szMsg);
		}

		//	Skin meshes
		sprintf(szMsg, "+-- Skin mesh number: %d\n", pSkin->GetSkinMeshNum());
		AddLine(szMsg);

		for (i=0; i < pSkin->GetSkinMeshNum(); i++)
		{
			A3DSkinMesh* pMesh = pSkin->GetSkinMesh(i);
			sprintf(szMsg, "+------ # %d: %s, T[%d], M[%d], vertex: %d, face: %d, submesh: %d\n", 
				i, pMesh->GetName(), pMesh->GetTextureIndex(), pMesh->GetMaterialIndex(), pMesh->GetVertexNum(), 
				pMesh->GetIndexNum() / 3, pMesh->GetSubMeshNum());
			AddLine(szMsg);
		}

		//	Rigid meshes
		sprintf(szMsg, "+-- Rigid mesh number: %d\n", pSkin->GetRigidMeshNum());
		AddLine(szMsg);

		for (i=0; i < pSkin->GetRigidMeshNum(); i++)
		{
			A3DRigidMesh* pMesh = pSkin->GetRigidMesh(i);
			sprintf(szMsg, "+------ # %d: %s, T[%d], M[%d], vertex: %d, face: %d\n", 
				i, pMesh->GetName(), pMesh->GetTextureIndex(), pMesh->GetMaterialIndex(), 
				pMesh->GetVertexNum(), pMesh->GetIndexNum() / 3);
			AddLine(szMsg);
		}

		//	Morph skin meshes
		sprintf(szMsg, "+-- Morph skin mesh number: %d\n", pSkin->GetMorphSkinMeshNum());
		AddLine(szMsg);

		for (i=0; i < pSkin->GetMorphSkinMeshNum(); i++)
		{
			A3DMorphSkinMesh* pMesh = pSkin->GetMorphSkinMesh(i);
			sprintf(szMsg, "+------ # %d: %s, T[%d], M[%d], vertex: %d, face: %d, submesh: %d, channel: %d\n", 
				i, pMesh->GetName(), pMesh->GetTextureIndex(), pMesh->GetMaterialIndex(), 
				pMesh->GetVertexNum(), pMesh->GetIndexNum() / 3, pMesh->GetSubMeshNum(), pMesh->m_MorphData.GetChannelNum());
			AddLine(szMsg);
		}
	
		//	Morph rigid meshes
		sprintf(szMsg, "+-- Morph rigid mesh number: %d\n", pSkin->GetMorphRigidMeshNum());
		AddLine(szMsg);

		for (i=0; i < pSkin->GetMorphRigidMeshNum(); i++)
		{
			A3DMorphRigidMesh* pMesh = pSkin->GetMorphRigidMesh(i);
			sprintf(szMsg, "+------ # %d: %s, T[%d], M[%d], vertex: %d, face: %d, channel: %d\n", 
				i, pMesh->GetName(), pMesh->GetTextureIndex(), pMesh->GetMaterialIndex(),
				pMesh->GetVertexNum(), pMesh->GetIndexNum() / 3, pMesh->m_MorphData.GetChannelNum());
			AddLine(szMsg);
		}
		
		//	Supple meshes
		sprintf(szMsg, "+-- Supple mesh number: %d\n", pSkin->GetSuppleMeshNum());
		AddLine(szMsg);

		for (i=0; i < pSkin->GetSuppleMeshNum(); i++)
		{
			A3DSuppleMesh* pMesh = pSkin->GetSuppleMesh(i);
			sprintf(szMsg, "+------ # %d: %s, T[%d], M[%d], vertex: %d, face: %d, spring: %d\n", 
				i, pMesh->GetName(), pMesh->GetTextureIndex(), pMesh->GetMaterialIndex(),
				pMesh->GetVertexNum(), pMesh->GetIndexNum() / 3, pMesh->GetSpringNum());
			AddLine(szMsg);
		}
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgModelInfo::AddLine(const char* szMsg)
{
	m_PropEdit.SetSel(-1, -1);
	m_PropEdit.ReplaceSel(szMsg);
}
