
#include "APhysXStdAfx.h"


APhysXCollisionChannelManager::APhysXCollisionChannelManager()
{
	// the default case is that all isolated channels are relative ones...
	m_iRelativeIsolatedChannelNum =  APX_ISOLATED_CHANNEL_NUM;

	Release();
}

APhysXCollisionChannelManager::~APhysXCollisionChannelManager()
{
	// do nothing...
}

void APhysXCollisionChannelManager::Release()
{
	m_MutexChannelRefCount.Lock();
	
	for(int i=0; i<APX_ISOLATED_CHANNEL_NUM; i++)
	{
		m_iIsolatedChannelRefCount[i] = 0;
	}

	m_MutexChannelRefCount.Unlock();
}

APhysXCollisionChannel APhysXCollisionChannelManager::GetRelativeIsolatedChannel()
{

	APhysXCollisionChannel apxIsoChannel = APX_COLLISION_CHANNEL_INVALID;

	m_MutexChannelRefCount.Lock();

	for(int i=0; i<m_iRelativeIsolatedChannelNum; i++)
	{
		if(m_iIsolatedChannelRefCount[i] == 0)
		{
			apxIsoChannel = i;
			break;
		}
	}

	m_MutexChannelRefCount.Unlock();	

	return apxIsoChannel;

}

APhysXCollisionChannel APhysXCollisionChannelManager::GetAbsoluteIsolatedChannel()
{
	APhysXCollisionChannel apxIsoChannel = APX_COLLISION_CHANNEL_INVALID;

	m_MutexChannelRefCount.Lock();

	for(int i=m_iRelativeIsolatedChannelNum; i<APX_ISOLATED_CHANNEL_NUM; i++)
	{
		if(m_iIsolatedChannelRefCount[i] == 0)
		{
			apxIsoChannel = i;
			break;
		}
	}

	m_MutexChannelRefCount.Unlock();	

	return apxIsoChannel;

}

NxGroupsMask APhysXCollisionChannelManager::GetChannelGroupMask(APhysXCollisionChannel apxCollisionChannel)
{
	NxGroupsMask resGroupMask;

	resGroupMask.bits0 = resGroupMask.bits1 = resGroupMask.bits2 = resGroupMask.bits3 = 0;

	if(apxCollisionChannel<32)			// collision channel: 0 ~~~ 31
	{
		resGroupMask.bits0 = 1 << apxCollisionChannel;
	}
	else if(apxCollisionChannel<64)		// collision channel: 32 ~~~ 63
	{
		resGroupMask.bits1 = 1 << (apxCollisionChannel - 32);
	}
	else if(apxCollisionChannel<96)		// collision channel: 64 ~~~ 95
	{
		resGroupMask.bits2 = 1 << (apxCollisionChannel - 64);
	}
	else if(apxCollisionChannel<128)	// collision channel: 96 ~~~ 127
	{
		resGroupMask.bits3 = 1 << (apxCollisionChannel - 96);
	}
	else if(apxCollisionChannel == APX_COLLISION_CHANNEL_INVALID)
	{
		// do nothing...
	}
	else if(apxCollisionChannel == APX_COLLISION_CHANNEL_COMMON)
	{
		if(m_iRelativeIsolatedChannelNum < 32)
		{
			resGroupMask.bits0 = (1 << m_iRelativeIsolatedChannelNum) -1;
		}
		else if(m_iRelativeIsolatedChannelNum == 32)
		{
			resGroupMask.bits0 = 0xffffffff;
		}
		else if(m_iRelativeIsolatedChannelNum < 64)
		{
			resGroupMask.bits1 = (1 << (m_iRelativeIsolatedChannelNum - 32)) -1;
			resGroupMask.bits0 = 0xffffffff;
		}
		else if(m_iRelativeIsolatedChannelNum == 64)
		{
			resGroupMask.bits1 = resGroupMask.bits0 = 0xffffffff;
		}
		else if(m_iRelativeIsolatedChannelNum < 96)
		{
			resGroupMask.bits2 = (1 << (m_iRelativeIsolatedChannelNum - 64)) -1;
			resGroupMask.bits1 = resGroupMask.bits0 = 0xffffffff;
		}
		else if(m_iRelativeIsolatedChannelNum == 96)
		{
			resGroupMask.bits2 = resGroupMask.bits1 = resGroupMask.bits0 = 0xffffffff;
		}
		else if(m_iRelativeIsolatedChannelNum < 128)
		{
			resGroupMask.bits3 = (1 << (m_iRelativeIsolatedChannelNum - 96)) -1;
			resGroupMask.bits2 = resGroupMask.bits1 = resGroupMask.bits0 = 0xffffffff;
		}
		else if(m_iRelativeIsolatedChannelNum == 128)
		{
			resGroupMask.bits3 = resGroupMask.bits2 = resGroupMask.bits1 = resGroupMask.bits0 = 0xffffffff;
		}


	}
	else if(apxCollisionChannel == APX_COLLISION_CHANNEL_COLLIDE_ALL)
	{
		resGroupMask.bits0 = resGroupMask.bits1 = resGroupMask.bits2 = resGroupMask.bits3 = 0xffffffff;
	}
	else if(apxCollisionChannel == APX_COLLISION_CHANNEL_CC_COMMON_HOST)
	{
		// currently, we just use the last relative isolated channel...
		resGroupMask = GetChannelGroupMask( m_iRelativeIsolatedChannelNum - 1 );
	}
	else if(apxCollisionChannel == APX_COLLISION_CHANNEL_CC_SKIP_COLLISION)
	{
		// currently, we just use the last relative isolated channel...
		resGroupMask = GetChannelGroupMask( m_iRelativeIsolatedChannelNum - 2 );
	}
	else if(apxCollisionChannel == APX_COLLISION_CHANNEL_DYNAMIC_CC)
	{
		// currently, we just use the last relative isolated channel...
		resGroupMask = GetChannelGroupMask( m_iRelativeIsolatedChannelNum - 3 );
	}


	return resGroupMask;
}

void APhysXCollisionChannelManager::AddChannelRef(APhysXCollisionChannel apxCollisionChannel)
{
	if(apxCollisionChannel < APX_ISOLATED_CHANNEL_NUM)
	{
		m_MutexChannelRefCount.Lock();
		
		m_iIsolatedChannelRefCount[apxCollisionChannel] ++;

		m_MutexChannelRefCount.Unlock();		
	}

}

void APhysXCollisionChannelManager::RemoveChannelRef(APhysXCollisionChannel apxCollisionChannel)
{
	if(apxCollisionChannel < APX_ISOLATED_CHANNEL_NUM)
	{
		m_MutexChannelRefCount.Lock();

		if(m_iIsolatedChannelRefCount[apxCollisionChannel])
			m_iIsolatedChannelRefCount[apxCollisionChannel] --;
		
		m_MutexChannelRefCount.Unlock();		
	}


}
