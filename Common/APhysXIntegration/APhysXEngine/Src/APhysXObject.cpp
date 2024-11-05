
#include "APhysXStdAfx.h"


bool APhysXObject::OnCreate(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{

	mUserData = NULL;

	//-----------------------------------------------
	// revised by Wenfeng, Jan. 14, 2013
	// the following consideration only solve the issue partially
	// since it can not work well for compound object.
	// so, we have to call m_MutexNxSceneWriting's Lock()/Unlock()
	// code only when real physx stuff are created.
	//
	// revised by Wenfeng, Dec. 11, 2012
	// move the m_MutexNxSceneWriting's Lock()/Unlock() here but
	// leave AddObjectDescRef() outside so that we can avoid
	// the dead-lock caused by acquiring both two locks( one is 
	// m_pAPhysXScene's m_MutexNxSceneWriting and another is
	// gPhysXEngine->GetObjectDescManager()'s m_MutexObjectDescs)
	// during object's creation and deletion...

	if(!PreCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
	{
		return false;
	}

	if(!OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
	{
		return false;
	}

	if(!PostCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
	{
		// before return, we should release the PhysX stuff created during last step
		OnReleaseBasicPhysXStuff();
		return false;
	}


	//??? compound object case???
	// if the APhysXObject is created successfully, increase reference count to its' object desc...
	if(gPhysXEngine->GetObjectDescManager()->AddObjectDescRef(apxObjInstanceDesc.mPhysXObjectDesc))
		m_pMyObjectDesc = apxObjInstanceDesc.mPhysXObjectDesc;
	else
		m_pMyObjectDesc = NULL;

	m_fCurLife = 0.0f;
	m_bIsDeleted = false;
	
	// user's creation callback
	gPhysXCreationDeletionReport->OnObjectCreationNotify(this);

	return true;

}

void APhysXObject::OnRelease()
{
	// user's deletion callback
	gPhysXCreationDeletionReport->OnObjectDeletionNotify(this);

	//-----------------------------------------------
	// revised by Wenfeng, Jan. 14, 2013
	// the following consideration only solve the issue partially
	// since it can not work well for compound object.
	// so, we have to call m_MutexNxSceneWriting's Lock()/Unlock()
	// code only when real physx stuff are created.
	//
	// revised by Wenfeng, Dec. 11, 2012
	// move the m_MutexNxSceneWriting's Lock()/Unlock() here but
	// leave RemoveObjectDescRef() outside so that we can avoid
	// the dead-lock caused by acquiring both two locks( one is 
	// m_pAPhysXScene's m_MutexNxSceneWriting and another is
	// gPhysXEngine->GetObjectDescManager()'s m_MutexObjectDescs)
	// during object's creation and deletion...

	
	PreReleaseBasicPhysXStuff();
	OnReleaseBasicPhysXStuff();
	PostReleaseBasicPhysXStuff();
	
	//-----------------------------------------------

	if(m_pMyObjectDesc)
		gPhysXEngine->GetObjectDescManager()->RemoveObjectDescRef(m_pMyObjectDesc);

	m_pMyObjectDesc = NULL;

	m_pAPhysXScene = NULL;
	m_pUserGraphicsSync = NULL;

}

APhysXObjectConnector* APhysXObject::GetInnerConnector(const char* szName) const
{
	if(!szName) return NULL;

	for(int i=0; i<m_arrInnerConnectors.size(); i++)
	{
		if(m_arrInnerConnectors[i]->GetName() &&  strcmp(szName, m_arrInnerConnectors[i]->GetName()) == 0 )
		{
			return m_arrInnerConnectors[i];
		}
	}

	return NULL;
	

}


void APhysXObject::RemoveInnerConnector(APhysXObjectConnector* pPhysXObjConnector)
{
	m_arrInnerConnectors.deleteEntry(pPhysXObjConnector);
	pPhysXObjConnector->OnReleaseByPhysXObject();
	delete pPhysXObjConnector;
}

// release all connectors
void APhysXObject::ReleaseInnerConnectors()
{
	for(int i=0; i<m_arrInnerConnectors.size(); i++)
	{
		m_arrInnerConnectors[i]->OnReleaseByPhysXObject();
		delete m_arrInnerConnectors[i];
	}

	m_arrInnerConnectors.clear();
}

void APhysXObject::RemoveOuterConnector(APhysXObjectConnector* pPhysXObjConnector)
{
	m_arrOuterConnectors.deleteEntry(pPhysXObjConnector);
	//pPhysXObjConnector->OnReleaseByPhysXScene(this);
}

void APhysXObject::ReleaseOuterConnectors()
{
	for(int i=0; i<m_arrOuterConnectors.size(); i++)
	{
		m_arrOuterConnectors[i]->OnReleaseByPhysXScene(this);
	}

	m_arrOuterConnectors.clear();
}

bool APhysXObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	m_pAPhysXScene = pAPhysXScene;
	m_pUserGraphicsSync = apxObjInstanceDesc.mPhysXUserGraphicsSync;
	m_bNetworkSync = apxObjInstanceDesc.mPhysXObjectDesc->mNetworkSync;
	m_apxObjectType = (APhysXObjectType)apxObjInstanceDesc.mPhysXObjectDesc->GetObjType();	

	// we can use the name of apxObjInstanceDesc directly since its'name has been allocated by gGlobalStringManager
	m_szName = apxObjInstanceDesc.GetName();	

	m_fLifeSpan = apxObjInstanceDesc.mPhysXObjectDesc->mLifeSpan;

	// note: this is just to make the m_CollisionChannel a valid value
	// and the real value is set via APhysXScene's calling SetCollisionChannel(...)
	// m_CollisionChannel = APX_COLLISION_CHANNEL_COMMON;
	m_CollisionChannel = apxObjInstanceDesc.mPhysXObjectDesc->GetCollisionChannel();

	// m_CollisionGroup = (APhysXCollisionGroup)apxObjInstanceDesc.mPhysXObjectDesc->mCollisionGroup;
	m_CollisionGroup = (APhysXCollisionGroup)apxObjInstanceDesc.mPhysXObjectDesc->GetCollisionGroup();

	m_pParentObject = NULL;
	
	m_apxObjectFlags = apxObjInstanceDesc.mPhysXObjectDesc->mFlags;

	if(!APhysXUtilLib::CheckProperRigidTransform(apxObjInstanceDesc.mGlobalPose))
	{
		gPhysXLog->Log("Warning: try to create an object in a Non Proper Rigid Transformation Pose!");
	}

	return true;

}

bool APhysXObject::PostCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	// set collision channel for real physx stuff, such as NxActor, NxCloth, NxFluid,...
	if(!IsCompound())
		SetCollisionChannel(m_CollisionChannel);		// do not affect the component objects when creating compound objects...

	// create the connectors...
	APhysXObjectConnectorDescManager& myInnerConnectorDescManager = apxObjInstanceDesc.mPhysXObjectDesc->mInnerConnectorDescManager;

	// this is for multiple PhysX connectors' creation case
	bool bFailedPostCreatePhysXStuff = false;

	for(int i= 0; i<myInnerConnectorDescManager.GetNum(); i++)
	{
		APhysXObjectConnectorDesc* pCurConnectorDesc = myInnerConnectorDescManager.GetPhysXConnectorDesc(i);
		
		// it's a bug since this operation will change the joint desc's data member
		// pCurConnectorDesc->TransformLocal2Global(apxObjInstanceDesc.mGlobalPose, apxObjInstanceDesc.mScale3D);
		
		APhysXObjectConnectorInstanceDesc curConnectorInstanceDesc;
		curConnectorInstanceDesc.mPhysXConnectorDesc = pCurConnectorDesc;
		curConnectorInstanceDesc.mGlobalPose = apxObjInstanceDesc.mGlobalPose;
		curConnectorInstanceDesc.mScale3D = apxObjInstanceDesc.mScale3D;

		APhysXObjectConnector* pConnector = APhysXObjectConnector::CreateConnectorPtr(pCurConnectorDesc->GetConnectorType());
		if(pConnector)
		{
			bool bResult = pConnector->OnCreateByPhysXObject(this, curConnectorInstanceDesc);
			if(bResult)
			{
				m_arrInnerConnectors.push_back(pConnector);
			}
			else
			{
				delete pConnector;

				bFailedPostCreatePhysXStuff = true;
				break;
			}
		}
		else
		{
			bFailedPostCreatePhysXStuff = true;
			break;
		}
	}

	if(bFailedPostCreatePhysXStuff)
	{
		ReleaseInnerConnectors();
		return false;
	}
	else
		return true;
}

void APhysXObject::PreReleaseBasicPhysXStuff()
{
	// we release connnectors here
	ReleaseInnerConnectors();
	ReleaseOuterConnectors();

}

void APhysXObject::PostReleaseBasicPhysXStuff()
{
}

void APhysXObject::PreSimulate(float dt)
{
	m_fCurLife += dt;

	if(m_fCurLife >= m_fLifeSpan)
	{
		Expire();
	}

}

void APhysXObject::PostSimulate(float dt)
{
	// do nothing...

}

void APhysXObject::Expire()
{
	// destroy some resources corresponding to me.
	
	if(m_pUserGraphicsSync)
	{
		m_pUserGraphicsSync->ReleaseGraphicsData();
	}

	if(m_pAPhysXScene)
	{
		m_pAPhysXScene->ReleasePhysXObject(this);
	}

}

void APhysXObject::SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef)
{
	if(m_CollisionChannel == apxCollisionChannel)
		return;

	if(m_pAPhysXScene && bChangeChannelRef)
	{
		m_pAPhysXScene->GetCollisionChannelManager()->RemoveChannelRef(m_CollisionChannel);
	}

	m_CollisionChannel = apxCollisionChannel;

	if(m_pAPhysXScene && bChangeChannelRef)
	{
		m_pAPhysXScene->GetCollisionChannelManager()->AddChannelRef(apxCollisionChannel);
	}
}

void APhysXObject::SetObjectFlag(APhysXObjectFlag flag, bool val)
{
	if(val)
		m_apxObjectFlags |= flag;
	else
		m_apxObjectFlags &= ~flag;

#if 1
	// Dec. 8, 2009, by wenfeng

	// should we support the following property changing operation?

	// currently, we can't support changing collision group at runtime...
	
	// change collision channel via flag setting 
	if(flag == APX_OBJFLAG_CC_SKIP_COLLISION)
	{
		if(val)	
			SetCollisionChannel(APX_COLLISION_CHANNEL_CC_SKIP_COLLISION);
		else
			SetCollisionChannel(APX_COLLISION_CHANNEL_COMMON);
	}

#endif

}

bool APhysXObject::GetObjectFlag(APhysXObjectFlag flag) const
{
	return (m_apxObjectFlags & flag)? true : false;
}
