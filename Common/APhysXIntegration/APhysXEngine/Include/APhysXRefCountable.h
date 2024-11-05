
/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, June.1, 2009

	Desc:

		The base class of reference countable objects


----------------------------------------------------------------------*/

#ifndef _APHYSXREFCOUNTABLE_H_
#define _APHYSXREFCOUNTABLE_H_


class APHYSX_DLL_EXPORT APhysXRefCountable
{

public:
	APhysXRefCountable():m_iRefCount(0) {}
	virtual ~APhysXRefCountable()
	{
		APHYSX_ASSERT(m_iRefCount == 0);
	}
	

	int AddRef() const
	{
		return (++m_iRefCount);
	}

	int RemoveRef() const 
	{
		APHYSX_ASSERT(m_iRefCount > 0);

		if(--m_iRefCount == 0)
		{
			delete this;
			return 0;
		}
		else
			return m_iRefCount;
	}

	int GetRefCount() const { return m_iRefCount;}


private:

	mutable int m_iRefCount;

};



#endif
