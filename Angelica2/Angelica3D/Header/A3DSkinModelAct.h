/*
 * FILE: A3DSkinModelAct.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/6/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSKINMODELACT_H_
#define _A3DSKINMODELACT_H_

#include "A3DObject.h"
#include "AArray.h"
#include "AString.h"
#include "AList2.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DAnimJoint;
class A3DSkinModel;
class A3DSMActionChannel;
class A3DSklTrackSet;
class A3DVECTOR3;
class A3DQUATERNION;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSkinModelActionCore
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinModelActionCore : public A3DObject
{
public:		//	Types

	//	Act channels
	enum
	{
		ACTCHA_MAX	= 16,	//	Keep this value same with A3DSkinModel::ACTCHA_MAX
	};

	friend class A3DSkinModelAction;
	friend class A3DSMActionChannel;
	friend class A3DSkinModel;
	friend class A3DSkinModelMan;

public:		//	Constructor and Destructor

	A3DSkinModelActionCore() { A3DSkinModelActionCore(30); }
	A3DSkinModelActionCore(int iFrameRate);
	virtual ~A3DSkinModelActionCore();

public:		//	Attributes

public:		//	Operations

	//	Skip action data in file
	static bool SkipFileData(AFile* pFile, DWORD dwVer);
	//	Convert frame to time
	static DWORD FrameToTime(float fFrame, int iFrameRate);

	//	Set name
	virtual void SetName(const char* szName);

	//	Load action from file
	bool Load(AFile* pFile, DWORD dwVer, const char* szTckDir);
	//	Save action from file
	bool Save(AFile* pFile);

	//	Get action name ID
	DWORD GetActionNameID() const { return m_dwNameID; }
	//	Get action frame rate
	int GetFrameRate() const { return m_iFrameRate; }
	//	Set action frame rate
	void SetFrameRate(int iRate) { m_iFrameRate = iRate; }
	//	Get time (ms) of this action played one time
	DWORD GetActionTime() const { return m_dwActTime; }
	//	Get start frame
	float GetStartFrame() const { return m_fStartFrame; }
	//	Get end frame
	float GetEndFrame() const { return m_fEndFrame; }
	//	Set start and end frame
	void SetStartAndEndFrame(float fStartFrame, float fEndFrame);
	//	Get start time
	DWORD GetStartTime() const { return m_dwStartTime; }
	//	Get end time
	DWORD GetEndTime() const { return m_dwEndTime; }

	//	Check whether action can be removed from cache
	bool IsOutOfCacheDate(DWORD dwCurTime);

	//	Get current bound track set
	A3DSklTrackSet* GetTrackSet() { return m_pTrackSet; }
	//	Get track set file name ID
	DWORD GetTrackSetFileNameID() const { return m_dwTckNameID; }
	//	Set track set file name
	void SetTrackSetFileName(const char* szFileName, bool bOwnTckFile);
	//	Get track set file name
	const char* GetTrackSetFileName() const { return m_strTrackName; }
	//	Get position and orientation of specified time
	bool GetBoneStateOnTrack(int idJoint, int iAbsTime, A3DVECTOR3& vPos, A3DQUATERNION& quRot);
	bool GetBoneStateOnAbsTrack(int idJoint, int iAbsTime, A3DVECTOR3& vPos, A3DQUATERNION& quRot);

	//	Set / Get action rank
	void SetRank(int n, BYTE byRank) { if (n >= 0 && n < ACTCHA_MAX) m_aRanks[n] = byRank; }
	BYTE GetRank(int n) { if (n >= 0 && n < ACTCHA_MAX) return m_aRanks[n]; else return 0; }

protected:	//	Attributes

	float	m_fStartFrame;	//	Start frame
	float	m_fEndFrame;	//	End frame
	DWORD	m_dwStartTime;	//	Animation start time
	DWORD	m_dwEndTime;	//	Animation end time
	DWORD	m_dwNameID;		//	Action name ID calculated by name string
	int		m_iFrameRate;	//	Action frame rate
	DWORD	m_dwActTime;	//	Time (ms) this action played one time

	BYTE	m_aRanks[ACTCHA_MAX];	//	Action rank of each action channel

	DWORD	m_dwTckNameID;	//	Track set ID generated by track set file name
	AString	m_strTrackName;	//	Track set file name
	bool	m_bOwnTckFile;	//	true, m_strTrackName is built by Load() function
	A3DSklTrackSet*	m_pTrackSet;	//	Track object

	int		m_iRefCnt;		//	Reference counter
	DWORD	m_dwRefTime;	//	Last reference time

protected:	//	Operations

	//	Try to bind track set
	bool BindTrackSet();
	//	Unbind track set
	void UnbindTrackSet();

	//	On start playing
	void OnStartPlaying();
	//	On stop playing
	void OnStopPlaying();
};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinModelAction
//
///////////////////////////////////////////////////////////////////////////

class A3DSkinModelAction
{
public:		//	Types

	//	Stop flag used by stop callback function
	enum
	{
		STOP_FORCED = 0,		//	stopped by Stop()
		STOP_INTERRUPTED,		//	stopped because another animation of this joint is played
		STOP_FINISH,			//	stopped because playing ends.
	};

	friend class A3DSkinModel;
	friend class A3DSMActionChannel;

public:		//	Constructors and Destructors

	A3DSkinModelAction(A3DSkinModel* pSkinModel, A3DSkinModelActionCore* pCore);
	virtual ~A3DSkinModelAction();
	
public:		//	Attributes

public:		//	Operations

	//	Update routine
	bool Update(int iDeltaTime, bool bApplyToBone=true);

	//	Set action loop time dynamically, this will effect all joint actions
	//	in this model action
	void SetLoopTime(int iLoop) { m_iLoopTime = iLoop; }
	//	Get action loop time
	int GetLoopTime() const { return m_iLoopTime; }

	//	Check whether action is stopped
	bool IsStopped() { return m_bStop; }
	//	Get action playing time counter (ms)
	DWORD GetTimeCounter() { return m_dwTimeCnt; }

	//	Get action core data
	A3DSkinModelActionCore* GetCoreData() { return m_pCore; }

protected:	//	Attributes

	A3DSkinModel*				m_pA3DSkinModel;	//	Skin model object
	A3DSkinModelActionCore*		m_pCore;			//	Core data
	A3DSMActionChannel*			m_pChannel;			//	Action channel

	int		m_iPlayCnt;			//	Played animation joint counter
	DWORD	m_dwTimeCnt;		//	Action playing time counter
	float	m_fDeltaTime;		//	Delta time error accumulate
	int		m_iLoopTime;		//	Loop time of this action
	int		m_iLoopCnt;			//	Loop counter
	bool	m_bStop;			//	Stop flag
	bool	m_bStopNextTick;	//	Stop at next tick
	DWORD	m_dwCurTime;		//	The current absolute playing time
	DWORD	m_dwTransTime;		//	Transition time
	bool	m_bAbsTrack;		//	true, use absolute track when necessary

protected:	//	Operations

	//	Play action
	void Play(A3DSMActionChannel* pChannel, int iTransTime, int iNumLoop, bool bAbsTrack);
	//	Stop action
	void Stop();

	//	Update joint actions
	void UpdateJointActions(DWORD dwAbsTime);
	//	When action stopped
	void OnStop(int iReason);
};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSMActionChannel
//
///////////////////////////////////////////////////////////////////////////

class A3DSMActionChannel
{
public:		//	Types

	//	Action playing mode of this channel
	enum
	{
		PLAY_NORMAL = 0,
		PLAY_COMBINE,
		PLAY_EXCLUSIVE,
	};

	//	Action node
	struct ACT_NODE
	{
		int		iRank;		//	Action rank

		A3DSkinModelAction*	pAction;	//	Action object
	};

	friend class A3DSkinModel;

public:		//	Constructors and Destructors

	A3DSMActionChannel(A3DSkinModel* pSkinModel, int iChannel);
	virtual ~A3DSMActionChannel();
	
public:		//	Attributes

public:		//	Operations

	//	Update
	bool Update(int iDeltaTime);

	//	Play action
	bool PlayActionByName(const char* szName, int iNumLoop=1, int iTransTime=200, bool bRestart=true, bool bAbsTrack=false);
	//	Stop action of specified rank
	void StopAction(int iRank=-1);

	//	Build bone index array
	void BuildBoneIndices(int iNumBone, int* aBones);
	//	Get bone number
	int GetBoneNum() { return m_aBones.GetSize(); }
	//	Get bone index
	int GetBoneIndex(int n) { return m_aBones[n]; }
	//	Get / Set weight of action played in this channel
	void SetWeight(float fWeight) { m_fWeight = fWeight; }
	float GetWeight() { return m_fWeight; }
	//	Get channel index in skin model
	int GetChannelIndex() { return m_iChannel; }
	//	Get / Set action playing mode
	void SetPlayMode(int iMode) { m_iPlayMode = iMode; }
	int GetPlayMode() { return m_iPlayMode; }

	//	Prepare channel bones' transition
	void PrepareBoneTransition(int iTransTime);

protected:	//	Attributes

	int					m_iChannel;			//	Index of this channel in skinmodel
	A3DSkinModel*		m_pA3DSkinModel;	//	Skin model object
	float				m_fWeight;			//	Weight of action played in this channel
	int					m_iPlayMode;		//	Action playing mode

	AArray<int, int>	m_aBones;			//	Bones contained by this channel
	APtrList<ACT_NODE*>	m_ActRankList;		//	Actions of different rank

protected:	//	Operations

	//	Get action who has the top rank
	A3DSkinModelAction* GetTopRankAction(int* piRank);
	//	Add action to rank list
	int AddActionToRankList(A3DSkinModelAction* pAction, bool bRestart);
	//	Stop specified action
	void StopAction(A3DSkinModelAction* pAction);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DSKINMODELACT_H_
