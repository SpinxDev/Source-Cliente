
#include "APhysXStdAfx.h"

bool APhysXTerrain::LoadConfigs(const APhysXTerrainConfigs& apxTerrainConfigs)
{
	m_iRowNumOfBlocks = apxTerrainConfigs.mRowNumOfBlocks;
	m_iColNumOfBlocks = apxTerrainConfigs.mColNumOfBlocks;

	m_fBlockSizeX = apxTerrainConfigs.mBlockSizeX;
	m_fBlockSizeZ = apxTerrainConfigs.mBlockSizeZ;
	m_fQuadSphereRadius = 0.5f * sqrtf(m_fBlockSizeX * m_fBlockSizeX + m_fBlockSizeZ * m_fBlockSizeZ);
	
	m_PhysXTerrainGlobalOrigin = apxTerrainConfigs.mTerrainGlobalOrigin;

	m_iMaxCachedQuadNum = apxTerrainConfigs.mMaxCachedBlockNum;
	m_iMaxCachedPatchNumOneBlock = apxTerrainConfigs.mMaxCachedPatchNumOneBlock;
	m_fSimRadius = apxTerrainConfigs.mSimRadius;
	SetSimRadius(m_fSimRadius);

	m_szTerrainBlockFilePrefix = gGlobalStringManager->GetString(apxTerrainConfigs.mTerrainBlockFilePrefix);

	m_bMultiThreadedLoading = apxTerrainConfigs.mMultithreadedLoading;

	return true;
}

bool APhysXTerrain::LoadConfigs(const char* szTerrainIniFile)
{
	AIniFile apxTerrainConfig;
	if(!apxTerrainConfig.Open(szTerrainIniFile))
	{
		gPhysXLog->Log("Error: Failed on loading APhysXTerrain config file.");
		return false;
	}

	
	// get the path of terrain blocks
	char szTerrainBlockPath[256];
	af_GetFilePath(szTerrainIniFile, szTerrainBlockPath, 256);
	m_szTerrainBlockPath = gGlobalStringManager->GetString(szTerrainBlockPath);

	m_iRowNumOfBlocks = apxTerrainConfig.GetValueAsInt("BlockInfo", "RowNumOfBlocks", 0);
	m_iColNumOfBlocks = apxTerrainConfig.GetValueAsInt("BlockInfo", "ColNumofBlocks", 0);

	m_fBlockSizeX = apxTerrainConfig.GetValueAsFloat("BlockInfo", "BlockSizeX", 1024.0f);
	m_fBlockSizeZ = apxTerrainConfig.GetValueAsFloat("BlockInfo", "BlockSizeZ", 1024.0f);
	m_fQuadSphereRadius = 0.5f * sqrtf(m_fBlockSizeX * m_fBlockSizeX + m_fBlockSizeZ * m_fBlockSizeZ);

	AString strGlobalOrigin = apxTerrainConfig.GetValueAsString("BlockInfo", "GlobalOrigin", "Center");
	if(strGlobalOrigin == "Center")
	{
		m_PhysXTerrainGlobalOrigin = APX_TerrainGlobalOrigin_Center;
		SetCenterOffset(0.0f, 0.0f);
	}
	else if(strGlobalOrigin == "LeftBottom")
	{
		// do nothing
		m_PhysXTerrainGlobalOrigin = APX_TerrainGlobalOrigin_LeftBottom;
	}
	else
	{
		// no other cases...
		m_PhysXTerrainGlobalOrigin = APX_TerrainGlobalOrigin_LeftBottom;
	}


	m_iMaxCachedQuadNum = apxTerrainConfig.GetValueAsInt("SimulateInfo", "MaxNumOfCachedBlocks", 4);
	m_iMaxCachedPatchNumOneBlock = apxTerrainConfig.GetValueAsInt("SimulateInfo", "MaxNumOfCachedPatchesInOneBlock", 20);
	m_fSimRadius = apxTerrainConfig.GetValueAsFloat("SimulateInfo", "SimulatRadius", 300.0f);
	SetSimRadius(m_fSimRadius);

	AString strTerrainBlockFilePrefix = apxTerrainConfig.GetValueAsString("FileInfo", "TerrainBlockFilePrefix", NULL);
	m_szTerrainBlockFilePrefix = gGlobalStringManager->GetString(strTerrainBlockFilePrefix);

	AString strMultithreadedLoad = apxTerrainConfig.GetValueAsString("LoadingInfo", "MultithreadedLoading", "false");
	if(strMultithreadedLoad == "true")
	{
		m_bMultiThreadedLoading = true;
		
	}
	else if(strMultithreadedLoad == "false")
	{
		m_bMultiThreadedLoading = false;
	}
	else
	{
		APHYSX_ASSERT(0);
	}

	apxTerrainConfig.Close();

	return true;
}

