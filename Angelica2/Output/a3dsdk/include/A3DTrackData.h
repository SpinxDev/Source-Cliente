/*
 * FILE: A3DTrackData.h
 *
 * DESCRIPTION: a template classes that define keyframe's informations
 *
 * CREATED BY: Hedi, 2003/7/14
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTRACKDATA_H_
#define _A3DTRACKDATA_H_

#include "AMemory.h"
#include "AAssist.h"
#include "AFile.h"
#include "A3DTypes.h"

//////////////////////////////////////////////////////////////////////////////////
// interpolation types of keyframe data
//////////////////////////////////////////////////////////////////////////////////

enum TRACKDATA_INTERTYPE
{
	TRACKDATA_INTERTYPE_NULL = 0,
	TRACKDATA_INTERTYPE_NEAREST = 1,
	TRACKDATA_INTERTYPE_LINEAR = 2,
	TRACKDATA_INTERTYPE_SLERP = 3,
	TRACKDATA_INTERTYPE_ACCELERATE = 4,
	TRACKDATA_INTERTYPE_DECELERATE = 5
};

//	Structure used to save and load track data
struct A3DTRACKSAVEDATA
{
	int		iNumKey;
	int		iNumSegment;
	int		iFrameRate;
};

//////////////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
//////////////////////////////////////////////////////////////////////////////////

inline bool a3d_LoadTrackInfo(AFile* pFile, A3DTRACKSAVEDATA* pData)
{
	DWORD dwRead;
	if (!pFile->Read(pData, sizeof (A3DTRACKSAVEDATA), &dwRead) || dwRead != sizeof (A3DTRACKSAVEDATA))
		return false;

	return true;
}

/*
Track cache is not needed at this time because we only use fixed frame rate data source.
//////////////////////////////////////////////////////////////////////////////////
//
// class TRACKCACHE is used to hold the intermedia result of a key interpolating
// algorithm. It will accelerate the process of key index finding the factor 
// interpolating calculating.
// TRACKCACHE is also used to hold the interpolation type information. So when getting
// a key value without specifying a track cache, it will use NEAREST interpolation as default.
// Because different interpolating algorithm will use this structure in different
// way, so when interpolation type changed, the cached data will expired!
//
//////////////////////////////////////////////////////////////////////////////////
class TRACKCACHE
{
private:
	TRACKDATA_INTERTYPE	m_interType;	// interpolate type, this member is used to
										// determine the interpolation type when fetching
										// key values.

public:
	BOOL				m_bValid;		// TRUE if cached data is valid.
	int					m_nKeyFloor;	// the nearest key that before current time
	int					m_nKeyRoof;		// the nearest key that just after current time

	FLOAT				m_vCacheData1;	// some cached datas;
	FLOAT				m_vCacheData2;		

	TRACKCACHE() { m_interType = TRACKDATA_INTERTYPE_LINEAR; m_bValid = FALSE; }

public:
	////////////////////////////////////////////////////////////////////////
	// some inline functions.
	////////////////////////////////////////////////////////////////////////
	inline TRACKDATA_INTERTYPE GetInterType() const		{ return m_interType; }
	inline void SetInterType(TRACKDATA_INTERTYPE type)	{ m_interType = type; m_bValid = FALSE; }

};*/

