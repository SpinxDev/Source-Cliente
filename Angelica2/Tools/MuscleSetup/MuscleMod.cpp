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

#include "musclemod.h"
#include <math.h>

MuscleMod::MuscleMod()
:m_theMuscles(64), m_nNumVerts(0), m_pOriginalVerts(NULL), m_pDeformedVerts(NULL), m_tmMC(true)
{
}

MuscleMod::~MuscleMod()
{
	for(int i=0; i<m_theMuscles.size(); i++)
	{
		MUSCLE& muscle = m_theMuscles[i];
		if( muscle.pIndices )
		{
			delete [] muscle.pIndices;
		}
		if( muscle.pWeights )
		{
			delete [] muscle.pWeights;
		}
	}			  
	
	if( m_pOriginalVerts )
	{
		delete [] m_pOriginalVerts;
		m_pOriginalVerts = NULL;
	}

	if( m_pDeformedVerts )
	{
		delete [] m_pDeformedVerts;
		m_pDeformedVerts = NULL;
	}

	m_nNumVerts = 0;
}

int MuscleMod::GetNumMuscles()
{
	return m_theMuscles.size();
}

// initialize the mesh's information
bool MuscleMod::SetupMesh(const Matrix3& tmMC, int nNumVerts, Point3 * pVerts)
{
	m_tmMC = tmMC;

	if( m_pOriginalVerts )
	{
		delete [] m_pOriginalVerts;
		m_pOriginalVerts = NULL;
	}

	if( m_pDeformedVerts )
	{
		delete [] m_pDeformedVerts;
		m_pDeformedVerts = NULL;
	}

	m_nNumVerts = 0;

	// allocate the deformed verts buffer and set its intial data
	m_nNumVerts = nNumVerts;

	m_pOriginalVerts = new Point3[nNumVerts];
	m_pDeformedVerts = new Point3[nNumVerts];
	//memcpy(m_pOriginalVerts, pVerts, nNumVerts * sizeof(Point3));
	//memcpy(m_pDeformedVerts, pVerts, nNumVerts * sizeof(Point3));

	for(int i=0; i<nNumVerts; i++)
	{
		m_pDeformedVerts[i] = m_pOriginalVerts[i] = tmMC * pVerts[i];
	}

	return UpdateAllMuscleAFR();
}

// recalculate the muscle's affected verts
bool MuscleMod::UpdateMuscleAFR(int nIndex)
{
	MUSCLE&		muscle = m_theMuscles[nIndex];
	MUSCLEDATA& data = muscle.data;
	
	if( muscle.nVerts )
	{
		delete [] muscle.pIndices;
		muscle.pIndices = NULL;

		delete [] muscle.pWeights;
		muscle.pWeights = NULL;

		muscle.nVerts = 0;
	}

	int nAffectedVerts = 0;
	Point3 vecCenter = data.vecCenter;
	
	// convert delta vector in object space to muscle space
	Matrix3 tmToMuscle(data.vecXAxis, data.vecYAxis, data.vecZAxis, Point3(0.0f, 0.0f, 0.0f));
	tmToMuscle.Invert();
	for(int i=0; i<m_nNumVerts; i++)
	{
		Point3 vecDelta = (m_pOriginalVerts[i] - vecCenter) * tmToMuscle;
		vecDelta = vecDelta / data.vecRadius;
		if( vecDelta.Length() < 1.0f )
			nAffectedVerts ++;
	}

	if( nAffectedVerts )
	{
		int  * pIndices = new int[nAffectedVerts];
		float * pWeights = new float[nAffectedVerts];

		int		n = 0;
		float	w;
		for(int i=0; i<m_nNumVerts; i++)
		{
			Point3 vecDelta = (m_pOriginalVerts[i] - vecCenter) * tmToMuscle;
			Point3 vecDeltaOrth = vecDelta / data.vecRadius;
			float vDisOrth = vecDeltaOrth.Length();
			if( vDisOrth < 1.0f )
			{
				// add this vert into the affected list
				pIndices[n] = i;

				// now determine the weight.
				w = GetCurvedWeight(1.0f - vDisOrth, data.vPitch);
				if( data.type == MUSCLE_TYPE_SINGLEDIR )
				{
				}
				else if( data.type == MUSCLE_TYPE_DOUBLEDIR )
				{
					if( DotProd(m_pOriginalVerts[i] - vecCenter, data.vecMuscleDir) < 0.0f )
						w *= -1.0f;
				}
				else if( data.type == MUSCLE_TYPE_CIRCLEDIR )
				{
					w *= data.vLength / vecDelta.Length();
				}
				pWeights[n] = w;

				n ++;
			}
		}

		muscle.nVerts = nAffectedVerts;
		muscle.pIndices = pIndices;
		muscle.pWeights = pWeights;
	}
	return true;
}

