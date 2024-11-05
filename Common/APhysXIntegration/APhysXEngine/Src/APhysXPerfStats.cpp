
#include "APhysXStdAfx.h"

//-------------------------------------------------------------------------
// APhysXPerfItem member functions...

#define FIX_FRAME_WINDOW


void APhysXPerfItem::Reset()
{
	m_fCurFrameSeconds = 0.0f;
	m_fMaxFrameSeconds = 0.0f;
	m_fMinFrameSeconds = 10.0f;
	
	m_fMaxFrameSecondsInLastWindow = 0.0f;
	m_fMinFrameSecondsInLastWindow = 0.0f;

	m_fMaxFrameSecondsInCurWindow = 0.0f;
	m_fMinFrameSecondsInCurWindow = 10.0f;

	m_fTotalFrameSeconds = 0.0f;

	m_fAccumulatedSeconds = 0.0f;

#ifdef FIX_FRAME_WINDOW
	m_iFrameNum = 1;
#else
	m_iFrameNum = 0;
#endif

	m_iCur = 0;

	m_bActive = false;
	m_bFrameDone = true;

}


void APhysXPerfItem::StartFrame()
{
	if(m_bFrameDone)
	{
		m_fCurFrameSeconds = 0.0f;
		m_bFrameDone = false;

		Resume();
	}
}

void APhysXPerfItem::EndFrame()
{
	if(m_bFrameDone) return;
	m_bFrameDone = true;

	Pause();

	// statistic processing
	if(m_fCurFrameSeconds > m_fMaxFrameSeconds) m_fMaxFrameSeconds = m_fCurFrameSeconds;
	if(m_fCurFrameSeconds < m_fMinFrameSeconds) m_fMinFrameSeconds = m_fCurFrameSeconds;

	if(m_fCurFrameSeconds > m_fMaxFrameSecondsInCurWindow) m_fMaxFrameSecondsInCurWindow = m_fCurFrameSeconds;
	if(m_fCurFrameSeconds < m_fMinFrameSecondsInCurWindow) m_fMinFrameSecondsInCurWindow = m_fCurFrameSeconds;


	// note by wenfeng, Dec. 30, 2010
	// there are two implementation to compute the average Frame seconds...
	// 1. Fix Window method:	update the average value every FRAME_WINDOW_NUM frame
	// 2. Slip Window method:	update the average value at each frame
	// And method 2# will consume extra memory for the window buffer...

#ifdef FIX_FRAME_WINDOW

	m_fAccumulatedSeconds += m_fCurFrameSeconds;
	m_iCur++;

	if(m_iCur >= FRAME_WINDOW_NUM)
	{
		m_fTotalFrameSeconds = m_fAccumulatedSeconds;
		m_iFrameNum = m_iCur;

		m_fAccumulatedSeconds = 0.0f;
		m_iCur = 0;

		m_fMaxFrameSecondsInLastWindow = m_fMaxFrameSecondsInCurWindow;
		m_fMinFrameSecondsInLastWindow = m_fMinFrameSecondsInCurWindow;

		m_fMaxFrameSecondsInCurWindow = 0.0f;
		m_fMinFrameSecondsInCurWindow = 10.0f;
	}

#else
	
	if(m_iFrameNum < FRAME_WINDOW_NUM)
	{
		m_FrameSeconds[m_iCur++] = m_fCurFrameSeconds;
		
		m_iFrameNum++;
		m_fTotalFrameSeconds += m_fCurFrameSeconds;
	}
	else
	{
		if(m_iCur == FRAME_WINDOW_NUM) m_iCur = 0;

		m_fTotalFrameSeconds += m_fCurFrameSeconds - m_FrameSeconds[m_iCur];
		m_FrameSeconds[m_iCur++] = m_fCurFrameSeconds;

	}

#endif

	
}


