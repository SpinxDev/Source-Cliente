/*
 * FILE: SkinMapper.h
 *
 * DESCRIPTION: Class to do skin mapping
 *
 * CREATED BY: Hedi, 2005/1/20
 *
 * HISTORY:
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _SKINMAPPER_H_
#define _SKINMAPPER_H_

#include "Max.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "vector.h"
#include "raytrace.h"

#if MAX_RELEASE >= 4000
#include "iskin.h"
#endif

using namespace abase;

enum MappingStrategy
{
	CLOSEST_VERTEX,
	CLOSEST_EDGE,
	CLOSEST_TRIANGLE,
};

class CBone
{
public:
	char		m_szName[128];
	Matrix3		m_tm;
	INode *		m_pNode;

	CBone(INode * pNode, const char * szName, const Matrix3& tm)
	{
		m_pNode = pNode;
		strncpy(m_szName, szName, 128);
		m_tm = tm;
	}
};

class CWeightedVertex
{
public:
	Point3					m_vertPos;
	Point3					m_vertNormal;
	
	vector<int> 			m_idBones;
	vector<float>			m_weightBones;
	
	CWeightedVertex(const Point3& pos)
	{
		m_vertPos = pos;
	}

	float GetBoneWeight(int idBone)
	{
		for(int i=0; i<m_idBones.size(); i++)
		{
			if( idBone == m_idBones[i] )
				return m_weightBones[i];
		}
		return 0.0f;
	}

	void NormalizeWeights()
	{
		int	i;
		int nNumBones = m_idBones.size();
		if( nNumBones == 0 )
			return;

		float w = 0.0f;
		for(i=0; i<nNumBones; i++)
			w += m_weightBones[i];

		if( w != 0.0f )
		{
			float rw;
			if( nNumBones > 1 )
			{
				rw = 1.0f / w;
				for(i=0; i<nNumBones; i++)
					m_weightBones[i] *= rw;
			}
			else
				m_weightBones[0] = 1.0f;
		}
	}

	void ClearWeights()
	{
		m_idBones.clear();
		m_weightBones.clear();
	}
};

class CSkinedMesh
{
public:
	vector<CBone>				m_bones;			// bone array of this mesh
	vector<CWeightedVertex>		m_verts;			// all verts of this mesh
	vector<int>					m_indices;			// all face index of this mesh
	vector<bool>				m_influenceDestMeshFlag;	

	const char* GetBoneName(int i) 
	{ 
		if(i<m_bones.size())
			return m_bones[i].m_szName;
		else
			return NULL;
	}

	void AddBone(INode * pNode, const char * szName, const Matrix3& tm)
	{
		CBone bone(pNode, szName, tm);
		m_bones.push_back(bone);
	}

	int AddVertex(const Point3& pos)
	{
		CWeightedVertex	vert(pos);
		m_verts.push_back(vert);

		return m_verts.size() - 1;
	}

	void AddFace(int v0, int v1, int v2)
	{
		m_indices.push_back(v0);
		m_indices.push_back(v1);
		m_indices.push_back(v2);
	}

	bool SetVertexWeight(int idVert, const char * szName, float weight)
	{
		if( idVert < 0 || idVert >= m_verts.size() )
			return false;

		// first find that bone;
		int idBone;

		for(idBone=0; idBone<m_bones.size(); idBone++)
		{
			if( stricmp(szName, m_bones[idBone].m_szName) == 0 )
				break;
		}

		if( idBone == m_bones.size() )
			return false;

		return SetVertexWeight(idVert, idBone, weight);
	}

	bool SetVertexWeight(int idVert, int idBone, float weight)
	{
		CWeightedVertex& theVert = m_verts[idVert];

		// then determine if the vert's weight array already include this bone
		int index;
		for(index=0; index<theVert.m_idBones.size(); index++)
		{
			if( theVert.m_idBones[index] == idBone )
			{
				theVert.m_weightBones[index] = weight;
				return true;
			}
		}

		if( index == theVert.m_idBones.size() )
		{
			theVert.m_idBones.push_back(idBone);
			theVert.m_weightBones.push_back(weight);
		}
		return true;
	}

	void ClearAllVertsWeight()
	{
		int i;
		for(i=0; i<m_verts.size(); i++)
			m_verts[i].ClearWeights();
	}

	void NormalizeAllVertsWeight()
	{
		int i;
		for(i=0; i<m_verts.size(); i++)
			m_verts[i].NormalizeWeights();
	}

	float InfluentialPoint(const Point3& pos, int* vertIndex)
	{
		float disMin = FLT_MAX;
		int		i;
		int		nearestTri = -1;		

		assert(m_indices.size()%3 == 0);
		assert(m_influenceDestMeshFlag.size() == m_indices.size());
		
		for(i=0; i<m_indices.size(); i++)
		{
			const Point3 & vert = m_verts[m_indices[i]].m_vertPos;
			float disThis = (pos-vert).Length();

			if( m_influenceDestMeshFlag[i] && disMin>disThis )
			{
				disMin = disThis;
				*vertIndex = m_indices[i];
			}
		}
		return disMin;
	}

	float InfluentialEdge(const Point3& pos, int indexEdge[2], float weights[2])
	{
		float disMin = FLT_MAX;
		Point3 nearest;

		weights[0] = weights[1] = 0;

		int		i;
		assert(m_indices.size()%3 == 0);
		assert(m_influenceDestMeshFlag.size() == m_indices.size());
		
		for(i=0; i<m_indices.size(); i+=3)
		{
			Point3 triVert[3];
			Point3 nearestThis;
			triVert[0] = m_verts[m_indices[i]].m_vertPos;
			triVert[1] = m_verts[m_indices[i+1]].m_vertPos;
			triVert[2] = m_verts[m_indices[i+2]].m_vertPos;

			float disThis;
			disThis = PointToLineSegmentDistance(pos, triVert[0], triVert[1], &nearestThis);
			if( (m_influenceDestMeshFlag[i] && m_influenceDestMeshFlag[i+1]) && disMin>disThis )
			{
				disMin = disThis;
				nearest = nearestThis;
				indexEdge[0] = m_indices[i];
				indexEdge[1] = m_indices[i+1];
			}
			disThis = PointToLineSegmentDistance(pos, triVert[1], triVert[2], &nearestThis);
			if( (m_influenceDestMeshFlag[i+1] && m_influenceDestMeshFlag[i+2]) && disMin>disThis )			
			{
				disMin = disThis;
				nearest = nearestThis;
				indexEdge[0] = m_indices[i+1];
				indexEdge[1] = m_indices[i+2];
			}

			disThis = PointToLineSegmentDistance(pos, triVert[2], triVert[0], &nearestThis);
			if( (m_influenceDestMeshFlag[i+2] && m_influenceDestMeshFlag[i]) && disMin>disThis )
			{
				disMin = disThis;
				nearest = nearestThis;
				indexEdge[0] = m_indices[i+2];
				indexEdge[1] = m_indices[i];
			}
		}

		const Point3& v0 = m_verts[indexEdge[0]].m_vertPos;
		const Point3& v1 = m_verts[indexEdge[1]].m_vertPos;
		float posv0l = (nearest-v0).Length();
		float posv1l = (nearest-v1).Length();

		float total = posv0l + posv1l;
		if(total < FLT_EPSILON)
		{
			weights[0] = 0.5;
			weights[1] = 0.5;
		}
		else
		{
			float invt = 1.0/total;
			weights[0] = posv1l/total;
			weights[1] = posv0l/total;
		}

		return disMin;
	}

	float InfluentialTriangle(const Point3& pos, int indexTri[3], float weights[3])
	{
		float disMin = FLT_MAX;
		Point3 nearest;

		weights[0] = weights[1] = weights[2] = 0;

		int		i;
		int		nearestTri = -1;		

		assert(m_indices.size()%3 == 0);
		assert(m_influenceDestMeshFlag.size() == m_indices.size());
		
		for(i=0; i<m_indices.size(); i+=3)
		{
			Point3 triVert[3];
			Point3 nearestThis;
			triVert[0] = m_verts[m_indices[i]].m_vertPos;
			triVert[1] = m_verts[m_indices[i+1]].m_vertPos;
			triVert[2] = m_verts[m_indices[i+2]].m_vertPos;
			float disThis = PointToTriangleDistance(pos, triVert, &nearestThis);
			int flags=0;
			if(m_influenceDestMeshFlag[i])	flags ++;
			if(m_influenceDestMeshFlag[i+1]) flags ++;
			if(m_influenceDestMeshFlag[i+2]) flags ++;

			if(flags>=2 && disMin>disThis)
			{
				nearest = nearestThis;
				disMin = disThis;
				indexTri[0] = m_indices[i];
				indexTri[1] = m_indices[i+1];
				indexTri[2] = m_indices[i+2];
				nearestTri = i / 3;
			}
		}

		assert(nearestTri>=0);
		const Point3& v0 = m_verts[indexTri[0]].m_vertPos;
		const Point3& v1 = m_verts[indexTri[1]].m_vertPos;
		const Point3& v2 = m_verts[indexTri[2]].m_vertPos;
			
		CalcWeights(nearest, v0, v1, v2, weights);

		return disMin;
	}

	bool HasCommonBones(const CSkinedMesh * pDestSkin, int index)
	{
		if(pDestSkin == NULL)	return false;

		bool has = false;

		for(size_t j=0; j<pDestSkin->m_bones.size(); j++)
		{
			const char * name = pDestSkin->m_bones[j].m_szName;
			size_t total = m_verts[index].m_idBones.size();
			for(size_t s=0; s<total; s++)
			{
				int boneid = m_verts[index].m_idBones[s];
				if(stricmp(name, m_bones[boneid].m_szName) == 0)
				{
					has = true;
					break;
				}
			}
		}
		return has;
	}

	void SetInfluenceFlags(const CSkinedMesh * pDestSkin)
	{
		m_influenceDestMeshFlag.clear();

		for(size_t i=0; i<m_indices.size(); i++)
		{
			m_influenceDestMeshFlag.push_back(HasCommonBones(pDestSkin, m_indices[i]));
		}
	}
};

Modifier* FindSkinModifier(INode* pINode);


class CSkinMapper
{
protected:
	vector<INode *>			m_srcNodes;
	MappingStrategy			m_eMappingStrategy;

protected:
	CSkinedMesh * CreateMeshFromNode(HWND hPanel, INode * pNode);

	void NearestPointMapping(vector<CSkinedMesh *>& srcSkins, CSkinedMesh * pDestSkin);
	void NearestEdgeMapping(vector<CSkinedMesh *>& srcSkins, CSkinedMesh * pDestSkin);
	void NearestTriangleMapping(vector<CSkinedMesh *>& srcSkins, CSkinedMesh * pDestSkin);

public:
	inline int GetNumSourceNode() { return m_srcNodes.size(); }

	CSkinMapper();
	~CSkinMapper();

	bool AddSourceNode(INode * pNode);
	bool RemoveSourceNode(int index);
	bool RemoveAllSourceNodes();
	INode * GetSourceNode(int index);
	
	void	SetMappingStategy(MappingStrategy ms) { m_eMappingStrategy = ms; }
	MappingStrategy GetMappingStrategey() { return m_eMappingStrategy; }

	bool DoMapping(HWND hPanel, INode * pNode);
};

#endif//_SKINMAPPER_H_
