 /*
 * FILE: MuscleMod.h
 *
 * DESCRIPTION: Muscle Modifier to Deform the mesh
 *
 * CREATED BY: Hedi, 2004/6/5
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _MUSCLEMOD_H_
#define _MUSCLEMOD_H_

#include <max.h>
#include "vector.h"

#define MUSCLESYSTEM_VERSION			0x10003005

enum MUSCLE_TYPE
{
	MUSCLE_TYPE_SINGLEDIR = 0,
	MUSCLE_TYPE_DOUBLEDIR = 1,
	MUSCLE_TYPE_CIRCLEDIR
};

struct MUSCLEDATA
{
	char			szName[32];

	Point3			vecCenter;		// muscle control point center
	Point3			vecMuscleDir;	// muscle end direction, this is vecCenter - vecEnd
	float			vLength;		// muscle length
	
	// parameter to describe the affect regions.
	Point3			vecXAxis;		// muscle orientation x
	Point3			vecYAxis;		// muscle orientation y
	Point3			vecZAxis;		// muscle orientation z
	Point3			vecRadius;		// radius at x, y and z axis
	float			vPitch;			// muscle affect region pitch value

	MUSCLE_TYPE		type;			// muscle type

	float			vTension;		// current tension of the muscle
};

struct MUSCLE
{
	MUSCLE() : idLink(64), nVerts(0), pIndices(NULL), pWeights(NULL) { memset(&data, 0, sizeof(data)); }
	MUSCLEDATA			data;
	int					id;
	abase::vector<int>	idLink;
	
	// automatically managed datas, do not modify
	int					nVerts;
	int *				pIndices;		// a buffer the length of which is nAffectedVerts;
	float *				pWeights;		// a buffer the length of which is nAffectedVerts;
};

class MuscleMod
{
private:
	// The muscle array
	abase::vector<MUSCLE> m_theMuscles;

	// modContext matrix, use this tm we can transform object space vertex into
	// modifier space
	Matrix3			m_tmMC;

	// the verts of the mesh which the muscles are applied to
	int				m_nNumVerts;
	Point3 *		m_pOriginalVerts;
	Point3 *		m_pDeformedVerts;

	// the id generator
	int				m_nIDSeed;
	inline int GenNewMuscleID()			{ return m_nIDSeed ++; }

public:
	inline void SetIDSeed(int nSeed)	{ m_nIDSeed = nSeed; }
	inline int GetIDSeed()				{ return m_nIDSeed; }

public:
	int GetNumMuscles();

protected:

public:
	MuscleMod();
	~MuscleMod();

	// initialize the mesh's information
	bool SetupMesh(const Matrix3& tmMC, int nNumVerts, Point3 * pVerts);

	// recalculate the muscle's affected verts
	bool UpdateMuscleAFR(int nIndex);
	bool UpdateAllMuscleAFR();

	// now deform the object using these muscle datas.
	bool DeformMesh(int nSoloMuscleID=-1);

	// add a new muscle to the muscle mod
	// return true if success, false if fail (maybe exceed the MAX_MUSCLES)
	bool AddMuscle(const MUSCLEDATA& muscledata, int& nIndex);
	bool AddLoadedMuscle(const MUSCLEDATA& muscledata, int id, int& nIndex);

	// remove a muscle from the list
	bool DelMuscle(int nIndex);
	bool RemoveAllMuscles();

	// set and get a muscle
	bool SetMuscleData(int nIndex, const MUSCLEDATA& muscledata);
	const MUSCLEDATA& GetMuscleData(int nIndex);
	// get a muscle's id by its index
	int GetMuscleID(int nIndex);

	// get the number of a muscle's linked muscle
	// nIndex the index of the muscle whose linked number to be retrieved
	int GetNumLinkedMuscles(int nIndex);
	// get the id of one's linked muscle
	int GetLinkedMuscleID(int nIndex, int linkIndex);
	// get the id of one muscle according to the muscle index
	int GetMuscleIDByIndex(int nIndex);
	// find a muscle's current index according to its id
	// return -1 when not found
	int FindMuscleByID(int id);

	// add a new linked muscle
	// nIndexHost	--- the index of the host muscle
	// nIndexChild	--- the index of the child muscle
	bool AddMuscleLink(int nIndexHost, int nIndexChild);
	bool RemoveMuscleLink(int nIndexHost, int nIndexChild);
	bool IsMuscleLinked(int nIndexHost, int nIndexChild);

	// retrieve a muscle's affected region information
	// nIndex	---		the index of the muscle
	// nVerts	---		number of verts affected
	// pIndices	---		the indices of these verts
	// pWeights ---		the weights of these verts
	bool GetMuscleAFRInfo(int nIndex, int& nVerts, int*& pIndices, float*& pWeights);

public:
	inline int GetNumVerts()				{ return m_nNumVerts; }
	inline Point3 * GetDeformedVerts()		{ return m_pDeformedVerts; }
	inline Point3 * GetOriginalVerts()		{ return m_pOriginalVerts; }
	inline Matrix3 GetModTM()				{ return m_tmMC; }

	float GetCurvedWeight(float w, float vPitch);
};

#endif//_MUSCLEMOD_H_
