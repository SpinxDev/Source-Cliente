/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

	Desc:
		
		some utility functions 

----------------------------------------------------------------------*/
#ifndef _APHYSXUTILLIB_H_
#define _APHYSXUTILLIB_H_


class APhysXRigidBodyObject;

enum APhysXOverlapTestResult
{

	// true or false...
	APX_OTRES_FALSE = 0,
	APX_OTRES_TRUE,
	
	// success or fail...
	APX_OTRES_FAIL,
	APX_OTRES_SUCCESS,
	
};


class APHYSX_DLL_EXPORT APhysXUtilLib
{
public:

	inline static bool CheckRotation(const NxMat33& mat33)
	{
		return mat33.isRotation();
	}

	inline static bool CheckProperRigidTransform(const NxMat34& matTransform)
	{
		return CheckRotation(matTransform.M);
	}

	//----------------------------------------------------------------------------
	// Lerp and Slerp routines, make sure that t is in the range [0.0f, 1.0f]
	// t = 0, v0 is returned, t = 1, v1 is returned
	static NxVec3 Lerp(const NxVec3& v0, const NxVec3& v1, float t);
	
	// make sure that v0 and v1 is unit vector...
	// t = 0, v0 is returned, t = 1, v1 is returned
	static NxVec3 Slerp(const NxVec3& v0, const NxVec3& v1, float t, const NxVec3& vRefAxis  = NxVec3(0.0f, 1.0f, 0.0f) );
	
	//----------------------------------------------------------------------------
	// generate some random variable
	static NxVec3 GenRndDir();							// generate some random direction vector, which is be normalized...
	
	static NxVec3 GenRndAngVel(float fMaxVelMag);		// generate some random angular velocity...
	
	//----------------------------------------------------------------------------
	// debug render routines...

	static void DebugDrawAPhysXObject(A3DWireCollector& a3dwc, const APhysXObject* pPhysXObject, const DWORD dwCol = 0);
	static void DebugDrawActor(A3DWireCollector& a3dwc, const NxActor& actor, const DWORD dwCol = 0);
	/*
	Remarks:
		This function won't draw the plane shape.
		This function won't draw the wheel shape.

		Using default color, if dwCol = 0.
		The color is ARGB format. See Angelica SDK for more details. 
		All trigger shapes will be drawn with cyan(0xFF00ffff).
	*/
	static void DebugDrawShape(A3DWireCollector& a3dwc, const NxShape& shape, const DWORD dwCol = 0);

	static void DebugDrawArraw(A3DWireCollector& a3dwc, const NxVec3& vStartPos, const NxVec3& vEndPos, const DWORD dwCol = 0xffffff00);

	static void EnableDebugRender(NxActor* pNxActor, bool bEnable = true);
	//----------------------------------------------------------------------------
	// overlap routines...

	static bool IsSphereInNxActor(const NxActor* pNxActor, const NxSphere& sphere);
	
	static bool IsPointInNxActor(const NxActor* pNxActor, const NxVec3& vPos, float fThickness = 0.001f);

	// NxActor level functions...
	static void GetActorAABB(const NxActor* pNxActor, NxBounds3& aabb);
	// static void GetAPhysXObjectAABB(const APhysXObject* pObject, NxBounds3& aabb);

	static bool GetFFShapeAABB(const NxForceFieldShape* pFFShape, NxBounds3& aabb);
	static bool GetFFShapeGroupAABB(NxForceFieldShapeGroup& ffShapeGroup, NxBounds3& aabb);

	static void SetActorGroupMask(NxActor* pNxActor, const NxGroupsMask& gm);
	//----------------------------------------------------------------------------
	// array routines...
   	template<class T> static int APhysXArray_Find_RtnIdx(const APhysXArray<T>& arr, const T& ele)
	{
		const NxU32 nSize = arr.size();
		for(NxU32 i = 0; i < nSize; ++i)
		{
			if(ele == arr[i])
				return i;
		}
		
		return -1;
	}

	template<class T> static bool APhysXArray_Find(const APhysXArray<T>& arr, const T& ele)
	{
		return (0 > APhysXArray_Find_RtnIdx(arr, ele))? false : true;
	}

	template<class T> static void APhysXArray_PushOnce(APhysXArray<T>& arr, const T& ele)
	{
		if(!APhysXArray_Find(arr, ele))
			arr.push_back(ele);
	}

	//--------------------------------------------------------------------------
	// Run the PhysX Benchmark test, with the special focus on Cloth and Fluid.
	// one thing to beware is that calling this function will block the calling
	// thread for the time of uTestSeconds. And call this function after the
	// gPhysXEngine->Init().
	//
	// Params:
	//		bRunOnHardware:	run on Hardware mode(GPU/PPU) or Software mode(CPU)
	//		uTestSeconds:	the time to run the Benchmark test, in seconds.
	//
	// Return:
	//		the FPS of the Benchmark, 0.0f for some error case, such as no HW support
	//		or PhysX SDK has not been initialized.
	//
	// here is some Benchmark results of my test: 
	//		SW mode:		25.3
	//			Intel(R) Core(TM) i7 CPU 920 @ 2.67GHz
	//
	//		8600 GT:		56.9
	//		9800 GTX:		87.2

	static float PhysXBenchmark(bool bRunOnHardware = true, APhysXU32 uTestSeconds = 1);

	/*
	enum APhysXLinearSweepType
	{
		APX_LSTYPE_X,		// only sweep X direction
		APX_LSTYPE_Y,
		APX_LSTYPE_Z,
		
		APX_LSTYPE_ALL
	};
	*/


