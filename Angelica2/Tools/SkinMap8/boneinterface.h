#pragma once

//  ================================================================================
//	Content of this file is copied from file: MAXSDK\samples\modifiers\bonesdef\bonesdef.h and it's
//	associated include files, so this file should be updated when these file changes
//  ================================================================================

#include "max.h"

//these are vertex flag attributes
#define VERTEXFLAG_MODIFIED		2	//whether the vertex has been hand weighted
#define VERTEXFLAG_UNNORMALIZED	4	//whether the vertex is normalized
#define VERTEXFLAG_RIGID		8	//whether the vertex is rigid,if it is rigid only one bone will be affect the vertex
#define VERTEXFLAG_RIGIDHANDLE	16	//only applies to patches, when set if it is a handle it will use the weights of the knot that owns the handle
#define VERTEXFLAG_TEMPSELECTED	32  //used internally to hold temporary selections for cut and paste
#define VERTEXFLAG_HIDDEN		64  //used to track hidden vertices passed up the stack

class BonesDefMod;

//this contains our vertex weight info
//basically it contains a index into the bones list, the weight and bunch of optional spline parameters
class VertexInfluenceListClass
{
public:
	int Bones;						// this is an index into the bones list,which bones belongs to the weight
	float Influences;				// this is the unnormalized weight for the Bones
	float normalizedInfluences;		// this is the normalized weight for Bones

	//extra data to hold spline stuff
	// this contains info on the closest point on the spline to the vertex
	// the data is based on the initial spline position
	int SubCurveIds;				// this which curve the point is closest to NOTE NOT USED YET
	int SubSegIds;					// this is the closest seg
	float u;						// this is the U value along the segment which is the closest point to the vertex
	Point3 Tangents;				// this is the tangent
	Point3 OPoints;					// this is the point in spline space

	VertexInfluenceListClass()
	{
		Bones = 0;          
		Influences = 0.0f;  
		normalizedInfluences = 0.0f;
		//extra data to hold spline stuff
		SubCurveIds = 0;
		SubSegIds=0;
		u=0.0f;
		Tangents = Point3(1.0f,0.0f,0.0f);
		OPoints = Point3(0.0f,0.0f,0.0f);
	}
};

// this is the per vertex data class
// it contains all the attributes of the vertex and a list of weights for this vertex
class VertexListClass
{
public:
	DWORD	flags; 

	// These are get/sets for our flag properties
	// the properties are
	// Modified		whether the vertex has been hand weighted
	// Unormalized  whether the vertex is normalized
	// Rigid		whether the vertex is rigid,if it is rigid only one bone will be affect the vertex
	// Rigid Handle only applies to patches, when set if it is a handle it will use the weights of the knot that owns the handle
	// TempSelected	used internally to hold temporary selections for cut and paste
	inline BOOL IsSelectedTemp()  
	{
		if (flags & VERTEXFLAG_TEMPSELECTED) return TRUE;
		else return FALSE;
	}

	inline BOOL IsModified()
	{
		if (flags & VERTEXFLAG_MODIFIED) return TRUE;
		else return FALSE;
	}
	inline BOOL IsUnNormalized()
	{
		if (flags & VERTEXFLAG_UNNORMALIZED) return TRUE;
		else return FALSE;
	}

	inline BOOL IsRigid()
	{
		if (flags & VERTEXFLAG_RIGID) return TRUE;
		else return FALSE;
	}

	inline BOOL IsRigidHandle()
	{
		if (flags & VERTEXFLAG_RIGIDHANDLE) return TRUE;
		else return FALSE;
	}

	inline BOOL IsHidden()
	{
		if (flags & VERTEXFLAG_HIDDEN) return TRUE;
		else return FALSE;
	}

	inline void SelectedTemp(BOOL sel)
	{
		if (sel)
			flags |= VERTEXFLAG_TEMPSELECTED;
		else flags &= ~VERTEXFLAG_TEMPSELECTED;
	}

	inline void Modified(BOOL modify)
	{
		if (modify)
		{
			if (!IsModified())
				NormalizeWeights();
			flags |= VERTEXFLAG_MODIFIED;
		}
		else flags &= ~VERTEXFLAG_MODIFIED;
	}

