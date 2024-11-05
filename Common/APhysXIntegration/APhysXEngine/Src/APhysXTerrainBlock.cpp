
#include "APhysXStdAfx.h"

//height - 16 bits, a signed 16 bit integer which specifies the elevation at the sample
const float NxHeightFieldShape_Height_Range = 65534.0f;	

void APhysXTerrainBlock::Release()
{
	if(m_arrTerrainPatchRBObjectDescs)
	{
		delete []m_arrTerrainPatchRBObjectDescs;
		m_arrTerrainPatchRBObjectDescs = NULL;
	}

	APhysXTerrainQuadManager::Release();
	
}

inline APhysXTerrainPatchRBObjectDesc* APhysXTerrainBlock::GetTerrainPatchRBObjectDesc(int iRow, int iCol)
{
	if(IsValidTerrainPatch(iRow, iCol))
		return & (m_arrTerrainPatchRBObjectDescs [iRow * m_iColumnNumOfPatches + iCol] );
	else
		return NULL;
}

void APhysXTerrainBlock::ImportTerrainData(const APhysXTerrainBlockData& apxTerrainBlockData)
{

	m_iPatchColumnNum = apxTerrainBlockData.mPatchColumnNum;
	m_iPatchRowNum = apxTerrainBlockData.mPatchRowNum;

	m_iColumnNumOfPatches = apxTerrainBlockData.mColumnNum / m_iPatchColumnNum;
	m_iRowNumOfPatches = apxTerrainBlockData.mRowNum / m_iPatchRowNum;

	m_fMaxHeight = apxTerrainBlockData.mMaxHeight;
	m_fMinHeight = apxTerrainBlockData.mMinHeight;

	m_fSizeX = apxTerrainBlockData.mSizeX;
	m_fSizeZ = apxTerrainBlockData.mSizeZ;

	m_fPatchSizeX = m_fSizeX / m_iColumnNumOfPatches;
	m_fPatchSizeZ = m_fSizeZ / m_iRowNumOfPatches;

	m_fQuadSphereRadius = 0.5f * sqrtf(m_fPatchSizeX * m_fPatchSizeX + m_fPatchSizeZ * m_fPatchSizeZ);

	// then we create the patch obj descriptions
	m_arrTerrainPatchRBObjectDescs = new APhysXTerrainPatchRBObjectDesc [m_iColumnNumOfPatches * m_iRowNumOfPatches];

	// construct each terrain patch object description
	APhysXActorDesc apxActorDesc;
	apxActorDesc.mHasBody = false;			// static
	
	APhysXMaterialDesc apxMaterialDesc;		// no need to set material parameters since Height Field shape can only use default material of scene at present
	
	APhysXHeightFieldShapeDesc apxHFShapeDesc;
	apxHFShapeDesc.mRowNum = m_iPatchRowNum + 1;
	apxHFShapeDesc.mColumnNum = m_iPatchColumnNum + 1;
	apxHFShapeDesc.mRowScale = m_fPatchSizeZ / m_iPatchRowNum;
	apxHFShapeDesc.mColumnScale = m_fPatchSizeX / m_iPatchColumnNum;
	apxHFShapeDesc.mHeightScale = (m_fMaxHeight - m_fMinHeight) / NxHeightFieldShape_Height_Range;
	apxHFShapeDesc.AllocZeroSampleData();
	
	APhysXTerrainPatchRBObjectDesc* pCurObjectDesc = NULL;
	float fScaleFactor = NxHeightFieldShape_Height_Range / (m_fMaxHeight - m_fMinHeight) ;

	for(int i=0; i<m_iRowNumOfPatches; i++)
	{
		for(int j=0; j<m_iColumnNumOfPatches; j++)	
		{
			pCurObjectDesc = GetTerrainPatchRBObjectDesc(i, j);
			NxU8* currentByte = (NxU8*)apxHFShapeDesc.mSampleData;

			// now, we should fill in the sample data of current terrain patch
			for (int row = 0; row < apxHFShapeDesc.mRowNum; row++)        
			{        
				for (int column = 0; column < apxHFShapeDesc.mColumnNum; column++)            
				{            
					NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;            
					*currentSample = ComputeTerrainPatchSampleData(apxTerrainBlockData, i, j, row, column, fScaleFactor);
					
					currentByte += sizeof(NxU32);
				}
			}

			apxHFShapeDesc.TransposeRowCol();

			APhysXArray<APhysXShapeDesc* > Shapes;
			Shapes.push_back(&apxHFShapeDesc);
			pCurObjectDesc->AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);
		}
	}
}