/////////////////////////////////////////////////////////////////////////////////
//
// class A3DTrackData is the data that a controller or a joint will need when 
// performing control of the animations.
//
// Now A3DTrackData only support fixed framerate data as input datas, because such
// kind of data can be easily convert between its time and frame number. And it can
// specify a track start and end time to save extra space when no animation is carrying
// out.
//
/////////////////////////////////////////////////////////////////////////////////
template <class T> class A3DTrackData
{
public:		//	Types

	//	Track segment
	struct SEGMENT
	{
		int		iStartTime;		//	Segment start time in ms
		int		iEndTime;		//	Segment end time in ms
		int		iStartKey;		//	Segment start key frame
		int		iEndKey;		//	Segment end key frame
	};

private:

	TRACKDATA_INTERTYPE	m_interType;	//	Value interpolation type;

	DWORD		m_dwDelFlag1;	//	Position in track manager
	DWORD		m_dwDelFlag2;
	int			m_nRefCount;	//	Reference count, because track data will
								//	Be a kind of shared memory resource.
	int			m_nNumKeys;		//	Key point number
	int			m_nFrameRate;	//	Frame rate of data in this track, it will be
								//	Used in time-frame converts.
	T*			m_pKeyFrames;	//	Key frame's data buffer;
	SEGMENT*	m_aSegments;	//	Segment data
	int			m_iNumSegment;	//	Number of segment
	int			m_iDataSize;	//	Approximate data size of this track

public:

	friend class A3DTrackMan;
	friend class A3DAbsTrackMaker;
	
	inline int GetNumKeys()	const { return m_nNumKeys; }
	inline int GetFrameRate() const { return m_nFrameRate; }
	inline int GetSegmentNum() const { return m_iNumSegment; }
	inline TRACKDATA_INTERTYPE GetInterType() const { return m_interType; }
	inline void SetInterType(TRACKDATA_INTERTYPE type) { m_interType = type; }
	inline int GetDataSize() const { return m_iDataSize; }
	inline int GetRefCount() const { return m_nRefCount; }

	//	Get start time of whole track
	inline int GetTrackStartTime() const
	{
		if (!m_iNumSegment)
			return 0;

		return m_aSegments[0].iStartTime;
	}

	//	Get end time of whole track
	inline int GetTrackEndTime() const
	{
		if (!m_iNumSegment)
			return 0;

		return m_aSegments[m_iNumSegment-1].iEndTime;
	}

	//	Set segment's time
	inline void SetSegmentTime(int iSegment, int iStart, int iEnd)
	{
		ASSERT(m_aSegments);
		m_aSegments[iSegment].iStartTime = iStart;
		m_aSegments[iSegment].iEndTime	 = iEnd;
	}

	//	Set segment's key frame
	inline void SetSegmentKeyFrame(int iSegment, int iStart, int iEnd)
	{
		ASSERT(m_aSegments);
		m_aSegments[iSegment].iStartKey	= iStart;
		m_aSegments[iSegment].iEndKey	= iEnd;
	}

	inline T* GetKeyFrames() const { return m_pKeyFrames; }
	inline SEGMENT* GetSegments() const { return m_aSegments; }

	void DownSample(int nFrameRate);

protected:

public:

	A3DTrackData();
	virtual ~A3DTrackData();

private:

public:

	//	Get current absolute frame number (not key number)
	float GetCurFrame(int iAbsTime);

	///////////////////////////////////////////////////////////////////////////////
	//
	// Reference managing methods.
	//
	// int AddRef()
	//		increase this track data's reference count by 1.
	// return the new reference count.
	//
	// int Release()
	//		decrease this track data's reference count by 1.
	// retrun the new reference count. when it reaches zero, the memory will be 
	// deallocated and this track data should not be used any more.
	///////////////////////////////////////////////////////////////////////////////
	int AddRef();
	int Release();

	//////////////////////////////////////////////////////////////////////////////
	//
	// Get a specific key index according to current time specified.
	//
	// int GetFloorKeyIndex(int nAbsTime)
	//		get the nearest key just before current time
	//
	//  nAbsTime		IN			time in millisecond
	//
	// return the nearest floor key index.
	//
	// int GetNearestKeyIndex(int nAbsTime)
	//		get the nearest key to current time
	//
	//  nAbsTime		IN			time in millisecond
	// 
	// return the nearest key index.
	//
	//////////////////////////////////////////////////////////////////////////////
	int GetFloorKeyIndex(int nAbsTime, int* piSegment=NULL);
	int GetNearestKeyIndex(int nAbsTime, int* piSegment=NULL);

	///////////////////////////////////////////////////////////////////////////////
	// key query methods.
	//
	// GetKeyValue(int nAbsTime, TRACKCACHE * pTrackCache)
	//
	//	get a value according to nAbsTime, using current interpolating type
	//		nAbsTime		IN		query time in millisecond
	//
	// SetKeyValue(int nFrameIndex, const T& value)
	//
	//  set a key frame's value according to its frame index.
	///////////////////////////////////////////////////////////////////////////////
	T GetKeyValue(int nAbsTime);
	void SetKeyValue(int nFrameIndex, const T& value);

	T GetKeyValueByIndex(int iKey)
	{
		ASSERT(iKey >= 0 && iKey < m_nNumKeys);
		return m_pKeyFrames[iKey];
	}

	///////////////////////////////////////////////////////////////////////////////
	// Set up functions.
	//	functions used to set up the data in the track
	//
	// bool Create(int nNumKeys, int nFrameRate, int nStartTime, int nEndTime)
	//	 alloc memory for this track data;
	//
	//	nNumKeys		IN			number of key frames
	//	nFrameRate		IN			frame rate of this track
	//	iNumSegment		IN			Number of segment
	// return true if succeed, or else false.
	// 
	// bool Load(ACFile * pFile)
	//	load this track from the specified file
	//	pFile		IN		pointer of file contains track's data
	// return true if no error, or else false.
	//
	// bool Save(ACFile * pFile)
	//	save this track's data into a specified file
	//	pFile		IN		pointer of file to put the track's data
	// return true if no error, or else false.
	///////////////////////////////////////////////////////////////////////////////
	bool Create(int nNumKeys, int nFrameRate, int iNumSegment);

	///////////////////////////////////////////////////////////////////////////////
	//
	//	The steps to load a track from file
	//	1. call A3DLoadTrackInfo() to get track's information from file
	//	2. Use A3DTrackMan to create track object.
	//	3. call A3DTrackData::Load() to load track's data.
	//
	///////////////////////////////////////////////////////////////////////////////
	//	Load track data
	bool Load(AFile* pFile);
	//	Save track to file
	bool Save(AFile* pFile);

	//	This function doesn't load track data, just skip track data in file
	bool FakeLoad(AFile* pFile);
};