	//-------------------------------------------------------------------------------------
	// some overlap utilities (for detailed overlap API docs, refer to OverlapDetector.h)

	// snapshot the scene for overlap testing: all actors in the scene will be mirrored
	// but not dynamic any longer. So all following tests are based on the snapshot states
	// of actors until you call this function next time.
	static APhysXOverlapTestResult SnapshotScene(NxScene& scene);
	static APhysXOverlapTestResult SnapshotScene(APhysXScene* pApxScene);


	struct APhysXDetailOverlapInfo
	{
		NxVec3 mOverlapNormal;					// the normal direction in which the main actor is blocked off...
	};

	// actor-pair overlap test
	static APhysXOverlapTestResult CheckOverlap(const NxActor& actor0, const NxActor& actor1, APhysXDetailOverlapInfo* pDetailInfo = NULL);
	static APhysXOverlapTestResult CheckOverlap_Inflate(const NxActor& actor0, const NxActor& actor1, float growSize, APhysXDetailOverlapInfo* pDetailInfo = NULL);

	// shape routines...
	static APhysXOverlapTestResult CheckOverlap_Inflate(const NxActor& actor, float growSize, NxArray<NxShape*>& HitShapes, NxArray<APhysXDetailOverlapInfo>* pDetailInfo = NULL);
	static APhysXOverlapTestResult LinearSweep(const NxActor& actor, const NxVec3& motion, NxArray<NxShape*>& HitShapes, NxArray<APhysXDetailOverlapInfo>* pDetailInfo = NULL);
	static APhysXOverlapTestResult LinearSweep(const NxActor& actor, const NxVec3& dir, const float distance, NxArray<NxShape*>& HitShapes, NxArray<APhysXDetailOverlapInfo>* pDetailInfo = NULL);
	
	// actor routines...
	static APhysXOverlapTestResult CheckOverlap_Inflate(const NxActor& actor, float growSize, NxArray<NxActor*>& HitActors, NxArray<APhysXDetailOverlapInfo>* pDetailInfo = NULL);
	static APhysXOverlapTestResult LinearSweep(const NxActor& actor, const NxVec3& motion, NxArray<NxActor*>& HitActors, NxArray<APhysXDetailOverlapInfo>* pDetailInfo = NULL);
	static APhysXOverlapTestResult LinearSweep(const NxActor& actor, const NxVec3& dir, const float distance, NxArray<NxActor*>& HitActors, NxArray<APhysXDetailOverlapInfo>* pDetailInfo = NULL);
};

//-----------------------------------------------------------------
// a simple flag operation class

class APhysXFlagOperation
{

public:

	static void RaiseFlag(APhysXU32& uFlags, APhysXU32 flag)
	{
		uFlags |= flag;
	}

	static void ClearFlag(APhysXU32& uFlags, APhysXU32 flag)
	{
		uFlags &= ~flag;
	}

	static bool CheckFlag(APhysXU32 uFlags, APhysXU32 flag)
	{
		return (uFlags & flag)? true:false;
	}

};


//-----------------------------------------------------------------
// a simple queue class

template<class T> 
class APhysXSimpleQueue
{
	static T s_TDefault;

public:
	APhysXSimpleQueue()
	{
		m_pFront = m_pLast = NULL;
		m_iSize = 0;
	}

	~APhysXSimpleQueue()
	{
		Release();
	}

	void Release()
	{

		if(!IsEmpty())
		{
			QueueElement* pQE = m_pFront;

			while (pQE)
			{
				m_pFront = pQE->mNext;
				delete pQE;
				pQE = m_pFront;
			}

			m_pLast = NULL;
		}

	}

	const T& GetFront() const
	{
		if(m_pFront)
		{
			return m_pFront->data;
		}
		else
			return s_TDefault;
	}
	
	T& GetFront()
	{
		if(m_pFront)
		{
			return m_pFront->data;
		}
		else
			return s_TDefault;
	}

	const T&  GetBack() const
	{
		if(m_pLast) 
		{
			return  m_pLast->data;
		}
		else
			return s_TDefault;
	}
	
	T&  GetBack()
	{
		if(m_pLast) 
		{
			return  m_pLast->data;
		}
		else
			return s_TDefault;
	}

	int GetSize() const 
	{
		return m_iSize;
	}

	bool IsEmpty() const
	{
		return m_pFront==NULL;
	}

	void PushBack(const T& ele)
	{
		QueueElement* pQE = new QueueElement;
		pQE->data = ele;

		if(m_pLast)	m_pLast->mNext = pQE;
		m_pLast = pQE;

		if(!m_pFront) m_pFront = pQE;

		m_iSize++;

	}

	bool PopFront(T& ele)
	{
		if(IsEmpty()) return false;

		QueueElement* pQE = m_pFront;
		ele = pQE->data;
		m_pFront = pQE->mNext;

		if (!m_pFront)
			m_pLast = NULL;

		delete pQE;

		m_iSize--;

		return true;
	}



private:

	struct QueueElement
	{
		T data;
		QueueElement* mNext;

		QueueElement()
		{
			mNext = NULL;
		}
	};

	QueueElement* m_pFront;
	QueueElement* m_pLast;
	int m_iSize;
};

// define the static data member of APhysXSimpleQueue
template<class T> 
T APhysXSimpleQueue<T>::s_TDefault;

//-----------------------------------------------------------------

#endif
