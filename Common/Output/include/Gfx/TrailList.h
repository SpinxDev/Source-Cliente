/*
* FILE: TrailList.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/04/23
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _TrailList_H_
#define _TrailList_H_

#include "RotList.h"
#include <vector.h>
#include <A3DVector.h>
#include <A3DTypes.h>

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

class A3DHermiteSpline;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Struct TRAIL_DATA
//	
///////////////////////////////////////////////////////////////////////////

struct TRAIL_DATA
{
	A3DVECTOR3	m_vSeg1;
	A3DVECTOR3	m_vSeg2;
	int			m_nSegLife;
	A3DCOLOR	m_Color;

	TRAIL_DATA() {}
	TRAIL_DATA& operator = (const TRAIL_DATA& src);
	TRAIL_DATA(const A3DVECTOR3& v1, const A3DVECTOR3& v2, int nSegLife, A3DCOLOR cl)
		: m_vSeg1(v1), m_vSeg2(v2), m_nSegLife(nSegLife), m_Color(cl) {}
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class TrailList
//	
///////////////////////////////////////////////////////////////////////////

class ITrailList
{
public:
	virtual ~ITrailList() = 0 {}
	virtual int GetCount() const = 0;
	virtual void AddKeyData(const TRAIL_DATA& keyData) = 0;
	virtual void UpdateRecentKeyData(const TRAIL_DATA& keyData) = 0;
	virtual void Tick(DWORD dwTickDelta) = 0;
	virtual void Reset() = 0;
	virtual int GetRenderCount() const = 0;
	virtual void GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const = 0;
	static ITrailList* CreateTrailList(bool bIsSplineMode);
	static void DestroyTrailList(ITrailList* pTrailImpl);
};

struct GRAVITY_DATA
{
	float	m_fGravity;
	float	m_fVerticalNoise;
	float	m_fVSpeed;
	float	m_fAmplitude;
	float	m_fHSpeed;
	bool	m_bXNoise;
	bool	m_bZNoise;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class SplineTrailList
//	
///////////////////////////////////////////////////////////////////////////

class SplineTrailList : public ITrailList
{
public:

	struct KPData
	{
		KPData(int nSegLife, A3DCOLOR Color)
			: m_nSegLife(nSegLife)
			, m_Color(Color)
		{
			
		}
		int m_nSegLife;
		A3DCOLOR m_Color;
	};

	SplineTrailList();
	virtual ~SplineTrailList();

	static INT DETAIL_POINTFACTOR;
	static FLOAT DETAIL_INVPOINTFACTOR;

protected:

	virtual int GetCount() const;
	virtual void AddKeyData(const TRAIL_DATA& keyData);
	virtual void UpdateRecentKeyData(const TRAIL_DATA& keyData);
	virtual void Tick(DWORD dwTickDelta);
	virtual void Reset();
	virtual int GetRenderCount() const;
	virtual void GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const;

protected:

	void AddTrailKeyPoint(const TRAIL_DATA& keyData);
	size_t GetKPNum() const;
	A3DVECTOR3 GetKPPosition(int nSpline, size_t nKPIndex) const;
	A3DVECTOR3 InterpPos(int nSpline, size_t nKPIndex, FLOAT t) const;
	A3DCOLOR InterpColor(size_t nKPIndex, FLOAT t) const;
	int InterpSegLife(size_t nKPIndex, FLOAT t) const;

private:

	abase::vector<KPData> m_aKPData;
	A3DHermiteSpline* m_pTrailSpline[2];
	size_t m_nStartIdx;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class LineTrailList
//	
///////////////////////////////////////////////////////////////////////////

class LineTrailList : public ITrailList
{
public:

	LineTrailList();

protected:
	
	virtual int GetCount() const;
	virtual void AddKeyData(const TRAIL_DATA& keyData);
	virtual void UpdateRecentKeyData(const TRAIL_DATA& keyData);
	virtual void Tick(DWORD dwTickDelta);
	virtual void Reset();
	virtual int GetRenderCount() const;
	virtual void GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const;

protected:
	RotList<TRAIL_DATA> m_TrailLst;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class LineTrailGravityList
//	
///////////////////////////////////////////////////////////////////////////
class LineTrailGravityList : public LineTrailList
{
public:
	LineTrailGravityList();
	virtual ~LineTrailGravityList();
public:
	virtual void AddKeyData(const TRAIL_DATA& keyData);
	virtual void Tick(DWORD dwTickDelta);
	
	virtual void SetGravityData(const GRAVITY_DATA& gravityData);
	void UseGravity(bool bUse);
	
protected:
	void TickGravity(DWORD dwTickDelta);
	void AffectGravity(int nIdx,float fGravity);
private:
	bool	m_bGravity;
	GRAVITY_DATA	m_GravityData;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_TrailList_H_


