
/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

	APhysXTerrain:
		
		a class which manages a number of terrain blocks

----------------------------------------------------------------------*/


#ifndef _APHYSXTERRAIN_H_
#define _APHYSXTERRAIN_H_


enum APhysXTerrainGlobalOrigin
{
	APX_TerrainGlobalOrigin_Center,
	APX_TerrainGlobalOrigin_LeftBottom
};

struct APhysXTerrainConfigs
{
	int mRowNumOfBlocks;
	int mColNumOfBlocks;

	float mBlockSizeX;
	float mBlockSizeZ;

	APhysXTerrainGlobalOrigin mTerrainGlobalOrigin;

	int mMaxCachedBlockNum;
	int mMaxCachedPatchNumOneBlock;
	float mSimRadius;

	const char* mTerrainBlockFilePrefix;
	
	bool mMultithreadedLoading;

	APhysXTerrainConfigs()
	{
		// default values...
		mRowNumOfBlocks = 0;
		mColNumOfBlocks = 0;

		mBlockSizeX = 1024.0f;
		mBlockSizeZ = 1024.0f;
		
		mTerrainGlobalOrigin = APX_TerrainGlobalOrigin_Center;

		mMaxCachedBlockNum = 4;
		mMaxCachedPatchNumOneBlock = 9;
		mSimRadius = 50.0f;
		
		mTerrainBlockFilePrefix = NULL;

		mMultithreadedLoading = false;
	}
};

class APhysXMutex;

//-----------------------------------------------------------
// define some flags for APhysXTerrain...
enum APhysXTerrainFlag
{
	// if this flag is raised, we will ignore the border blocks of terrain 
	// since in some projects the outer border blocks is just for  sightseeing,
	// so we can just ignore loading of them...
	APX_TERRAINFLAG_IGNORE_BORDER_BLOCK		=	1 << 0,

};


//-----------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXTerrain: public APhysXTerrainQuadManager
{

public:

	APhysXTerrain()
	{
		m_pAPhysXScene = NULL;
		m_szTerrainBlockPath = NULL;
		m_szTerrainBlockFilePrefix = NULL;
		
		m_bMultiThreadedLoading = false;

		m_pRequestLoadQueueMutex = NULL;
		m_pDoneLoadQueueMutex = NULL;

		m_hLoaderThread = NULL;

		m_uFlags = 0;
	}

	~APhysXTerrain()
	{
		Release();
	}

	virtual void Release()
	{
		if(m_bMultiThreadedLoading)
		{
			EndLoaderThread();

			m_bMultiThreadedLoading = false;
		}

		m_pAPhysXScene = NULL;
		m_szTerrainBlockPath = NULL;
		m_szTerrainBlockFilePrefix = NULL;

		APhysXTerrainQuadManager::Release();
	}

	bool Init(APhysXScene* pAPhysXScene, const char* szTerrainIniFile)
	{
		m_pAPhysXScene = pAPhysXScene;
		bool bRes = LoadConfigs(szTerrainIniFile);

		if(m_bMultiThreadedLoading)
		{
			StartLoaderThread();
		}

		return bRes;
	}

	bool LoadConfigs(const char* szTerrainIniFile);
	bool LoadConfigs(const APhysXTerrainConfigs& apxTerrainConfigs);

	void SaveConfigs(const char* szTerrainIniFile);
	

	// override these two functions to call terrain blocks' method...
	virtual bool UpdateSimCenter(const NxVec3& vCenter);
	virtual void SetSimRadius(float fRadius);

	float GetSimRadius() { return m_fSimRadius; }

//protected:

	virtual int GetRowNumOfQuads() { return m_iRowNumOfBlocks; }
	virtual int GetColNumOfQuads() { return m_iColNumOfBlocks; }

	virtual float GetQuadSizeX() { return m_fBlockSizeX; }
	virtual float GetQuadSizeZ() { return m_fBlockSizeZ; }

	virtual void ReleaseQuad(void* pQuad); 
	virtual void* LoadQuad(int iRow, int iCol);

	bool IsValidTerrainBlock(int iRow, int iCol)
	{
		return IsValidTerrainQuad(iRow, iCol);
	}

	void GetTerrainBlockFileName(int iRow, int iCol, char* szTBFileName); 

	void RaiseFlag(APhysXTerrainFlag flag)
	{
		m_uFlags |= flag;
	}
	
	void ClearFlag(APhysXTerrainFlag flag)
	{
		m_uFlags &= ~flag;
	}

	bool GetFlag(APhysXTerrainFlag flag) const
	{
		return (m_uFlags & flag)? true : false;
	}

protected:
	
	APhysXScene* m_pAPhysXScene;

	int m_iRowNumOfBlocks;
	int m_iColNumOfBlocks;

	float m_fBlockSizeX;
	float m_fBlockSizeZ;

	APhysXTerrainGlobalOrigin m_PhysXTerrainGlobalOrigin;

	int m_iMaxCachedPatchNumOneBlock;
	float m_fSimRadius;

	const char* m_szTerrainBlockPath;
	const char* m_szTerrainBlockFilePrefix;

	APhysXU32 m_uFlags;

//-----------------------------------------------------------------
// for multi-threaded terrain loading...

	friend DWORD WINAPI ThreadLoaderFunc(void * arg);

protected:
	
	// at present, we can not change the terrain loading way runtimely...
	//void EnableMultiThreadedLoad( bool bEnable = true );

	void StartLoaderThread();
	void EndLoaderThread();


	APhysXMutex* GetRequestLoadQueueMutex() { return m_pRequestLoadQueueMutex; }
	APhysXMutex* GetDoneLoadQueueMutex() { return m_pDoneLoadQueueMutex; }

	bool IsRequestLoadQueueEmpty() { return m_arrRequestLoadQueue.empty(); }
	bool IsDoneLoadQueueEmpty() { return m_arrDoneLoadQueue.empty(); }

	APhysXTerrainQuadItem GetRequestLoadQueueFront() { return m_arrRequestLoadQueue.front(); }
	APhysXTerrainQuadItem GetDoneLoadQueueFront() { return m_arrDoneLoadQueue.front(); }

	void PushRequestLoadQueue(const APhysXTerrainQuadItem& item) { m_arrRequestLoadQueue.push_back(item);}
	void PushDoneLoadQueue(const APhysXTerrainQuadItem& item) { m_arrDoneLoadQueue.push_back(item);}
	
	void PopRequestLoadQueueFront()
	{
		m_arrRequestLoadQueue.erase(m_arrRequestLoadQueue.begin(), m_arrRequestLoadQueue.begin() + 1);
	}

	void PopDoneLoadQueueFront()
	{
		m_arrDoneLoadQueue.erase(m_arrDoneLoadQueue.begin(), m_arrDoneLoadQueue.begin() + 1);
	}

	void ReleaseLoaderThreadResources();

	virtual void CreateTerrainQuad(int iRow, int iCol);

	bool SearchLoadThreadQueue(int iRow, int iCol);		// search whether the item has been push to the queue of loading thread...

	virtual void UpdateCacheQuads();
private:

	bool m_bMultiThreadedLoading;
	
	APhysXMutex* m_pRequestLoadQueueMutex;
	APhysXMutex* m_pDoneLoadQueueMutex;

	APhysXArray<APhysXTerrainQuadItem> m_arrRequestLoadQueue;
	APhysXArray<APhysXTerrainQuadItem> m_arrDoneLoadQueue;

	HANDLE m_hLoaderThread;

};

#endif