NxHeightFieldSample APhysXTerrainBlock::ComputeTerrainPatchSampleData(const APhysXTerrainBlockData& apxTerrainBlockData, int iPatchRow, int iPatchCol, int iRow, int iCol, float fScaleFactor)
{
	int iSampleRow = m_iPatchRowNum * iPatchRow + iRow;
	int iSampleCol = m_iPatchColumnNum * iPatchCol + iCol;
	float fCurHeight = apxTerrainBlockData.GetSample(iSampleRow, iSampleCol);
	float fMidHeight = (m_fMaxHeight + m_fMinHeight) * 0.5f;
	
	NxHeightFieldSample curSample;
	curSample.height = (int) ((fCurHeight - fMidHeight) * fScaleFactor);
	
	switch( apxTerrainBlockData.mTessellationWay)
	{
	case APX_TP_TESS_ZERO:
		curSample.tessFlag = 0;
		break;

	case APX_TP_TESS_ONE:
		curSample.tessFlag = 1;
		break;

	case APX_TP_TESS_ANGELICATERRAINWAY:
		
		if(iCol == 0 && iRow == 0 ||											// the left-bottom cell
		   iCol == m_iPatchColumnNum - 1 && iRow == m_iPatchRowNum - 1)			// the right-top cell
		{
			curSample.tessFlag = 0;
		}
		else
			curSample.tessFlag = 1;

		break;

	default:
		break;
	}

	bool bPatchHole = apxTerrainBlockData.GetExtraPatchData(iPatchRow, iPatchCol) & APX_TP_EXTRADATA_HOLE;
	bool bCellHole = false;
	if(iRow < m_iPatchRowNum && iCol < m_iPatchColumnNum)
		bCellHole = apxTerrainBlockData.GetExtraCellData(iSampleRow, iSampleCol) & APX_TC_EXTRADATA_HOLE;
	
	if(bPatchHole || bCellHole)
	{
		curSample.materialIndex0 = curSample.materialIndex1 = APhysXHeightFieldShapeDesc::APHYSX_HF_HOLE_MATERIAL_ID;
	}

	return curSample;
}

bool APhysXTerrainBlock::Save(NxStream* pNxStream)
{
	APhysXSerializerBase::Save(pNxStream);

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeDword(m_iRowNumOfPatches);
	pNxStream->storeDword(m_iColumnNumOfPatches);
	for(int i=0; i<m_iRowNumOfPatches * m_iColumnNumOfPatches; i++)
	{
		bool bSaveObjDesc = m_arrTerrainPatchRBObjectDescs[i].Save(pNxStream);
		APHYSX_ASSERT(bSaveObjDesc);
		if(!bSaveObjDesc)
		{
			gPhysXLog->Log("Warning: Terrain Patch RB Object Desc %d failed when saving.", i);
		}
	}

	pNxStream->storeDword(m_iPatchRowNum);
	pNxStream->storeDword(m_iPatchColumnNum);

	pNxStream->storeFloat(m_fMinHeight);
	pNxStream->storeFloat(m_fMaxHeight);

	pNxStream->storeFloat(m_fSizeX);
	pNxStream->storeFloat(m_fSizeZ);

	pNxStream->storeFloat(m_fOffsetX);
	pNxStream->storeFloat(m_fOffsetZ);


	return true;
}

bool APhysXTerrainBlock::Load(NxStream* pNxStream)
{

	APhysXSerializerBase::Load(pNxStream);

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{
		m_iRowNumOfPatches = pNxStream->readDword();
		m_iColumnNumOfPatches = pNxStream->readDword();

		int iPatchNum = m_iColumnNumOfPatches * m_iRowNumOfPatches;
		m_arrTerrainPatchRBObjectDescs = new APhysXTerrainPatchRBObjectDesc [iPatchNum];
		
		for(int i=0; i<iPatchNum; i++)
		{
			bool bLoadObjDesc = m_arrTerrainPatchRBObjectDescs[i].Load(pNxStream);
			APHYSX_ASSERT(bLoadObjDesc);
			if(!bLoadObjDesc)
			{
				gPhysXLog->Log("Warning: Terrain Patch RB Object Desc %d failed when loading.", i);
			}
		}

		m_iPatchRowNum = pNxStream->readDword();
		m_iPatchColumnNum = pNxStream->readDword();

		m_fMinHeight = pNxStream->readFloat();
		m_fMaxHeight = pNxStream->readFloat();

		m_fSizeX = pNxStream->readFloat();
		m_fSizeZ = pNxStream->readFloat();

		m_fOffsetX = pNxStream->readFloat();
		m_fOffsetZ = pNxStream->readFloat();

		m_fPatchSizeX = m_fSizeX / m_iColumnNumOfPatches;
		m_fPatchSizeZ = m_fSizeZ / m_iRowNumOfPatches;
		
		m_fQuadSphereRadius = 0.5f * sqrtf(m_fPatchSizeX * m_fPatchSizeX + m_fPatchSizeZ * m_fPatchSizeZ);

		return true;
	}
	else
		return false;
}

bool APhysXTerrainBlock::Save(const char* szFile)
{
	APhysXUserStream apxFileStream(szFile, false);
	if (apxFileStream.IsValid())
		return Save(&apxFileStream);
	return false; 
}

bool APhysXTerrainBlock::Load(const char* szFile)
{
	APhysXFileImageStream apxFileImage(szFile, true);
	if (apxFileImage.IsValid())
		return Load(&apxFileImage);
	return false;
}

void APhysXTerrainBlock::ReleaseQuad(void* pQuad)
{
	APhysXObject* pTerrainPatchRBObject = (APhysXObject*) pQuad;
	m_pAPhysXScene->ReleasePhysXObject(pTerrainPatchRBObject);
}

void* APhysXTerrainBlock::LoadQuad(int iRow, int iCol)
{
	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mGlobalPose.t = GetQuadPos(iRow, iCol);
	apxObjInstanceDesc.mGlobalPose.t.y = (m_fMaxHeight + m_fMinHeight) * 0.5f;

	apxObjInstanceDesc.mPhysXObjectDesc = GetTerrainPatchRBObjectDesc(iRow, iCol);

	APhysXObject* pTerrainPatchObj = m_pAPhysXScene->CreatePhysXObject(apxObjInstanceDesc);

	return (void *)pTerrainPatchObj;
}

