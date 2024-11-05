/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

	Description:

----------------------------------------------------------------------*/

#ifndef _APHYSXTERRAINPATCHRBOBJECT_H_
#define _APHYSXTERRAINPATCHRBOBJECT_H_


class APHYSX_DLL_EXPORT APhysXTerrainPatchRBObject : public APhysXStaticRBObject
{

	// friend class
	friend class APhysXScene;

public:



protected:

	APhysXTerrainPatchRBObject()
	{
		m_pNxHeightField = NULL;
	}

	virtual void OnCreateNxActorDesc(NxActorDesc* pNxActorDesc, int iActorID);

	virtual void OnRelease();

protected:

	NxHeightField* m_pNxHeightField;

};


#endif