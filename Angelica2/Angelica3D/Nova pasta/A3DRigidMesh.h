/*
 * FILE: A3DRigidMesh.h
 *
 * DESCRIPTION: A3D rigid mesh class
 *
 * CREATED BY: duyuxin, 2003/10/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DRIGIDMESH_H_
#define _A3DRIGIDMESH_H_

#include "A3DMeshBase.h"
#include "A3DVertex.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DStream;
class A3DTexture;
class A3DMaterial;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DRigidMesh
//
///////////////////////////////////////////////////////////////////////////

class A3DRigidMesh : public A3DMeshBase
{
public:		//	Types

	//	Struct used to save and load rigid mesh
	struct RIGIDMESHDATA
	{
		int		iBoneIdx;	//	Index of bone which this mesh linked to
		int		iTexture;	//	Texture index
		int		iMaterial;	//	Material index
		int		iNumVert;	//	Number of vertex
		int		iNumIdx;	//	Number of index
	};

public:		//	Constructors and Destructors

	A3DRigidMesh();
	virtual ~A3DRigidMesh();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();

	//	Load skin mesh data form file
	virtual bool Load(AFile* pFile, A3DSkin* pSkin);
	//	Save skin mesh data from file
	virtual bool Save(AFile* pFile);

	//	Render mesh
	virtual bool Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin);

	//	Get approximate mesh data size
	virtual int GetDataSize();

	//	Create original data buffers
	bool CreateOriginalBuffers(int iNumVert, int iNumIdx);

	//	Set static buffer flag. Call this function before Load()
	void SetStaticBufferFlag(bool bStatic) { m_bStaticBuf = bStatic; }
	//	Get static buffer flag
	bool GetStaticBufferFlag() { return m_bStaticBuf; }

	//	Set / Get Index of bone which this mesh linked to
	void SetBoneIndex(int iIndex) { m_iBoneIdx = iIndex; }
	int GetBoneIndex() { return m_iBoneIdx; }

	//	Get vertex buffer which contains original vertex data
	A3DVERTEX* GetOriginVertexBuf() { return m_aVerts; }
	//	Get index buffer which contains original index data
	WORD* GetOriginIndexBuf() { return m_aIndices; }

protected:	//	Attributes

	A3DVERTEX*		m_aVerts;			//	Original vertex data
	WORD*			m_aIndices;			//	Original index data
	A3DStream*		m_pA3DStream;		//	Stream used in hardware rendering

	bool			m_bStaticBuf;		//	Static buffer flag
	int				m_iBoneIdx;			//	Index of bone which this mesh linked to

protected:	//	Operations

	//	Software rendering
	virtual bool RenderSoftware(A3DViewport* pViewport, A3DSkinModel* pSkinModel);

	//	Release original data buffers
	void ReleaseOriginalBuffers();
	//	Create A3D stream
	bool CreateStream();
	//	Calculate initial mesh aabb
	void CalcInitMeshAABB();
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DRIGIDMESH_H_