typedef A3DTrackData<A3DVECTOR3>		A3DVector3Track;
typedef A3DTrackData<A3DQUATERNION>		A3DQuaternionTrack;
typedef A3DTrackData<float>				A3DFloatTrack;

///////////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////////

template <class T>
A3DTrackData<T>::A3DTrackData()
{
	m_interType		= TRACKDATA_INTERTYPE_LINEAR;
	m_nFrameRate	= 30;		// 30 fps
	m_nNumKeys		= 0;
	m_pKeyFrames	= NULL;
	m_nRefCount		= 1;
	m_dwDelFlag1	= 0;
	m_dwDelFlag2	= 0;
	m_aSegments		= NULL;
	m_iNumSegment	= 0;
	m_iDataSize		= 0;
}

template <class T>
A3DTrackData<T>::~A3DTrackData()
{
	if (m_pKeyFrames)
	{
		delete [] m_pKeyFrames;
		m_pKeyFrames = NULL;
	}

	if (m_aSegments)
	{
		delete [] m_aSegments;
		m_aSegments = NULL;
	}
}

template <class T>
int A3DTrackData<T>::GetFloorKeyIndex(int nAbsTime, int* piSegment/* NULL */)
{
	ASSERT(m_aSegments);

	int iKey, iSegment;

	if (nAbsTime <= m_aSegments[0].iStartTime)
	{
		iSegment = 0;
		iKey = 0;
	}
	else if (nAbsTime >= m_aSegments[m_iNumSegment-1].iEndTime)
	{
		iSegment = m_iNumSegment - 1;
		iKey = m_nNumKeys - 1;
	}
	else
	{
		for (int i=0; i < m_iNumSegment; i++)
		{
			SEGMENT* pSegment = &m_aSegments[i];

			if (pSegment->iEndTime > nAbsTime)
			{
				//	pSegment->iStartKey == pSegment->iEndKey means all keys in the segament are same
				if (pSegment->iStartKey == pSegment->iEndKey)
					iKey = pSegment->iStartKey;
				else
					iKey = pSegment->iStartKey + (nAbsTime - pSegment->iStartTime) * m_nFrameRate / 1000;

				iSegment = i;
				break;
			}
		}
	}

	if (piSegment)
		*piSegment = iSegment;

	return iKey;
}