void APhysXTerrain::SaveConfigs(const char* szTerrainIniFile)
{
	AIniFile apxTerrainConfig;
	
	apxTerrainConfig.WriteIntValue("BlockInfo", "RowNumOfBlocks", m_iRowNumOfBlocks);
	apxTerrainConfig.WriteIntValue("BlockInfo", "ColNumofBlocks", m_iColNumOfBlocks);

	apxTerrainConfig.WriteFloatValue("BlockInfo", "BlockSizeX", m_fBlockSizeX);
	apxTerrainConfig.WriteFloatValue("BlockInfo", "BlockSizeZ", m_fBlockSizeZ);
	if(m_PhysXTerrainGlobalOrigin == APX_TerrainGlobalOrigin_Center)
		apxTerrainConfig.WriteStringValue("BlockInfo", "GlobalOrigin", "Center");
	else
		apxTerrainConfig.WriteStringValue("BlockInfo", "GlobalOrigin", "LeftBottom");

	apxTerrainConfig.WriteIntValue("SimulateInfo", "MaxNumOfCachedBlocks", m_iMaxCachedQuadNum);
	apxTerrainConfig.WriteIntValue("SimulateInfo", "MaxNumOfCachedPatchesInOneBlock", m_iMaxCachedPatchNumOneBlock);
	apxTerrainConfig.WriteFloatValue("SimulateInfo", "SimulatRadius", m_fSimRadius);

	apxTerrainConfig.WriteStringValue("FileInfo", "TerrainBlockFilePrefix", m_szTerrainBlockFilePrefix);

	if(m_bMultiThreadedLoading)
		apxTerrainConfig.WriteStringValue("LoadingInfo", "MultithreadedLoading", "true");
	else
		apxTerrainConfig.WriteStringValue("LoadingInfo", "MultithreadedLoading", "false");

	apxTerrainConfig.Save(szTerrainIniFile);
	apxTerrainConfig.Close();
}

void APhysXTerrain::GetTerrainBlockFileName(int iRow, int iCol, char* szTBFileName)
{
/*********************************************************************************

		Terrain Block File Naming Rule in Angelica integration is as following:



			z
		   /|\
			|
			|
			---------------------------------  ---
			|				|				|  /|\
			|				|				|	|
			|	Terrain		|	Terrain		|	|
	Row #1	|	Block _1	|	Block _2	|	|
			|				|				|	|
			|				|				|	|
			|				|				|	|
			--------------------------------- Row Size
			|				|				|	|
			|				|				|	|
			|	Terrain		|	Terrain		|	|
	Row #0	|	Block _3	|	Block _4	|	|
			|				|				|	|
			|				|				|	|
			|				|				|  \|/	
			---------------------------------------------> x
		(0, 0)	Column #0		Column #1



**********************************************************************************/



	int id = (m_iRowNumOfBlocks - 1 - iRow) * m_iColNumOfBlocks + iCol + 1;
	if(m_szTerrainBlockPath)
	{
		if(m_szTerrainBlockFilePrefix)
			sprintf(szTBFileName, "%s\\%s_%d.ptb", m_szTerrainBlockPath, m_szTerrainBlockFilePrefix, id);
		else
			sprintf(szTBFileName, "%s\\%d.ptb", m_szTerrainBlockPath, id);
	}
	else
	{
		if(m_szTerrainBlockFilePrefix)
			sprintf(szTBFileName, "%s_%d.ptb", m_szTerrainBlockFilePrefix, id);
		else
			sprintf(szTBFileName, "%d.ptb", id);
	}
}