	inline void UnNormalized(BOOL unnormalized)
	{
		if (unnormalized)
		{
			flags |= VERTEXFLAG_UNNORMALIZED;
			if (!IsModified())
			{
				Modified(TRUE);					
			}
		}
		else 
		{
			BOOL wasNormal =  !(flags  | VERTEXFLAG_UNNORMALIZED);
			flags &= ~VERTEXFLAG_UNNORMALIZED;
			if (wasNormal)
				Modified(TRUE);

		}
	}
	inline void Rigid(BOOL rigid)
	{
		if (rigid)
			flags |= VERTEXFLAG_RIGID;
		else flags &= ~VERTEXFLAG_RIGID;
	}

	inline void RigidHandle(BOOL rigidHandle)
	{
		if (rigidHandle)
			flags |= VERTEXFLAG_RIGIDHANDLE;
		else flags &= ~VERTEXFLAG_RIGIDHANDLE;
	}

	inline void Hide(BOOL hide)
	{
		if (hide)
			flags |= VERTEXFLAG_HIDDEN;
		else flags &= ~VERTEXFLAG_HIDDEN;
	}

	Point3 LocalPos;				//this is local position of the vertex before any skin deformation
	Point3 LocalPosPostDeform;		//this is local position of the vertex before after skin deformation

	//table of misc data    

	VertexListClass()
	{
		flags = 0; 
		closestBone = -1;
		closestBoneCache = -1;
		closetWeight = 1.0f;
		Modified (FALSE);		
		LocalPos = Point3(0.0f,0.0f,0.0f);
		LocalPosPostDeform = Point3(0.0f,0.0f,0.0f);
	}

