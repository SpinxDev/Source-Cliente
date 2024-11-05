/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008


	APhysXCollisionChannelManager is a class to manage the collision channels
of APhysXObjects. The collision channel can provide more flexible and comprehensive
functions than collision group...


----------------------------------------------------------------------*/

#ifndef _APHYSXCOLLISIONCHANNELMANAGER_H_
#define _APHYSXCOLLISIONCHANNELMANAGER_H_

//---------------------------------------------------------------------
// the max num of isolated channels we can support is 128, this is 
// due to the internal limit of PhysX SDK.so do not change this!
#define APX_ISOLATED_CHANNEL_NUM 128
//---------------------------------------------------------------------


typedef APhysXU32 APhysXCollisionChannel;


/************************************************************************
	
	About Collision Channel:

	0 ~~~ 127 : 
			Isolated channel means APhysXObjects with the different
		channels will not collide with each other. Only those objects 
		having same channel can collide. Furthermore, we have two types
		of isolated channels. One is called absolute isolated channel, it
		won't collide with APX_COLLISION_CHANNEL_COMMON. Another is called
		relative isolated channel, on the contrary, it can collide with 
		APX_COLLISION_CHANNEL_COMMON. Detailed absolute and relative isolated
		channels are as following
		
		{ 0 ~ m_iRelativeIsolatedChannelNum-1:		relative isolated channel
		{
		{ m_iRelativeIsolatedChannelNum ~ 127:		absolute isolated channel


	APX_COLLISION_CHANNEL_INVALID:
			
			The collision channel with group mask's all bits set to Zero. 
		Objects in this channel will not collide with any other objects.
		(even objects in this channel as well)

			It is regarded as the invalid channel here.

	APX_COLLISION_CHANNEL_COMMON:

			The default collision channel and most APhysXObjects are set 
		to this channel. Objects with this channel will collide each other
		and will collide with those in channel relative isolated channel 
		but not those in absolute isolated channel.
			
			The ground(terrain), buildings and common objects in the world
		should be set to this channel.

	APX_COLLISION_CHANNEL_COLLIDE_ALL:

			This channel's objects will collide all other objects even those
		in absolute isolated channel.
	
	APX_COLLISION_CHANNEL_CC_COMMON_HOST:

			This channel is used for Character Controlling(CC). The host object
		which the CC is binding to should use this channel but not Common channel.
		So CC's Move(...) function can filter the host object correctly.

************************************************************************/


enum
{
	APX_COLLISION_CHANNEL_INVALID			= 128,
	APX_COLLISION_CHANNEL_COMMON,
	APX_COLLISION_CHANNEL_COLLIDE_ALL,


	APX_COLLISION_CHANNEL_CC_COMMON_HOST,					// for the actors which are parts of skeleton RB object bound to APhysXCC (also include the cloths attached)
	
	APX_COLLISION_CHANNEL_CC_SKIP_COLLISION,				// 1. for the actors which users want to skip their collision by APhysXCC's move() function, such as grasses...
															// 2. for the kinematic actors behind APhysXCC

	APX_COLLISION_CHANNEL_DYNAMIC_CC,						// for the actors created for APhysX Dynamic CC: APhysXDynCharCtrler

};


class APHYSX_DLL_EXPORT APhysXCollisionChannelManager
{


public:
	APhysXCollisionChannelManager();
	~APhysXCollisionChannelManager();

	// note: this should be called just after the constructor.
	void InitRelativeIsolatedChannelNum(int iNum)
	{
		// we should have at least one relative isolated channel...
		if(iNum >= 1 && iNum <= APX_ISOLATED_CHANNEL_NUM)
			m_iRelativeIsolatedChannelNum = iNum;
	}

	void Release();

	APhysXCollisionChannel GetRelativeIsolatedChannel();
	APhysXCollisionChannel GetAbsoluteIsolatedChannel();
	
	NxGroupsMask GetChannelGroupMask(APhysXCollisionChannel apxCollisionChannel);

	void AddChannelRef(APhysXCollisionChannel apxCollisionChannel);
	void RemoveChannelRef(APhysXCollisionChannel apxCollisionChannel);


protected:

	int m_iIsolatedChannelRefCount[APX_ISOLATED_CHANNEL_NUM];
	int m_iRelativeIsolatedChannelNum;

	APhysXMutex m_MutexChannelRefCount;

};

#endif