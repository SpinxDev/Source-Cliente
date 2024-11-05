
/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008


	APhysXBreakable: the virtual interface of breakable object...
----------------------------------------------------------------------*/

#ifndef _APHYSXBREAKABLE_H_
#define _APHYSXBREAKABLE_H_


class APhysXBreakable;
class APhysXObject;

class APHYSX_DLL_EXPORT APhysXUserBreakReport
{

public:

	// must be overridden in child classes...
	virtual void OnBreakNotify(APhysXBreakable* pBreakable) = 0;				// each break event

	virtual void OnFirstBreakNotify(APhysXBreakable* pBreakable) = 0;			// the event from the normal state to break state

	virtual void OnAllBreakNotify(APhysXBreakable* pBreakable) = 0;				// the event that all parts are broken

protected:

	virtual ~APhysXUserBreakReport() {}

};

class APHYSX_DLL_EXPORT APhysXBreakable
{

public:

	APhysXBreakable() : m_vBreakCenter(0.0f), m_vBreakForce(0.0f)
	{
		m_pUserBreakReport = NULL;
		mBreakUserData = NULL;

		m_bBroken = false;
		m_bFullyBroken = false;

	}

	// retrieve the APhysXObject's interface....
	virtual APhysXObject* IsAPhysXObject()
	{
		return NULL; 
	}

	// reset the status to the initial state ( sanity state )
	virtual void Reset()
	{
		m_bBroken = false;
		m_bFullyBroken = false;
	}

	void SetUserBreakReport(APhysXUserBreakReport* pUserBreakReport)
	{
		m_pUserBreakReport = pUserBreakReport;
	}

	bool IsBroken() const { return m_bBroken; }
	bool IsFullyBroken() const { return m_bFullyBroken; }

	// some extra info of breaking...
	const NxVec3& GetBreakPos() const						// it always be the hit pos or some other pos?
	{
		return m_vBreakCenter;
	}
	
	float GetBreakMagnitude() const					// it describe the how hard is the break effect?
	{
		return m_vBreakForce.magnitude();
	}

public:
	
	// public data member: to keep the pointer of user data...
	void* mBreakUserData;				


protected:

	virtual ~APhysXBreakable() {}

	virtual void PreBreak() {}
	virtual void OnBreak() {}
	virtual void PostBreak()
	{
		NotifyBreak();
	}

	virtual void Break()
	{
		PreBreak();
		OnBreak();
		PostBreak();
	}

	virtual void NotifyBreak()
	{
		if(m_pUserBreakReport)
		{
			m_pUserBreakReport->OnBreakNotify(this);
		}

	}

	virtual void NotifyFirstBreak()
	{
		m_bBroken = true;

		if(m_pUserBreakReport)
		{
			m_pUserBreakReport->OnFirstBreakNotify(this);
		}

	}

	virtual void NotifyAllBreak()
	{
		m_bFullyBroken = true;
		if(m_pUserBreakReport)
		{
			m_pUserBreakReport->OnAllBreakNotify(this);
		}

	}


protected:

	APhysXUserBreakReport* m_pUserBreakReport;


	bool m_bBroken;
	bool m_bFullyBroken;

	NxVec3 m_vBreakForce;
	NxVec3 m_vBreakCenter;
};

//-----------------------------------------------------------------------
// APhysXSimpleBreakable:
//
//		Define one type of simple breakable stuff, which will break all at once
//
class APHYSX_DLL_EXPORT APhysXSimpleBreakable : public APhysXBreakable
{
public:

	APhysXSimpleBreakable()
	{
		m_fBreakLimit = NX_MAX_F32;
		m_bBroken = false;
	}

	void SetBreakLimit(float fLimit) { m_fBreakLimit = fLimit; }
	float GetBreakLimit() const { return m_fBreakLimit; }

	virtual void CheckBreak(float fBreakValue)
	{
		if(!IsBroken() && fBreakValue > m_fBreakLimit)
		{
			// oh, I'm broken...
			Break();
			
			NotifyFirstBreak();
			NotifyAllBreak();
		}
	}

	


protected:

	float m_fBreakLimit;

};

#endif
