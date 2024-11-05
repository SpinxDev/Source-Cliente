#pragma once

#include "max.h"

class BonesDefMod;

class VertexInfluenceListClass
{
public:
        int Bones;          
        float Influences;  
		float normalizedInfluences;
//extra data to hold spline stuff
        int SubCurveIds;
        int SubSegIds;
        float u;
        Point3 Tangents;
        Point3 OPoints;
};

class VertexListClass
        {
public:
        BOOL selected;
        BOOL modified;
        Point3 LocalPos;
//table of misc data    
        Tab<VertexInfluenceListClass> d;
		int GetMostAffectedBone()
			{
			if (d.Count() == 0) return -1;
			int largestID = d[0].Bones;
			float largestVal = d[0].Influences;
			for (int i = 1; i < (d.Count()); i++)
				{
				for (int j = i; j < d.Count(); j++)
					{
					if (d[j].Influences > largestVal)
						{
						largestVal = d[j].Influences;
						largestID = d[j].Bones;
						}
					}
				}
			return largestID;

			}
		int GetMostAffectedBoneID()
			{
			if (d.Count() == 0) return -1;
			int largestID = 0;
			float largestVal = d[0].Influences;
			for (int i = 1; i < (d.Count()); i++)
				{
				for (int j = i; j < d.Count(); j++)
					{
					if (d[j].Influences > largestVal)
						{
						largestVal = d[j].Influences;
						largestID = j;
						}
					}
				}
			return largestID;
			}
        };


class LocalGizmoData
{
private:
//	BitArray affectedVerts;
	Tab<int> affectedVerts;
public:
	int whichGizmo;
	BOOL IsAffected(int index);
	BOOL IsInList(int index,int &where);
	void SetVert(int index);
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);

};

class VertexDistanceClass
{
public:
	float dist;
	float u;
    int SubCurveIds;
    int SubSegIds;
    Point3 Tangents;
    Point3 OPoints;
};

class CSkinCallback;
class ISkinEngine;
class WeightRestore;

class ExclusionListClass
{
private:
	Tab<int> exList;
public:
	int Count() { return exList.Count();}
	int Vertex(int id) 
	{ 
		if ((id >=0) && (id < exList.Count()))
			return exList[id];
		return 0;
	}
	IOResult Save(ISave *isave); 
	IOResult Load(ILoad *iload); 
	BOOL isInList(int vertID, int &where);
	BOOL isExcluded(int vertID);
	BOOL isInList(int vertID);
	void ExcludeVerts(Tab<int> exList);
	void IncludeVerts(Tab<int> incList);
	void SetExclusionVerts(Tab<int> exclusionList);
};

class BoneModData : public LocalModData, public ISkinContextData {
	public:
		BOOL initialShadeVC, initialDisplayVC;

		float squashAmount;
		BitArray autoInteriorVerts;
		Tab<int> vecOwnerList;
//gizmo data
		Tab<LocalGizmoData*> gizmoData;
//vertex info
		// NS: flag that shows if the InitMesh TM has to be recomputed
		bool recompInitMeshTM;
        int VertexDataCount;
        float effect;

		// This data value is needed for the CSkinCallabck (Eval)
		TimeValue CurrentTime;
		
		// SkinCallback variables :
		DWORD		cookie;
		CSkinCallback *pSkinCallback;

		// The engine pointer
		ISkinEngine *pSE;

		Point3 hitPoint;
		BOOL isHit;

		Tab<VertexListClass*> VertexData;
		Matrix3 BaseTM,InverseBaseTM;

		// This is the node TM of the mesh at initialization time
		Matrix3 BaseNodeTM;
		ObjectState *hitState;
//		Mesh mesh;
//		PatchMesh patch;
		BOOL isMesh;
		BOOL isPatch;
		BOOL isSurf;
//selectio info
        BitArray selected;

		Tab<VertexDistanceClass> DistCache;
		int CurrentCachePiece;

		Tab<Matrix3> tmCacheToBoneSpace;
		Tab<Matrix3> tmCacheToObjectSpace;

		Tab<Point3> tempTableL1;
		Tab<Point3> tempTableL2;
		Tab<Point3> tempTableL1ObjectSpace;
		Tab<Point3> tempTableL2ObjectSpace;

		BOOL forceUpdate;
		Point3 localCenter;

		WeightRestore *wundo;
//6-18-99
		BOOL inputObjectIsNURBS;
		BonesDefMod *mod;  //ns
//watje 9-7-99  198721 
        BOOL reevaluate;
        BOOL justRedraw;

//		BOOL addGizmo;

		Tab<ExclusionListClass*> exclusionList;
		BOOL isExcluded(int boneID, int vertID);
		void ExcludeVerts(int boneID, Tab<int> exList);
		void IncludeVerts(int boneID, Tab<int> incList);
		void SelectExcludedVerts(int boneID);

		BoneModData(BonesDefMod *m)
			{
			recompInitMeshTM = false; //ns
			pSkinCallback = NULL;
			InitSkinEngine();
			VertexDataCount = 0;
			isMesh = FALSE;
			forceUpdate = FALSE;
			effect = -1.0f;
			mod = m;
//watje 9-7-99  198721 
            reevaluate = FALSE;
			justRedraw = FALSE;
//			addGizmo = FALSE;
		
			//mesh = NULL;
//			sel.ZeroCount();
			}
		~BoneModData()
			{
			VertexData.ZeroCount();
			VertexDataCount =0;
			}	
		LocalModData*	Clone()
			{
//			BoneModData* d = new BoneModData();
//			d->VertexDataCount = -1;

			return NULL;
			}

		void InitSkinEngine();
		
		// From ISkinContextData (Skin API) ns
		virtual int GetNumPoints();
		virtual int GetNumAssignedBones(int pointIdx);
		virtual int GetAssignedBone(int pointIdx, int boneIdx);
		virtual float GetBoneWeight(int pointIdx, int boneIdx);

		virtual int GetSubCurveIndex(int pointIdx, int boneIdx);
		virtual int GetSubSegmentIndex(int pointIdx, int boneIdx);
		virtual float GetSubSegmentDistance(int pointIdx, int boneIdx);
		virtual Point3 GetTangent(int pointIdx, int boneIdx);
		virtual Point3 GetOPoint(int pointIdx, int boneIdx);
};