// recalculate all muscle's affected verts
bool MuscleMod::UpdateAllMuscleAFR()
{
	for(int i=0; i<m_theMuscles.size(); i++)
		UpdateMuscleAFR(i);

	return true;
}

// add a new muscle to the muscle mod
// return true if success, false if fail (maybe exceed the MAX_MUSCLES)
bool MuscleMod::AddMuscle(const MUSCLEDATA& muscledata, int& nIndex)
{
	MUSCLE newMuscle;
	newMuscle.id = GenNewMuscleID();
	newMuscle.data = muscledata;
	
	m_theMuscles.push_back(newMuscle);
	nIndex = m_theMuscles.size() - 1;
	return UpdateMuscleAFR(nIndex);
}

bool MuscleMod::AddLoadedMuscle(const MUSCLEDATA& muscledata, int id, int& nIndex)
{
	MUSCLE newMuscle;
	newMuscle.id = id;
	newMuscle.data = muscledata;
	
	m_theMuscles.push_back(newMuscle);
	nIndex = m_theMuscles.size() - 1;
	return UpdateMuscleAFR(nIndex);
}


// remove a muscle from the list
bool MuscleMod::DelMuscle(int nIndex)
{
	MUSCLE& muscle = m_theMuscles[nIndex];
	if( muscle.pIndices )
	{
		delete [] muscle.pIndices;
	}
	if( muscle.pWeights )
	{
		delete [] muscle.pWeights;
	}
	m_theMuscles.erase(m_theMuscles.begin()+nIndex);
	return true;
}

bool MuscleMod::RemoveAllMuscles()
{
	for(int i=0; i<m_theMuscles.size(); i++)
	{
		MUSCLE& muscle = m_theMuscles[i];
		if( muscle.pIndices )
		{
			delete [] muscle.pIndices;
		}
		if( muscle.pWeights )
		{
			delete [] muscle.pWeights;
		}
	}			  

	m_theMuscles.clear();
	return true;
}

// set and get a muscle
bool MuscleMod::SetMuscleData(int nIndex, const MUSCLEDATA& muscledata)
{
	// update the muscle's data in the vector array
	m_theMuscles[nIndex].data = muscledata;
	return UpdateMuscleAFR(nIndex);
}

const MUSCLEDATA& MuscleMod::GetMuscleData(int nIndex)
{
	return m_theMuscles[nIndex].data;
}

bool MuscleMod::DeformMesh(int nSoloMuscleIndex)
{
	// first restore the deformed mesh to be original one
	memcpy(m_pDeformedVerts, m_pOriginalVerts, sizeof(Point3) * m_nNumVerts);

	for(int i=0; i<m_theMuscles.size(); i++)
	{
		// see if only one muscle should be used
		if( nSoloMuscleIndex != -1 && i != nSoloMuscleIndex )
			continue;

		MUSCLE&		muscle = m_theMuscles[i];
		MUSCLEDATA&	muscledata = muscle.data;
		Point3		vecMuscle = muscledata.vLength * muscledata.vecMuscleDir;
		float		vScale = muscledata.vTension;

		if( muscledata.type == MUSCLE_TYPE_CIRCLEDIR )
		{
			for(int j=0; j<muscle.nVerts; j++)
			{
				int index = muscle.pIndices[j];
				float weight = muscle.pWeights[j];

				Point3 vecDelta = m_pOriginalVerts[index] - muscledata.vecCenter;
				
				m_pDeformedVerts[index] += vecDelta * vScale * weight;
			}
		}
		else
		{
			for(int j=0; j<muscle.nVerts; j++)
			{
				int index = muscle.pIndices[j];
				float weight = muscle.pWeights[j];
				
				m_pDeformedVerts[index] += vecMuscle * vScale * weight;
			}
		}
	}

	return true;
}

