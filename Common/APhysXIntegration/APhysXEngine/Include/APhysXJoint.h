/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXJOINT_H_
#define _APHYSXJOINT_H_


class APHYSX_DLL_EXPORT APhysXJoint: public APhysXObjectConnector
{

	friend class APhysXScene;
	friend class APhysXObject;

public:

	APhysXJoint() 
	{
		m_pNxJoint = NULL;
	}

	virtual ~APhysXJoint() {}

	virtual APhysXJoint* GetJoint() { return this; }

	NxJoint* GetNxJoint() const { return m_pNxJoint; }

protected:

	virtual bool CreateConnector(const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc);
	virtual void ReleaseConnector();

protected:

	NxJoint* m_pNxJoint;

};


#endif