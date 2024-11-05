
#include "APhysXStdAfx.h"

void APhysXTerrainQuadManager::ForceReleaseCacheQuads()
{

	if(m_arrCachedQuads.size())
	{
		for(int i=0; i<m_arrCachedQuads.size(); i++)
		{
			ReleaseQuad(m_arrCachedQuads[i].mTerrainQuad);
		}

		m_arrCachedQuads.clear();
		m_iCurObsoleteQuadIndex = 0;
	}
}

void APhysXTerrainQuadManager::Release()
{
	ForceReleaseCacheQuads();
}

void APhysXTerrainQuadManager::CreateTerrainQuad(int iRow, int iCol)
{
	// first, search the terrain patch rb object list
	for(int i=0; i<m_arrCachedQuads.size(); i++)
	{
		if(iRow == m_arrCachedQuads[i].mRow && iCol == m_arrCachedQuads[i].mCol)
		{
			// found and return
			HitCache(i);			// update the m_iCurObsoleteQuadIndex so that we won't remove the latest added quad item!
			return;
		}
	}

	// try to create a terrain patch rb object
	void* pQuad = LoadQuad(iRow, iCol);
	// APHYSX_ASSERT(pQuad);
	if(!pQuad) return;		// if fail to load a new quad, return directly...

	// push back the created obj to the list
	if(m_arrCachedQuads.size() == m_iMaxCachedQuadNum)
	{
		// ok, the cache is full, we should remove some patch inactive
		int iInactiveQuadID = GetObsoleteQuadIndex();
		
		if(iInactiveQuadID == -1)	
		{
			//??? cache quad list is full and all quads are active...
			// in this case, maybe I should increase the m_iMaxCachedQuadNum?
			APHYSX_ASSERT(0);
			return;					
		}

		// release the inactive patch object
		void* pInactiveQuad = m_arrCachedQuads[iInactiveQuadID].mTerrainQuad;
		ReleaseQuad(pInactiveQuad);

		m_arrCachedQuads[iInactiveQuadID].mRow = iRow;
		m_arrCachedQuads[iInactiveQuadID].mCol = iCol;
		m_arrCachedQuads[iInactiveQuadID].mTerrainQuad = pQuad;

		IncreaseCurObsoleteQuadIndex();
	}
	else
	{
		APhysXTerrainQuadItem apxNewTerrainQuadItem;
		apxNewTerrainQuadItem.mRow = iRow;
		apxNewTerrainQuadItem.mCol = iCol;
		apxNewTerrainQuadItem.mTerrainQuad = pQuad;
		m_arrCachedQuads.push_back(apxNewTerrainQuadItem);

	}

}

bool APhysXTerrainQuadManager::UpdateSimCenter(const NxVec3& vCenter)
{
	if(m_fSimRadius == 0.0f)		// not set sim radius yet?
		return true;

#define TMP_EPSILON 0.001f
	
	if(NxMath::equals(vCenter.x, m_vSimCenter.x, TMP_EPSILON) && NxMath::equals(vCenter.z, m_vSimCenter.z, TMP_EPSILON)	&& m_arrCachedQuads.size())
	{
		// vCenter is so close to the last sim center ( we only consider x, z component), so we ignore the update
		// while, at the first time, if m_arrCachedQuads.size() is zero which means that we have not loaded any terrain,
		// so we should still update the terrain
		return true;
	}

	m_vSimCenter = vCenter;

#undef TMP_EPSILON

	// first, we locate the patch id of vCenter
	int iQuadRow, iQuadCol;
	GetQuadRowAndColFromPos(vCenter, iQuadRow, iQuadCol);

#if 1

	bool bAllQuadInvalid = true;
	bool bAllQuadNoOverlap = true;

	//-----------------------------------------------------------------------
	// our iteration strategy:
	//		visit the quads which are around the center quad within the distance 
	// range of m_iMaxRowRange and m_iMaxColRange. However  not in the order from 
	// the nearer to the father, but in the order of per row and per column. So
	// You should make sure that the cached list is enough...

	for(int i = iQuadRow - m_iMaxRowRange; i<= iQuadRow + m_iMaxRowRange; i++)
	{
		for(int j = iQuadCol - m_iMaxColRange; j<= iQuadCol + m_iMaxColRange; j++)
		{
			if(!IsValidTerrainQuad(i, j))
				continue;

			bAllQuadInvalid = false;
			
			//--------------------------------------------------------------------------
			// Note by wenfeng, March 17, 2010
			// here, we should use the exact 2D sphere-box overlap test instead...
#if 0
			NxVec3 vQuadCenter = GetQuadCenterPos(i, j);
			if(Sphere2DOverlapTest(vQuadCenter, vCenter, m_fSqrSumRadius))
#else
			NxVec3 vQuadMin = GetQuadPos(i, j);
			NxVec3 vQuadMax(vQuadMin.x + GetQuadSizeX(), 0.0f, vQuadMin.z + GetQuadSizeZ());
			if(SphereBox2DOverlapTest(vCenter, m_fSimRadius, vQuadMin, vQuadMax))
#endif	
			{
				CreateTerrainQuad(i, j);
				bAllQuadNoOverlap = false;
			}
		}

	}

	// in multi-threaded loading case, we should try to update cache quads here...
	UpdateCacheQuads();

	//if(bAllQuadInvalid)
	if(bAllQuadNoOverlap)
	{
		ForceReleaseCacheQuads();
		// in fact, in this case, I can be released...
		return false;
	}
	else
		return true;

#else

	for(int i = m_iMaxRowRange; i>0; i--)
	{

	}



#endif

}

