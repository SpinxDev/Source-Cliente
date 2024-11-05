/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

		an abstract class which manages runtime creating/releasing a set 
	of terrain quad objects

		since both APhysXTerrain and APhysXTerrainBlock have the function of managing
	a number of 2D sub-blocks, and they have some many common operations, so that
	It is necessary for me to refactor these two classes into a common class
	

----------------------------------------------------------------------*/

#ifndef _APHYSXTERRAINQUADMANAGER_H_
#define _APHYSXTERRAINQUADMANAGER_H_


class APHYSX_DLL_EXPORT APhysXTerrainQuadManager
{

public:

	APhysXTerrainQuadManager() 
	{
		m_fOffsetX = 0.0f;
		m_fOffsetZ = 0.0f;

		m_iMaxCachedQuadNum = 10;

		m_vSimCenter.x = m_vSimCenter.z = NX_MIN_F32;
		m_vSimCenter.y = 0.0f;

		m_fSimRadius = 0.0f;

		m_iCurObsoleteQuadIndex = 0;
	}
	~APhysXTerrainQuadManager() { Release(); }
	
	virtual void Release();
	virtual void ForceReleaseCacheQuads();

	void SetMaxCachedQuadNum(int iMax) { m_iMaxCachedQuadNum = iMax; }

	// called each frame to update the active terrain patch rb object list
	virtual bool UpdateSimCenter(const NxVec3& vCenter);		// return false if I can be released, for the case of APhysXTerrainBlock
	virtual void SetSimRadius(float fRadius);

	
//--------------------------------------------------------------------------------------------------------------
// static functions...

	static bool Sphere2DOverlapTest(const NxVec3& vCenter1, const NxVec3& vCenter2, float fSqrSumRadius)
	{
		return Sphere2DOverlapTest(vCenter1.x, vCenter1.z, vCenter2.x, vCenter2.z, fSqrSumRadius);
	}

	static bool Sphere2DOverlapTest(float c1x, float c1z, float c2x, float c2z, float fSqrSumRadius)
	{
		float sx = c1x - c2x;
		float sz = c1z - c2z;

		return ( sx * sx + sz * sz < fSqrSumRadius );
	}

	// we will ignore the y coordinate in 2D test...
	static bool SphereBox2DOverlapTest(const NxVec3& vSphereCenter, float fRadius, const NxVec3& vBoxMin, const NxVec3& vBoxMax);

public:

	//-----------------------------------------------------------
	// interfaces which must be overridden...
	virtual int GetRowNumOfQuads() = 0;
	virtual int GetColNumOfQuads() = 0;

	virtual float GetQuadSizeX() = 0;
	virtual float GetQuadSizeZ() = 0;

	virtual void ReleaseQuad(void* pQuad) = 0; 
	virtual void* LoadQuad(int iRow, int iCol) = 0;

	//-----------------------------------------------------------
	// we will ignore the y component of vOffset
	void SetOffset(const NxVec3& vOffset)
	{
		SetOffset(vOffset.x, vOffset.z);
	}

	void SetOffset(float x, float z)
	{
		m_fOffsetX = x;
		m_fOffsetZ = z;
	}

	void SetCenterOffset(const NxVec3& vOffset)
	{
		SetCenterOffset(vOffset.x, vOffset.z);
	}

	void SetCenterOffset(float x, float z)
	{
		SetOffset(x - GetQuadSizeX() * GetColNumOfQuads() * 0.5f, z - GetQuadSizeZ() * GetRowNumOfQuads() * 0.5f);
	}

	void GetQuadRowAndColFromLocalPos(const NxVec3& vLocalPos, int& row, int& col)
	{
		col = (int) (vLocalPos.x / GetQuadSizeX());
		row = (int) (vLocalPos.z / GetQuadSizeZ());
	}

	void GetQuadRowAndColFromPos(const NxVec3& vPos, int& row, int& col)
	{
		GetQuadRowAndColFromLocalPos(vPos - NxVec3(m_fOffsetX, 0.0f, m_fOffsetZ), row, col);
	}

	// return the left-bottom pos of the patch
	NxVec3 GetQuadPos(int iRow, int iCol)
	{
		NxVec3 vPos(0.0f);
		vPos.x = GetQuadSizeX() * iCol  + m_fOffsetX;
		vPos.z = GetQuadSizeZ() * iRow  + m_fOffsetZ;
		return vPos;
	}

	NxVec3 GetQuadCenterPos(int iRow, int iCol)
	{
		NxVec3 vCenter = GetQuadPos(iRow, iCol);
		vCenter.x += GetQuadSizeX() * 0.5f;
		vCenter.z += GetQuadSizeZ() * 0.5f;

		return vCenter;
	}

	bool IsValidTerrainQuad(int iRow, int iCol)
	{
		return ( iRow < GetRowNumOfQuads() && iRow >= 0 && iCol < GetColNumOfQuads() && iCol >= 0);
	}

	virtual void CreateTerrainQuad(int iRow, int iCol);

protected:

	void IncreaseCurObsoleteQuadIndex()
	{
		m_iCurObsoleteQuadIndex++;
		if(m_iCurObsoleteQuadIndex == (int)m_arrCachedQuads.size())
			m_iCurObsoleteQuadIndex = 0;
	}

	void HitCache(int id)
	{
		// reset the mCacheCounter to 1
		m_arrCachedQuads[id].mCacheCounter = 1;

		if(id != m_iCurObsoleteQuadIndex)
		{
			APhysXTerrainQuadItem tmpQuadItem = m_arrCachedQuads[id];
			m_arrCachedQuads[id] = m_arrCachedQuads[m_iCurObsoleteQuadIndex];
			m_arrCachedQuads[m_iCurObsoleteQuadIndex] = tmpQuadItem;
		}

		IncreaseCurObsoleteQuadIndex();
	}

	int GetObsoleteQuadIndex()
	{
		return m_iCurObsoleteQuadIndex;
	}

	// for multithreaded loading...
	virtual void UpdateCacheQuads() {}

protected:

/*
	int m_iRowNumOfQuads;
	int m_iColNumOfQuads;

	float m_fQuadSizeX;
	float m_fQuadSizeZ;
*/

	float m_fOffsetX;
	float m_fOffsetZ;

	// for physics simulation management...
	NxVec3 m_vSimCenter;

	// some temp data saved to optimize the update of simulate center
	float m_fSimRadius;
	float m_fQuadSphereRadius;			// we use the bounding-sphere of patch rect to test
	
	float m_fSqrSumRadius;

	int m_iMaxRowRange;
	int m_iMaxColRange;

public:
	struct APhysXTerrainQuadItem
	{
		int mRow;
		int mCol;
		APhysXU32 mCacheCounter;		
		void* mTerrainQuad;

		APhysXTerrainQuadItem()
		{
			mTerrainQuad = NULL;
			mCacheCounter = 1;
		}
	};

protected:
	//  terrain patch object cache list
	APhysXArray<APhysXTerrainQuadItem> m_arrCachedQuads;
	int m_iMaxCachedQuadNum;
	int m_iCurObsoleteQuadIndex;

};


#endif