	//this returns the bone that most affects this vertex
	int GetMostAffectedBone()
	{
		if ((d.Count() == 0) && (closestBone != -1))
			return closestBone;

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

	//this returns the ith index of the bone that most affects this vertex
	int GetMostAffectedBoneID()
	{
		if ((d.Count() == 0) && (closestBone != -1))
			return 0;

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

	//this loops though the unnormalized weights 
	// and stuffs the normalized values in the normalizedInfluences variable
	void NormalizeWeights()
	{
		float sum = 0.0f;
		for (int i = 0; i < d.Count(); i++)
			sum +=  d[i].Influences;

		for ( i = 0; i < d.Count(); i++)
		{
			if (sum == 0.0f)
				d[i].Influences = 0.0f;
			else d[i].Influences = d[i].Influences/sum ;
			d[i].normalizedInfluences = d[i].Influences;
		}

	}	

	int WeightCount() 
	{ 
		if ((d.Count() == 0) && (closestBone != -1))
			return 1;
		return d.Count(); 
	}
	void SetWeightCount(int ct) { d.SetCount(ct); }
	void ZeroWeights() { d.ZeroCount(); }

	int GetBoneIndex(int index) 
	{ 
		if ((d.Count() == 0) && (closestBone != -1))
			return closestBone;
		return d[index].Bones; 
	}
	void SetBoneIndex(int index, int boneID) 
	{ 
		if (index < d.Count())
			d[index].Bones = boneID; 
	}

	float GetNormalizedWeight(int index) 
	{ 
		if ((d.Count() == 0) && (closestBone != -1))
			return 1.0f;
		return d[index].normalizedInfluences; 
	}
	float GetWeight(int index) 
	{ 
		if ((d.Count() == 0) && (closestBone != -1))
			return 1.0f;
		return d[index].Influences; 
	}
	void SetWeight(int index,float w) 
	{ 
		if (index < d.Count())
			d[index].Influences = w; 
		else if (closestBone != -1)
		{
			VertexInfluenceListClass vd;
			vd.Bones = closestBone;
			vd.normalizedInfluences = w;
			vd.Influences = w;
			AppendWeight(vd);
		}
	}
	void SetNormalizedWeight(int index,float w) 
	{ 
		if (index < d.Count())
			d[index].normalizedInfluences = w; 
		/*
		else if (closestBone != -1)
		{
		VertexInfluenceListClass vd;
		vd.Bones = closestBone;
		vd.normalizedInfluences = w;
		vd.Influences = w;
		AppendWeight(vd);
		}
		*/

	}


	void SetWeightInfo(int index,int boneIndex, float w, float nw) 
	{ 

		if (index < d.Count())
		{
			d[index].Bones = boneIndex;
			d[index].Influences = w; 
			d[index].normalizedInfluences = nw; 
		}
	}
	float GetCurveU(int index) 
	{ 
		if (index >= d.Count())
			return 0.0f;
		return d[index].u; 
	}
	int GetCurveID(int index) 
	{ 
		if (index >= d.Count())
			return 0;
		return d[index].SubCurveIds; 
	}
	int GetSegID(int index) 
	{ 
		if (index >= d.Count())
			return 0;
		return d[index].SubSegIds; 
	}
	Point3 GetOPoint(int index) 
	{ 
		if (index >= d.Count())
			return Point3(0.0f,0.0f,0.0f);
		return d[index].OPoints; 
	}
	Point3 GetTangent(int index) 
	{ 
		if (index >= d.Count())
			return Point3(0.0f,0.0f,0.0f);
		return d[index].Tangents; 
	}

	void SetCurveU(int index, float u) 
	{ 
		if (index < d.Count())
			d[index].u = u; 
	}
	void SetCurveID(int index, int id) 
	{ 
		if (index < d.Count())
			d[index].SubCurveIds = id; 
	}
	void SetSegID(int index, int id) 
	{ 
		if (index < d.Count())
			d[index].SubSegIds = id; 
	}
	void SetOPoint(int index, Point3 p) 
	{ 
		if (index < d.Count())
			d[index].OPoints = p; 
	}
	void SetTangent(int index, Point3 p) 
	{ 
		if (index < d.Count())
			d[index].Tangents = p; 
	}


	void SetWeightSplineInfo(int index,float u, int curve, int seg, Point3 p, Point3 t) 
	{ 
		if (index < d.Count())
		{
			d[index].u = u;
			d[index].SubCurveIds = curve; 
			d[index].SubSegIds = seg; 
			d[index].OPoints = p;
			d[index].Tangents = t;
		}
	}

	void DeleteWeight(int index) 
	{ 
		if (index < d.Count())
			d.Delete(index,1); 
	}
	void AppendWeight(VertexInfluenceListClass w) { d.Append(1,&w,4); }
	void Sort();

	VertexInfluenceListClass CopySingleWeight(int index) 
	{ 
		if (index < d.Count())
			return d[index]; 
		else
		{
			VertexInfluenceListClass td;
			td.Influences = 1.0f;
			td.normalizedInfluences = 1.0f;
			td.Bones = closestBone;
			return td;
		}
	}
	void PasteSingleWeight(int index,VertexInfluenceListClass td ) 
	{ 
		if (index < d.Count())
			d[index] = td; 
		else if (d.Count() == 0) 
			d.Append(1,&td);
	}

	Tab<VertexInfluenceListClass> CopyWeights() { return d; }
	void PasteWeights(Tab<VertexInfluenceListClass> w) { d = w; }

	//THESE should not be used and only are available for the com engine
	int* GetBoneIndexAddr(int index) { return &d[index].Bones; }
	float* GetNormalizedWeightAddr(int index) 
	{ 
		if ((d.Count() == 0) && (closestBone != -1))
			return &closetWeight;
		return &d[index].normalizedInfluences; 
	}
	float* GetCurveUAddr(int index) { return &d[index].u; }
	int* GetCurveIDAddr(int index) { return &d[index].SubCurveIds; }
	int* GetSegIDAddr(int index) { return &d[index].SubSegIds; }
	Point3* GetOPointAddr(int index) { return &d[index].OPoints; }
	Point3* GetTangentAddr(int index) { return &d[index].Tangents; }

	int GetClosestBone() { return closestBone; }
	void SetClosestBone(int bid) { closestBone = bid; }

	int GetClosestBoneCache() { return closestBoneCache; }
	void SetClosestBoneCache(int bid) { closestBoneCache = bid; }

public:

	int closestBone;
	int closestBoneCache;
	float closetWeight;			
	Tab<VertexInfluenceListClass> d; //this is the table of of bones and weights that affect this vertex
};


//this is the gizmo local data
//since every gizmo can use indices into the vertex list there needs to be local 
//data per gizmo in case the skin modifier is instanced
//this basically contains a list of vertices that the gizmo affects
class LocalGizmoData
{
private:
	//new fix 2
	int start;				//start is the the first vertex index that is being deformed
	//just used to save some space for the bitarray

	BitArray deformingVerts;	//this is a temporary bitarray containing a list of all
	//vertices that the gizmo will deform.  This is a temporary
	//list that is created before deformation and destroyed rigth after
	//this list is used to speed up look ups of vertices that the gizmo owns

	Tab<int> affectedVerts;		//this is the actual list of all vertices owned by the gizmo

public:
	int whichGizmo;							//this is the gizmo index into the param block that owns this local data
	BOOL IsAffected(int index);
	BOOL IsInList(int index,int &where);
	void SetVert(int index);
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);

	int Count() { return affectedVerts.Count(); }
	void ZeroCount() { affectedVerts.ZeroCount(); }
	void Replace(int where, int index ) { affectedVerts[where] = index; }
	void Replace(Tab<int> &list ) { affectedVerts = list; }
	int GetVert(int i ) { return affectedVerts[i]; }
	void BuildDeformingList();
	void FreeDeformingList();
	inline BOOL IsInDeformingList(int index)
	{
		index = index - start;
		if ((index >=0) && (index<deformingVerts.GetSize()))
			return deformingVerts[index];
		return FALSE;
	};
};

//this class is used to cache our distances
//every time a bone is selected all the distances are computed from this bone
//and stored in a table of this class
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
class ExclusionListClass;
class AdjEdgeList;

// this is local data for skin.  This mainly contains per vertex weight and attributes
class BoneModData : public LocalModData, public ISkinContextData {

public:

