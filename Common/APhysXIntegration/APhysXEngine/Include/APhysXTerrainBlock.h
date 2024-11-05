/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

		APhysXTerrainBlock is corresponding to one block of the whole 
	game terrain which include a number of terrain patches. It handles 
	the creation, saving/loading, runtime loading control of terrain patchs.

----------------------------------------------------------------------*/

#ifndef _APHYSXTERRAINBLOCK_H_
#define _APHYSXTERRAINBLOCK_H_


/*----------------------------------------------------------------------
			One Terrain Block (e.g., 2*2 patches)
	    z
	   /|\
		|
		|<----------Column Size-------->|
		---------------------------------  ---
		|				|				|  /|\
		|				|				|	|
		|	Terrain		|	Terrain		|	|
Row #1	|	 Patch		|	 Patch		|	|
		|				|				|	|
		|				|				|	|
		|				|				|	|
		--------------------------------- Row Size
		|				|				|	|
		|				|				|	|
		|	Terrain		|	Terrain		|	|
Row #0	|	 Patch		|	 Patch		|	|
		|				|				|	|
		|				|				|	|
		|				|				|  \|/	
		---------------------------------------------> x
	(0, 0)	Column #0		Column #1


////////////////////////////////////////////////////////////////////////

Note: 
	the left-bottom position is the Zero (0, 0) pos in the local frame.

////////////////////////////////////////////////////////////////////////


----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
				
				relationship of cell and sample


						 Cells
		    |      |      |      |      |
			|      |      |      |      |
		   \|/    \|/    \|/    \|/    \|/
		x------x------x------x------x------x
	   /|\    /|\    /|\    /|\    /|\    /|\
		|      |      |      |      |      |
		|      |      |      |      |      |
		               Samples

////////////////////////////////////////////////////////////////////////

Note:
	We can see that the cell number is one less than the sample number. In 
the struct APhysXTerrainBlockData, the mColumnNum and mRowNum all mean the 
cell's number but not the sample's number.

----------------------------------------------------------------------*/



enum APhysXImportTerrainDataFormat
{
	APX_ITDF_ROW_MAJOR_INCREASE,		// row major and the zero #row in the first
	APX_ITDF_ROW_MAJOR_DECREASE			// row major and the biggest #row in the first
};


/*----------------------------------------------------------------------
				One Cell Tessellation Way

				Tess #0			Tess #1
				-----			-----
				|  /|			|\  | 
				| / |			| \ |
				|/  |			|  \|
				-----			-----

/////////////////////////////////////////////////////////////////////////
				Terrain Patch Tessellation Way

				   APX_TP_TESS_ZERO

			-------------			---------
			|  /|  /|  /|			|  /|  /|
			| / | / | / | ... ...	| / | / |
			|/  |/  |/  |			|/  |/  |
			-------------			----------
			......		...			...
			......		...			...
			-------------			---------
			|  /|  /|  /|			|  /|  /|
			| / | / | / | ... ...	| / | / |
			|/  |/  |/  |			|/  |/  |
			-------------			---------

					APX_TP_TESS_ONE

			-------------			---------
			|\  |\  |\  |			|\  |\  |
			| \ | \ | \ | ... ...	| \ | \ |
			|  \|  \|  \|			|  \|  \|
			-------------			----------
			......		...			...
			......		...			...
			-------------			---------
			|\  |\  |\  |			|\  |\  |
			| \ | \ | \ | ... ...	| \ | \ |
			|  \|  \|  \|			|  \|  \|
			-------------			---------

				APX_TP_TESS_ANGELICATERRAINWAY
			-------------			---------
			|\  |\  |\  |			|\  |  /|
			| \ | \ | \ | ... ...	| \ | / |
			|  \|  \|  \|			|  \|/  |
			-------------			----------
				......		...			...
				......		...			...
			-------------			---------
			|  /|\  |\  |			|\  |\  |
			| / | \ | \ | ... ...	| \ | \ |
			|/  |  \|  \|			|  \|  \|
			-------------			---------

----------------------------------------------------------------------*/
enum APhysXTerrainPatchTessellationWay
{
	APX_TP_TESS_ZERO,
	APX_TP_TESS_ONE,
	APX_TP_TESS_ANGELICATERRAINWAY
};

enum APhysXTerrainPatchExtraData
{
	APX_TP_EXTRADATA_HOLE			= 1<<0,
	
	// to add...

};

enum APhysXTerrainCellExtraData
{
	APX_TC_EXTRADATA_HOLE			= 1<<0,

	// to add...
};


struct APhysXTerrainBlockData
{
	int mColumnNum;						// cells number: how many column in one block
	int mRowNum;						// cells number: how many row in one block

