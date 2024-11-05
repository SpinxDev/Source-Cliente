/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXCLOTHOBJECT_H_
#define _APHYSXCLOTHOBJECT_H_


class APHYSX_DLL_EXPORT APhysXClothObject: public APhysXObject
{

	// friend class
	friend class APhysXScene;


protected:

	// make the constructor "protected" so that the user can not instantialize it directly
	
	APhysXClothObject()
	{
		m_pNxCloth = NULL;
		m_pNxClothMesh = NULL;

		m_uCurVertexNum = 0;
		m_uCurIndexNum = 0;
		m_uCurParentIndexNum = 0;
		m_uMeshDirtyFlags = 0;

		m_bEnableConstraints = false;

		m_BufConstraintPos = NULL;
		m_BufConstraintPosVel = NULL;

		m_fSubSimulationDtCounter = 0.0f;
		m_uLastSetConstraintPosFrame = 0;
	};
	

public:

	NxCloth* GetNxCloth() { return m_pNxCloth; }

	// for render to fetch the physics data ...
	APhysXU32 GetVertexNum() { return m_uCurVertexNum; }
	const NxVec3* GetVertexPositions() { return (NxVec3 *) m_ReceiveBuffers.verticesPosBegin; }
	const NxVec3* GetVertexNormals() { return (NxVec3 *) m_ReceiveBuffers.verticesNormalBegin; }
	
	// this should be constant during the simulation even if the cloth is teared...
	APhysXU32 GetIndexNum() { return m_uCurIndexNum; }
	const APhysXU32* GetIndices() { return (APhysXU32 *) m_ReceiveBuffers.indicesBegin; }

	APhysXU32 GetParentIndexNum() { return m_uCurParentIndexNum; }
	const APhysXU32* GetParentIndices() { return (APhysXU32 *) m_ReceiveBuffers.parentIndicesBegin; }

	virtual void PutToSleep();
	virtual void WakeUp();
	virtual bool IsSleeping()
	{ 
		if(m_pNxCloth) 
			return m_pNxCloth->isSleeping();
		else
			return true;
	}

	virtual void SetPose(const NxMat34& globalPose);

	virtual NxMat34 GetPose() const;
	virtual void GetAABB(NxBounds3& aabb) const;

	virtual void SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef = true);
	virtual void SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup);

	virtual void EnableCollision(bool bEnable = true);

	virtual void EnableDebugRender(bool bEnable = true);

	bool IsConstraintsEnabled() const { return m_bEnableConstraints; }

	void SetWindAcceleration(const NxVec3& vWindAcceleration);

	//--------------------------------------------------------------------------------------------
	// for cloth constrain feature... 
	// call these functions before PhysX's simulate()...
	void SetConstrainPositions(NxVec3* vPosBuf);
	void SetConstrainNormals(NxVec3* vNBuf);

	void ExecSubSimulationTask(float dt);

protected:

	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

	virtual void OnReleaseBasicPhysXStuff();


	void AllocateReceiveBuffers(int iMaxVertNum, int iMaxTriNum, bool bTearable = false);
	void ReleaseReceiveBuffers();

protected:
	
	NxCloth* m_pNxCloth;				// NxCloth object...

	NxClothMesh* m_pNxClothMesh;		// NxClothMesh reference...

	//--------------------------------------------------------------------------------------------
	// the vertex, index and other data which are passed back for renderer after simulation
	NxMeshData m_ReceiveBuffers;		

	APhysXU32 m_uCurVertexNum;
	APhysXU32 m_uCurIndexNum;
	APhysXU32 m_uCurParentIndexNum;

	APhysXU32 m_uMeshDirtyFlags;

	bool m_bEnableConstraints;

	//--------------------------------------------------------------------------------------------
	// data buffer for constraint cloth
	NxVec3* m_BufConstraintPos;
	NxVec3* m_BufConstraintPosVel;

	float m_fSubSimulationDtCounter;
	APhysXU32 m_uLastSetConstraintPosFrame;

};


#endif