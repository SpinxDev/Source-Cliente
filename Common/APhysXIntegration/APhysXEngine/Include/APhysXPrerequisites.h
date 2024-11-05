/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXPREREQUISITES_H_
#define _APHYSXPREREQUISITES_H_

// only consider for Win32 platform for now
#if defined WIN32
	
//---------------------------------------------------------------------
// Build mode: DLL OR LIB
#if defined APHYSX_DLL


	#if defined(APHYSX_BUILD_ENGINE)

		#define APHYSX_DLL_EXPORT __declspec(dllexport)

	#else

		#define APHYSX_DLL_EXPORT __declspec(dllimport)

	#endif


#else

	// default case....
	//APHSYX_STATICLIB mode
	#define APHYSX_DLL_EXPORT 

#endif	// end of #if defined APHYSX_DLL

/*
	#if defined(APHYSX_DLL)
	
		#define APHYSX_DLL_EXPORT __declspec(dllexport)
	
	#elif defined(APHYSX_USE_DLL)
		
		#define APHYSX_DLL_EXPORT __declspec(dllimport)
	
	#elif defined(APHSYX_STATICLIB)
		
		#define APHYSX_DLL_EXPORT

	#elif defined(APHSYX_USE_STATICLIB)

		#define APHYSX_DLL_EXPORT
		
	#endif
*/

//---------------------------------------------------------------------
	#if defined _DEBUG
		#define APHYSX_DEBUG
		#define APHYSX_DEBUG_RENDER
	#endif

	#ifdef _WIN64
		#define APHYSX_WIN64
	#endif

//---------------------------------------------------------------------
// handle some VC6.0 and VC2005 imcompatible issues...
/*
	_MSC_VER	MSVC Version
	1200	VC6.0 
	1201	EVC4.2 
	1300	VC2002 
	1310	VC2003 
	1400	VC2005
*/

	#if _MSC_VER < 1400

		#ifdef APHYSX_BUILD_ENGINE
			#define for if(0);else for	
		#endif

	#endif


//---------------------------------------------------------------------

#endif	// end of #if defined WIN32



//---------------------------------------------------------------------
// Note here, we should walk around Windonws predefined min/max
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//---------------------------------------------------------------------
// compiler configuration...
#pragma warning(disable:4018)
#pragma warning(disable:4275)


//---------------------------------------------------------------------
// Angelica Engine Headers...
#include "AC.h"
#include "AF.h"
#include "AM.h"
#include "A3D.h"

//---------------------------------------------------------------------
// PhysX Engine Headers...

#include "NxPhysics.h"
#include "NxCooking.h"


//---------------------------------------------------------------------
// types
#ifdef USE_STD_TYPE

	#include <map>
	#define APhysXMap std::map
	#define APhysXPair std::pair

#else

	#define APhysXMap abase::hash_map
	#define APhysXPair abase::pair
	
#endif

// macros
#ifndef APHYSX_USE_ANGELICA_3
	#define APHYSX_ANGELICA_VERSION 200
#else
	#define APHYSX_ANGELICA_VERSION 300
#endif

// get the PhysX SDK version number from x.y.z, such as 2.8.1...
#define APHYSX_GET_PHYSX_SDK_VERSION(x, y, z)		(( x << 24 ) + ( y << 16 ) + ( z << 8 ) + 0)
#define APHYSX_CURRENT_PHYSX_SDK_VERSION			NX_PHYSICS_SDK_VERSION

#define APhysXString AString
#define APhysXArray NxArray
#define APHYSX_ASSERT(x) NX_ASSERT(x)

#define APHYSX_ANGELICA_ALLOCATOR	0
#define APHYSX_TRACE_MEMORY_ALLOC	0

#define APHYSX_ENABLE_PROFILING		1

// support compartment or not...
// NxCompartment may be removed in Version 3.0...
#if APHYSX_CURRENT_PHYSX_SDK_VERSION < APHYSX_GET_PHYSX_SDK_VERSION(3, 0, 0)
	#define APHYSX_SUPPORT_COMPARTMENT 1
#else
	#define APHYSX_SUPPORT_COMPARTMENT 0
#endif

// typedefs
typedef unsigned int		APhysXU32;
typedef unsigned short		APhysXU16;
typedef unsigned char		APhysXU8;

//---------------------------------------------------------------------
// constants
#define APHYSX_STD_FPS			60
#define APHYSX_STD_TICKTIME		(1.0f/APHYSX_STD_FPS)
#define APHYSX_STD_GRAVITY		NxVec3(0, -9.81f, 0)

#define APHYSX_PI				3.141592653589793f

#define APHYSX_FLOAT_EPSILON	0.00001f
#define APHYSX_SCALE_EPSILON	0.001f

//---------------------------------------------------------------------
// common functions...

// timing function
#define APHYSX_TIMEFUNC 1
inline static DWORD APhysX_GetMilliseconds()
{

#if		APHYSX_TIMEFUNC == 1
	
	return a_GetTime();

#elif	APHYSX_TIMEFUNC == 2

	return ::GetTickCount();	// having some issues???

#elif	APHYSX_TIMEFUNC == 3

	static LARGE_INTEGER freq;
	static bool init = false;
	if(!init){
		QueryPerformanceFrequency(&freq);
		init=true;
	}

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	
	//??? some overflow risk when do 1000 * currentTime.QuadPart
	return (DWORD)( 1000 * currentTime.QuadPart / freq.QuadPart);			

#endif

}

inline static double APhysX_GetClockFreq()
{
	static bool init = false;
	static double dClockFreq = 0.0;
	
	if(!init)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		dClockFreq = (double)freq.QuadPart;
		init=true;
	}

	return dClockFreq;
}


inline static double APhysX_GetTimeTicks()
{
	LARGE_INTEGER a;
	QueryPerformanceCounter (&a);
	return double(a.QuadPart);
}

inline static double APhysX_GetElapsedSeconds()
{
	static double dLastTicks = APhysX_GetTimeTicks();

	double dCurTicks = APhysX_GetTimeTicks();
	double dElapsedTicks = dCurTicks - dLastTicks;
	dLastTicks = dCurTicks;

	return (dElapsedTicks / APhysX_GetClockFreq());

}

#endif