float MuscleMod::GetCurvedWeight(float w, float vPitch)
{
	//return (float)pow(w, vPitch);

	if( vPitch >= 1.0f )
	{
		return (float)pow(w, vPitch + 1.0f);
	}
	else
	{
		float t=10.0f, k=(1.0f - vPitch) * 10.0f + 3.0f;
		float k2 = k * k;
		float k12 = (k-1) * (k-1);
		float t2 = t*t;
		float M = k12*k*t2 - k12*(k-2)*t;
		float N = k2 * (k - 1) * t2 - 2*(k-1)*(k-2)*t + (k-2)*(k-3);
		float C = M/N;
		float B = -2 * C + k;
		float A = C - (k - 1);

		w = 1.0f - w;
		float wk2 = pow(w, k - 2);
		return 1.0f - (A * w * w * wk2 + B * w * wk2 + C * wk2);
	}
	
	/*
	float a = vPitch;
	float b = 10;//log(a) * 2.0f;
	float eb = pow(2.73f, -b);
	
	float k = a * (1.0f - eb) / (1 + a) / (1 + a * eb);
	float y = k / (1.0f + a * pow(2.73f, -b*w)) - k / (1.0f + a);
	return y;
	*/

	/*
	float t=10.0f, k=vPitch + 2.5f;
	float k2 = k * k;
	float k12 = (k-1) * (k-1);
	float t2 = t*t;
	float M = k12*k*t2 - k12*(k-2)*t;
	float N = k2 * (k - 1) * t2 - 2*(k-1)*(k-2)*t + (k-2)*(k-3);
	float C = M/N;
	float B = -2 * C + k;
	float A = C - (k - 1);

	w = 1.0f - w;
	float wk2 = pow(w, k - 2);
	return 1.0f - (A * w * w * wk2 + B * w * wk2 + C * wk2);
	*/
}

bool MuscleMod::GetMuscleAFRInfo(int nIndex, int& nVerts, int*& pIndices, float*& pWeights)
{
	if( nIndex < 0 || nIndex >= m_theMuscles.size() )
		return false;

	nVerts = m_theMuscles[nIndex].nVerts;
	pIndices = m_theMuscles[nIndex].pIndices;
	pWeights = m_theMuscles[nIndex].pWeights;
	return true;
}

bool MuscleMod::AddMuscleLink(int nIndexHost, int nIndexChild)
{
	MUSCLE& muscleHost = m_theMuscles[nIndexHost];
	MUSCLE& muscleChild = m_theMuscles[nIndexChild];

	int idHost, idChild;
	idHost = muscleHost.id;
	idChild = muscleChild.id;

	int i;
	for(i=0; i<muscleHost.idLink.size(); i++)
	{
		if( idChild == muscleHost.idLink[i] )
			break;
	}
	// if not found
	if( i == muscleHost.idLink.size() )
		muscleHost.idLink.push_back(muscleChild.id);

	for(i=0; i<muscleChild.idLink.size(); i++)
	{
		if( idHost == muscleChild.idLink[i] )
			break;
	}
	// if not found
	if( i == muscleChild.idLink.size() )
		muscleChild.idLink.push_back(muscleHost.id);
	return true;
}

bool MuscleMod::RemoveMuscleLink(int nIndexHost, int nIndexChild)
{
	MUSCLE& muscleHost = m_theMuscles[nIndexHost];
	MUSCLE& muscleChild = m_theMuscles[nIndexChild];

	int idHost, idChild;
	idHost = muscleHost.id;
	idChild = muscleChild.id;

	int i;
	for(i=0; i<muscleHost.idLink.size(); i++)
	{
		if( idChild == muscleHost.idLink[i] )
		{
			muscleHost.idLink.erase(muscleHost.idLink.begin() + i);
			break;
		}
	}

	for(i=0; i<muscleChild.idLink.size(); i++)
	{
		if( idHost == muscleChild.idLink[i] )
		{
			muscleChild.idLink.erase(muscleChild.idLink.begin() + i);
			break;
		}
	}
	
	return true;
}

bool MuscleMod::IsMuscleLinked(int nIndexHost, int nIndexChild)
{
	MUSCLE& muscleHost	= m_theMuscles[nIndexHost];
	MUSCLE& muscleChild	= m_theMuscles[nIndexChild];

	int idChild = muscleChild.id;

	int i;
	for(i=0; i<muscleHost.idLink.size(); i++)
	{
		if( muscleHost.idLink[i] == idChild )
			return true;
	}

	return false;
}

int MuscleMod::GetMuscleIDByIndex(int nIndex)
{
	return m_theMuscles[nIndex].id;
}

int MuscleMod::FindMuscleByID(int id)
{
	for(int i=0; i<m_theMuscles.size(); i++)
	{
		if( m_theMuscles[i].id == id )
			return i;
	}

	return -1;
}

int MuscleMod::GetNumLinkedMuscles(int nIndex)
{
	MUSCLE& muscle = m_theMuscles[nIndex];
	return muscle.idLink.size();
}
	
int MuscleMod::GetLinkedMuscleID(int nIndex, int linkIndex)
{
	MUSCLE& muscle = m_theMuscles[nIndex];
	return muscle.idLink[linkIndex];
}

int MuscleMod::GetMuscleID(int nIndex)
{
	MUSCLE& muscle = m_theMuscles[nIndex];
	return muscle.id;
}