	int mPatchColumnNum;				// cells number: how many column in one patch
	int mPatchRowNum;					// cells number: how many row in one patch

	// so we should have (mColumnNum + 1) * (mRowNum + 1) samples
	// the sample data is 
	float* mSampleData;	

	float mMinHeight;
	float mMaxHeight;

	float mSizeX;
	float mSizeZ;

	APhysXImportTerrainDataFormat mSampleDataFormat;
	APhysXTerrainPatchTessellationWay mTessellationWay;

	APhysXU32* mExtraPatchData;			// extra data of patch, such as hole info
	APhysXU32* mExtraCellData;			// extra data of cell, such as hole info

	APhysXTerrainBlockData()
	{
		mSampleData = NULL;
		// this is the default format in the Angelica Engine
		mSampleDataFormat = APX_ITDF_ROW_MAJOR_DECREASE;	
		mTessellationWay = APX_TP_TESS_ANGELICATERRAINWAY;

		mExtraPatchData = NULL;
		mExtraCellData = NULL;
	}

	//---------------------------------------------------------------------------------
	float GetSample(int iRow, int iCol) const
	{
		APHYSX_ASSERT(  mSampleData
						&& iRow >= 0 && iRow <= mRowNum
						&& iCol >= 0 && iCol <= mColumnNum
						);
		
		if(!mSampleData) return -1.0f;			// error sample data...

		switch (mSampleDataFormat)
		{

		case APX_ITDF_ROW_MAJOR_INCREASE:
			
			return mSampleData[iRow * (mColumnNum + 1) + iCol];
			
			break;

		case APX_ITDF_ROW_MAJOR_DECREASE:
			
			return mSampleData[(mRowNum - iRow) * (mColumnNum + 1) + iCol];
			
			break;

		default:
			
			return 0.0f;

			break;

		}
	}

	void SetSample(int iRow, int iCol, float fSample)
	{
		APHYSX_ASSERT(  mSampleData
			&& iRow >= 0 && iRow <= mRowNum
			&& iCol >= 0 && iCol <= mColumnNum
			);

		if(!mSampleData) return ;			// error sample data...

		//--------------------------------------------------------------------
		// note:
		//		all SetXXXData() functions will just use Normal access mode
		// as APX_ITDF_ROW_MAJOR_INCREASE. and the real conversion is done
		// by GetXXXData() functions...

		mSampleData[iRow * (mColumnNum + 1) + iCol] = fSample;

/*
		switch (mSampleDataFormat)
		{

		case APX_ITDF_ROW_MAJOR_INCREASE:

			mSampleData[iRow * (mColumnNum + 1) + iCol] = fSample;

			break;

		case APX_ITDF_ROW_MAJOR_DECREASE:

			mSampleData[(mRowNum - iRow) * (mColumnNum + 1) + iCol] = fSample;

			break;

		default:

			break;
		}
*/

	};

	//---------------------------------------------------------------------------------
	APhysXU32 GetExtraPatchData(int iRow, int iCol) const
	{
		if(!mExtraPatchData) return 0;
		
		int iColumnNumOfPatches = mColumnNum / mPatchColumnNum;
		int iRowNumOfPatches = mRowNum / mPatchRowNum;

		APHYSX_ASSERT(
						iRow >= 0 && iRow < iRowNumOfPatches
						&& iCol >= 0 && iCol < iColumnNumOfPatches
						);

		switch (mSampleDataFormat)
		{

		case APX_ITDF_ROW_MAJOR_INCREASE:

			return mExtraPatchData[iRow * iColumnNumOfPatches  + iCol];

			break;

		case APX_ITDF_ROW_MAJOR_DECREASE:

			return mExtraPatchData[(iRowNumOfPatches - 1 - iRow) * iColumnNumOfPatches  + iCol];

			break;

		default:

			return 0;

			break;

		}
	}

