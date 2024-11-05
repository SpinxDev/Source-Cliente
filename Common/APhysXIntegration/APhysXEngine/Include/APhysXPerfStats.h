/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXPERFSTATS_H_
#define _APHYSXPERFSTATS_H_


class APHYSX_DLL_EXPORT APhysXPerfItem
{

public:
	
	APhysXPerfItem(const char* szName)
	{
		m_strName = szName;
		Reset();
	}

	void Reset();
	
	void StartFrame();
	void EndFrame();

	void Pause();
	void Resume();


	bool IsName(const char* szName) const
	{
		return (m_strName == szName);
	}

	const APhysXString& GetName() const
	{ 
		return m_strName; 
	}

	float GetCurFPS() const
	{
		return 1.0f/m_fCurFrameSeconds;
	}

	float GetMinFPS() const
	{
		return 1.0f/m_fMinFrameSeconds;
	}

	float GetMaxFPS() const
	{
		return 1.0f/m_fMaxFrameSeconds;
	}

	float GetAvgFPS() const
	{
		return (1.0f * m_iFrameNum / m_fTotalFrameSeconds);
	}

	float GetCurFrameMs() const
	{
		return m_fCurFrameSeconds * 1000; 
	}

	float GetMinFrameMs() const
	{
		return m_fMinFrameSeconds * 1000; 
	}

	float GetMaxFrameMs() const
	{
		return m_fMaxFrameSeconds * 1000; 
	}

	float GetAvgFrameMs() const
	{
		return m_fTotalFrameSeconds * 1000 / m_iFrameNum; 
	}

	float GetCurFrameSeconds() const
	{ 
		return m_fCurFrameSeconds; 
	}
	float GetMinFrameSeconds() const
	{ 
		return m_fMinFrameSeconds; 
	}
	float GetMaxFrameSeconds() const
	{ 
		return m_fMaxFrameSeconds; 
	}

	float GetMinFrameSecondsInLastWindow() const
	{ 
		return m_fMinFrameSecondsInLastWindow; 
	}
	float GetMaxFrameSecondsInLastWindow() const
	{ 
		return m_fMaxFrameSecondsInLastWindow; 
	}


	float GetAvgFrameSeconds() const
	{ 
		return m_fTotalFrameSeconds / m_iFrameNum; 
	}

	bool IsFrameDone() const { return m_bFrameDone; }

protected:

	APhysXString m_strName;

	//---------------------------------------------------------
	// MS VC6.0 do not support static member variables to be initialized inside the class
	//static const APhysXU32 FRAME_WINDOW_NUM = 30;
#define FRAME_WINDOW_NUM 30
	//---------------------------------------------------------	


/*

	
	APhysXU32 m_MinFrameMs;
	APhysXU32 m_MaxFrameMs;
	APhysXU32 m_CurFrameMs;

	APhysXU32 m_TotalFrameMs;
	
	// frame window to compute average FPS
	APhysXU32 m_FrameMs[FRAME_WINDOW_NUM];

	DWORD m_dwCurMs;
*/

	// Min and Max frame time in last window...
	float m_fMinFrameSecondsInLastWindow;
	float m_fMaxFrameSecondsInLastWindow;
	
	// Min and Max frame time in cur window...
	float m_fMinFrameSecondsInCurWindow;
	float m_fMaxFrameSecondsInCurWindow;

	// global Min and Max frame time...
	float m_fMinFrameSeconds;
	float m_fMaxFrameSeconds;
	
	float m_fCurFrameSeconds;
	float m_fTotalFrameSeconds;
	
	float m_fAccumulatedSeconds;
	double m_dCurTimeTicks;

	// frame window to compute average FPS
	float m_FrameSeconds[FRAME_WINDOW_NUM];
	
	int m_iFrameNum;
	int m_iCur;

	bool m_bActive;
	bool m_bFrameDone;

};


class APHYSX_DLL_EXPORT APhysXPerfStats
{

public:

	//--------------------------------------------------------

	typedef APhysXMap<APhysXString, APhysXPerfItem*>	PerfItemMap;
	typedef PerfItemMap::value_type						PerfItemPair;
	typedef PerfItemMap::iterator						PerfItemIterator;
	typedef PerfItemMap::const_iterator					PerfItemConstIterator;

	//--------------------------------------------------------

	~APhysXPerfStats()
	{
		Release();
	}

	void Init();
	void Release();

	APhysXPerfItem* StartPerfStat(const char* szStatsName);
	void EndPerfStat();

	int GetPerfStatsNum() const { return m_PerfStats.size(); }

	// for outside traverse...
	/*
	PerfItemConstIterator BeginIterator() const { return m_PerfStats.begin(); }
	PerfItemConstIterator EndIterator() const { return m_PerfStats.end(); }
	const APhysXPerfItem* GetPerfItem(const PerfItemConstIterator& it) const { return it->second; }
	APhysXPerfItem* GetPerfItem(const PerfItemConstIterator& it)	{ return it->second; }
	*/

	const APhysXPerfItem* GetPerfItem(int iNum) const { return m_PerfStats[iNum]; }
	APhysXPerfItem* GetPerfItem(int iNum) { return m_PerfStats[iNum]; }


	APhysXPerfItem* GetPerfItem(const char* szStatsName);

	void ResumePerfStat(const char* szStatsName);
	void PausePerfStat(const char* szStatsName);

	// this will call every APhysXPerfItem's EndFrame()...
	void EndFrame();

protected:

	APhysXArray<APhysXPerfItem* > m_PerfStats;
	PerfItemMap m_PerfStatsMap;

	APhysXArray<APhysXPerfItem* > m_PerfStatsStack;

};


#endif