void APhysXTerrain::ReleaseQuad(void* pQuad)
{
	APhysXTerrainBlock* pTerrainBlock = (APhysXTerrainBlock*) pQuad;
	delete pTerrainBlock;
}

void* APhysXTerrain::LoadQuad(int iRow, int iCol)
{

	if(GetFlag(APX_TERRAINFLAG_IGNORE_BORDER_BLOCK))
	{
		// ignore the loading of border block...
		if(m_iRowNumOfBlocks > 1 && 
			(iRow == 0 || iRow == m_iRowNumOfBlocks-1) )
			return NULL;

		if(m_iColNumOfBlocks > 1 && 
			(iCol == 0 || iCol == m_iColNumOfBlocks-1) )
			return NULL;
	}


	char szTBFileName[256];
	GetTerrainBlockFileName(iRow, iCol, szTBFileName);

#if APHYSX_ANGELICA_VERSION < 300
	
	if (!af_IsFileExist(szTBFileName))

#else

	if (!af_IsFileExist(szTBFileName, true))

#endif
		return NULL;

	APhysXTerrainBlock* pTerrainBlock = new APhysXTerrainBlock;


	// use AFileImage to load data
	APhysXFileImageStream apxFileImage(szTBFileName, true);
	bool bLoad = pTerrainBlock->Load(&apxFileImage);
	APHYSX_ASSERT(bLoad);
	
	if(!bLoad)
	{
		gPhysXLog->Log("Warning: Failed on loading APhysXTerrainBlock File: %s.", szTBFileName);

		delete pTerrainBlock;
		return NULL;
	}

	pTerrainBlock->SetAPhysXScene(m_pAPhysXScene);
	
	pTerrainBlock->SetOffset(GetQuadPos(iRow, iCol));
	pTerrainBlock->SetMaxTerrainPatchObjNum(m_iMaxCachedPatchNumOneBlock);
	pTerrainBlock->SetSimRadius(m_fSimRadius);
	
	return (void *)pTerrainBlock;

}


bool APhysXTerrain::UpdateSimCenter(const NxVec3& vCenter)
{

	if(m_pAPhysXScene)
	{
		//add performance statistic 
		m_pAPhysXScene->BeginProfiling("APXTerrain_Update");

		APhysXTerrainQuadManager::UpdateSimCenter(vCenter);

		for(int i=0; i<m_arrCachedQuads.size(); i++)
		{
			if(m_arrCachedQuads[i].mCacheCounter)
			{
				APhysXTerrainBlock* pTerrainBlock = (APhysXTerrainBlock*) m_arrCachedQuads[i].mTerrainQuad;
				
				if(!pTerrainBlock)
					continue;

				if(!pTerrainBlock->UpdateSimCenter(vCenter))
				{
					m_arrCachedQuads[i].mCacheCounter = 0;
				}
			}
		}

		m_pAPhysXScene->EndProfiling("APXTerrain_Update");
	}

	// in case of APhysXTerrain, always return true...
	return true;
}

void APhysXTerrain::SetSimRadius(float fRadius)
{
	APhysXTerrainQuadManager::SetSimRadius(fRadius);

	for(int i=0; i<m_arrCachedQuads.size(); i++)
	{
		APhysXTerrainBlock* pTerrainBlock = (APhysXTerrainBlock*) m_arrCachedQuads[i].mTerrainQuad;
		pTerrainBlock->SetSimRadius(fRadius);
	}
}


