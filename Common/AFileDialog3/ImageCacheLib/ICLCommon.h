#ifndef _ICL_COMMON_H_
#define _ICL_COMMON_H_

template<typename Locker>
class lockerd_wrapper
{
public:
	lockerd_wrapper(Locker& cs)
		: m_cs(cs)
	{
		m_cs.Lock();
	}
	~lockerd_wrapper()
	{
		m_cs.Unlock();
	}
private:
	Locker& m_cs;
};


template<typename bufbyte>
class buffer_wrapper
{
public:
	buffer_wrapper(bufbyte*& pBuf)
		: m_pBuf(pBuf)
	{

	}
	~buffer_wrapper()
	{
		delete [] m_pBuf;
	}

private:
	bufbyte*& m_pBuf;
};

template<typename Container>
void clearPointerArray(Container& conter)
{
	for (Container::iterator itr = conter.begin(); itr != conter.end(); ++itr)
		delete *itr;

	conter.clear();
}

#endif