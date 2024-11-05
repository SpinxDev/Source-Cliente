/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXCOMPOUNDOBJECTDESC_H_
#define _APHYSXCOMPOUNDOBJECTDESC_H_



class APHYSX_DLL_EXPORT APhysXCompoundObjectDesc:public APhysXObjectDesc
{

	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_COMPOUND);

	DisableCopyConstructor(APhysXCompoundObjectDesc);

protected:

	struct APhysXObjectComponentDesc
	{
		APhysXObjectDesc* mPhysXObjectDesc;
		NxMat34 mLocalPose;
		const char* mLocalName;

		APhysXObjectComponentDesc()
		{
			mPhysXObjectDesc = NULL;
			mLocalName = NULL;
		}

		~APhysXObjectComponentDesc()
		{
			delete mPhysXObjectDesc;
		}
	};

public:

	APhysXCompoundObjectDesc() {}

	virtual ~APhysXCompoundObjectDesc() { Release(); }

	virtual void Release();

	bool AddObjectComponent(const APhysXObjectDesc* pObjectDesc, const char* szLoacalName, const NxMat34& matLocalPose = NxMat34(true));

	// add my name as the prefix to component object desc name to generate a global name...
	void CompleteGlobalObjectComponentName();

	int GetObjectComponentDescNum() const { return m_arrObjectComponentDescs.size(); }
	APhysXObjectComponentDesc* GetObjectComponentDesc(int i) const { return m_arrObjectComponentDescs[i];}

	const APhysXCompoundObjectDesc& operator= (const APhysXCompoundObjectDesc& other);

	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXCompoundObjectDesc)

protected:

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	// copy my object component descs to other APhysXCompoundObjectDesc object...
	void CopyObjectComponentDescs(APhysXCompoundObjectDesc* pOtherCompoundObjectDesc, bool bReleaseOtherComponents = false) const;


protected:



	APhysXArray<APhysXObjectComponentDesc* > m_arrObjectComponentDescs;

};

#endif