	TimeValue lastTMCacheTime;					//this is the last evalutaion time
	BOOL nukeValidCache;						//this is flag used to nuke the caches

	BitArray autoInteriorVerts;					//this is a bitarray used when the object is a patch
	//flags which vertices are auto interior which should not
	//be deformed

	Tab<int> vecOwnerList;						//the is for patches, it is a list points and if that point is 
	//handle it points to the knot that owns it

	BitArray validVerts;						//this is a list of which vertices have been normalized

	//gizmo data
	Tab<LocalGizmoData*> gizmoData;				//this is the table of local gizmo data

	//vertex info
	// NS: flag that shows if the InitMesh TM has to be recomputed
	// this is only applicable for older pre max ver 3 files now
	bool recompInitMeshTM;

	float effect;  //this is just a variable to hold the value of the Abs Effect spinner

	// This data value is needed for the CSkinCallabck (Eval)
	TimeValue CurrentTime;

	// SkinCallback variables :
	DWORD		cookie;
	CSkinCallback *pSkinCallback;

	// The engine pointer
	ISkinEngine *pSE;


	//This is the list of vertices and their weights
	//each vertex contains a list of bones and how much each bone contributes(weight)
	Tab<VertexListClass*> VertexData;

	Matrix3 BaseTM,InverseBaseTM;   //BaseTM takes vertices from local space(skin) to world space
	//InverseBaseTM transforms from worlds space to local(skin)
	Matrix3 baseNodeOffsetTM;

	//these are 2 helper matrices to help gizmos deal with the double transform problem
	Matrix3 gizmoRemoveDoubleOffset;   //this is the matrix to pull out the double transform
	Matrix3 gizmoPutBackDoubleOffset;	//this is a matric to put back the double transform


	// This is the node TM of the mesh at initialization time
	Matrix3 BaseNodeTM;

	//these are just some flags to tell us what type of object the local date belongs to
	BOOL isMesh;
	BOOL isPatch;
	BOOL inputObjectIsNURBS;			//this is whether the object is a nurbs surface

	//vertex selection info
	BitArray selected;

	//this is a temporary selection so we can keep our veretx selection when a bone is picked		
	BitArray tempSelected;

	//this is the distance cache, basically whenever a bone is selected we compute the 
	//distance of the vertices to the envelope and store it here so we dont have to 
	//keep continually computing this data
	Tab<VertexDistanceClass> DistCache;

	//this is the bone that owns the DistCache
	int CurrentCachePiece;

	//these martix caches, just a bunch of matrix contations to save some time
	Tab<Matrix3> tmCacheToBoneSpace;		//this takes us from local space(skin) to bonespace
	Tab<Matrix3> tmCacheToObjectSpace;		//this takes us from bone space to local space(skin)

	//these are various tables of
	//these are used to compute distance since these use the initial tm
	Tab<Point3> tempTableL1;				//start points in local space(skin) using the initial tms
	Tab<Point3> tempTableL2;				//end points in local space(skin) using the initial tms
	//these are used for display since they use the current tm
	Tab<Point3> tempTableL1ObjectSpace;		//start points in local space(skin) using the current tms
	Tab<Point3> tempTableL2ObjectSpace;		//end points in local space(skin) using the current tms