template <class T>
int A3DTrackData<T>::GetNearestKeyIndex(int nAbsTime, int* piSegment/* NULL */)
{
	ASSERT(m_aSegments);

	int iKey, iSegment;

	if (nAbsTime <= m_aSegments[0].iStartTime)
	{
		iSegment = 0;
		iKey = 0;
	}
	else if (nAbsTime >= m_aSegments[m_iNumSegment-1].iEndTime)
	{
		iSegment = m_iNumSegment - 1;
		iKey = m_nNumKeys - 1;
	}
	else
	{
		for (int i=0; i < m_iNumSegment; i++)
		{
			SEGMENT* pSegment = &m_aSegments[i];

			if (pSegment->iEndTime > nAbsTime)
			{
				//	pSegment->iStartKey == pSegment->iEndKey means all keys in the segament are same
				if (pSegment->iStartKey == pSegment->iEndKey)
					iKey = pSegment->iStartKey;
				else
					iKey = pSegment->iStartKey + (int)((nAbsTime - pSegment->iStartTime) * m_nFrameRate / 1000.0f + 0.5f);

				iSegment = i;
				break;
			}
		}
	}

	if (piSegment)
		*piSegment = iSegment;

	return iKey;
}

//	Get current absolute frame number (not key number)
template <class T>
float A3DTrackData<T>::GetCurFrame(int iAbsTime)
{
	return iAbsTime * m_nFrameRate / 1000.0f;
}

template <class T>
T A3DTrackData<T>::GetKeyValue(int nAbsTime)
{
	ASSERT(m_aSegments);

	if (m_interType == TRACKDATA_INTERTYPE_NEAREST)
	{
		int nNearestIndex = GetNearestKeyIndex(nAbsTime);
		return m_pKeyFrames[nNearestIndex];
	}

	if (nAbsTime <= m_aSegments[0].iStartTime)
	{
		//	Return the first frame
		return m_pKeyFrames[0];
	}

	int iSegment, nFloorIndex;
	
	nFloorIndex = GetFloorKeyIndex(nAbsTime, &iSegment);
	if (nFloorIndex >= m_nNumKeys - 1)
	{
		//	Return the last frame
		return m_pKeyFrames[m_nNumKeys - 1];
	}

	if (m_aSegments[iSegment].iStartKey == m_aSegments[iSegment].iEndKey)
	{
		//	This is a static segment
		return m_pKeyFrames[m_aSegments[iSegment].iStartKey];
	}

	int nRoofIndex = nFloorIndex + 1;

	const T& value1 = m_pKeyFrames[nFloorIndex];
	const T& value2 = m_pKeyFrames[nRoofIndex];

	FLOAT f;		// linear factor of current time between these two key point
	FLOAT f1, f2;	// factors that has been transformed using current interpolation type.
	FLOAT vTimeGap = 1000.0f / m_nFrameRate; // time duration of each frame in millisecond

	int iKeyOffset = nFloorIndex - m_aSegments[iSegment].iStartKey;
	f = (nAbsTime - m_aSegments[iSegment].iStartTime) / vTimeGap - iKeyOffset;
	a_Clamp(f, 0.0f, 1.0f);

	switch (m_interType)
	{
	case TRACKDATA_INTERTYPE_LINEAR:
	
		f1 = 1.0f - f;
		f2 = f;
		break;
		
	case TRACKDATA_INTERTYPE_SLERP: // for Quaternion Interpolation only
	{	
		FLOAT cosine, sign;
		cosine = DotProduct(value1, value2);
		if (cosine < 0.0f)
		{
			cosine = -cosine;
			sign = -1.0f;
		}
		else
			sign = 1.0f;

		if (cosine > 1.0f - SLERP_EPSILON)
		{
			// the from and to value are very close, so use LERP will be ok
			f1 = 1.0f - f;
			f2 = f * sign;
		}
		else
		{
			FLOAT theta;
			theta = (FLOAT)acos(cosine);
			FLOAT sine;
			sine = (FLOAT)sin(theta);
			f1 = (FLOAT)(sin((1.0f - f) * theta) / sine);
			f2 = (FLOAT)(sin(f * theta) / sine) * sign;
		}
		
		break;
	}
	case TRACKDATA_INTERTYPE_ACCELERATE:
	
		f1 = f * f;
		f2 = 1.0f - f1;
		break;

	case TRACKDATA_INTERTYPE_DECELERATE:
	
		f1 = (2.0f - f) * f;
		f2 = 1.0f - f1;
		break;
	}

	return (value1 * f1 + value2 * f2);
}

