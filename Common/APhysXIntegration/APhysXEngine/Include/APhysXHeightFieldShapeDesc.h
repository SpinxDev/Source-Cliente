/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXHEIGHTFIELDSHAPEDESC_H_
#define _APHYSXHEIGHTFIELDSHAPEDESC_H_


class APHYSX_DLL_EXPORT APhysXHeightFieldShapeDesc:public APhysXShapeDesc
{
	DeclareVersion(0xcc000001);
	DeclareShapeType(APX_SHAPETYPE_HEIGHTFIELD);

	enum
	{
		APHYSX_HF_HOLE_MATERIAL_ID = 127,			// 2^7 - 1 
	};

public:

	APhysXHeightFieldShapeDesc()
	{
		SetToDefault();
	}

	virtual ~APhysXHeightFieldShapeDesc() { Release(); }

	void Release();

	void AllocZeroSampleData();

	virtual void SetToDefault()
	{
		APhysXShapeDesc::SetToDefault();

		mRowNum = 0;
		mColumnNum = 0;
		mSampleData = NULL;
		
		mHeightScale = 1.0f;
		mRowScale = 1.0f;
		mColumnScale = 1.0f;
	}

	const APhysXHeightFieldShapeDesc& operator= (const APhysXHeightFieldShapeDesc& other);

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	// to be aware that this class just new the pointer
	// to NxShapeDesc but not save it, so save and release
	// of the pointer is responsibility of APhysXShapeDescManager
	virtual NxShapeDesc* CreateNxShapeDesc(const NxVec3& vSacle3D = NxVec3(1.0f));
	virtual void GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D = NxVec3(1.0f));

	APhysXU32 GetSample(int iRow, int iCol)
	{
		return mSampleData[iRow * mColumnNum + iCol];
	}

	void SetSample(int iRow, int iCol, APhysXU32 uSample)
	{
		mSampleData[iRow * mColumnNum + iCol] = uSample;
	}

	// transpose all data related to row and col
	void TransposeRowCol();

	// virtual interface
	virtual APhysXShapeDesc* Clone() const
	{
		APhysXShapeDesc* pPhysXShapeDesc = new APhysXHeightFieldShapeDesc;
		*(APhysXHeightFieldShapeDesc *)pPhysXShapeDesc = *this;
		return pPhysXShapeDesc;
	}

protected:

	NxHeightField* CreateNxHeightField();

	// properties are public for direct access
public:

	//----------------------------------------------------------------
	// the sample definition is as following
	// mRowNum * mColumnNum vertices
	// (mRowNum - 1) * (mColumnNum - 1) cells
	// as for sample data format, refer to enum NxHeightFieldFormat {NX_HF_S16_TM,}; in line 2123 of Nxp.h
	APhysXU32 mRowNum;
	APhysXU32 mColumnNum;
	APhysXU32* mSampleData;
	//----------------------------------------------------------------

	float mHeightScale;
	float mRowScale;
	float mColumnScale;

};




#endif