	Point3 localCenter;					//this is local center of the sub object selection

	BonesDefMod *mod;  //ns Just a pointer back to the modifier that owns this data

	//watje 9-7-99  198721 
	BOOL reevaluate;	//this is a flag that will force all computed weights to get recomputed
	//it will rebuild all caches

	BOOL reevaluateActiveBone;	//this is a flag that will force all computed weights to get recomputed
	//on the active bone

	//every bone contains a list of vertices which it will not affect
	//these are functions to examine and manipulate these lists
	Tab<ExclusionListClass*> exclusionList;		//these are the tables of exclusion lists

	BOOL forceRecomuteBaseNode;	//this causes all the initial bone matrices to be recomputed
	//this is used when the user resets skin or twiddles with 
	//Always Deform check box

	INode *meshNode;			//this is a pointer to the node that owns this local data

	BOOL needTMsRebuilt;		//this forces all the base node(the object that has skin applied to it) matrices to be rebuilt
	BOOL rebuildWeights;		//this forces all the weights to be normalized.  This usually does not need to be set

	//edge connection list
	AdjEdgeList *edgeList;
	Tab<MNEdge> mnEdges;
	Tab<MNEdge> patchEdges;
	Tab<float> blurredWeights;
	//blurred amount list

	BOOL unlockVerts;			//this flag causes the selected bones to be reset(they become unmodified and are put back under control of envelopes)


	BoneModData(BonesDefMod *m)
	{
		unlockVerts = FALSE;
		edgeList = NULL;

		lastTMCacheTime = -999931;
		recompInitMeshTM = false; //ns
		pSkinCallback = NULL;
		InitSkinEngine();
		isMesh = FALSE;
		effect = -1.0f;
		mod = m;
		//watje 9-7-99  198721 
		reevaluate = FALSE;
		reevaluateActiveBone = FALSE;
		nukeValidCache = TRUE;

		meshNode = NULL;
		forceRecomuteBaseNode = FALSE;
		needTMsRebuilt = FALSE;
		rebuildWeights = TRUE;

		mesh = NULL;
		patch = NULL;
		mnMesh = NULL;

	//	RegisterNotification(DeleteNodeNotifyProc, (void*)this, NOTIFY_SCENE_PRE_DELETED_NODE);
	}
	~BoneModData()
	{
	}	

	//this Initializes all the skin component engines
	void InitSkinEngine();

	// From ISkinContextData (Skin API) ns
	//returns the number of vertices
	virtual int GetNumPoints();
	//returns the number of bones assigned to a vertex
	//	pointIdx the vertex to look at
	virtual int GetNumAssignedBones(int pointIdx);
	//returns which bone is assigned to the vertex
	//	pointIdx which vertex
	//	boneIdx the ith entry in the bone list
	virtual int GetAssignedBone(int pointIdx, int boneIdx);
	//returns the weight of bone which is assigned to the vertex
	//	pointIdx which vertex
	//	boneIdx the ith entry in the bone list
	virtual float GetBoneWeight(int pointIdx, int boneIdx);

	//these return any spline info if the vertex is affacted by a spline

	//this returns which sub curve the vertex belongs to 
	//	pointIdx which vertex
	//	boneIdx the ith entry in the bone list
	virtual int GetSubCurveIndex(int pointIdx, int boneIdx);
	//this returns which sub segment the vertex belongs to 
	//	pointIdx which vertex
	//	boneIdx the ith entry in the bone list
	virtual int GetSubSegmentIndex(int pointIdx, int boneIdx);
	//this returns the sub segment U value
	//	pointIdx which vertex
	//	boneIdx the ith entry in the bone list
	virtual float GetSubSegmentDistance(int pointIdx, int boneIdx);
	//this returns the sub segment tangent value
	//	pointIdx which vertex
	//	boneIdx the ith entry in the bone list
	virtual Point3 GetTangent(int pointIdx, int boneIdx);
	//this returns the point on the sub segment
	//	pointIdx which vertex
	//	boneIdx the ith entry in the bone list
	virtual Point3 GetOPoint(int pointIdx, int boneIdx);

	Tab<BitArray*> namedSelSets;		//named selection sets 

	Mesh *mesh;
	PatchMesh *patch;
	MNMesh *mnMesh;
};