//-----------------------------------------------------------------
/*
static APhysXMutex s_RequestLoadQueueMutex;
static APhysXMutex s_DoneLoadQueueMutex;

typedef APhysXArray<APhysXTerrainQuadManager::APhysXTerrainQuadItem> APhysXTerrainQuadItemArray;
typedef APhysXArray<APhysXTerrainQuadManager::APhysXTerrainQuadItem>::Iterator APhysXTerrainQuadItemIterator;

static APhysXTerrainQuadItemArray s_arrRequestLoadQueue;
static APhysXTerrainQuadItemArray s_arrDoneLoadQueue;

*/

bool APhysXTerrain::SearchLoadThreadQueue(int iRow, int iCol)
{
	APHYSX_ASSERT(m_pRequestLoadQueueMutex && m_pDoneLoadQueueMutex);
	if(m_pRequestLoadQueueMutex && m_pDoneLoadQueueMutex)
	{
		m_pRequestLoadQueueMutex->Lock();
		for(int i=0; i<m_arrRequestLoadQueue.size(); i++)
		{
			if(m_arrRequestLoadQueue[i].mRow == iRow && m_arrRequestLoadQueue[i].mCol == iCol)
			{
				m_pRequestLoadQueueMutex->Unlock();
				return true;
			}
		}
		m_pRequestLoadQueueMutex->Unlock();

		m_pDoneLoadQueueMutex->Lock();
		for(int i=0; i<m_arrDoneLoadQueue.size(); i++)
		{
			if(m_arrDoneLoadQueue[i].mRow == iRow && m_arrDoneLoadQueue[i].mCol == iCol)
			{
				m_pDoneLoadQueueMutex->Unlock();
				return true;
			}
		}
		m_pDoneLoadQueueMutex->Unlock();

		return false;
	}

	return false;

}

void APhysXTerrain::CreateTerrainQuad(int iRow, int iCol)
{
	if(m_bMultiThreadedLoading)
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

		// push back the quad item to request queue...
		APHYSX_ASSERT(m_pRequestLoadQueueMutex && m_pDoneLoadQueueMutex);

		if(!SearchLoadThreadQueue(iRow, iCol))
		{
			// avoid push the item more than once...
			
			APhysXTerrainQuadItem quadItem;
			quadItem.mRow = iRow;
			quadItem.mCol = iCol;
			m_pRequestLoadQueueMutex->Lock();
			m_arrRequestLoadQueue.push_back(quadItem);
			m_pRequestLoadQueueMutex->Unlock();

		}


	}
	else
	{
		APhysXTerrainQuadManager::CreateTerrainQuad(iRow, iCol);
	}

}

void APhysXTerrain::UpdateCacheQuads()
{
	if(m_bMultiThreadedLoading && m_pDoneLoadQueueMutex)
	{
		// in multi-threaded loading mode, we try to update the cache quads list

		// pop up all the elements of done queue
		// note here: we use none-block lock
		if(m_pDoneLoadQueueMutex->Trylock())
		{
			for(int i=0; i<m_arrDoneLoadQueue.size(); i++)
			{
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

					m_arrCachedQuads[iInactiveQuadID] = m_arrDoneLoadQueue[i];

					IncreaseCurObsoleteQuadIndex();
				}
				else
				{
					m_arrCachedQuads.push_back(m_arrDoneLoadQueue[i]);
				}

			}

			m_arrDoneLoadQueue.clear();		// remove all the elements in the m_arrDoneLoadQueue

			m_pDoneLoadQueueMutex->Unlock();
		}


	}

}

