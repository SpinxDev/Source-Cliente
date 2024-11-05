/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXCOMPOUNDOBJECT_H_
#define _APHYSXCOMPOUNDOBJECT_H_


// still Abstract class
class APHYSX_DLL_EXPORT APhysXCompoundObject: public APhysXObject
{

public:

	APhysXObject* GetPhysXObjectComponent(const char* szName);

	int GetPhysXObjectComponentNum()
	{
		return m_arrObjectComponents.size();
	}
	
	APhysXObject* GetPhysXObjectComponent(int i)
	{
		return m_arrObjectComponents[i]->mPhysXObject;
	}


	void SetComponentUserGraphicsSync(const char* szComponentName, APhysXUserGraphicsSync* pPhysXUserGraphicsSync)
	{
		APhysXObject* pObject = GetPhysXObjectComponent(szComponentName);
		if(pObject) pObject->SetUserGraphicsSync(pPhysXUserGraphicsSync);
	}

	virtual void PutToSleep()
	{
		for(unsigned int i=0; i<m_arrObjectComponents.size(); i++)
		{
			m_arrObjectComponents[i]->mPhysXObject->PutToSleep();
		}
		
	}

	virtual void WakeUp() 
	{
		for(unsigned int i=0; i<m_arrObjectComponents.size(); i++)
		{
			m_arrObjectComponents[i]->mPhysXObject->WakeUp();
		}
	}	

	virtual bool IsSleeping()
	{ 
		for(unsigned int i=0; i<m_arrObjectComponents.size(); i++)
		{
			if(!m_arrObjectComponents[i]->mPhysXObject->IsSleeping())
				return false;
		}

		return true;
	}

	virtual void SetPose(const NxMat34& globalPose);

	virtual NxMat34 GetPose() const;
	virtual void GetAABB(NxBounds3& aabb) const;

	virtual void SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef = true);
	virtual void SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup);

	virtual void EnableCollision(bool bEnable = true);

	virtual void EnableDebugRender(bool bEnable = true);

	//--------------------------------------------------------------------
	// revised by wenfeng, May 8, 2009
	// call each component's own pre/post-simulate function...

	virtual void PreSimulate(float dt)
	{
		APhysXObject::PreSimulate(dt);

		for(unsigned int i=0; i<m_arrObjectComponents.size(); i++)
		{
			m_arrObjectComponents[i]->mPhysXObject->PreSimulate(dt);
		}

	}

	virtual void PostSimulate(float dt)
	{
		APhysXObject::PostSimulate(dt);

		for(unsigned int i=0; i<m_arrObjectComponents.size(); i++)
		{
			m_arrObjectComponents[i]->mPhysXObject->PostSimulate(dt);
		}

	}

	//??? to do... how about the Expire() function???

	//--------------------------------------------------------------------

protected:

	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

	virtual void OnReleaseBasicPhysXStuff();

	virtual void Delete();

protected:

	struct APhysXObjectComponent
	{
		APhysXObject* mPhysXObject;
		const char* mLocalName;			// name in the scope of APhysXCompoundObject
	};


	APhysXArray<APhysXObjectComponent* > m_arrObjectComponents;

};


#endif