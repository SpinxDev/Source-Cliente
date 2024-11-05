// ICLThreadWrapper.h: interface for the ICLThreadWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICLTHREADWRAPPER_H__826DC23B_082D_4A66_8506_3219CD8D34EF__INCLUDED_)
#define AFX_ICLTHREADWRAPPER_H__826DC23B_082D_4A66_8506_3219CD8D34EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ICLThreadWrapper  
{
public:
	ICLThreadWrapper(ICLThreadPool* pPool);
	virtual ~ICLThreadWrapper();

public:
	void Start();
	void Resume();
	void Suspend();
	void SyncStop();
	bool IsThreadValid() const;
	bool IsStopFlag() const { return m_bStopFlag; }
	ICLThreadPool* GetThreadPool() const { return m_pPool; }

protected:
	virtual bool InnerCreateThread();

protected:
	ICLThreadPool* m_pPool;
	CWinThread* m_pThread;
	bool m_bStopFlag;
};

#endif // !defined(AFX_ICLTHREADWRAPPER_H__826DC23B_082D_4A66_8506_3219CD8D34EF__INCLUDED_)