static DWORD WINAPI ThreadLoaderFunc(void * arg)
{
	APhysXTerrain* pTerrain = (APhysXTerrain*) arg;
	if(!pTerrain) return 1;

	while(1)
	{
		pTerrain->GetRequestLoadQueueMutex()->Lock();
		if(!pTerrain->IsRequestLoadQueueEmpty())
		{
			// firstly, we get the first element in Request queue...
			APhysXTerrainQuadManager::APhysXTerrainQuadItem curQuadItem = pTerrain->GetRequestLoadQueueFront();

			if(curQuadItem.mCol == -1 && curQuadItem.mRow == -1)
			{
				// exit the thread...
				pTerrain->GetRequestLoadQueueMutex()->Unlock();
				break;
			}

			pTerrain->GetRequestLoadQueueMutex()->Unlock();

			// load the terrain block...
			curQuadItem.mTerrainQuad = pTerrain->LoadQuad(curQuadItem.mRow, curQuadItem.mCol);
			if(curQuadItem.mTerrainQuad)
			{
				// loading is successful...
				pTerrain->GetDoneLoadQueueMutex()->Lock();

				pTerrain->PushDoneLoadQueue(curQuadItem);

				pTerrain->GetDoneLoadQueueMutex()->Unlock();
			}

			//------------------------------------------------------------------------
			// note here:
			// at last, we pop the first element of request queue so that we can make sure all quads requested
			// must be in Request queue, Done queue, or the Cached list...
			// while, if we pop the first element of request queue before push it into Done queue, there may be
			// some interval when the element are not in both request queue and done queue...
			pTerrain->GetRequestLoadQueueMutex()->Lock();
			pTerrain->PopRequestLoadQueueFront();
			pTerrain->GetRequestLoadQueueMutex()->Unlock();

		}
		else
		{
			pTerrain->GetRequestLoadQueueMutex()->Unlock();
			::Sleep(50);
		}
	}

	pTerrain->ReleaseLoaderThreadResources();

	return(0);
}

void APhysXTerrain::StartLoaderThread()
{
	APHYSX_ASSERT(m_pRequestLoadQueueMutex == NULL && m_pDoneLoadQueueMutex == NULL);
	
	m_pRequestLoadQueueMutex = new APhysXMutex;
	m_pDoneLoadQueueMutex = new APhysXMutex;

	DWORD threadID;
	m_hLoaderThread = ::CreateThread(0, 0, ThreadLoaderFunc, (void *)this, 0, &threadID);	
}

void APhysXTerrain::EndLoaderThread()
{
	APHYSX_ASSERT(m_pRequestLoadQueueMutex);

	if(m_pRequestLoadQueueMutex)
	{
		APhysXTerrainQuadItem apxEndLoaderThreadQuadItem;
		apxEndLoaderThreadQuadItem.mRow = apxEndLoaderThreadQuadItem.mCol = -1;

		m_pRequestLoadQueueMutex->Lock();

		m_arrRequestLoadQueue.push_back(apxEndLoaderThreadQuadItem);

		m_pRequestLoadQueueMutex->Unlock();

		// wait untill the loader thread is exited...
		::WaitForSingleObject(m_hLoaderThread, INFINITE);
		::CloseHandle(m_hLoaderThread);
		m_hLoaderThread = NULL;

	}

}

void APhysXTerrain::ReleaseLoaderThreadResources()
{
	APHYSX_ASSERT(m_pRequestLoadQueueMutex && m_pDoneLoadQueueMutex);

	if(m_pRequestLoadQueueMutex && m_pDoneLoadQueueMutex)
	{
		m_pRequestLoadQueueMutex->Lock();
		m_arrRequestLoadQueue.clear();
		m_pRequestLoadQueueMutex->Unlock();

		m_pDoneLoadQueueMutex->Lock();
		// a little different from the case of m_arrRequestLoadQueue...
		// we should release the quads we have loaded but still not push to cached quad list
		for(int i=0; i<m_arrDoneLoadQueue.size(); i++)
		{
			ReleaseQuad(m_arrDoneLoadQueue[i].mTerrainQuad);
		}

		m_arrDoneLoadQueue.clear();

		m_pDoneLoadQueueMutex->Unlock();

		delete m_pRequestLoadQueueMutex;
		m_pRequestLoadQueueMutex = NULL;

		delete m_pDoneLoadQueueMutex;
		m_pDoneLoadQueueMutex = NULL;
	}
}

/*
void APhysXTerrain::EnableMultiThreadedLoad(bool bEnable)
{
	m_bMultiThreadedLoad = bEnable;

	if(m_bMultiThreadedLoad)
	{
		// start the thread?

	}
	else
	{
		//if(m_pRequestLoadQueueMutex && m_pDoneLoadQueueMutex)
		{
			
		}
	}
}
*/