template <class T>
int A3DTrackData<T>::AddRef()
{
	return ++ m_nRefCount;
}

template <class T>
int A3DTrackData<T>::Release()
{
	m_nRefCount--;
	if (m_nRefCount)
		return m_nRefCount;

	// Now no one references this object any more, so just delete it.
	delete this;
	return 0;
}

template <class T>
bool A3DTrackData<T>::Create(int nNumKeys, int nFrameRate, int iNumSegment)
{
	ASSERT(nNumKeys > 0 && iNumSegment > 0);

	m_nNumKeys		= nNumKeys;
	m_iNumSegment	= iNumSegment;
	m_nFrameRate	= nFrameRate;
	m_iDataSize		= nNumKeys * sizeof (T) + iNumSegment * sizeof (SEGMENT);

	if (!(m_pKeyFrames = new T[nNumKeys]))
		return false;

	if (!(m_aSegments = new SEGMENT[iNumSegment]))
		return false;

	return true;
}

template <class T>
bool A3DTrackData<T>::Load(AFile* pFile)
{
	ASSERT(m_pKeyFrames);

	DWORD dwRead;

	//	Load key frame data
	if (!pFile->Read(m_pKeyFrames, m_nNumKeys * sizeof (T), &dwRead))
		return false;

	//	Load segment data
	if (!pFile->Read(m_aSegments, m_iNumSegment * sizeof (SEGMENT), &dwRead))
		return false;

	return true;
}

//	This function doesn't load track data, just skip track data in file
template <class T>
bool A3DTrackData<T>::FakeLoad(AFile* pFile)
{
	DWORD dwBytes = m_nNumKeys * sizeof (T);
	dwBytes += m_iNumSegment * sizeof (SEGMENT);
	return pFile->Seek(dwBytes, AFILE_SEEK_CUR);
}

template <class T>
bool A3DTrackData<T>::Save(AFile* pFile)
{
	//	Fill data
	A3DTRACKSAVEDATA Data;

	Data.iNumKey		= m_nNumKeys;
	Data.iNumSegment	= m_iNumSegment;
	Data.iFrameRate		= m_nFrameRate;

	DWORD dwWrite;

	//	Write track data
	if (!pFile->Write(&Data, sizeof (Data), &dwWrite))
		return false;

	if (m_pKeyFrames)
	{
		//	Write key frame data
		if (!pFile->Write(m_pKeyFrames, m_nNumKeys * sizeof (T), &dwWrite))
			return false;
	}

	if (m_aSegments)
	{
		//	Write segments data
		if (!pFile->Write(m_aSegments, m_iNumSegment * sizeof (SEGMENT), &dwWrite))
			return false;
	}

	return true;
}

template <class T>
void A3DTrackData<T>::SetKeyValue(int nFrameIndex, const T& value)
{
	// If the index is invalid, just ignore it.
	if( nFrameIndex < 0 || nFrameIndex >= m_nNumKeys )
		return;

	m_pKeyFrames[nFrameIndex] = value;
	return;
}

template <class T>
void A3DTrackData<T>::DownSample(int nFrameRate)
{
	int rate = m_nFrameRate / nFrameRate;
	if( rate * nFrameRate != m_nFrameRate )
		return;

	if( rate == 1 )
		return;

	int nNumKeys = m_nNumKeys / rate;
	if( nNumKeys < 1 )
		return;

	T * pKeyFrames = new T[nNumKeys];

	int i;
	for(i=0; i<nNumKeys; i++)
		pKeyFrames[i] = m_pKeyFrames[i * rate];

	// release old ones.
	delete [] m_pKeyFrames;
	m_pKeyFrames = NULL;

	m_nNumKeys = nNumKeys;
	m_pKeyFrames = pKeyFrames;
	m_nFrameRate = nFrameRate;

	for(i=0; i<m_iNumSegment; i++)
	{
		m_aSegments[i].iStartKey /= rate;
		m_aSegments[i].iEndKey /= rate;
	}
}

#endif	//	_A3DTRACKDATA_H_

