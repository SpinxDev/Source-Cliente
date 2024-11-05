#ifndef _SYNCWRAPPER_H_
#define _SYNCWRAPPER_H_

template<class SyncObject>
class SyncWrapper
{
public:
	SyncWrapper(SyncObject& obj)
		: m_obj(obj)
	{
		m_obj.Lock();
	}

	~SyncWrapper()
	{
		m_obj.Unlock();
	}

private:
	SyncObject& m_obj;
};

#endif