	void SetExtraPatchData(int iRow, int iCol, APhysXU32 uData)
	{
		if(!mExtraPatchData) return;

		int iColumnNumOfPatches = mColumnNum / mPatchColumnNum;
		int iRowNumOfPatches = mRowNum / mPatchRowNum;

		APHYSX_ASSERT(
			iRow >= 0 && iRow < iRowNumOfPatches
			&& iCol >= 0 && iCol < iColumnNumOfPatches
			);

		//--------------------------------------------------------------------
		// note:
		//		all SetXXXData() functions will just use Normal access mode
		// as APX_ITDF_ROW_MAJOR_INCREASE. and the real conversion is done
		// by GetXXXData() functions...

		mExtraPatchData[iRow * iColumnNumOfPatches  + iCol] = uData;

/*
		switch (mSampleDataFormat)
		{

		case APX_ITDF_ROW_MAJOR_INCREASE:

			mExtraPatchData[iRow * iColumnNumOfPatches  + iCol] = uData;

			break;

		case APX_ITDF_ROW_MAJOR_DECREASE:

			mExtraPatchData[(iRowNumOfPatches - 1 - iRow) * iColumnNumOfPatches  + iCol] = uData;

			break;

		default:

			break;

		}
*/

	}
	//---------------------------------------------------------------------------------
	APhysXU32 GetExtraCellData(int iRow, int iCol) const
	{
		if(!mExtraCellData) return 0;

		APHYSX_ASSERT(
			iRow >= 0 && iRow < mRowNum
			&& iCol >= 0 && iCol < mColumnNum
			);

		switch (mSampleDataFormat)
		{

		case APX_ITDF_ROW_MAJOR_INCREASE:

			return mExtraCellData[iRow * mColumnNum  + iCol];

			break;

		case APX_ITDF_ROW_MAJOR_DECREASE:

			return mExtraCellData[(mRowNum - 1 - iRow) * mColumnNum  + iCol];

			break;

		default:

			return 0;

			break;

		}
	}

	void SetExtraCellData(int iRow, int iCol, APhysXU32 uData)
	{
		if(!mExtraCellData) return;

		APHYSX_ASSERT(
			iRow >= 0 && iRow < mRowNum
			&& iCol >= 0 && iCol < mColumnNum
			);

		//--------------------------------------------------------------------
		// note:
		//		all SetXXXData() functions will just use Normal access mode
		// as APX_ITDF_ROW_MAJOR_INCREASE. and the real conversion is done
		// by GetXXXData() functions...

		mExtraCellData[iRow * mColumnNum  + iCol] = uData;
/*
		switch (mSampleDataFormat)
		{

		case APX_ITDF_ROW_MAJOR_INCREASE:

			mExtraCellData[iRow * mColumnNum  + iCol] = uData;

			break;

		case APX_ITDF_ROW_MAJOR_DECREASE:

			mExtraCellData[(mRowNum - 1 - iRow) * mColumnNum  + iCol] = uData;

			break;

		default:

			break;

		}
*/

  }


};	// end of class


class APHYSX_DLL_EXPORT APhysXTerrainBlock: public APhysXSerializerBase, public APhysXTerrainQuadManager
{

	DeclareVersion(0xcc000001);

public:

	APhysXTerrainBlock() 
	{
		m_arrTerrainPatchRBObjectDescs = NULL;
		m_pAPhysXScene = NULL;
	}

	~APhysXTerrainBlock() 
	{
		Release();
	}

	virtual void Release();

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	bool Save(const char* szFile);
	bool Load(const char* szFile);

	void ImportTerrainData(const APhysXTerrainBlockData& apxTerrainBlockData);

	void SetAPhysXScene(APhysXScene* pAPhysXScene) 
	{
		m_pAPhysXScene = pAPhysXScene;
	}

	void SetMaxTerrainPatchObjNum(int iMax) { SetMaxCachedQuadNum(iMax); }

protected:
	
	virtual int GetRowNumOfQuads() { return m_iRowNumOfPatches; }
	virtual int GetColNumOfQuads() { return m_iColumnNumOfPatches; }

	virtual float GetQuadSizeX() { return m_fPatchSizeX; }
	virtual float GetQuadSizeZ() { return m_fPatchSizeZ; }

	virtual void ReleaseQuad(void* pQuad); 
	virtual void* LoadQuad(int iRow, int iCol);

	APhysXTerrainPatchRBObjectDesc* GetTerrainPatchRBObjectDesc(int iRow, int iCol);
	
	inline bool IsValidTerrainPatch(int iRow, int iCol)
	{
		return IsValidTerrainQuad(iRow, iCol);
	}

	NxHeightFieldSample ComputeTerrainPatchSampleData(const APhysXTerrainBlockData& apxTerrainBlockData, int iPatchRow, int iPatchCol, int iRow, int iCol, float fScaleFactor);

	void CreateTerrainPatchRBObject(int iRow, int iCol);
	

protected:
	
	int m_iRowNumOfPatches;				// how many row of patches in one block
	int m_iColumnNumOfPatches;			// how many column of patches in one block
	APhysXTerrainPatchRBObjectDesc* m_arrTerrainPatchRBObjectDescs;
	
	int m_iPatchRowNum;			// how many row of cells in one patch
	int m_iPatchColumnNum;		// how many column of cells in one patch
	
	float m_fMinHeight;
	float m_fMaxHeight;

	float m_fSizeX;
	float m_fSizeZ;

	float m_fPatchSizeX;
	float m_fPatchSizeZ;



	APhysXScene* m_pAPhysXScene;

};



#endif