void APhysXPerfItem::Pause()
{
	if(m_bActive)
	{
		double dCurTimeTicks = APhysX_GetTimeTicks();
		m_fCurFrameSeconds += (float)((dCurTimeTicks - m_dCurTimeTicks) / APhysX_GetClockFreq());
		m_bActive = false;
	}
}

void APhysXPerfItem::Resume()
{
	if(!m_bActive)
	{
		m_dCurTimeTicks = APhysX_GetTimeTicks();
		m_bActive = true;
	}
}


//-------------------------------------------------------------------------
// APhysXPerfStats member functions...

void APhysXPerfStats::Init()
{
	// to do...

}


void APhysXPerfStats::Release()
{
	for(int i=0; i<m_PerfStats.size(); i++)
	{
		delete m_PerfStats[i];
	}

	/*
	for(PerfItemIterator it = m_PerfStats.begin(); it != m_PerfStats.end(); ++it)
	{
		APhysXPerfItem* pPerfItem = it->second;
		delete pPerfItem;
	}
	*/

	m_PerfStats.clear();
	m_PerfStatsMap.clear();
	m_PerfStatsStack.clear();
}

APhysXPerfItem* APhysXPerfStats::StartPerfStat(const char* szStatsName)
{
	if(!gPhysXEngine->GetFlag(APX_ENGINEFLAG_ENABLEPROFILE)) return	NULL;

	APhysXPerfItem* pPerfItem = GetPerfItem(szStatsName);

	if(!pPerfItem)
	{
		pPerfItem = new APhysXPerfItem(szStatsName);
		m_PerfStatsMap.insert(PerfItemPair(szStatsName, pPerfItem));
		m_PerfStats.push_back(pPerfItem);
	}

	m_PerfStatsStack.push_back(pPerfItem);
	pPerfItem->StartFrame();

	return pPerfItem;
}

void APhysXPerfStats::EndPerfStat()
{
	if(!gPhysXEngine->GetFlag(APX_ENGINEFLAG_ENABLEPROFILE)) return;

	m_PerfStatsStack.back()->EndFrame();
	
	m_PerfStatsStack.pop_back();
}


APhysXPerfItem* APhysXPerfStats::GetPerfItem(const char* szStatsName)
{

	APhysXPerfItem* pPerfItem = NULL;

	PerfItemIterator it = m_PerfStatsMap.find(szStatsName);
	if(it != m_PerfStatsMap.end())
	{
		pPerfItem = it->second;
	}

	return pPerfItem;
}

void APhysXPerfStats::ResumePerfStat(const char* szStatsName)
{
	if(!gPhysXEngine->GetFlag(APX_ENGINEFLAG_ENABLEPROFILE)) return;

	APhysXPerfItem* pPerfItem = GetPerfItem(szStatsName);
	if(!pPerfItem)
	{
		pPerfItem = new APhysXPerfItem(szStatsName);
		m_PerfStatsMap.insert(PerfItemPair(szStatsName, pPerfItem));
		m_PerfStats.push_back(pPerfItem);
	}
	
	if(pPerfItem->IsFrameDone())
		pPerfItem->StartFrame();
	else
		pPerfItem->Resume();


}

void APhysXPerfStats::PausePerfStat(const char* szStatsName)
{
	if(!gPhysXEngine->GetFlag(APX_ENGINEFLAG_ENABLEPROFILE)) return;

	APhysXPerfItem* pPerfItem = GetPerfItem(szStatsName);
	if(pPerfItem)
	{
		pPerfItem->Pause();
	}
}


void APhysXPerfStats::EndFrame()
{
	for(int i=0; i<m_PerfStats.size(); i++)
	{
		APhysXPerfItem* pPerfItem = m_PerfStats[i];
		//---------------------------------------------------------------------
		// force to clear some item which may be not called at each frame

		if(pPerfItem->IsFrameDone())
			pPerfItem->StartFrame();
		else
			pPerfItem->Resume();
		pPerfItem->Pause();

		//---------------------------------------------------------------------

		pPerfItem->EndFrame();
	}	
}