void APhysXTerrainQuadManager::SetSimRadius(float fRadius)
{
	// try to compute which patches are in the range of fRadius
	if(m_fSimRadius == fRadius)
		return;
	
	m_fSimRadius = fRadius;

	m_fSqrSumRadius = m_fSimRadius + m_fQuadSphereRadius;
	m_fSqrSumRadius *= m_fSqrSumRadius;

	m_iMaxRowRange = 1 + (int) (fRadius / GetQuadSizeZ());
	m_iMaxColRange = 1 + (int) (fRadius / GetQuadSizeX());

	//--------------------------------------------------------------------------------
	//??? there may be some risks in the following snippets ???
	// if the fRadius is much larger than the patch size, and the patches inside the range
	// are more than m_iMaxTerrainPatchObjNum, this incurs that the kernel patch(where the 
	// vCenter lays) is not push back to the cache list yet while the cache list reach its
	// max number...
	// now, I just walk around it by enlarge the value of m_iMaxTerrainPatchObjNum
	
	//--------------------------------------------------------------------------------
	// Note by wenfeng, March 16, 2010...
	// how to compute the exact Max Cached Quad Number is a great issue...

	// this is a very coarse computation...
	int iMaxPatchNum = (2 * m_iMaxRowRange + 1) * ( 2 * m_iMaxColRange + 1);
	
	// the following method is disabled since it seems no to be correct...
	// and the following are some law found by me, it's cool but hard to conclude to some equations...

	// we always assume that the quad is square but not a common rectangle...
	float fFactor = fRadius / GetQuadSizeX();


#define SQRT_TWO		1.414213562373f
#define HALF_SQRT_TWO	0.7071f
#define HALF			0.5f

	if(fFactor < HALF * 1) 
		iMaxPatchNum = 4;					// 2 * 2
//	else if(fFactor < HALF_SQRT_TWO * 1)
//		iMaxPatchNum = 6;					// 2 * 3
	else if(fFactor < HALF * 2)
		iMaxPatchNum = 9;					// 3 * 3
//	else if(fFactor < NxMath::sqrt(2.0f))
//		iMaxPatchNum = 12;					// 3 * 4
	else if(fFactor < HALF * 3)
		iMaxPatchNum = 16;					// 4 * 4
//	else if(fFactor < NxMath::sqrt(3.0f))
//		iMaxPatchNum = 20;					// 4 * 5
	else if(fFactor < HALF * 4)
		iMaxPatchNum = 25;					// 5 * 5
//	else if(fFactor < NxMath::sqrt(5.0f))
//		iMaxPatchNum = 30;					// 5 * 6
	else if(fFactor < HALF * 5)
		iMaxPatchNum = 36;					// 6 * 6
//	else if(fFactor < NxMath::sqrt(6.0f))
//		iMaxPatchNum = 42;					// 6 * 7
	
	// to be continued...


	if (iMaxPatchNum > m_iMaxCachedQuadNum)
		m_iMaxCachedQuadNum = iMaxPatchNum;



}

bool APhysXTerrainQuadManager::SphereBox2DOverlapTest(const NxVec3& vSphereCenter, float fRadius, const NxVec3& vBoxMin, const NxVec3& vBoxMax)
{
	// aabb check...
	if(vSphereCenter.x - fRadius > vBoxMax.x)
		return false;
	else if(vSphereCenter.z - fRadius > vBoxMax.z)
		return false;
	else if(vSphereCenter.x + fRadius < vBoxMin.x)
		return false;
	else if(vSphereCenter.z + fRadius < vBoxMin.z)
		return false;

	// more complicated check...
	NxVec3 vBorder(0.0f);
	if(vSphereCenter.x > vBoxMax.x)
		vBorder.x = vBoxMax.x;
	else if(vSphereCenter.x < vBoxMin.x)
		vBorder.x = vBoxMin.x;
	else
		return true;

	if(vSphereCenter.z > vBoxMax.z)
		vBorder.z = vBoxMax.z;
	else if(vSphereCenter.z < vBoxMin.z)
		vBorder.z = vBoxMin.z;
	else
		return true;

	vBorder -= vSphereCenter;
	if(vBorder.x * vBorder.x + vBorder.z * vBorder.z > fRadius * fRadius)
		return false;
	else
		return true;


}