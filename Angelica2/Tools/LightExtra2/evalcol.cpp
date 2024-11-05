/**********************************************************************
 *<
	FILE:			EvalCol.cpp
	DESCRIPTION:	Vertex Color Renderer
	CREATED BY:		Christer Janson
	HISTORY:		Created Monday, December 12, 1996

 *>	Copyright (c) 1997 Kinetix, All Rights Reserved.
 **********************************************************************/
//***************************************************************************
// December 12/13   1996	CCJ
// January  8		1997	CCJ  Bugfix
// June		1		1997	CCJ  Port to MAX 2.0
// June		6		1997	CCJ  Implemented full ShadeContext
// Dec		1		1998	CCJ  Shadow casting n' colors per face (unmixed vertex colors)
//
// Description:
// These functions calculates the diffuse, ambient or pre-lit color at each
// vertex or face of an INode.
//
// Exports:
// BOOL calcMixedVertexColors(INode*, TimeValue, int, ColorTab&);
//      This function calculates the interpolated diffuse or ambient 
//      color at each vetex of an INode.
//      Usage: Pass in a node pointer and the TimeValue to generate
//      a list of Colors corresponding to each vertex in the mesh
//      Use the int flag to specify if you want to have diffuse or 
//      ambient colors, or if you want to use the scene lights.
//      Note: 
//        You are responsible for deleting the Color objects in the table.
//      Additional note:
//        Since materials are assigned by face, this function renders each
//        face connected to the specific vertex (at the point of the vertex)
//        and mixes the colors afterwards. If this is not what you want
//        you can use the calcFaceColors() to calculate the color at the
//        centerpoint of each face.
//
//***************************************************************************

#include "max.h"
#include "bmmlib.h"
#include "evalcol.h"
#include "toneop.h"
#include "stdmat.h"
#include "instance.h"
#include <A3D.h> // for timing usage

// Factor by which the evaluation point is moved, from the vertex towards the face center
static const float vertexEvalEspilon = 1e-4f;
// Distance, along the vertex normal, from which the evaluation ray is cast.
static const float vertexEvalDistance = 1e-2f;

DWORD WINAPI dummyProgressStart(LPVOID arg);

// Enable this to print out debug information
// #define EVALCOL_DEBUG

class SContext;
class RefEnumProc;
class MeshInstance;

// Information about closest hit for shadow casting
struct HitInfo {
	MeshInstance*	instance;
	float			distance;	// Distance from light to shaded point
	Point3			hitPos;
	int				nFaceNum;	// index of the face
	Point3			baryCoord;	// position pos in bary coordinates
	};

typedef float Plane[4];	// A plane definition.

Point3		interpVertexNormal(Mesh* mesh, Matrix3 tm, unsigned int vxNo, BitArray& faceList);
void		AddSceneLights(INode* node, SContext* sc, MtlBaseLib* mtls, int* numLights);
int			LoadMapFiles(INode* node, SContext* sc, MtlBaseLib& mtls, TimeValue t);
void		EnumRefs(ReferenceMaker *rm, RefEnumProc &proc);
TriObject*	GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);

// Shadow raycasting
// These functions are essentially ripped out of context from the
// sample renderer. For full information please refer to
// maxsdk\samples\cjrender
BOOL		intersectMesh(Ray* ray, HitInfo& hitInfo);
BOOL		intersectTri(Ray* ray, Point3* tri, Point3& baryCoord, Point3& normal, Point3& hitPos);
BOOL		intersectTriMy(Ray* ray, Point3* tri, Point3& baryCoord, Point3& normal, Point3& hitPos);
void		CompPlaneEqn(Plane plane, const Point3& p0, const Point3& p1, const Point3& p2);
BOOL		rayBoundHit(Ray* ray, Box3 boundingBox);
BOOL		TMNegParity(Matrix3 &m);
Point3		CalcBaryCoords(Point3 p0, Point3 p1, Point3 p2, Point3 p);

#define BIGFLOAT (float(1.0e30))


//***************************************************************************
//* The is the map enumerator class used for collecting projector lights for
//* spotlights
//***************************************************************************

class GetMaps: public RefEnumProc {
	MtlBaseLib *mlib;
	public:
	void proc(ReferenceMaker *rm);
	GetMaps(MtlBaseLib *mbl);
};

//***************************************************************************
//* The is the Light descriptor object for default lights
//***************************************************************************
class DefObjLight : public ObjLightDesc 
{
	public:
		Color intensCol;   // intens*color 
		DefObjLight(DefaultLight *l);
		void DeleteThis() {delete this;}
		int Update(TimeValue t, const RendContext& rc, RenderGlobalContext *rgc, BOOL shadows, BOOL shadowGeomChanged);
		int UpdateViewDepParams(const Matrix3& worldToCam);
		BOOL Illuminate(ShadeContext& sc, Point3& normal, Color& color, Point3 &dir, float &dot_nl, float &diffCoef);
};

class LightInfo {
public:
	LightInfo(INode* node, MtlBaseLib* mtls);
	LightInfo(INode * node, ObjLightDesc * pLightDesc, LightObject * pLight, MtlBaseLib* mtls);
	LightInfo(DefaultLight* l);
	~LightInfo();

	ObjLightDesc* lightDesc;
	LightObject *light;
	bool		bRefGlobal;
	BOOL		bIsObscured;
	float		distanceToShadedPoint;
};

typedef Tab<LightInfo*> LightTab;

//***************************************************************************
//* The MeshInstance list is a list of all meshes in world space
//***************************************************************************

class MeshInstance {
	public:

	MeshInstance(INode* node);
	~MeshInstance();

	INode*	node;
	Mesh*	mesh;	// A copy of the mesh

	BOOL	negParity;

	Point3	center;	// Bounding sphere
	float	radsq;

	Box3	boundingBox;
	};

MeshInstance::MeshInstance(INode* node)
	{
	mesh = NULL;
	int	v;

	this->node = node;

	BOOL	deleteIt;
	TriObject* tri = GetTriObjectFromNode(node, GetCOREInterface()->GetTime(), deleteIt);
	if (tri) {
		mesh = new Mesh;
		*mesh = *&tri->GetMesh();

		Point3 vx;
		Matrix3 objToWorld = node->GetObjTMAfterWSM(GetCOREInterface()->GetTime());

		negParity = TMNegParity(objToWorld);

		mesh->buildRenderNormals();

		// Transform the vertices
		for (v=0; v<mesh->numVerts; v++) {
			vx =  objToWorld * mesh->getVert(v);
			mesh->setVert(v, vx);
			}

		Matrix3 normalObjToWorld(1);
		// Calculate the inverse-transpose of objToWorld for transforming normals.
		for (int it=0; it<3; it++) {
			Point4 p = Inverse(objToWorld).GetColumn(it);
			normalObjToWorld.SetRow(it,Point3(p[0],p[1],p[2]));
			}

		// Transform the face normals
		for (int nf = 0; nf < mesh->numFaces; nf++) {
			Point3	fn = mesh->getFaceNormal(nf);
			Point3	nfn = VectorTransform(normalObjToWorld, fn);
			mesh->setFaceNormal(nf, nfn);
		}

		boundingBox = mesh->getBoundingBox();

		// Get the bounding sphere
		center = 0.5f*(boundingBox.pmin+boundingBox.pmax);
		radsq = 0.0f;
		Point3 d;
		float nr;
		for (v= 0; v<mesh->numVerts; v++) {
			d = mesh->verts[v] - center;
			nr = DotProd(d,d);
			if (nr>radsq) radsq = nr;
			}

		if (deleteIt) {
			delete tri;
			}
		}
	}

MeshInstance::~MeshInstance()
	{
	if (mesh) {
		mesh->DeleteThis();
		}
	}

typedef Tab<MeshInstance*> MeshInstanceTab;

void InitInstanceList(INode* node, MeshInstanceTab& ilist)
	{
	if (!node->IsRootNode()) {
		if (!node->IsHidden()) {
			ObjectState os = node->EvalWorldState(GetCOREInterface()->GetTime());
			if (os.obj && os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
				MeshInstance* inst = new MeshInstance(node);
				ilist.Append(1, &inst, 50);
				}
			}
		}

	int numChildren = node->NumberOfChildren();
	for (int c=0; c<numChildren; c++) {
		InitInstanceList(node->GetChildNode(c), ilist);
		}
	}

void FreeInstanceList(MeshInstanceTab& ilist)
	{
	int numItems = ilist.Count();
	for (int i=0; i<numItems; i++) {
		delete ilist[i];
		}
	ilist.ZeroCount();
	ilist.Shrink();
	}

MeshInstance* GetNodeFromMeshInstance(INode* node, MeshInstanceTab& ilist)
	{
	int numItems = ilist.Count();
	for (int i=0; i<numItems; i++) {
		if (ilist[i]->node == node) {
			return ilist[i];
			}
		}
	return NULL;
	}

//***************************************************************************
//* RendContext is used to evaluate the lights
//***************************************************************************

 class RContext: public RendContext {
	public:
		Matrix3	WorldToCam() const { return Matrix3(1); }
		ShadowBuffer*	NewShadowBuffer() const;
		ShadowQuadTree*	NewShadowQuadTree() const;
		Color	GlobalLightLevel() const;
		int	Progress(int done, int total) {
			return 1;
		}
};

//***************************************************************************
// ShadeContext for evaluating materials
//***************************************************************************

class SContext : public ShadeContext {
public:
	SContext();
	~SContext();

	TimeValue CurTime();
	int NodeID();
	INode* Node();
	Point3 BarycentricCoords();
	int FaceNumber();
	Point3 Normal();
	float Curve();

	LightDesc*	Light(int lightNo);
	Point3	GNormal(void);
	Point3	ReflectVector(void);
	Point3	RefractVector(float ior);
	Point3	CamPos(void);
	Point3	V(void);
	Point3	P(void);
	Point3	DP(void);
	Point3	PObj(void);
	Point3	DPObj(void);
	Box3	ObjectBox(void);
	Point3	PObjRelBox(void);
	Point3	DPObjRelBox(void);
	void	ScreenUV(Point2 &uv,Point2 &duv);
	IPoint2	ScreenCoord(void);
	Point3	UVW(int chan);
	Point3	DUVW(int chan);
	void	DPdUVW(Point3 [], int chan);
	void	GetBGColor(Color &bgCol, Color &transp, int fogBG);
	Point3	PointTo(const Point3 &p, RefFrame ito);
	Point3	PointFrom(const Point3 &p, RefFrame ito);
	Point3	VectorTo(const Point3 &p, RefFrame ito);
	Point3	VectorFrom(const Point3 &p, RefFrame ito);
	int		InMtlEditor();
	void	SetView(Point3 v);

	int		ProjType();
	void	SetNodeAndTime(INode* n, TimeValue tm);
	void	SetMesh(Mesh* m);
	void	SetBaryCoord(Point3 bary);
	void	SetFaceNum(int f);
	void	SetMtlNum(int mNo);
	void	SetTargetPoint(Point3 tp);
	void	SetViewPoint(Point3 vp);
	void	SetViewDir(Point3 vd);
	void	CalcNormals();
	void	CalcBoundObj();
	void	ClearLights();
	void	AddLight(LightInfo* li);
	void	SetAmbientLight(Color c);
	void	UpdateLights(RenderGlobalContext * pRGC=NULL);
	void	calc_size_ratio();
	float	RayDiam() {return 0.1f;}
	void	getTVerts(int chan);
	void	getObjVerts();

	int		NRenderElements();
	IRenderElement*	GetRenderElement(int n);

	void	CalcShadow(MeshInstanceTab& ilist);
	void	TurnOffObscuredLights(MeshInstance* mi);
	void	SetDiffuseOnly(bool d);

public:
	LightTab lightTab;
	LightTab allLights;
	Matrix3 tmAfterWSM;
	Point3 vxNormals[3];

private:
	INode* node;
	Mesh* mesh;
	Point3 baryCoord;
	int faceNum;
	Point3 targetPt;
	Point3 viewDir;
	Point3 viewPoint;
	TimeValue t;
	UVVert tv[MAX_MESHMAPS][3];
	Point3 bumpv[MAX_MESHMAPS][3];
	Box3 boundingObj;
	RContext rc;
	Point3	obpos[3];
	Point3	dobpos;
	float ratio;
	float curve;
	bool	bDiffuseOnly;
};

//***************************************************************************
//* Dummy Material : Simple Phong shader using Node color
//* This material is assigned to each node that does not have a material
//* previously assigned. The diffuse color is assigned based on the 
//* wireframe color.
//* This way we can assume that all nodes have a material assigned.
//***************************************************************************

#define DUMMTL_CLASS_ID	Class_ID(0x4d696bff, 0xf384f50)

#define DUMSHINE	.20f
#define DUMSPEC		.20f

class DumMtl: public Mtl {
	Color diff, spec;
	float phongexp;
	public:
		DumMtl(Color c);
		void Update(TimeValue t, Interval& valid);
		void Reset();
		Interval Validity(TimeValue t);
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
		Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
		Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
		float GetShininess(int mtlNum=0, BOOL backFace=FALSE);
		float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
		float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
		void SetAmbient(Color c, TimeValue t);
		void SetDiffuse(Color c, TimeValue t);
		void SetSpecular(Color c, TimeValue t);
		void SetShininess(float v, TimeValue t);
		Class_ID ClassID();
		void DeleteThis();
    	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	    	PartID& partID,  RefMessage message);
		void Shade(ShadeContext& sc);
};


// Return a pointer to a TriObject given an INode or return NULL
// if the node cannot be converted to a TriObject
TriObject* GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(t).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) { 
		TriObject *tri = (TriObject *) obj->ConvertToType(t, 
			Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri) deleteIt = TRUE;
		return tri;
	}
	else {
		return NULL;
	}
}

// Since vertices might have different normals depending on the face
// you are accessing it through, we get the normal for each face that
// connects to this vertex and interpolate these normals to get a single
// vertex normal fairly perpendicular to the mesh at the point of
// this vertex.
Point3 interpVertexNormal(Mesh* mesh, Matrix3 tm, unsigned int vxNo, BitArray& faceList)
{
	Point3 iNormal = Point3(0.0f, 0.0f, 0.0f);
	int numNormals = 0;

	for (int f = 0; f < mesh->numFaces; f++) {
		for (int fi = 0; fi < 3; fi++) {
			if (mesh->faces[f].v[fi] == vxNo) {
				Point3& fn = VectorTransform(tm, mesh->getFaceNormal(f));
				iNormal += fn;
				numNormals++;
				faceList.Set(f);
			}
		}
	}

	iNormal = iNormal / (float)numNormals;

	return Normalize(iNormal);
}


//***************************************************************************
// LightInfo encapsulates the light descriptor for standard and default lights
//***************************************************************************

LightInfo::LightInfo(INode* node, MtlBaseLib* mtls)
{
	ObjectState ostate = node->EvalWorldState(0);

	light = (LightObject*)ostate.obj;
	lightDesc = light->CreateLightDesc(node);
	bIsObscured = FALSE;

	// Process projector maps
	GetMaps getmaps(mtls);
	EnumRefs(light,getmaps);

	bRefGlobal = false;
}

LightInfo::LightInfo(INode* node, ObjLightDesc * pLightDesc, LightObject * pLight, MtlBaseLib* mtls)
{
	light = pLight;
	lightDesc = pLightDesc;
	bIsObscured = FALSE;

	// Process projector maps
	GetMaps getmaps(mtls);
	EnumRefs(light,getmaps);

	bRefGlobal = true;
}

LightInfo::LightInfo(DefaultLight* l)
{
	lightDesc = new DefObjLight(l);
	light = NULL;

	bRefGlobal = false;
}

LightInfo::~LightInfo()
{
	if (lightDesc && !bRefGlobal) {
		delete lightDesc;
	}
}


//***************************************************************************
// Light Descriptor for the diffuse light we use
//***************************************************************************

DefObjLight::DefObjLight(DefaultLight *l) : ObjLightDesc(NULL)
{
	inode = NULL;
	ls = l->ls;
	lightToWorld = l->tm;
	worldToLight = Inverse(lightToWorld);
}


//***************************************************************************
// Update
//***************************************************************************

int DefObjLight::Update(TimeValue t, const RendContext& rc, RenderGlobalContext *rgc, BOOL shadows, BOOL shadowGeomChanged)
{
	intensCol  = ls.intens*ls.color;
	return 1;
}


//***************************************************************************
// Update viewdependent parameters
//***************************************************************************

int DefObjLight::UpdateViewDepParams(const Matrix3& worldToCam)
{
	lightToCam = lightToWorld * worldToCam;
	camToLight = Inverse(lightToCam);
	lightPos   = lightToCam.GetRow(3);  // light pos in camera space
	return 1;
}


//***************************************************************************
// Illuminate method for default lights
// This is a special illumination method in order to evaluate diffuse color
// only, with no specular etc.
//***************************************************************************

BOOL DefObjLight::Illuminate(ShadeContext& sc, Point3& normal, Color& color, Point3 &dir, float &dot_nl, float &diffCoef)
{
	dir = FNormalize(lightPos-sc.P());
	color = intensCol;
	diffCoef = dot_nl = DotProd(normal,dir);
	return (dot_nl<=0.0f)?0:1;
}

static inline Point3 pabs(Point3 p) { return Point3(fabs(p.x),fabs(p.y),fabs(p.z)); }

// The ShadeContext used to shade a material a a specific point.
// This ShadeContext is setup to have full ambient light and no other
// lights until you call SetLight(). This will cause the ambient light to
// go black.
SContext::SContext()
{
	mode = SCMODE_NORMAL;
	shadow = TRUE;
	doMaps = TRUE;
	filterMaps = TRUE;
	
	backFace = FALSE;
	ambientLight = Color(1.0f, 1.0f, 1.0f);
	mtlNum = 0;

	nLights = 0;
}

SContext::~SContext()
{
	ClearLights();
}


// When the mesh and face number is specified we calculate 
// and store the vertex normals
void SContext::CalcNormals()
{
	RVertex* rv[3];
	Face* f = &mesh->faces[faceNum];
	DWORD smGroup = f->smGroup;
	int numNormals;

	// Get the vertex normals
	for (int i = 0; i < 3; i++) {
		rv[i] = mesh->getRVertPtr(f->getVert(i));

		// Is normal specified
		// SPCIFIED is not currently used, but may be used in future versions.
		if (rv[i]->rFlags & SPECIFIED_NORMAL) {
			vxNormals[i] = rv[i]->rn.getNormal();
			vxNormals[i] = VectorTransform(tmAfterWSM, vxNormals[i]);
		}
		// If normal is not specified it's only available if the face belongs
		// to a smoothing group
		else if ((numNormals = rv[i]->rFlags & NORCT_MASK) && smGroup)
		{
			// If there is only one vertex is found in the rn member.
			if (numNormals == 1) {
				vxNormals[i] = rv[i]->rn.getNormal();
			}
			else 
			{
				// If two or more vertices are there you need to step through them
				// and find the vertex with the same smoothing group as the current face.
				// You will find multiple normals in the ern member.
				for (int j = 0; j < numNormals; j++) 
				{
					if (rv[i]->ern[j].getSmGroup() & smGroup) 
					{
						vxNormals[i] = rv[i]->ern[j].getNormal();
					}
				}
			}
			vxNormals[i] = VectorTransform(tmAfterWSM, vxNormals[i]);
		}
		else 
		{
			// We must not transform face normal, because it has been transformed already by
			// the shitting SContext
			vxNormals[i] = mesh->getFaceNormal(faceNum);
		}
	}

	vxNormals[0] = Normalize(vxNormals[0]);
	vxNormals[1] = Normalize(vxNormals[1]);
	vxNormals[2] = Normalize(vxNormals[2]);
}

void SContext::SetBaryCoord(Point3 bary)
{
	baryCoord = bary;
}

int SContext::ProjType()
{
	return bDiffuseOnly ? PROJ_PARALLEL : PROJ_PERSPECTIVE;
}

void SContext::SetNodeAndTime(INode* n, TimeValue tv)
{
	node = n;
	t = tv;
	tmAfterWSM = node->GetObjTMAfterWSM(t,NULL);
}

void SContext::SetFaceNum(int f)
{
	faceNum = f;
}

void SContext::SetMtlNum(int mNo)
{
	mtlNum = mNo;
}

void SContext::SetViewPoint(Point3 vp)
{
	viewPoint = vp;
}

void SContext::SetTargetPoint(Point3 tp)
{
	targetPt = tp;
}

void SContext::SetViewDir(Point3 vd)
{
	viewDir = vd;
}

void SContext::SetMesh(Mesh * m)
{
	mesh = m;
}

void SContext::AddLight(LightInfo* li)
{
	lightTab.Append(1, &li);
}

void SContext::ClearLights()
{
	for (int i=0; i<lightTab.Count(); i++) {
		delete lightTab[i];
	}
	lightTab.ZeroCount();
	lightTab.Shrink();
	allLights.ZeroCount();
	allLights.Shrink();

	nLights = 0;
}

void SContext::UpdateLights(RenderGlobalContext * pRGC)
{
	/*
	for (int i=0; i<lightTab.Count(); i++) {
		((LightInfo*)lightTab[i])->lightDesc->Update(t, rc, pRGC, TRUE, TRUE);
		((LightInfo*)lightTab[i])->lightDesc->UpdateViewDepParams(Matrix3(1));
	}*/

	nLights = lightTab.Count();
}

void SContext::SetAmbientLight(Color c)
{
	ambientLight = c;
}

void SContext::CalcBoundObj()
{
	if (!mesh)
		return;

	boundingObj.Init();

	// Include each vertex in the bounding box
	for (int nf = 0; nf < mesh->numFaces; nf++) {
		Face* f = &(mesh->faces[nf]);

		boundingObj += mesh->getVert(f->getVert(0));
		boundingObj += mesh->getVert(f->getVert(1));
		boundingObj += mesh->getVert(f->getVert(2));
	}
}

// Return current time
TimeValue SContext::CurTime()
{
	return t;
}

int SContext::NodeID()
{
	return -1;
}

INode* SContext::Node()
{
	return node;
}

Point3 SContext::BarycentricCoords()
{
	return baryCoord;
}

int SContext::FaceNumber()
{
	return faceNum;
}


// Interpolated normal
Point3 SContext::Normal()
{
	return Normalize(baryCoord.x*vxNormals[0] + baryCoord.y*vxNormals[1] + baryCoord.z*vxNormals[2]);
}

// Geometric normal (face normal)
Point3 SContext::GNormal(void)
{
	// The face normals are already in camera space
	return VectorTransform(tmAfterWSM, mesh->getFaceNormal(faceNum));
}

// Return a Light descriptor
LightDesc *SContext::Light(int lightNo)
{
	return ((LightInfo*)lightTab[lightNo])->lightDesc;
}

// Return reflection vector at this point.
// We do it like this to avoid specular color to show up.
Point3 SContext::ReflectVector(void)
	{
	Point3 nrm;
	if (bDiffuseOnly) {
		// If we aren't using scene lights, then we fake a light
		// just above the render point, therefore we need to remove
		// all specularity (otherwise it's all gonna be white)
		// We can do this by pointing the reflection vector away
		// from the viewpoint
		nrm = -Normal();
		}
	else {
		Point3 N = Normal();
		float VN = -DotProd(viewDir,N);
		nrm = Normalize(2.0f*VN*N + viewDir);
		}

	return nrm;
	}

// Foley & vanDam: Computer Graphics: Principles and Practice, 
//     2nd Ed. pp 756ff.
Point3 SContext::RefractVector(float ior)
{
	Point3 N = Normal();
	float VN,nur,k;
	VN = DotProd(-viewDir,N);
	if (backFace) nur = ior;
	else nur = (ior!=0.0f) ? 1.0f/ior: 1.0f;
	k = 1.0f-nur*nur*(1.0f-VN*VN);
	if (k<=0.0f) {
		// Total internal reflection: 
		return ReflectVector();
	}
	else {
		return (nur*VN-(float)sqrt(k))*N + nur*viewDir;
	}
}

Point3 SContext::CamPos(void)
{
	return viewPoint;
}

// Screen coordinate beeing rendered
IPoint2 SContext::ScreenCoord(void)
{
	return IPoint2(0,0);
}

// Background color
void SContext::GetBGColor(class Color &bgCol,class Color &transp,int fogBG)
{
	bgCol = Color(0.0f, 0.0f, 0.0f);
	transp = Color(0.0f, 0.0f, 0.0f);
}

// Transforms the specified point from internal camera space to the specified space.
Point3 SContext::PointTo(const class Point3 &p, RefFrame ito)
{
	if (ito==REF_OBJECT) {
		return Inverse(tmAfterWSM) * p;
	}

	return p;
}

// Transforms the specified point from the specified coordinate system
// to internal camera space.
Point3 SContext::PointFrom(const class Point3 &p, RefFrame ito)
{
	if (ito==REF_OBJECT) {
		return tmAfterWSM * p;
	}
	return p;
}

// Transform the vector from internal camera space to the specified space.
Point3 SContext::VectorTo(const class Point3 &p, RefFrame ito)
{
	if (ito==REF_OBJECT) {
		return VectorTransform(Inverse(tmAfterWSM), p);
	}
	return p;
}

// Transform the vector from the specified space to internal camera space.
Point3 SContext::VectorFrom(const class Point3 &p, RefFrame ito)
{
	if (ito==REF_OBJECT) {
		return VectorTransform(tmAfterWSM, p);
	}
	return p;
}

// This method returns the unit view vector, from the camera towards P,
// in camera space.
Point3 SContext::V(void)
{
	return viewDir;
}

// Returns the point to be shaded in camera space.
Point3 SContext::P(void)
{
	return targetPt;
}

// This returns the derivative of P, relative to the pixel.
// This gives the renderer or shader information about how fast the position
// is changing relative to the screen.
// TBD

#define DFACT .1f

Point3 SContext::DP(void)
{
	float d = (1.0f+DFACT)*(RayDiam())/(DFACT+(float)fabs(DotProd(Normal(),viewDir)));
	return Point3(d,d,d);
}

// Retrieves the point relative to the screen where the lower left
// corner is 0,0 and the upper right corner is 1,1.
void SContext::ScreenUV(class Point2 &uv,class Point2 &duv)
{
	Point2 p;

	uv.x = .5f;
	uv.y = .5f;
	duv.x = 1.0f;
	duv.y = 1.0f;
}

// Bounding box in object coords
Box3 SContext::ObjectBox(void)
{
	return boundingObj;
}

// Returns the point to be shaded relative to the object box where each
// component is in the range of -1 to +1.
Point3 SContext::PObjRelBox(void)
{
	Point3 q;
	Point3 p = PObj();
	Box3 b = ObjectBox(); 
	q.x = 2.0f*(p.x-b.pmin.x)/(b.pmax.x-b.pmin.x) - 1.0f;
	q.y = 2.0f*(p.y-b.pmin.y)/(b.pmax.y-b.pmin.y) - 1.0f;
	q.z = 2.0f*(p.z-b.pmin.z)/(b.pmax.z-b.pmin.z) - 1.0f;
	return q;
}

// Returns the derivative of PObjRelBox().
// This is the derivative of the point relative to the object box where
// each component is in the range of -1 to +1.
Point3 SContext::DPObjRelBox(void)
{
	Box3 b = ObjectBox(); 
	Point3 d = DPObj();
	d.x *= 2.0f/(b.pmax.x-b.pmin.x); 
	d.y *= 2.0f/(b.pmax.y-b.pmin.y); 
	d.z *= 2.0f/(b.pmax.z-b.pmin.z); 
	return d;
}

// Returns the point to be shaded in object coordinates.
Point3 SContext::PObj(void)
{
	return Inverse(tmAfterWSM) * P();
}

// Returns the derivative of PObj(), relative to the pixel.
// TBD
Point3 SContext::DPObj(void)
{
	Point3 d = DP();
	return VectorTransform(Inverse(tmAfterWSM),d);
}

// Returns the UVW coordinates for the point.
Point3 SContext::UVW(int chan)
{
	Point3 uvw = Point3(0.0f, 0.0f, 0.0f);
	UVVert tverts[3];

	if (mesh->mapSupport(chan)) {
		TVFace* tvf = &mesh->mapFaces(chan)[faceNum];
		tverts[0] = mesh->mapVerts(chan)[tvf->getTVert(0)];
		tverts[1] = mesh->mapVerts(chan)[tvf->getTVert(1)];
		tverts[2] = mesh->mapVerts(chan)[tvf->getTVert(2)];

		uvw = baryCoord.x*tverts[0] +
				baryCoord.y*tverts[1] +
				baryCoord.z*tverts[2];
	}

	return uvw;
}

static Point3 basic_tva[3] = { Point3(0.0,0.0,0.0),Point3(1.0,0.0,0.0),Point3(1.0,1.0,0.0)};
static Point3 basic_tvb[3] = { Point3(1.0,1.0,0.0),Point3(0.0,1.0,0.0),Point3(0.0,0.0,0.0)};
static int nextpt[3] = {1,2,0};
static int prevpt[3] = {2,0,1};

void MakeFaceUV(Face *f, UVVert *tv)
{
	int na,nhid,i;
	Point3 *basetv;
	/* make the invisible edge be 2->0 */
	nhid = 2;
	if (!(f->flags&EDGE_A))  nhid=0; 
	else if (!(f->flags&EDGE_B)) nhid = 1;
	else if (!(f->flags&EDGE_C)) nhid = 2;
	na = 2-nhid;
	basetv = (f->v[prevpt[nhid]]<f->v[nhid]) ? basic_tva : basic_tvb;
	for (i=0; i<3; i++) {
		tv[i] = basetv[na]; 
		na = nextpt[na];
	}
}

void SContext::getTVerts(int chan) 
{
	if (chan!=0&&(node->GetMtl()->Requirements(mtlNum)&MTLREQ_FACEMAP)) {
		MakeFaceUV(&mesh->faces[faceNum],tv[0]);
	}
	else {
		Mesh* m = mesh;
		if (!m->mapSupport(chan))
			return;

		UVVert* tverts = m->mapVerts(chan);
		TVFace* tvf = &m->mapFaces(chan)[faceNum];
		tv[chan][0] = tverts[tvf->t[0]];
		tv[chan][1] = tverts[tvf->t[1]];
		tv[chan][2] = tverts[tvf->t[2]];
	}
}

int SContext::NRenderElements()
{
	return 0;
}

IRenderElement* SContext::GetRenderElement(int n)
{
	return NULL;
}

void SContext::getObjVerts()
{
	// TBD
}

// Returns the UVW derivatives for the point.
Point3 SContext::DUVW(int chan)
{
	getTVerts(chan);
	calc_size_ratio();
	return 0.5f*(pabs(tv[chan][1]-tv[chan][0])+pabs(tv[chan][2]-tv[chan][0]))*ratio;
}

// This returns the bump basis vectors for UVW in camera space.
void SContext::DPdUVW(Point3 dP[3], int chan)
{
	getTVerts(chan);
	calc_size_ratio();
	Point3 bv[3];
	getObjVerts();
	ComputeBumpVectors(tv[chan], obpos, bv);
	bumpv[chan][0] = Normalize(bv[0]);
	bumpv[chan][1] = Normalize(bv[1]);
	bumpv[chan][2] = Normalize(bv[2]);
	dP[0] = bumpv[chan][0];
	dP[1] = bumpv[chan][1];
	dP[2] = bumpv[chan][2];
}

//--------------------------------------------------------------------
// Computes the average curvature per unit surface distance in the face
//--------------------------------------------------------------------
float ComputeFaceCurvature(Point3 *n, Point3 *v, Point3 bc)
{
	Point3 nc = (n[0]+n[1]+n[2])/3.0f;
	Point3 dn0 = n[0]-nc;
	Point3 dn1 = n[1]-nc;
	Point3 dn2 = n[2]-nc;
	Point3 c = (v[0] + v[1] + v[2]) /3.0f;
	Point3 v0 = v[0]-c;
	Point3 v1 = v[1]-c;
	Point3 v2 = v[2]-c;
	float d0 = DotProd(dn0,v0)/LengthSquared(v0);
	float d1 = DotProd(dn1,v1)/LengthSquared(v1);
	float d2 = DotProd(dn2,v2)/LengthSquared(v2);
	float ad0 = (float)fabs(d0);
	float ad1 = (float)fabs(d1);
	float ad2 = (float)fabs(d2);
	return (ad0>ad1)? (ad0>ad2?d0:d2): ad1>ad2?d1:d2;
}

static inline float size_meas(Point3 a, Point3 b, Point3 c)
{
	double d  = fabs(b.x-a.x);
	d += fabs(b.y-a.y);
	d += fabs(b.z-a.z);
	d += fabs(c.x-a.x);
	d += fabs(c.y-a.y);
	d += fabs(c.z-a.z);
	return float(d/6.0);
}

// This is an estimate of how fast the normal is varying.
// For example if you are doing enviornment mapping this value may be used to
// determine how big an area of the environment to sample.
// If the normal is changing very fast a large area must be sampled otherwise
// you'll get aliasing.  This is an estimate of dN/dsx, dN/dsy put into a
// single value.
// Signed curvature:
float SContext::Curve() {
	Point3 tpos[3];
	Face &f = mesh->faces[faceNum];
	tpos[0] = mesh->verts[f.v[0]];
	tpos[1] = mesh->verts[f.v[1]];
	tpos[2] = mesh->verts[f.v[2]];
	float d = ComputeFaceCurvature(vxNormals,tpos,baryCoord);
	curve = d*RayDiam();
	return backFace?-curve:curve;
}

#define SZFACT 1.5f

// Approximate how big fragment is relative to whole face.
void SContext::calc_size_ratio()
{
	Point3 dp = DP();
	Point3 cv[3];
	cv[0] = *mesh->getVertPtr((&mesh->faces[faceNum])->v[0]);
	cv[1] = *mesh->getVertPtr((&mesh->faces[faceNum])->v[1]);
	cv[2] = *mesh->getVertPtr((&mesh->faces[faceNum])->v[2]);
	float d = size_meas(cv[0], cv[1], cv[2]);
	ratio = SZFACT*(float)fabs(dp.x)/d;
}

int	SContext::InMtlEditor()
{
	return FALSE;
}

void SContext::SetView(Point3 v)
{
	viewPoint = v;
}

void SContext::SetDiffuseOnly(bool d)
	{
	bDiffuseOnly = d;
	}

/****************************************************************************
// Shadow buffer
 ***************************************************************************/

ShadowBuffer* RContext::NewShadowBuffer() const
{
	return NULL;
}

ShadowQuadTree* RContext::NewShadowQuadTree() const
{
	return NULL;
}

Color	RContext::GlobalLightLevel() const
{
	return Color(1,1,1); // TBD
}


/****************************************************************************
// Scan the scene for all lights and add them for the ShadeContext's lightTab
 ***************************************************************************/

void sceneLightEnum(INode* recvNode, INode* node, SContext* sc, MtlBaseLib* mtls, int* numLights)
{
	// For each child of this node, we recurse into ourselves 
	// until no more children are found.
	for (int c = 0; c < node->NumberOfChildren(); c++) {
		sceneLightEnum(recvNode, node->GetChildNode(c), sc, mtls, numLights);
	}

	// Get the ObjectState.
	// The ObjectState is the structure that flows up the pipeline.
	// It contains a matrix, a material index, some flags for channels,
	// and a pointer to the object in the pipeline.
	ObjectState ostate = node->EvalWorldState(0);
	if (ostate.obj==NULL) 
		return;

	// Examine the superclass ID in order to figure out what kind
	// of object we are dealing with.
	if (ostate.obj->SuperClassID() == LIGHT_CLASS_ID) {
		// Get the light object from the ObjectState
		LightObject *light = (LightObject*)ostate.obj;

		(*numLights)++;

		// Is this light turned on?
		if (light->GetUseLight()) {
			bool bUseLight = true;

			ExclList* nt = light->GetExclList(); // DS 8/31/00- changed from NameTab
			if (nt && nt->TestFlag(NT_AFFECT_ILLUM)) {
				if (light->Include()) {
					// Inclusion list
					if (nt->FindNode(recvNode) == -1) {
						bUseLight = false;
						}
					}
				else {
					if (nt->FindNode(recvNode) != -1) {
						bUseLight = false;
						}
					}
				}

			// Create a RenderLight and append it to our list of lights
			if (bUseLight) {
				LightInfo* li = new LightInfo(node, mtls);
				sc->lightTab.Append(1, &li);
				}
			}
		}
	}

void AddSceneLights(INode* node, SContext* sc, MtlBaseLib* mtls, int* numLights)
{
	INode* scene = GetCOREInterface()->GetRootNode();
	for (int i=0; i<scene->NumberOfChildren(); i++) {
		sceneLightEnum(node, scene->GetChildNode(i), sc, mtls, numLights);
	}
}

/****************************************************************************
// Material enumerator functions
// Before evaluating a material we need to load the maps used by the material
// and then tell the material to prepare for evaluation.
 ***************************************************************************/

class CheckFileNames: public NameEnumCallback {
	public:
		NameTab* missingMaps;
		BitmapInfo bi;
		CheckFileNames(NameTab* n);
		void RecordName(TCHAR *name);
};

//***************************************************************************
// Class to manage names of missing maps
//***************************************************************************

CheckFileNames::CheckFileNames(NameTab* n)
{
	missingMaps = n;
}

//***************************************************************************
// Add a name to the list if it's not already there
//***************************************************************************

void CheckFileNames::RecordName(TCHAR *name)
{ 
	if (name) {
		if (name[0]!=0) {
			if (missingMaps->FindName(name)<0) {
			    missingMaps->AddName(name);
			}
		}
	}
}

class MtlEnum {
	public:
		virtual int proc(MtlBase *m, int subMtlNum) = 0;
};

class MapLoadEnum:public MtlEnum {
	public:
		TimeValue t;

	   	MapLoadEnum(TimeValue time);
		virtual int proc(MtlBase *m, int subMtlNum);
};

//***************************************************************************
// Constructor of map loader
//***************************************************************************

MapLoadEnum::MapLoadEnum(TimeValue time)
{ 
	t = time; 
}

//***************************************************************************
// Map loader enum proc
//***************************************************************************

int MapLoadEnum::proc(MtlBase *m, int subMtlNum)
{
	Texmap *tm = (Texmap *)m;
	tm->LoadMapFiles(t);
	return 1;
}

// Map Deactive or active class
// this class is used to deactivate or reactivate all texture maps before and after shading
class MapActivateEnum : public MtlEnum {
	public:
		Mtl		  * m_pTopMtl;
		bool	  m_bActivate;
		TimeValue t;

	   	MapActivateEnum(TimeValue time, Mtl * pTopMtl, bool bActivate);
		virtual int proc(MtlBase *m, int subMtlNum);
};

//***************************************************************************
// Constructor of map loader
//***************************************************************************

MapActivateEnum::MapActivateEnum(TimeValue time, Mtl * pTopMtl, bool bActivate)
{ 
	t = time; 
	m_pTopMtl = pTopMtl;
	m_bActivate = bActivate;
}

//***************************************************************************
// Map loader enum proc
//***************************************************************************

int MapActivateEnum::proc(MtlBase *m, int subMtlNum)
{
	Texmap *tm = (Texmap *)m;
	
	return 1;
}

int EnumMaps(MtlBase *mb, int subMtl,  MtlEnum &tenum)
{
	if (IsTex(mb)) {
		if (!tenum.proc(mb,subMtl)) {
			return 0;
		}
	}
	for (int i=0; i<mb->NumSubTexmaps(); i++) {
		Texmap *st = mb->GetSubTexmap(i); 
		if (st) {
			int subm = (mb->IsMultiMtl()&&subMtl<0)?i:subMtl;
			if (mb->SubTexmapOn(i)) {
				if (!EnumMaps(st,subm,tenum)) {
					return 0;
				}
			}
		}
	}
	if (IsMtl(mb)) {
		Mtl *m = (Mtl *)mb;
		for (i=0; i<m->NumSubMtls(); i++) {
			Mtl *sm = m->GetSubMtl(i);
			if (sm) {
				int subm = (mb->IsMultiMtl()&&subMtl<0)?i:subMtl;
				if (!EnumMaps(sm,subm,tenum)) {
					return 0;
				}
			}
		}
	}
	return 1;
}

void EnumRefs(ReferenceMaker *rm, RefEnumProc &proc)
{
	proc.proc(rm);
	for (int i=0; i<rm->NumRefs(); i++) {
		ReferenceMaker *srm = rm->GetReference(i);
		if (srm) {
			EnumRefs(srm,proc);		
		}
	}
}

//***************************************************************************
// Constructor of map enumerator
//***************************************************************************

GetMaps::GetMaps(MtlBaseLib *mbl)
{
	mlib = mbl;
}

//***************************************************************************
// Implementation of the map enumerator
//***************************************************************************

void GetMaps::proc(ReferenceMaker *rm)
{
	if (IsTex((MtlBase*)rm)) {
		mlib->AddMtl((MtlBase *)rm);
	}
}

int LoadMapFiles(INode* node, SContext* sc, MtlBaseLib& mtls, TimeValue t)
{
	NameTab mapFiles;
	CheckFileNames checkNames(&mapFiles);

	node->EnumAuxFiles(checkNames, FILE_ENUM_MISSING_ONLY | FILE_ENUM_1STSUB_MISSING);

	// Check the lights
	for (int i = 0; i < sc->lightTab.Count(); i++) {
		if (((LightInfo*)sc->lightTab[i])->light != NULL) {
			((LightInfo*)sc->lightTab[i])->light->EnumAuxFiles(checkNames, 
				FILE_ENUM_MISSING_ONLY | FILE_ENUM_1STSUB_MISSING);
		}
	}

	if (mapFiles.Count()) {
		// Error! Missing maps.
		// not sure how to handle this so we gladly continue.
			
		//if (MessageBox(hWnd, "There are missing maps.\nDo you want to render anyway?", "Warning!", MB_YESNO) != IDYES) {
		//	return 0;
		//}
	}

	// Load the maps
	MapLoadEnum mapload(t);
	for (i=0; i<mtls.Count(); i++) {
		EnumMaps(mtls[i],-1, mapload);
	}

	return 1;
}

//***************************************************************************
// This material is used when a node does not have a material assigned.
//***************************************************************************

DumMtl::DumMtl(Color c)
{ 
	diff = c; spec = Color(DUMSPEC,DUMSPEC,DUMSPEC); 
	phongexp = (float)pow(2.0, DUMSHINE*10.0);
}

void DumMtl::Update(TimeValue t, Interval& valid)
{
}

void DumMtl::Reset()
{
}

Interval DumMtl::Validity(TimeValue t)
{
	return FOREVER;
}

ParamDlg* DumMtl::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
{
	return NULL;
}

Color DumMtl::GetAmbient(int mtlNum, BOOL backFace)
{
	return diff;
}

Color DumMtl::GetDiffuse(int mtlNum, BOOL backFace)
{
	return diff;
}

Color DumMtl::GetSpecular(int mtlNum, BOOL backFace)
{
	return spec;
}

float DumMtl::GetShininess(int mtlNum, BOOL backFace)
{
	return 0.0f;
}

float DumMtl::GetShinStr(int mtlNum, BOOL backFace)
{
	return 0.0f;
}

float DumMtl::GetXParency(int mtlNum, BOOL backFace)
{
	return 0.0f;
}

void DumMtl::SetAmbient(Color c, TimeValue t)
{
}		

void DumMtl::SetDiffuse(Color c, TimeValue t)
{
}

void DumMtl::SetSpecular(Color c, TimeValue t)
{
}

void DumMtl::SetShininess(float v, TimeValue t)
{
}

Class_ID DumMtl::ClassID()
{
	return DUMMTL_CLASS_ID;
}

void DumMtl::DeleteThis()
{
	delete this;
}

RefResult DumMtl::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	    	PartID& partID,  RefMessage message)
{
	return REF_SUCCEED;
}

//***************************************************************************
// Shade method for the dummy material
// If a node does not have a material assigned we create
// a dummy material that inherits the wireframe color of
// the node
//***************************************************************************

void DumMtl::Shade(ShadeContext& sc)
{
	Color lightCol;
	Color diffwk(0.0f,0.0f,0.0f);
	Color specwk(0.0f,0.0f,0.0f);
	Color ambwk(0.0f,0.0f,0.0f);
	Point3 N = sc.Normal();
	Point3	R = sc.ReflectVector();
	LightDesc *l;
	for (int i = 0; i<sc.nLights; i++) {
		l = sc.Light(i);
		register float NL, diffCoef;
		Point3 L;
		if (!l->Illuminate(sc, N, lightCol, L, NL, diffCoef))
			continue;

		if (l->ambientOnly) {
			ambwk += lightCol;
			continue;
			}
		// diffuse
		if (l->affectDiffuse)
			diffwk += diffCoef*lightCol;
		// specular
		if (l->affectSpecular) {
			float c = DotProd(L,R);
			if (c>0.0f) {
				c = (float)pow((double)c, (double)phongexp); 
				specwk += c*lightCol*NL;   // multiply by NL to SOFTEN 
			}
		}
	}
	sc.out.t = Color(0.0f,0.0f,0.0f);
	sc.out.c = (0.3f*sc.ambientLight + diffwk)*diff + specwk*spec+ambwk;		
}

#define LENGTH_EPSILON		0.01f
void SContext::CalcShadow(MeshInstanceTab& ilist)
{
	return;

	int i;
	for (i=0; i<allLights.Count(); i++) {
		allLights[i]->bIsObscured = FALSE;
		// Calculate the distance from the vertex to the light position
		allLights[i]->distanceToShadedPoint = Length(allLights[i]->lightDesc->LightPosition()-targetPt) - LENGTH_EPSILON;
		}

	if (node->RcvShadows()) {
		for (i=0; i<ilist.Count(); i++) {
			TurnOffObscuredLights(ilist[i]);
			}
		}

	lightTab.ZeroCount();
	for (i=0; i<allLights.Count(); i++) {
		if (!allLights[i]->bIsObscured) {
			lightTab.Append(1, &(allLights[i]), 5);
			}
		}

	nLights = lightTab.Count();
	}

#define LSQ(v) (v.x*v.x + v.y*v.y + v.z*v.z)

void SContext::TurnOffObscuredLights(MeshInstance* inst)
	{
	Mesh*		mesh = inst->mesh;
	LightDesc*	ld;
	Ray			r;
	Point3		pos;
	HitInfo		hitInfo;

	// For self shadowing, we should turn off this condition
	if (0 && inst->node == node)
		return;

	if (inst->node && !inst->node->CastShadows())
		return;

	hitInfo.instance = inst;

	int numLights = allLights.Count();
	for (int l=0; l<numLights; l++) {
		ld = ((LightInfo*)allLights[l])->lightDesc;

		if (allLights[l]->light) {	// CCJ 11/13/99 - This is NULL for default lights
			ExclList* nt = allLights[l]->light->GetExclList();  //DS 8/31/00 changed from NameTab
			if (nt && nt->TestFlag(NT_AFFECT_SHADOWCAST)) {
				if (allLights[l]->light->Include()) {
					// If it's not found in the inclusion list, it's out
					if (nt->FindNode(node) == -1) {
						allLights[l]->bIsObscured = FALSE;
						continue;
						}
					}
				else {
					// If it's found in the exclusion list, it's out
					if (nt->FindNode(node) != -1) {
						allLights[l]->bIsObscured = FALSE;
						continue;
						}
					}
				}
			}

		if (!allLights[l]->bIsObscured) {
			if (allLights[l]->light && allLights[l]->light->GetShadowMethod()!=LIGHTSHADOW_NONE) {

				pos = ld->LightPosition();
				r.p = pos;
				r.dir = targetPt-pos;
				Normalize(r.dir);

				// Bounding sphere test
				Point3 pc = inst->center - r.p;
				float v = DotProd(pc,r.dir);
				if ((inst->radsq - LSQ(pc) + v*v) >= 0.0f) {
					// Bounding box test
					if (rayBoundHit(&r, inst->boundingBox)) {
						if (intersectMesh(&r, hitInfo)) {
							if (hitInfo.distance /*Length(hitInfo.hitPos-r.p)*/ < allLights[l]->distanceToShadedPoint) {
								allLights[l]->bIsObscured = TRUE;
								}
							}
						}
					}
				}
			}
		}
	}





//***************************************************************************
// Determine if the ray hits the bounding box of the node.
// This is done for trivial hit rejection.
//***************************************************************************

BOOL rayBoundHit(Ray* ray, Box3 boundingBox)
{
	float t, tmin, tmax;
	float dir, pos;

	tmax = BIGFLOAT;
	tmin = 0.0f;

	dir = ray->dir.x;
	pos = ray->p.x;

	if (dir < 0.0f) {
		t = (boundingBox.pmin.x - pos) / dir;
		if (t < tmin)
			return FALSE;
		if (t <= tmax)
			tmax = t;
		t = (boundingBox.pmax.x - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return FALSE;
			tmin = t;
		}
	} else if (dir > 0.0f) {
		t = (boundingBox.pmax.x - pos) / dir;
		if (t < tmin)
			return FALSE;
		if (t <= tmax)
			tmax = t;
		t = (boundingBox.pmin.x - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return FALSE;
			tmin = t;
		}
	} else if (pos < boundingBox.pmin.x || pos > boundingBox.pmax.x)
		return FALSE;

	dir = ray->dir.y;
	pos = ray->p.y;

	if (dir < 0.0f) {
		t = (boundingBox.pmin.y - pos) / dir;
		if (t < tmin)
			return FALSE;
		if (t <= tmax)
			tmax = t;
		t = (boundingBox.pmax.y - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return FALSE;
			tmin = t;
		}
	} else if (dir > 0.0f) {
		t = (boundingBox.pmax.y - pos) / dir;
		if (t < tmin)
			return FALSE;
		if (t <= tmax)
			tmax = t;
		t = (boundingBox.pmin.y - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return FALSE;
			tmin = t;
		}
	} else if (pos < boundingBox.pmin.y || pos > boundingBox.pmax.y)
		return FALSE;

	dir = ray->dir.z;
	pos = ray->p.z;

	if (dir < 0.0f) {
		t = (boundingBox.pmin.z - pos) / dir;
		if (t < tmin)
			return FALSE;
		if (t <= tmax)
			tmax = t;
		t = (boundingBox.pmax.z - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return FALSE;
			tmin = t;
		}
	} else if (dir > 0.0f) {
		t = (boundingBox.pmax.z - pos) / dir;
		if (t < tmin)
			return FALSE;
		if (t <= tmax)
			tmax = t;
		t = (boundingBox.pmin.z - pos) / dir;
		if (t >= tmin) {
			if (t > tmax)
				return FALSE;
			tmin = t;
		}
	} else if (pos < boundingBox.pmin.z || pos > boundingBox.pmax.z)
		return FALSE;

	return TRUE;
}


//***************************************************************************
// Intersect a ray with a mesh
//***************************************************************************

// BOOL intersectMesh(Ray* ray, RenderNode* rn, int& faceNo, Point3& baryCoord, Point3& normal, Point3& hitPos)
BOOL intersectMesh(Ray* ray, HitInfo& hitInfo)
{
	Point3 tri[3];
	BOOL hit = FALSE;
	float dist = BIGFLOAT;
	Mesh* mesh = hitInfo.instance->mesh;

	// For each face in the mesh...
	for (int nf = 0; nf < mesh->numFaces; nf++) {
		Face* f = &(mesh->faces[nf]);

		/*
		// We have a pre-calculated bounding sphere for each face.
		// here we can do a trivial hit rejection to see if we can
		// discard the face.

		BoundingSphere* bs = hitInfo.instance->faceBoundSpheres[nf];
		Point3 pc = bs->bsCenter - ray->p;
		float v = DotProd(pc,ray->dir);
		if (bs->bsRadSq - LSQ(pc) + v*v < 0.0f) {
			// No point proceeding with this face...
			continue;
		}
		*/

		// Get the vertices
		if (!hitInfo.instance->negParity) {
			tri[0] = mesh->getVert(f->getVert(0));
			tri[1] = mesh->getVert(f->getVert(1));
			tri[2] = mesh->getVert(f->getVert(2));
		}
		else {
			// Scaling is negative, get the vertives
			// counter clockwise.
			tri[0] = mesh->getVert(f->getVert(2));
			tri[1] = mesh->getVert(f->getVert(1));
			tri[2] = mesh->getVert(f->getVert(0));
		}

		/* TBD: This slowed things down in some test scenes, but might work better
		// for real life scenes..
		// Do hit rejection on the triangle bouding box
		// to get a few extra triangles out of the way
		Box3 triBound;
		triBound.Init();

		triBound += tri[0];
		triBound += tri[1];
		triBound += tri[2];

		// If we don't hit the bounding box we don't need to hit test the
		// triangle
		if (!rayBoundHit(ray, triBound)) {
			continue;
		}
		*/

		// Intersect ray with triangle
		Point3 bc;
		Point3 n;
		Point3 hp;
		if (intersectTriMy(ray, tri, bc, n, hp)) {
			float dl = Length(ray->p - hp); // bug fixed: the length dl must be the length from ray pos to hit pos, the old one is just the length of the hit pos vector
			if (dl < dist) {
				hitInfo.hitPos = hp;
				if( hitInfo.instance->negParity )
					hitInfo.baryCoord = Point3(bc.z, bc.y, bc.x);
				else
					hitInfo.baryCoord = bc;
				hitInfo.nFaceNum = nf;
				hitInfo.distance = dl;
				dist = dl;
				hit = TRUE;
			}
		}
	}

	return hit;
}

//***************************************************************************
// Intersect a ray with a triangle.
//***************************************************************************
BOOL intersectTriMy(Ray* ray, Point3* tri, Point3& baryCoord, Point3& normal, Point3& hitPos)
{
	FLOAT t, u, v;
	Point3 v0, v1, v2;
	
#define ERR_T	1e-6f

	v0 = tri[0];
	v1 = tri[1];
	v2 = tri[2];
	// Find vectors for two edges sharing vert0
	Point3 edge1 = v1 - v0;
	Point3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	Point3 pvec;
	pvec = CrossProd(ray->dir, edge2);

	// If determinant is near zero, ray lies in plane of triangle
	FLOAT det = DotProd(edge1, pvec);
	if( det < ERR_T )
		return false;

	// Calculate distance from vert0 to ray origin
	Point3 tvec = ray->p - v0;

	// Calculate U parameter and test bounds
	u = DotProd(tvec, pvec);
	if( u < -ERR_T || u > det + ERR_T )
		return false;

	// Prepare to test V parameter
	Point3 qvec;
	qvec = CrossProd(tvec, edge1);

	// Calculate V parameter and test bounds
	v = DotProd(ray->dir, qvec);
	if( v < -ERR_T || u + v > det + ERR_T)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	t = DotProd(edge2, qvec);
	FLOAT fInvDet = 1.0f / det;
	t *= fInvDet;
	u *= fInvDet;
	v *= fInvDet;

	if( t < 0 )
		return false;

	normal = Point3(0.0f,0.0f,1.0f); // Not used!
	hitPos = ray->p + ray->dir * t;
	baryCoord = Point3(1.0f - u - v, u, v);
	return true;
}

//***************************************************************************
// Intersect a ray with a triangle.
//***************************************************************************

BOOL intersectTri(Ray* ray, Point3* tri, Point3& baryCoord,
				  Point3& normal, Point3& hitPos)
{
	Plane p;

	CompPlaneEqn(p, tri[0], tri[1], tri[2]);

	Point3 Pn = Point3(p[0], p[1], p[2]);
	Point3 Rd = ray->dir;
	Point3 R0 = ray->p;

	float Vd = Pn.x * Rd.x + Pn.y * Rd.y + Pn.z * Rd.z;
	if (Vd >= 0.0f)
		return FALSE;

	float V0 = -(Pn.x * R0.x + Pn.y * R0.y + Pn.z * R0.z + p[3]);
	float t = V0 / Vd;

	if (t < 0.0f)
		return FALSE;

	// Intersection with plane.
	Point3 Pi = Point3(R0.x+Rd.x*t, R0.y+Rd.y*t, R0.z+Rd.z*t);

	// Get the barycentric coordinates of the hitPoint.
	// If any of the components are > 1.0 the hit is outside the triangle
	baryCoord = CalcBaryCoords(tri[0], tri[1], tri[2], Pi);

	if (baryCoord.x >= 0.0f && baryCoord.x <= 1.0f) {
		if (baryCoord.y >= 0.0f && baryCoord.y <= 1.0f) {
			if (baryCoord.z >= 0.0f && baryCoord.z <= 1.0f) {
				normal = Point3(0.0f,0.0f,1.0f); // Not used!
				hitPos = Pi; // Intersection point (the point we render)
				return TRUE;
			}
		}
	}

	return FALSE;
}


//***************************************************************************
// Compute the plane equation for the three points making up the plane.
//***************************************************************************

void CompPlaneEqn(Plane plane, const Point3& p0,
				  const Point3& p1, const Point3& p2)
{
	Point3 e1 = p1-p0;
	Point3 e2 = p2-p0;
	Point3 p = CrossProd(e1,e2);
	p = Normalize(p);
	plane[0] = p.x;
	plane[1] = p.y;
	plane[2] = p.z;
	plane[3] = -DotProd(p0,p);
}

//***************************************************************************
// Determine is the node has negative scaling.
// This is used for mirrored objects for example. They have a negative scale
// so when calculating the normal we take the vertices counter clockwise.
// If we don't compensate for this the objects will be 'inverted'
//***************************************************************************

BOOL TMNegParity(Matrix3 &m)
{
    return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}


//***************************************************************************
// Calculate the determinant
// Thanks to Ruediger Hoefert, Absolute Software
//***************************************************************************

static float det2x2( float a, float b, float c, float d )
{
    float ans;
    ans = a * d - b * c;
    return ans;
}

//***************************************************************************
// 
// float = det3x3(  a1, a2, a3, b1, b2, b3, c1, c2, c3 )
//   
// calculate the determinant of a 3x3 matrix
// in the form
//
//     | a1,  b1,  c1 |
//     | a2,  b2,  c2 |
//     | a3,  b3,  c3 |
//
// Thanks to Ruediger Hoefert, Absolute Software
//***************************************************************************

static float det3x3( Point3 a,Point3 b,Point3 c )

{
   float a1, a2, a3, b1, b2, b3, c1, c2, c3;    
   float ans;
   
   a1 = a.x ; a2 = a.y ; a3 = a.z ;   
   b1 = b.x ; b2 = b.y ; b3 = b.z ;   
   c1 = c.x ; c2 = c.y ; c3 = c.z ;   

   ans = a1 * det2x2( b2, b3, c2, c3 )
       - b1 * det2x2( a2, a3, c2, c3 )
       + c1 * det2x2( a2, a3, b2, b3 );
   return ans;
}


//***************************************************************************
// Given three points in space forming a triangle (p0,p1,p2), 
// and a fourth point in the plane of that triangle, returns the
// barycentric coords of that point relative to the triangle.
// Thanks to Ruediger Hoefert, Absolute Software
//***************************************************************************

Point3 CalcBaryCoords(Point3 p0, Point3 p1, Point3 p2, Point3 p)
{ 
	Point3 bary;
	/*
	Point3 tpos[3];
	Point3 cpos;

	tpos[0] = p0;
	tpos[1] = p1;
	tpos[2] = p2;
	cpos = p;
	*/
 /*
 
 S.304 Curves+Surfaces for Computer aided design:

     u + v + w = 1

         area(p,b,c)    area(a,p,c)   area(a,b,p)
 u = -----------, v = -----------, w = -----------, 
    area(a,b,c)    area(a,b,c)   area(a,b,c)
 
   ax  bx  cx
   
 area(a,b,c) = 0.5 * ay  by  cy
   
   az  bz  cz
    
   
 */

	float area_abc, area_pbc, area_apc, area_abp; 
	
	// bug found, it is not correct to use det calculating the triangle area
	/*
	area_abc= det3x3(tpos[0],tpos[1],tpos[2]);
	area_abc=1.0f/(area_abc == 0 ? 0.001f : area_abc);
	
	area_pbc =det3x3(cpos   ,tpos[1],tpos[2]); 
	area_apc =det3x3(tpos[0],cpos   ,tpos[2]);
	area_abp =det3x3(tpos[0],tpos[1],cpos   );
	*/
	Point3 edge_ca, edge_ba, edge_cb;
	Point3 line_pa, line_pb, line_pc;

	edge_ba = p1 - p0;
	edge_ca = p2 - p0;
	edge_cb = p2 - p1;
	line_pa = p - p0;
	line_pb = p - p1;
	line_pc = p - p2;

	area_abc = CrossProd(edge_ba, edge_ca).Length() / 2.0f;
	area_abc=1.0f/(area_abc == 0 ? 0.001f : area_abc);

	area_pbc = CrossProd(line_pb, line_pc).Length() / 2.0f;
	area_apc = CrossProd(line_pa, line_pc).Length() / 2.0f;
	area_abp = CrossProd(line_pa, line_pb).Length() / 2.0f;

	
	bary.x = area_pbc * area_abc ;
	bary.y = area_apc * area_abc ;
	bary.z = area_abp * area_abc ;
	
	return bary;
}

// Dummy renderer class to pass with the render global context. This class simply does nothing useful.
class NullRenderer : public Renderer  {
public:
    class NullRendParamDlg : public RendParamDlg {
        virtual void AcceptParams() {}
        virtual void DeleteThis() { delete this; }
    };

    virtual int Open(INode *scene, INode *vnode,ViewParams *viewPar,RendParams &rp, HWND hwnd, DefaultLight* defaultLights=NULL,int numDefLights=0) { return 0; }
    virtual int Render(TimeValue t, Bitmap *tobm, FrameRendParams &frp, HWND hwnd, RendProgressCallback *prog=NULL, ViewParams *viewPar=NULL) { return 0; }
    virtual void Close (HWND hwnd) {}
    virtual RendParamDlg *CreateParamDialog(IRendParams *ir,BOOL prog=FALSE) { return new NullRendParamDlg;  }
    virtual void ResetParams() {}

    static NullRenderer m_theInstance;
};

NullRenderer NullRenderer::m_theInstance;

// Initializes the render global context. Most of the values are fake, since all
// we really need is the tone operator for the radiosity
void InitRenderGlobalContext(NodeRenderGlobalContext& renderGC) 
{
	renderGC.renderer = &NullRenderer::m_theInstance;
	/*
	Interface * ip = GetCOREInterface();
	Renderer * pRenderer = ip->CreateDefaultScanlineRenderer();
	ViewParams viewParam;
	ip->GetViewParamsFromNode(ip->GetSelNode(0), viewParam, 0);
	RendParams rp;

	ZeroMemory(&rp, sizeof(rp));
	rp.colorCheck = false;
	rp.force2Side = false;
	rp.rendHidden = false;
	rp.superBlack = false;
	rp.fieldRender = false;
	rp.isNetRender = false;
	rp.rendType = RENDTYPE_NORMAL;
	rp.width  = 100;
	rp.height = 100;

	if( !pRenderer->Open(ip->GetRootNode(), NULL, &viewParam, rp, NULL) )
		return;

	//int nRet = ip->OpenRenderer(pRenderer, &viewParam, RENDTYPE_NORMAL, 100, 100);
	renderGC.renderer = pRenderer;
	*/
    
    DbgAssert(renderGC.renderer != NULL);

	renderGC.devWidth = 100;
    renderGC.devHeight = 100;
	renderGC.xscale = 1.0;
    renderGC.yscale = 1.0;
	renderGC.camToWorld.IdentityMatrix();
    renderGC.worldToCam.IdentityMatrix();
	renderGC.globalLightLevel = Color(1,1,1);   // Not sure what this does
	renderGC.time = GetCOREInterface()->GetTime();

	// Set the tone operator
    ToneOperatorInterface* toneOpInt = static_cast<ToneOperatorInterface*>(
        GetCOREInterface(TONE_OPERATOR_INTERFACE));
    if(toneOpInt != NULL) {
        renderGC.pToneOp = toneOpInt->GetToneOperator();
    }

	// Add instance here;
	renderGC.NodeEnum(GetCOREInterface()->GetRootNode());

	int i;
	TimeValue t = GetCOREInterface()->GetTime();
	for(i=0; i<renderGC.instTab.Count(); i++) 
	{
		NullView nullView;

		renderGC.instTab[i]->mtl->Update(t, FOREVER);
		renderGC.instTab[i]->Update(t, nullView);
	}

	// Transform geometry from object space to Camera (view) space	
	for(i=0; i<renderGC.instTab.Count(); i++) 
	{
		renderGC.instTab[i]->UpdateViewTM(renderGC.worldToCam);
		renderGC.instTab[i]->mesh->buildRenderNormals();
	}
}

//****************************************************************************
// LightDescInstance encapsulates the light descriptor for all lights in scene
//****************************************************************************

LightDescInstance::LightDescInstance(INode* node, MtlBaseLib* mtls)
{
	ObjectState ostate = node->EvalWorldState(0);

	pNode = node;
	pLightObject = (LightObject*)ostate.obj;
	pObjLightDesc = pLightObject->CreateLightDesc(node);

	// Process projector maps
	GetMaps getmaps(mtls);
	EnumRefs(pLightObject, getmaps);
}

LightDescInstance::~LightDescInstance()
{
	if( pObjLightDesc )
	{
		delete pObjLightDesc;
	}
}

void NodeShador::enumLights(INode * node, MtlBaseLib* mtls)
{
	// For each child of this node, we recurse into ourselves 
	// until no more children are found.
	for(int c=0; c<node->NumberOfChildren(); c++) 
	{
		enumLights(node->GetChildNode(c), mtls);
	}

	ObjectState ostate = node->EvalWorldState(0);
	if( ostate.obj==NULL ) 
		return;

	// Examine the superclass ID in order to figure out what kind
	// of object we are dealing with.
	if( ostate.obj->SuperClassID() == LIGHT_CLASS_ID )
	{
		// Get the light object from the ObjectState
		LightObject *light = (LightObject*)ostate.obj;

		// Is this light turned on?
		if( light->GetUseLight() ) 
		{
			LightDescInstance * pLightDI = new LightDescInstance(node, mtls);
			
			m_pLightList->Append(1, &pLightDI);
		}
	}
}

void NodeShador::AddShadeLights(INode * pRecvNode, SContext * sc, MtlBaseLib * mtls, int * numLight)
{
	int nLightCount = 0;
	for(int i=0; i<m_pLightList->Count(); i++)
	{
		LightDescInstance * pLDI = (*m_pLightList)[i];
		LightObject * light = pLDI->pLightObject;
		bool bUseLight = true;

		ExclList* nt = light->GetExclList(); // DS 8/31/00- changed from NameTab
		if( nt && nt->TestFlag(NT_AFFECT_ILLUM) ) 
		{
			// First set use light flag to default value, that is the value when
			// the node is not in the list;
			bUseLight = light->Include() ? false : true;

			// If the recvNode is a memeber of a group, we must refer to the 
			// group dummy node to test include flag;
			INode * recvNode = pRecvNode;
			while( true )
			{
				if( light->Include() ) 
				{
					// Inclusion list
					if (nt->FindNode(recvNode) != -1) 
					{
						// If the list has specified this node, just use it;
						bUseLight = true;
						break;
					}
				}
				else 
				{
					if (nt->FindNode(recvNode) != -1) 
					{
						// If the list has specified this node, just use it;
						bUseLight = false;
						break;
					}
				}

				if( !recvNode->IsGroupMember() || recvNode->IsRootNode() )
					break;
				recvNode = recvNode->GetParentNode();
			}
		}	

		if( bUseLight )
		{
			LightInfo* li = new LightInfo(pLDI->pNode, pLDI->pObjLightDesc, pLDI->pLightObject, mtls);
			sc->lightTab.Append(1, &li);

			nLightCount ++;
		}
	}

	*numLight = nLightCount;
}

void NodeShador::ReleaseLights()
{
	for(int i=0; i<m_pLightList->Count(); i++)
	{
		LightDescInstance * pLDI = (*m_pLightList)[i];

		delete pLDI;
	}

	m_pLightList->ZeroCount();
	m_pLightList->Shrink();
}
/*******************************************************************************
*	Class used to shade a node's all faces								       *
*																               *
*	Created by Hedi for Angelica 3D light maps generating routine by using Max *
********************************************************************************/
NodeShador::NodeShador(NodeShadorOptions shadeOptions)
{
	m_pInstanceList = NULL;
	m_pSC			= NULL;
	m_pMesh			= NULL;
	m_pNode			= NULL;
	m_pMtl			= NULL;
	m_pMtls			= NULL;

	// Anylize the options;
	m_shadeOptions	= shadeOptions;
	m_bCastShadows	= shadeOptions.bCastShadows;

	// setup global scene mesh instances this will not change during node shading;
	m_pInstanceList = new MeshInstanceTab();
	InitInstanceList(GetCOREInterface()->GetRootNode(), *m_pInstanceList);

	// Create a RenderGlobalContext, necessary for using the tone operator with radiosity
    InitRenderGlobalContext(m_NodeRenderGlobalContext);

	// Update all lights here;
	// we should pre-load all light objDesc and update them here;
	m_pLightMtls = new MtlBaseLib;
	m_pLightList = new LightDescInstanceTab;

	enumLights(GetCOREInterface()->GetRootNode(), m_pLightMtls);
	RContext rc;
	GetCOREInterface()->ProgressStart(_T("Preparing Lights..."), TRUE, dummyProgressStart, NULL);
	for(int i=0; i<m_pLightList->Count(); i++)
	{
		GetCOREInterface()->ProgressUpdate(i * 100 / m_pLightList->Count());
		LightDescInstance * pLDI = (*m_pLightList)[i];
		pLDI->pObjLightDesc->Update(GetCOREInterface()->GetTime(), rc, &m_NodeRenderGlobalContext, TRUE, TRUE);
		pLDI->pObjLightDesc->UpdateViewDepParams(Matrix3(1));
	}
	GetCOREInterface()->ProgressUpdate(i * 100);
}

NodeShador::~NodeShador()
{
	AfterShade();

	ReleaseLights();
	if( m_pLightList )
	{
		delete m_pLightList;
		m_pLightList = NULL;
	}
	if( m_pLightMtls )
	{
		delete m_pLightMtls;
		m_pLightMtls = NULL;
	}
	if( m_pInstanceList )
	{
		FreeInstanceList(*m_pInstanceList);
		delete m_pInstanceList;
		m_pInstanceList = NULL;
	}
}

int TurnoffAllTexMaps(MtlBase *mb)
{
	if( !IsMtl(mb) )
		return 1;

	for(int i=0; i<mb->NumSubTexmaps(); i++)
	{
		mb->SetSubTexmap(i, NULL); 
	}

	Mtl *m = (Mtl *)mb;
	for(i=0; i<m->NumSubMtls(); i++) 
	{
		Mtl *sm = m->GetSubMtl(i);
		if( sm )
		{	
			if( !TurnoffAllTexMaps(sm) )
				return 0;
		}
	}
	
	return 1;
}

int TurnoffAllRaytraceSubMtls(MtlBase * mb)
{
	if( !IsMtl(mb) )
		return 1;

	for(int i=0; i<mb->NumSubTexmaps(); i++)
	{
		MtlBase * pMtl = mb->GetSubTexmap(i);
		if( !pMtl )
			continue;
		
		if( pMtl->ClassID() == Class_ID(0x27190ff4, 0x329b106e) ) // Raytrace material
		{
			mb->SetSubTexmap(i, NULL); 
		}
	}

	Mtl *m = (Mtl *)mb;
	for(i=0; i<m->NumSubMtls(); i++) 
	{
		Mtl *sm = m->GetSubMtl(i);
		if( sm )
		{	
			if( sm->ClassID() == Class_ID(0x27190ff4, 0x329b106e) )
			{
				m->SetSubMtl(i, NULL);
			}
			else
			{
				if( !TurnoffAllRaytraceSubMtls(sm) )
					return 0;
			}
		}
	}
	
	return 1;
}

int TurnoffDiffuseTexMaps(MtlBase *mb)
{
	MtlBase * realmb = mb;

	Mtl * pMtl = (Mtl *) mb;
	if( pMtl->ClassID() == Class_ID(0x459a16ee, 0x27977f9c) ) // Lightscape mtl
	{
		// we just get the lightscape mtl's baseobject mtl
		realmb = pMtl->GetSubMtl(0);
	}

	// we only turn off the texture maps that we used in IBL Mesh
	if( realmb->ClassID() == Class_ID(DMTL_CLASS_ID, 0) )
	{
		StdMat * pStdMtl = (StdMat *) realmb;
		
		if( pStdMtl->SubTexmapOn(ID_DI)  )
		{
			Texmap * pTexmap = pStdMtl->GetSubTexmap(ID_DI);
			if( pTexmap )
			{
				if( pTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
				{
					// We must turn off the texture mapping as well as set the diffuse to pure white to extra the exact light
					pStdMtl->SetSubTexmap(ID_DI, NULL);
					pStdMtl->SetAmbient(Color(1.0f, 1.0f, 1.0f), GetCOREInterface()->GetTime());
					pStdMtl->SetDiffuse(Color(1.0f, 1.0f, 1.0f), GetCOREInterface()->GetTime());
				}
			}
		}
		if( pStdMtl->SubTexmapOn(ID_OP)  )
		{
			Texmap * pTexmap = pStdMtl->GetSubTexmap(ID_OP);
			if( pTexmap )
			{
				pStdMtl->SetSubTexmap(ID_OP, NULL);
				/*if( pTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
				{
					// We must turn off the texture mapping as well as set the diffuse to pure white to extract the exact light
					pStdMtl->SetSubTexmap(ID_DI, NULL);
					pStdMtl->SetAmbient(Color(1.0f, 1.0f, 1.0f), GetCOREInterface()->GetTime());
					pStdMtl->SetDiffuse(Color(1.0f, 1.0f, 1.0f), GetCOREInterface()->GetTime());
				}*/
			}
		}
	}
	
	return 1;
}

bool NodeShador::BeginShade(INode * pNode, TimeValue t)
{
	m_pNode = pNode;
	m_time  = t;

	// Get mesh;
	MeshInstance* mi = GetNodeFromMeshInstance(m_pNode, *m_pInstanceList);
	if( !mi )
		return false;

	m_pMesh = mi->mesh;
	if( !m_pMesh )
		return false;

	m_nFaceCount = m_pMesh->numFaces;

	// prepare a shade context
	m_pSC = new SContext();
	m_pSC->SetNodeAndTime(m_pNode, m_time);

	// prepare the material
	Mtl * pNodeMtl = m_pNode->GetMtl();

	if( pNodeMtl )
	{
		if( pNodeMtl->ClassID() != Class_ID(0x27190ff4, 0x329b106e) )
		{
			// Duplicate the node's mtl if exist and is not a raytrace material;
			m_pMtl = (Mtl *) pNodeMtl->Clone();
			TurnoffAllRaytraceSubMtls(m_pMtl);
		}
		else
		{
			m_pMtl = new DumMtl(pNodeMtl->GetDiffuse());
			m_pMtl->SetAmbient(pNodeMtl->GetAmbient(), m_time);
			m_pMtl->SetSpecular(pNodeMtl->GetSpecular(), m_time);
			m_pMtl->SetShininess(pNodeMtl->GetShininess(), m_time);
		}
	}
	else
	{
		m_pMtl = new DumMtl(m_pNode->GetWireColor());
	}

	if( !m_shadeOptions.bUseMaps )
	{
		TurnoffDiffuseTexMaps(m_pMtl);
	}

	m_pMtls = new MtlBaseLib();
	m_pMtls->AddMtl(m_pMtl);

	// setup the shade context
	m_pSC->SetMesh(m_pMesh);
	m_pSC->CalcBoundObj();
	m_pSC->shadow = m_bCastShadows;
	m_pSC->doMaps = true;
	m_pSC->SetDiffuseOnly(false);
	m_pSC->globContext = &m_NodeRenderGlobalContext;
    
	// If we're using the real lights, we need to find them first
	if( 1 ) 
    {
        // Add the lights to the shade context
		int numLights = 0;
		AddShadeLights(m_pNode, m_pSC, m_pMtls, &numLights);

		// Add default lights if there are no lights in the scene
		if( numLights == 0 && m_shadeOptions.bDefaultLights )
		{
			DefaultLight	dl1, dl2;
			dl1.ls.intens	= 1.0f;
			dl1.ls.color	= Color(1.0f, 1.0f, 1.0f);
			dl1.ls.type		= OMNI_LGT;
			dl1.tm			= TransMatrix(1000.0f * Point3(-900.0f, -1000.0f, 1500.0f));

			dl2.ls.intens	= 1.0f;
			dl2.ls.color	= Color(1.0f, 1.0f, 1.0f);
			dl2.ls.type		= OMNI_LGT;
			dl2.tm			= TransMatrix(-1000.0f * Point3(-900.0f, -1000.0f, 1500.0f));

			LightInfo * l1, * l2;
			l1 = new LightInfo(&dl1);
			l2 = new LightInfo(&dl2);
			m_pSC->AddLight(l1);
			m_pSC->AddLight(l2);

			RContext rc;
			l1->lightDesc->Update(t, rc, &m_NodeRenderGlobalContext, TRUE, TRUE);
			l1->lightDesc->UpdateViewDepParams(Matrix3(1));
			l2->lightDesc->Update(t, rc, &m_NodeRenderGlobalContext, TRUE, TRUE);
			l2->lightDesc->UpdateViewDepParams(Matrix3(1));
        }

		m_pSC->SetAmbientLight(GetCOREInterface()->GetAmbient(t, FOREVER));
	}

	if( m_bCastShadows )
	{
		/*for(int i=0; i<m_pSC->lightTab.Count(); i++) 
		{
			m_pSC->allLights.Append(1, &(m_pSC->lightTab[i]), 5);
		}*/
	}

	// Below function call only update the shade context's light count varialbe
	m_pSC->UpdateLights(&m_NodeRenderGlobalContext);

	// Update material
	m_pMtl->Update(m_time, FOREVER);

    return true;
}

bool NodeShador::ShadeLightGrid(Point3& minPos, Point3& maxPos, float vGridSize, float vGridDim, A3DIBLLightGrid * pIBLLightGrid)
{
	// First add light source to pIBLLightGrid
	Tab<LightType> allLightTypes;

	int i;
	for(i=0; i<m_pLightList->Count(); i++)
	{
		A3DIBLLIGHTSOURCE source;

		LightDescInstance * pLightInstance = (*m_pLightList)[i];
		LightState ls;
		pLightInstance->pLightObject->EvalLightState(GetCOREInterface()->GetTime(), FOREVER, &ls);

		switch(ls.type)
		{
		case DIRECT_LGT:
			source.type = A3DIBLLIGHTTYPE_DIRECT;
			break;

		case OMNI_LGT:
			source.type = A3DIBLLIGHTTYPE_POINT;
			break;

		case SPOT_LGT:
			source.type = A3DIBLLIGHTTYPE_SPOT;
			break;

		case AMBIENT_LGT:
			source.type = A3DIBLLIGHTTYPE_POINT;
			break;

		default:
			source.type = A3DIBLLIGHTTYPE_POINT;
			break;
		}

		Point3 pos = pLightInstance->pObjLightDesc->lightToWorld.GetRow(3);
		Point3 dir = -pLightInstance->pObjLightDesc->lightToWorld.GetRow(2);

		source.vecPos.x = pos.x;
		source.vecPos.y = pos.z;
		source.vecPos.z = pos.y;

		source.vecDir.x = dir.x;
		source.vecDir.y = dir.z;
		source.vecDir.z = dir.y;

		Color colorLight = ls.color * ls.intens;
		source.color.r = colorLight.r;
		source.color.g = colorLight.g;
		source.color.b = colorLight.b;
		source.color.a = 1.0f;

		source.nLampStyle = -1;
		source.nLampSize = 60;
		source.vLampRange = 100.0f;
		source.vLampIntensity = 1.0f;

		int		nval;
		float	val;
		if( pLightInstance->pNode->GetUserPropInt("光晕类型", nval) )
		{
			switch(nval)
			{
			case 0:
				source.nLampStyle = LAMPSTYLE_SUN;
				break;
			case 1:
				source.nLampStyle = LAMPSTYLE_UNREAL;
				break;
			default:
				source.nLampStyle = -1;
				break;
			}
		}
		if( pLightInstance->pNode->GetUserPropInt("光晕大小", nval) )
		{
			source.nLampSize = nval;
		}
		if( pLightInstance->pNode->GetUserPropFloat("光晕范围", val) )
		{
			source.vLampRange = val;
		}
		if( pLightInstance->pNode->GetUserPropFloat("光晕亮度", val) )
		{
			source.vLampIntensity = val;
		}

		pIBLLightGrid->AddLightSource(source);
		allLightTypes.Append(1, &ls.type, 0);
	}

	// Then set light grid's dimension
	A3DVECTOR3 vecMin, vecMax;
	vecMin.x = minPos.x;
	vecMin.y = minPos.z;
	vecMin.z = minPos.y;

	vecMax.x = maxPos.x;
	vecMax.y = maxPos.z;
	vecMax.z = maxPos.y;

	int nLength, nWidth, nHeight;
	nLength = int((vecMax.x - vecMin.x) / vGridSize + 0.5f);
	nWidth = int((vecMax.z - vecMin.z) / vGridSize + 0.5f);
	nHeight = int((vecMax.y - vecMin.y) / vGridSize + 0.5f);

	pIBLLightGrid->SetDimension(nLength, nWidth, nHeight, vecMin, vecMax);

	A3DVECTOR3 vecStart, vecEnd;
	A3DVECTOR3 vecDelta;
	pIBLLightGrid->GetSamplePosition(0, &vecStart);
	pIBLLightGrid->GetSamplePosition((nLength + 1) * (nWidth + 1) + (nLength + 1) + 1, &vecEnd);
	vecDelta = (vecEnd - vecStart) / 3.0f;

	Point3 axis[6];
	axis[0] = Point3(1.0f, 0.0f, 0.0f); axis[1] = Point3(-1.0f, 0.0f, 0.0f);
	axis[2] = Point3(0.0f, 1.0f, 0.0f); axis[3] = Point3(0.0f, -1.0f, 0.0f);
	axis[4] = Point3(0.0f, 0.0f, 1.0f); axis[5] = Point3(0.0f, 0.0f, -1.0f);

	int x, y, z;
	int nLastProgress = 0;
	for(y=0; y<=nHeight; y++)
	{
		for(z=0; z<=nWidth; z++)
		{
			for(x=0; x<=nLength; x++)
			{
				int nIndex = y * (nLength + 1) * (nWidth + 1) + z * (nLength + 1) + x;

				// Update progress when neccessary
				int nProgress = nIndex * 100 / ((nLength + 1) * (nWidth + 1) * (nHeight + 1));
				if( nProgress > nLastProgress )
				{
					GetCOREInterface()->ProgressUpdate(nProgress);
					nLastProgress = nProgress;
				}

				A3DVECTOR3 vecPos;
				Point3     viewTarget, viewPoint, viewDir;

				pIBLLightGrid->GetSamplePosition(nIndex, &vecPos);
				viewTarget.x = vecPos.x;
				viewTarget.z = vecPos.y;
				viewTarget.y = vecPos.z;

				Tab<Color> allLightColors;
				Tab<Point3> allLightDirs;

				// Then traverse on each grid point to do illuminate();
				for(i=0; i<m_pLightList->Count(); i++)
				{
					LightDescInstance * pLightInstance = (*m_pLightList)[i];
					Point3 N, L;
					register float NL, diffCoef;
					Color lightAccumulate(0.0f, 0.0f, 0.0f);
					Color lightCol(0.0f, 0.0f, 0.0f);
					if( allLightTypes[i] == DIRECT_LGT )
						viewDir = -pLightInstance->pObjLightDesc->lightToWorld.GetRow(2).Normalize();
					else
						viewDir = viewTarget - pLightInstance->pObjLightDesc->lightToWorld.GetRow(3);
					viewDir = viewDir.Normalize();
					viewPoint = viewTarget - viewDir;

					m_pSC->SetViewPoint(viewPoint);
					m_pSC->SetTargetPoint(viewTarget);
					m_pSC->SetViewDir(viewDir);

					N = -viewDir;
					if( !pLightInstance->pObjLightDesc->Illuminate(*m_pSC, N, lightCol, L, NL, diffCoef) )
						lightCol = Color(0.0f, 0.0f, 0.0f);

					lightAccumulate += lightCol;

					if( m_shadeOptions.bSmoothGrid )
					{
						for(int k=0; k<8; k++)
						{
							Point3 delta;
							delta.x = -vecDelta.x + (k % 2) * vecDelta.x * 2.0f;
							delta.z = -vecDelta.y + ((k / 2) % 2) * vecDelta.y * 2.0f;
							delta.y = -vecDelta.z + (k / 4) * vecDelta.z * 2.0f;

							m_pSC->SetViewPoint(viewPoint + delta);
							m_pSC->SetTargetPoint(viewTarget + delta);
							m_pSC->SetViewDir(viewDir);

							N = -viewDir;
							if( !pLightInstance->pObjLightDesc->Illuminate(*m_pSC, N, lightCol, L, NL, diffCoef) )
								lightCol = Color(0.0f, 0.0f, 0.0f);

							lightAccumulate += lightCol;
						}
						
						lightAccumulate /= 9.0f;
					}

					allLightColors.Append(1, &lightCol, 0);
					allLightDirs.Append(1, &viewDir, 0); // view dir is just the lightDir
				}

				Color maxColor = allLightColors[0];
				int   nMaxIndex = 0;
#define BRIGHTNESS(color) ((color).r * 0.3f + (color).g * 0.59f + (color).b * 0.11f)

				for(i=1; i<allLightColors.Count(); i++)
				{
					if( BRIGHTNESS(maxColor) < BRIGHTNESS(allLightColors[i]) )
					{
						maxColor = allLightColors[i];
						nMaxIndex = i;
					}
				}

				Color ambientColor = m_pSC->ambientLight;
				Color ambientEqual(0.0f, 0.0f, 0.0f);
				for(i=0; i<allLightColors.Count(); i++)
				{
					if( i != nMaxIndex )
					{
						/*
						float vFactor = 0.0f;
						int   nFactorCount = 0;
						
						for(int j=0; j<6; j++)
						{
							float vDot = -DotProd(allLightDirs[nMaxIndex], axis[j]);
							if( vDot < 0.0f )
							{
							}
							else
							{
								vDot = -DotProd(allLightDirs[i], axis[j]);
								if( vDot > 0.0f )
								{
									vFactor += vDot;
									nFactorCount ++;
								}
							}
						}

						if( nFactorCount )
						{
							vFactor /= nFactorCount;
							if( vFactor > 0.0f )
							{
								ambientEqual += allLightColors[i] * vFactor * 1.5f;
								nAmbientCount ++;
							}
							//else
							//	maxColor += allLightColors[i] * (-vFactor);
						}*/

						/*
						float vDot;
						vDot = -DotProd(allLightDirs[i], allLightDirs[nMaxIndex]);
						if( vDot > 0.0f )
						{
							if( vDot > 0.5f )
								vDot = 0.5f;
							ambientEqual += allLightColors[i] * vDot;
						}*/
						float vDot = -DotProd(allLightDirs[i], allLightDirs[nMaxIndex]);
						if( allLightTypes[nMaxIndex] == DIRECT_LGT && allLightTypes[i] == DIRECT_LGT )
						{
							float vFactor;
							if( vDot < 0.0f )
							{
								vFactor = -vDot;
								maxColor += allLightColors[i] * vFactor;
							}
							else
							{
								vFactor = vDot;
								if( vDot > 0.666f )
									vDot = 0.666f;
								ambientEqual += allLightColors[i] * vFactor;
							}
						}
						else
						{
							// For point light's direction will change a lot, we shall not
							// use directions to evaluate the weight of the light
							if( vDot < 0.0f )
							{
								maxColor += allLightColors[i] * 0.333f;
							}
							else
							{
								ambientEqual += allLightColors[i] * 0.333f;	
							}
						}
					}
				}

				// determine ambientColor;
				ambientColor += ambientEqual;
				// reduce direct light to make it more real
				maxColor *= 1.0f - BRIGHTNESS(ambientEqual) * 0.8f;

				// Apply the tone operator
				if( (m_pSC->globContext != NULL) && (m_pSC->globContext->pToneOp != NULL) )
				{
					m_pSC->globContext->pToneOp->ScaledToRGB(maxColor);
					m_pSC->globContext->pToneOp->ScaledToRGB(ambientColor);
				}

				// Do some scale here;
				maxColor *= m_shadeOptions.vScale * vGridDim;
				ambientColor *= m_shadeOptions.vScale * vGridDim;

				maxColor.ClampMinMax();
				ambientColor.ClampMinMax();

				int r, g, b, a; // We use a to represent the brightness of the light color
				A3DIBLLIGHTSAMPLE sample;
				sample.wLightIndex = nMaxIndex;
				r = maxColor.r * 255;
				g = maxColor.g * 255;
				b = maxColor.b * 255;
				a = BRIGHTNESS(maxColor) * 255;
				sample.colorDirect = A3DCOLORRGBA(r, g, b, a);
				r = ambientColor.r * 255;
				g = ambientColor.g * 255;
				b = ambientColor.b * 255;
				a = BRIGHTNESS(ambientColor) * 255;
				sample.colorAmbient = A3DCOLORRGBA(r, g, b, a);

				pIBLLightGrid->SetLightSample(nIndex, sample);

				allLightColors.ZeroCount();
				allLightColors.Shrink();
			}
		}
	}

	allLightTypes.ZeroCount();
	allLightTypes.Shrink();
	
	return true;
}

bool NodeShador::ShadeIntoTriangle(int nFaceIndex, LPBYTE * ppFaceColors, int nWidth, int nHeight, int nVMap0, int nVMap1, int nVMap2)
{
	Matrix3 tm;
	
	int f = nFaceIndex;

	// Get face's 3 vertex position;
	Point3 vt[3];
	vt[0] = m_pMesh->verts[m_pMesh->faces[f].v[0]];
	vt[1] = m_pMesh->verts[m_pMesh->faces[f].v[1]];
	vt[2] = m_pMesh->verts[m_pMesh->faces[f].v[2]];
	
	int nVMap[3];
	nVMap[0] = nVMap0;
	nVMap[1] = nVMap1;
	nVMap[2] = nVMap2;
	
	// Calculate the coordinate of the center of the face
    Point3 faceCenter = (vt[0] + vt[1] + vt[2]) / 3;
    
	m_pSC->SetFaceNum(f);
	Face* face = &m_pMesh->faces[f];
	m_pSC->SetMtlNum(face->getMatID());
	m_pSC->CalcNormals();

	Point3 n[3];
	n[0] = m_pSC->vxNormals[0];
	n[1] = m_pSC->vxNormals[1];
	n[2] = m_pSC->vxNormals[2];

	LPBYTE pFaceColors;
	pFaceColors = (LPBYTE)a_malloc(nWidth * nHeight * 3 * sizeof(BYTE));
	if( NULL == pFaceColors )
		return false;

	ZeroMemory(pFaceColors, nWidth * nHeight * 3 * sizeof(BYTE));
	Color color;
	for(int y=0; y<nHeight; y++)
	{
		for(int x=0; x<nWidth; x++)
		{
			float u, v, w;

			// let the mesh-triangle cover the entire top-triangle bitmap
			// the sample point is consider at the center of each pixel
			//   0.0f             1.0f
			//	  *---*---*---*---*
			//	  |	++|+++|+++|++ |			  |
			//	  *-+-*---*---*-+-*
			//	  |	+ |	  |	  |	+ |
			//	  *-+-*---*---*---*
			//	  |	+ |	  |	  |	+ |
			//	  *-+-*---*---*---*
			//	  |	++|+++|+++|++ |
			//	  *---*---*---*---*
			// *---* is the bitmap image
			// +++++ is the mesh sample point
			
			// we must get the color of the edge exactly, or we will have a sharp edge there
			// and we must obey the texture maping rules;
			u = x * 1.0f / (nWidth - 1);
			v = y * 1.0f / (nHeight - 1);
			w = 1.0f - u - v;
			if( w < -0.1f && nWidth > 16 && nHeight > 16 ) // go into the other half
				continue;

			float m[3];
			m[nVMap[0]] = w;
			m[nVMap[1]] = u;
			m[nVMap[2]] = v;
			m_pSC->SetBaryCoord(Point3(m[0], m[1], m[2]));

			Point3 vxNormal = w * n[nVMap[0]] + u * n[nVMap[1]] + v * n[nVMap[2]];
			Point3 viewTarget = w * vt[nVMap[0]] + u * vt[nVMap[1]] + v * vt[nVMap[2]];

			// Move the view point slightly towards to center of the face to avoid floating-point precision problems
			viewTarget += (faceCenter - viewTarget) * vertexEvalEspilon;

			Point3 viewPoint = viewTarget + vertexEvalDistance * vxNormal;
			Point3 viewDir = -vxNormal;
			Point3 lightPos = viewPoint;

			// render vertex for this face.
			m_pSC->SetViewPoint(viewPoint);
			m_pSC->SetTargetPoint(viewTarget);
			m_pSC->SetViewDir(viewDir);
			
			m_pMtl->Shade(*m_pSC);
			color.r = m_pSC->out.c.r;
			color.g = m_pSC->out.c.g;
			color.b = m_pSC->out.c.b;
			
			// Apply the tone operator
			if( (m_pSC->globContext != NULL) && (m_pSC->globContext->pToneOp != NULL) )
			{
				m_pSC->globContext->pToneOp->ScaledToRGB(color);
			}

			color *= m_shadeOptions.vScale;
			color.ClampMinMax();

			pFaceColors[y * nWidth * 3 + x * 3 + 0] = BYTE(color.b * 255);
			pFaceColors[y * nWidth * 3 + x * 3 + 1] = BYTE(color.g * 255);
			pFaceColors[y * nWidth * 3 + x * 3 + 2] = BYTE(color.r * 255);
		}
	}

	*ppFaceColors = pFaceColors;
	
	return true;
}
 
bool NodeShador::ShadeTriangle(int nFaceIndex, LPBYTE * ppFaceColors, float vPrecision, int * pWidth, int * pHeight, int * pnVMap0, int * pnVMap1, int * pnVMap2)
{
	float vStep = 1.0f;

	if( vPrecision > 0.0f )
		vStep = vPrecision;

	Matrix3 tm;
	
	int f = nFaceIndex;

	// Get face's 3 vertex position;
	Point3 vt[3], e10, e20, e21;
	float  l10, l20, l21;
	vt[0] = m_pMesh->verts[m_pMesh->faces[f].v[0]];
	vt[1] = m_pMesh->verts[m_pMesh->faces[f].v[1]];
	vt[2] = m_pMesh->verts[m_pMesh->faces[f].v[2]];
	e10 = vt[1] - vt[0];
	e20 = vt[2] - vt[0];
	e21 = vt[2] - vt[1];
	l10 = e10.Length();
	l20 = e20.Length();
	l21 = e21.Length();

	int nVMap[3];
	float lh, lv;
	Point3 edgeH, edgeV;

	// 0*--------1
	//  |      /
	//  |    /
	//  |  /
	//  |/
	// 2
	if( l10 > l20 && l10 > l21 )
	{
		// the 3-rd vertex is the perpendicular
		nVMap[0] = 2;
		nVMap[1] = 0;
		nVMap[2] = 1;
		lh = l20;
		lv = l21;
		edgeH = e20;
		edgeV = e21;
	}
	else if( l20 > l10 && l20 > l21 )
	{
		// the 2-nd vertex is the perpendicular
		nVMap[0] = 1;
		nVMap[1] = 2;
		nVMap[2] = 0;
		lh = l21;
		lv = l10;
		edgeH = e21;
		edgeV = e10;
	}
	else
	{
		// the first vertex is the perpendicular
		nVMap[0] = 0;
		nVMap[1] = 1;
		nVMap[2] = 2;
		lh = l10;
		lv = l20;
		edgeH = e10;
		edgeV = e20;
	}

    // Calculate the coordinate of the center of the face
    Point3 faceCenter = (vt[0] + vt[1] + vt[2]) / 3;
    
	m_pSC->SetFaceNum(f);
	Face* face = &m_pMesh->faces[f];
	m_pSC->SetMtlNum(face->getMatID());
	m_pSC->CalcNormals();

	Point3 n[3];
	n[0] = m_pSC->vxNormals[0];
	n[1] = m_pSC->vxNormals[1];
	n[2] = m_pSC->vxNormals[2];

	int nWidth = lh / vStep;
	int nHeight = lv / vStep;

	if( 1 )
	{
		// now we only create pow2 width and height
		int nBits = 1;
		while( nWidth > (1 << nBits) )
		{
			nBits ++;
		}
		nWidth = (1 << nBits);

		nBits = 1;
		while( nHeight > (1 << nBits) )
		{
			nBits ++;
		}
		nHeight = (1 << nBits);
	}
	if( nWidth < 4 )
		nWidth = 4;
	if( nWidth > 512 )
		nWidth = 512;

	if( nHeight < 4 )
		nHeight = 4;
	if( nHeight > 512 )
		nHeight = 512;
	
	LPBYTE pFaceColors;
	pFaceColors = (LPBYTE)a_malloc(nWidth * nHeight * 3 * sizeof(BYTE));
	if( NULL == pFaceColors )
		return false;

	Color color;
	for(int y=0; y<nHeight; y++)
	{
		for(int x=0; x<nWidth; x++)
		{
			float u, v, w;

			// let the mesh-triangle cover the entire top-triangle bitmap
			// the sample point is consider at the center of each pixel
			//   0.0f             1.0f
			//	  *---*---*---*---*
			//	  |	++|+++|+++|++ |			  |
			//	  *-+-*---*---*-+-*
			//	  |	+ |	  |	  |	+ |
			//	  *-+-*---*---*---*
			//	  |	+ |	  |	  |	+ |
			//	  *-+-*---*---*---*
			//	  |	++|+++|+++|++ |
			//	  *---*---*---*---*
			// *---* is the bitmap image
			// +++++ is the mesh sample point
			
			// we must get the color of the edge exactly, or we will have a sharp edge there
			// and we must obey the texture maping rules;
			u = x * 1.0f / (nWidth - 1);
			v = y * 1.0f / (nHeight - 1);
			w = 1.0f - u - v;
			if( w < -0.1f && nWidth > 16 && nHeight > 16 ) // go into the other half
				continue;

			float m[3];
			m[nVMap[0]] = w;
			m[nVMap[1]] = u;
			m[nVMap[2]] = v;
			m_pSC->SetBaryCoord(Point3(m[0], m[1], m[2]));

			Point3 vxNormal = w * n[nVMap[0]] + u * n[nVMap[1]] + v * n[nVMap[2]];
			Point3 viewTarget = w * vt[nVMap[0]] + u * vt[nVMap[1]] + v * vt[nVMap[2]];

			// Move the view point slightly towards to center of the face to avoid floating-point precision problems
			viewTarget += (faceCenter - viewTarget) * vertexEvalEspilon;

			Point3 viewPoint = viewTarget + vertexEvalDistance * vxNormal;
			Point3 viewDir = -vxNormal;
			Point3 lightPos = viewPoint;

			// render vertex for this face.
			m_pSC->SetViewPoint(viewPoint);
			m_pSC->SetTargetPoint(viewTarget);
			m_pSC->SetViewDir(viewDir);
			
			if( m_bCastShadows )
			{
				//m_pSC->CalcShadow(*m_pInstanceList);
			}

			m_pMtl->Shade(*m_pSC);
			color.r = m_pSC->out.c.r;
			color.g = m_pSC->out.c.g;
			color.b = m_pSC->out.c.b;
			
			// Apply the tone operator
			if( (m_pSC->globContext != NULL) && (m_pSC->globContext->pToneOp != NULL) )
			{
				m_pSC->globContext->pToneOp->ScaledToRGB(color);
			}

			color *= m_shadeOptions.vScale;
			color.ClampMinMax();

			pFaceColors[y * nWidth * 3 + x * 3 + 0] = BYTE(color.b * 255);
			pFaceColors[y * nWidth * 3 + x * 3 + 1] = BYTE(color.g * 255);
			pFaceColors[y * nWidth * 3 + x * 3 + 2] = BYTE(color.r * 255);
		}
	}

	*ppFaceColors = pFaceColors;
	*pWidth = nWidth;
	*pHeight = nHeight;
	*pnVMap0 = nVMap[0];
	*pnVMap1 = nVMap[1];
	*pnVMap2 = nVMap[2];
	return true;
}

bool NodeShador::DetermineBoxShadeInfo(float vPrecision, int * pnDimension, NodeBoxShadeInfo * pInfo)
{
	float vStep = 1.0f;

	if( vPrecision > 0.0f )
		vStep = vPrecision;

	NodeBoxShadeInfo	info;
	Matrix3				tm = m_pNode->GetObjTMAfterWSM(m_time);
	
	// For the shit m_pMesh has been tranformed by the fucking SContext, so we have to 
	// convert another triobj for use
	ObjectState os = m_pNode->EvalWorldState(m_time);
	if( !os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
		return false;
	
	TriObject	*objTri;
	objTri = (TriObject *)os.obj->ConvertToType(m_time, Class_ID(TRIOBJ_CLASS_ID, 0));
	if( !objTri )
		return false;

	Box3 bb = objTri->mesh.getBoundingBox();	

	if( objTri != os.obj )
		objTri->DeleteMe();

	// First find a proper texture size for the light map of the whole bounding box
	float lx, ly, lz;
	lx = tm.VectorTransform(bb[0] - bb[1]).Length();
	ly = tm.VectorTransform(bb[0] - bb[2]).Length();
	lz = tm.VectorTransform(bb[0] - bb[4]).Length();

	float area = (lx * ly + lx * lz + ly * lz) * 2.0f;
	int nArea = area / vStep / vStep;

	int nDimension = (int) sqrt(nArea);
	int nBits = 3; // minimum is 16*16
	while( nDimension > (1 << nBits) )
	{
		nBits ++;
	}
	nDimension = 1 << nBits;
	if( nDimension > 128 )
		nDimension = 128; // maximum is 128 * 128

	info.nTexWidth	= nDimension;
	info.nTexHeight = nDimension;
	// Now we only put the six rectangle into two columns and three rows
	// First face is right axis plane
	info.x[0]		= 0;
	info.y[0]		= 0;
	info.width[0]	= nDimension / 2;
	info.height[0]	= nDimension / 3;
	info.lt[0]		= tm.PointTransform(bb[5]);
	info.r[0]		= tm.VectorTransform(bb[7] - bb[5]);
	info.d[0]		= tm.VectorTransform(bb[1] - bb[5]);
	info.normal[0]	= tm.VectorTransform(bb[5] - bb[4]).Normalize();
	info.c[0]		= tm.PointTransform((bb[5] + bb[3]) / 2.0f);

	// then left axis plane
	info.x[1]		= 0;
	info.y[1]		= nDimension / 3;
	info.width[1]	= nDimension / 2;
	info.height[1]	= nDimension / 3;
	info.lt[1]		= tm.PointTransform(bb[6]);
	info.r[1]		= tm.VectorTransform(bb[4] - bb[6]);
	info.d[1]		= tm.VectorTransform(bb[2] - bb[6]);
	info.normal[1]	= tm.VectorTransform(bb[6] - bb[7]).Normalize();
	info.c[1]		= tm.PointTransform((bb[6] + bb[0]) / 2.0f);

	// then top axis plane
	info.x[2]		= 0;
	info.y[2]		= nDimension / 3 + nDimension / 3;
	info.width[2]	= nDimension / 2;
	info.height[2]	= nDimension - info.y[2];
	info.lt[2]		= tm.PointTransform(bb[6]);
	info.r[2]		= tm.VectorTransform(bb[7] - bb[6]);
	info.d[2]		= tm.VectorTransform(bb[4] - bb[6]);
	info.normal[2]	= tm.VectorTransform(bb[6] - bb[2]).Normalize();
	info.c[2]		= tm.PointTransform((bb[6] + bb[5]) / 2.0f);

	// then bottom axis plane
	info.x[3]		= nDimension / 2;
	info.y[3]		= nDimension / 3 + nDimension / 3;
	info.width[3]	= nDimension / 2;
	info.height[3]	= nDimension - info.y[3];
	info.lt[3]		= tm.PointTransform(bb[0]);
	info.r[3]		= tm.VectorTransform(bb[1] - bb[0]);
	info.d[3]		= tm.VectorTransform(bb[2] - bb[0]);
	info.normal[3]	= tm.VectorTransform(bb[0] - bb[4]).Normalize();
	info.c[3]		= tm.PointTransform((bb[0] + bb[3]) / 2.0f);

	// then front axis plane
	info.x[4]		= nDimension / 2;
	info.y[4]		= 0;
	info.width[4]	= nDimension / 2;
	info.height[4]	= nDimension / 3;
	info.lt[4]		= tm.PointTransform(bb[7]);
	info.r[4]		= tm.VectorTransform(bb[6] - bb[7]);
	info.d[4]		= tm.VectorTransform(bb[3] - bb[7]);
	info.normal[4]	= tm.VectorTransform(bb[7] - bb[5]).Normalize();
	info.c[4]		= tm.PointTransform((bb[7] + bb[2]) / 2.0f);

	// then back axis plane
	info.x[5]		= nDimension / 2;
	info.y[5]		= nDimension / 3;
	info.width[5]	= nDimension / 2;
	info.height[5]	= nDimension / 3;
	info.lt[5]		= tm.PointTransform(bb[4]);
	info.r[5]		= tm.VectorTransform(bb[5] - bb[4]);
	info.d[5]		= tm.VectorTransform(bb[0] - bb[4]);
	info.normal[5]	= tm.VectorTransform(bb[4] - bb[6]).Normalize();
	info.c[5]		= tm.PointTransform((bb[4] + bb[1]) / 2.0f);

	* pnDimension = nDimension;
	* pInfo = info;
	return true;
}

bool NodeShador::ShadeBoundingBox(LPBYTE * ppFaceColors, float vPrecision, int * pWidth, int * pHeight, NodeBoxShadeInfo * pInfo)
{
	// Now determine the box info;
	int nDimension;
	NodeBoxShadeInfo boxInfo;

	DetermineBoxShadeInfo(vPrecision, &nDimension, &boxInfo);

	LPBYTE pFaceColors;
	pFaceColors = (LPBYTE)a_malloc(nDimension * nDimension * 3 * sizeof(BYTE));
	if( NULL == pFaceColors )
		return false;
	ZeroMemory(pFaceColors, nDimension * nDimension * 3 * sizeof(BYTE));

	bool * pIntersectFlag = (bool *)a_malloc(nDimension * nDimension * sizeof(bool));
	if( NULL == pIntersectFlag )
		return false;
	ZeroMemory(pIntersectFlag, nDimension * nDimension * sizeof(bool));

	MeshInstance* mi = GetNodeFromMeshInstance(m_pNode, *m_pInstanceList);
	if( NULL == mi )
		return false;

	int		f = -1;
	for(int k=0; k<6; k++)
	{
		Ray		r;
		HitInfo hitInfo;
		hitInfo.instance = mi;
		
		int		x, y;
		// By now we use a ray cast method to find out pixel by pixel which triangle the pixel lays on
		// but this kind of method is quite time consuming when there are many faces
		// a scan-line 'line casting' algorithm can be implemented to accelerate the rendering
		// process

		// AND maybe we can let max to do the rendering for us!
		for(y=0; y<boxInfo.height[k]; y++)
		{
			for(x=0; x<boxInfo.width[k]; x++)
			{
				int nWX, nWY, nDest;

				nWX		= boxInfo.x[k] + x;
				nWY		= boxInfo.y[k] + y;
				nDest	= nWY * nDimension * 3 + nWX * 3;

				float vX, vY;
				vX = x * 1.0f / (boxInfo.width[k] - 1);
				vY = y * 1.0f / (boxInfo.height[k] - 1);
				r.p = boxInfo.lt[k] + boxInfo.r[k] * vX + boxInfo.d[k] * vY;
				r.dir = -boxInfo.normal[k];

				if( !intersectMesh(&r, hitInfo) )
				{
					pFaceColors[nDest + 0] = 0;
					pFaceColors[nDest + 1] = 0;
					pFaceColors[nDest + 2] = 0;
					pIntersectFlag[nWY * nDimension + nWX] = false;
					continue;
				}

				pIntersectFlag[nWY * nDimension + nWX] = true;

				if( f != hitInfo.nFaceNum )
				{
					f = hitInfo.nFaceNum;
					m_pSC->SetFaceNum(f);
					Face* face = &m_pMesh->faces[f];
					m_pSC->SetMtlNum(face->getMatID());
					m_pSC->CalcNormals();
				}

				Point3 vt[3];
				vt[0] = m_pMesh->verts[m_pMesh->faces[f].v[0]];
				vt[1] = m_pMesh->verts[m_pMesh->faces[f].v[1]];
				vt[2] = m_pMesh->verts[m_pMesh->faces[f].v[2]];

				Point3 faceCenter = (vt[0] + vt[1] + vt[2]) / 3;

				Point3 n[3];
				n[0] = m_pSC->vxNormals[0];
				n[1] = m_pSC->vxNormals[1];
				n[2] = m_pSC->vxNormals[2];

				float w, u, v;
				w = hitInfo.baryCoord.x;
				u = hitInfo.baryCoord.y;
				v = hitInfo.baryCoord.z;
				m_pSC->SetBaryCoord(hitInfo.baryCoord);

				Point3 vxNormal = w * n[0] + u * n[1] + v * n[2];
				Point3 viewTarget = w * vt[0] + u * vt[1] + v * vt[2];

				// Move the view point slightly towards to center of the face to avoid floating-point precision problems
				viewTarget += (faceCenter - viewTarget) * vertexEvalEspilon;

				Point3 viewPoint = viewTarget + vertexEvalDistance * vxNormal;
				Point3 viewDir = -vxNormal;

				// render the point for this face.
				m_pSC->SetViewPoint(viewPoint);
				m_pSC->SetTargetPoint(viewTarget);
				m_pSC->SetViewDir(viewDir);
				
				if( m_bCastShadows )
				{
					//m_pSC->CalcShadow(*m_pInstanceList);
				}

				Color	color;
				m_pMtl->Shade(*m_pSC);
				color.r = m_pSC->out.c.r;
				color.g = m_pSC->out.c.g;
				color.b = m_pSC->out.c.b;
		
				// Apply the tone operator
				if( (m_pSC->globContext != NULL) && (m_pSC->globContext->pToneOp != NULL) )
				{
					m_pSC->globContext->pToneOp->ScaledToRGB(color);
				}

				color *= m_shadeOptions.vScale;
				color.ClampMinMax();

				pFaceColors[nDest + 0] = BYTE(color.b * 255);
				pFaceColors[nDest + 1] = BYTE(color.g * 255);
				pFaceColors[nDest + 2] = BYTE(color.r * 255);
			}
		}

		int nExpandCount = 3;
EXPAND:
		// Now we must get rid of the black edge effects caused by missing raytracing
		for(y=0; y<boxInfo.height[k]; y++)
		{
			for(x=0; x<boxInfo.width[k]; x++)
			{
				int nWX, nWY;
				nWX = boxInfo.x[k] + x;
				nWY = boxInfo.y[k] + y;

				if( !pIntersectFlag[nWY * nDimension + nWX] )
				{
					// find a set pixel and copy color of that to this;
					int nNX = -1, nNY = -1;
					for(int s=-1; s<=1; s++)
					{
						for(int t=-1; t<=1; t++)
						{
							if( x + t >= 0 && x + t < boxInfo.width[k] && 
								y + s >= 0 && y + s < boxInfo.height[k] )
							{
								int wy = nWY + s;
								int wx = nWX + t;
								if( pIntersectFlag[wy * nDimension + wx] )
								{
									nNX = wx;
									nNY = wy;
									break;
								}
							}
						}
					}

					if( nNX != -1 && nNY != -1 )
					{
						memcpy(&pFaceColors[nWY * nDimension * 3 + nWX * 3], &pFaceColors[nNY * nDimension * 3 + nNX * 3], sizeof(BYTE) * 3);
						pIntersectFlag[nWY * nDimension + nWX] = true;
					}
				}
			}
		}

		nExpandCount --;
		if( nExpandCount )
			goto EXPAND;
	}

	a_free(pIntersectFlag);

	*ppFaceColors = pFaceColors;
	*pWidth = nDimension;
	*pHeight = nDimension;
	*pInfo = boxInfo;
	return true;
}

bool NodeShador::ShadeVertexColors(bool bHasUV, LPBYTE * ppVertexColors, int * pVertexCount)
{
	int numVerts = bHasUV ? m_pMesh->numTVerts : m_pMesh->numVerts;
	int numFaces = m_pMesh->numFaces;
	
	LPBYTE pVertexColors;
	pVertexColors = (LPBYTE)a_malloc(numVerts * 3 * sizeof(BYTE));
	if( NULL == pVertexColors )
		return false;

	Color color, tmpColor;
	for(int i=0; i<numVerts; i++)
	{
		int f;
		int numShades = 0;
		BitArray faceList;
		faceList.SetSize(numFaces, 0);

		// First get face list that contains this vertex
		for(f=0; f<numFaces; f++)
		{
			for(int fi=0; fi<3; fi++) 
			{
				if( bHasUV )
				{
					if( m_pMesh->tvFace[f].t[fi] == i ) 
					{
						faceList.Set(f);
					}
				}
				else 
				{
					if( m_pMesh->faces[f].v[fi] == i ) 
					{
						faceList.Set(f);
					}
				}
			}
		}

		// We now have a viewpoint and a view target.
		// Now we just have to shade this point on the mesh in order
		// to get it's color.
		// Note: 
		// Since materials are assigned on Face basis we need to render each
		// vertex as many times as it has connecting faces.
		// the colors collected are mixed to get the resulting
		// color at each vertex.

		// set color to black for accumulating;
		color.Black();
		for(f=0; f<faceList.GetSize(); f++) 
		{
			if( faceList[f] )
			{
				m_pSC->SetFaceNum(f);
				Face* face = &m_pMesh->faces[f];
				m_pSC->SetMtlNum(face->getMatID());
				m_pSC->CalcNormals();

				Point3 vt[3];
				vt[0] = m_pMesh->verts[m_pMesh->faces[f].v[0]];
				vt[1] = m_pMesh->verts[m_pMesh->faces[f].v[1]];
				vt[2] = m_pMesh->verts[m_pMesh->faces[f].v[2]];

				Point3 faceCenter = (vt[0] + vt[1] + vt[2]) / 3;

				Point3 n[3];
				n[0] = m_pSC->vxNormals[0];
				n[1] = m_pSC->vxNormals[1];
				n[2] = m_pSC->vxNormals[2];
				
				int vi;
				// Setup the barycentric coordinate
				if( bHasUV )
				{
					if( m_pMesh->tvFace[f].t[0] == i )
						vi = 0;
					else if( m_pMesh->tvFace[f].t[1] == i )
						vi = 1;
					else if( m_pMesh->tvFace[f].t[2] == i )
						vi = 2;
				}
				else
				{
					if( m_pMesh->faces[f].v[0] == i )
						vi = 0;
					else if( m_pMesh->faces[f].v[1] == i )
						vi = 1;
					else if( m_pMesh->faces[f].v[2] == i )
						vi = 2;
				}

				float c[3];

				memset(c, 0, 3 * sizeof(float));
				c[vi] = 1.0f;
				
				m_pSC->SetBaryCoord(Point3(c[0], c[1], c[2]));

				Point3 vxNormal = n[vi];
				Point3 viewTarget = vt[vi];

				// Move the view point slightly towards to center of the face to avoid floating-point precision problems
				viewTarget += (faceCenter - viewTarget) * vertexEvalEspilon;

				Point3 viewPoint = viewTarget + vertexEvalDistance * vxNormal;
				Point3 viewDir = -vxNormal;
				Point3 lightPos = viewPoint;

				// render vertex for this face.
				m_pSC->SetViewPoint(viewPoint);
				m_pSC->SetTargetPoint(viewTarget);
				m_pSC->SetViewDir(viewDir);
				
				if( m_bCastShadows )
				{
					//m_pSC->CalcShadow(*m_pInstanceList);
				}

				m_pMtl->Shade(*m_pSC);
				tmpColor.r = m_pSC->out.c.r;
				tmpColor.g = m_pSC->out.c.g;
				tmpColor.b = m_pSC->out.c.b;
				
				// Apply the tone operator
				if( (m_pSC->globContext != NULL) && (m_pSC->globContext->pToneOp != NULL) )
				{
					m_pSC->globContext->pToneOp->ScaledToRGB(tmpColor);
				}

				color += tmpColor;
				numShades++;
			}
		}

		// The color mixes. We just add the colors together and 
		// then divide with as many colors as we added.
		if( numShades > 0 )
		{
			color /= (float)numShades;
		}
		   
		color *= m_shadeOptions.vScale;
		color.ClampMinMax();

		pVertexColors[i * 3 + 0] = BYTE(color.b * 255);
		pVertexColors[i * 3 + 1] = BYTE(color.g * 255);
		pVertexColors[i * 3 + 2] = BYTE(color.r * 255);
	}

	*ppVertexColors = pVertexColors;
	*pVertexCount = numVerts;
	return true;
}

bool NodeShador::AfterShade()
{
	// Do the necessary post-render stuff
	if( m_bCastShadows )
	{
		/*
		if( m_pSC ) 
		{	// this operation is confirm the lightTab will contail all created light infos
			// then the deconstructor of SContext will delete them in lightTab to avoid mem leak
			m_pSC->lightTab.ZeroCount();
			for(int i=0; i<m_pSC->allLights.Count(); i++) 
			{
				m_pSC->lightTab.Append(1, &(m_pSC->allLights[i]), 5);
			}
		}*/
	}

	m_pNode = NULL;
	m_pMesh = NULL;

	if( m_pSC )
	{
		delete m_pSC;
		m_pSC = NULL;
	}
	if( m_pMtl )
	{
		delete m_pMtl;
		m_pMtl = NULL;
	}
	if( m_pMtls )
	{
		m_pMtls->Empty();
		delete m_pMtls;
		m_pMtls = NULL;
	}

	return true;
}

/*******************************************************************/
/*  Render Instance section
/*
/*******************************************************************/
Instance::Instance(INode* node, MtlBaseLib* mtls, int nodeID)
{
	pNode	= node;
	mesh	= NULL;
	flags	= 0;

	// Get material for node
	mtl = node->GetMtl();
	if (!mtl) {
		// Node has no material, create a dummy material based on wireframe color
		// This is done so the  renderer does not have to worry about nodes
		// without materials. It will, in effect, assure that every node has
		// a valid material.
		mtl = new DumMtl(node->GetWireColor());
	}

	// Add material to our list
	mtls->AddMtl(mtl);

	this->nodeID = nodeID;
}

Instance::~Instance()
{
	FreeAll();

	// If this is a dummy material we need to delete it
	if (mtl->ClassID() == DUMMTL_CLASS_ID) {
		delete mtl;
	}
}


int Instance::Update(TimeValue t, View& vw)
{
	FreeAll();

	SetFlag(INST_RCV_SHADOWS, pNode->RcvShadows() ? 1 : 0);

	// Check visibility
	vis = pNode->GetVisibility(t);
	if (vis < 0.0f) {
		vis = 0.0f;
		SetFlag(INST_HIDE, 1);
		return 1;
	}
	if (vis > 1.0f) vis = 1.0f;
	SetFlag(INST_HIDE, 0);

	// TM's
	Interval tmValid(FOREVER);
	objToWorld = pNode->GetObjTMAfterWSM(t,&tmValid);

	// Is this node negatively scaled
	SetFlag(INST_TM_NEGPARITY, TMNegParity(objToWorld));

	// Get Object
	ObjectState os = pNode->EvalWorldState(t);
	pObject = os.obj;

	//******************************************************************
	// Mesh access.
	// In this sample renderer we retrieve the mesh for all nodes,
	// then we transform all the vertices to camera space and finally
	// we render the frame.
	// Problem: When we retrieve the mesh from instanced objects
	// we will get the same mesh for all instances.
	// As we, in this case, transform the vertices we would do multiple
	// (and invalid) transformations of the mesh for instanced objects.
	// To solve this in an easy (but expensive way) we make copies of
	// all meshes. We can get away with that, because we are
	// a sample renderer - however, you are not.
	//******************************************************************

	// Make a complete copy of the mesh...
	BOOL deleteMesh;
	mesh = new Mesh;
	Mesh* nodeMesh = ((GeomObject*)pObject)->GetRenderMesh(t, pNode, vw, deleteMesh);

	/*
	mesh->DeepCopy(nodeMesh, GEOM_CHANNEL | TOPO_CHANNEL | TEXMAP_CHANNEL |
		MTL_CHANNEL | DISP_ATTRIB_CHANNEL | TM_CHANNEL);
	*/

	*mesh = *nodeMesh;

	// If the mesh is not a part of the cache, delete it.
	if (deleteMesh) {
		delete nodeMesh;
	}

	for (int i=0; i<mesh->numFaces; i++) {
		BoundingSphere* bs = new BoundingSphere;
		faceBoundSpheres.Append(1, &bs, 25);
	}

	return 1;
}

void Instance::UpdateViewTM(Matrix3 affineTM)
{
	objToCam = objToWorld * affineTM;
	camToObj = Inverse(objToCam);
	normalObjToCam.IdentityMatrix();

	// Calculate the inverse-transpose of objToCam for transforming normals.
	for (int it=0; it<3; it++) {
		Point4 p = Inverse(objToCam).GetColumn(it);
		normalObjToCam.SetRow(it,Point3(p[0],p[1],p[2]));
	}

	normalCamToObj = Inverse(normalObjToCam);

	CalcBoundingSphere();
}

void Instance::TransformGeometry(Matrix3 pointMat, Matrix3 vecMat)
{
	/*
	if (!mesh)
		return;

	// TBD: Do I need to transform back in reverse order?
	// TBD: Do I need to build normals?

	// Transform the mesh points to camera space
	for (int nv = 0; nv < mesh->numVerts; nv++) {
		Point3 vx = pointMat * mesh->getVert(nv);
		mesh->setVert(nv, vx);
	}

	// transform the face normals to camera space
	for (int nf = 0; nf < mesh->numFaces; nf++) {
		Point3 fn = VectorTransform(vecMat, mesh->getFaceNormal(nf));
		mesh->setFaceNormal(nf, fn);
	}*/
}

void Instance::CalcBoundingSphere()
{
	// Get the boundingbox first
	obBox.Init();

	Point3 vx[3];

	// Include each vertex in the bounding box
	for (int nf = 0; nf < mesh->numFaces; nf++) {
		Face* f = &(mesh->faces[nf]);
		vx[0] = mesh->getVert(f->getVert(0));
		vx[1] = mesh->getVert(f->getVert(1));
		vx[2] = mesh->getVert(f->getVert(2));

		obBox += vx[0];
		obBox += vx[1];
		obBox += vx[2];

		// Pre-calculate bounding spheres for the faces
		BoundingSphere* bs = (BoundingSphere*)faceBoundSpheres[nf];
		bs->bsCenter = objToCam * ((vx[0] + vx[1] + vx[2]) / 3.0f);
		bs->bsRadSq = 0.0f;
		Point3 d;
		float nr = 0.0f;
		for (int i=0; i<3; i++) {
			d = objToCam * vx[i] - bs->bsCenter;
			nr = DotProd(d,d);
			if (nr>bs->bsRadSq) bs->bsRadSq = nr;
		}
	}

	camBox = obBox * objToCam;

	// Now get the bounding sphere forthe object itself
	center = 0.5f*(objToCam*obBox.pmin+objToCam*obBox.pmax);
	radsq = 0.0f;
	for (int i= 0; i<mesh->numVerts; i++) {
		Point3 d = objToCam*mesh->verts[i] - center;
		float nr = DotProd(d,d);
		if (nr>radsq) radsq = nr;		
	}
}

void Instance::FreeAll()
{
	if (mesh) {
		delete mesh;
		mesh = NULL;
	}

	for (int i=0; i<faceBoundSpheres.Count(); i++) {
		delete faceBoundSpheres[i];
	}
	faceBoundSpheres.ZeroCount();
	faceBoundSpheres.Shrink();
}

RenderInstance* Instance::Next()
{
	return next;
}

Interval Instance::MeshValidity()
{
	return FOREVER;
}

int Instance::NumLights()
{
	return 0;
}

LightDesc* Instance::Light(int n)
{
	return NULL;
}

int Instance::NumShadLights()
{
	return NULL;
}

LightDesc* Instance::ShadLight(int n)
{
	return NULL;
}

INode* Instance::GetINode()
{
	return pNode;
}

Object* Instance::GetEvalObject()
{
	return pObject;
}

unsigned long Instance::MtlRequirements(int mtlNum, int faceNum)
{
	return mtl->Requirements(mtlNum);
}

Mtl* Instance::GetMtl(int faceNum)
{
	return NULL;
}

Point3 Instance::GetFaceNormal(int faceNum)
{
	return Point3(0,0,0);
}

Point3 Instance::GetFaceVertNormal(int faceNum, int vertNum)
{
	return Point3(0,0,0);
}

void Instance::GetFaceVertNormals(int faceNum, Point3 n[3])
{
}

Point3 Instance::GetCamVert(int vertNum)
{
	return Point3(0,0,0);
}


void Instance::GetObjVerts(int fnum, Point3 obp[3])
{
	Face* f = &mesh->faces[fnum];
	obp[0] = mesh->verts[f->v[0]];
	obp[1] = mesh->verts[f->v[1]];
	obp[2] = mesh->verts[f->v[2]];
}

void Instance::GetCamVerts(int fnum, Point3 cp[3])
{
	Face* f = &mesh->faces[fnum];
	cp[0] = objToCam*mesh->verts[f->v[0]];
	cp[1] = objToCam*mesh->verts[f->v[1]];
	cp[2] = objToCam*mesh->verts[f->v[2]];
}

TCHAR* Instance::GetName()
{
	return pNode->GetName();
}

int Instance::CastsShadowsFrom(const ObjLightDesc& lt)
{
	// Can this take some time?
	// we should get these cast shadow flag when this instance initialize
	ObjectState os = lt.inode->EvalWorldState(GetCOREInterface()->GetTime());
	assert(os.obj->SuperClassID()==LIGHT_CLASS_ID);
	LightObject* light = (LightObject *)os.obj;  

	ExclList* nt = light->GetExclList();
	if( nt && nt->TestFlag(NT_AFFECT_SHADOWCAST) ) 
	{
		INode * recvNode = pNode;
		bool bUseLight = light->Include() ? false : true;
		while( true )
		{
			if( light->Include() ) 
			{
				// Inclusion list
				if (nt->FindNode(recvNode) != -1) 
				{
					// If the list has specified this node, just use it;
					bUseLight = true;
					break;
				}
			}
			else 
			{
				if (nt->FindNode(recvNode) != -1) 
				{
					// If the list has specified this node, just use it;
					bUseLight = false;
					break;
				}
			}

			if( !recvNode->IsGroupMember() || recvNode->IsRootNode() )
				break;
			recvNode = recvNode->GetParentNode();
		}

		if( bUseLight )
			return TRUE;
		else
			return FALSE;
	}

	return TRUE;
}

//===========================================================================
//
// Class NodeRenderGlobalContext : public RenderGlobalContext
//
//===========================================================================

//***************************************************************************
// Initialize our custom options.
//***************************************************************************

NodeRenderGlobalContext::NodeRenderGlobalContext()
{
	SetDefaults();

	envMap = NULL;
	atmos = NULL;
	rendType = RENDTYPE_NORMAL;
	nMinx = 0;
	nMiny = 0;
	nMaxx = 0;
	nMaxy = 0;
	nNumDefLights = 0;
	nRegxmin = 0;
	nRegxmax = 0;
	nRegymin = 0;
	nRegymax = 0;
	scrDUV = Point2(0.0f, 0.0f);
	pDefaultLights = NULL;
	pFrp = NULL;
	bVideoColorCheck = 0;
	bForce2Sided = FALSE;
	bRenderHidden = FALSE;
	bSuperBlack = FALSE;
	bRenderFields = FALSE;
	bNetRender = FALSE;

	renderer = NULL;
	projType = PROJ_PERSPECTIVE;
	devWidth = 0;
	devHeight = 0;
	xscale = 0;
	yscale = 0;
	xc = 0;
	yc = 0;
	antialias = FALSE;
	nearRange = 0;
	farRange = 500.0f;
	devAspect = 1.0f;
	frameDur = 1.0f;
	time = 0;
	wireMode = FALSE;
	wire_thick = 1.0f;
	force2Side = FALSE;
	inMtlEdit = FALSE;
	fieldRender = FALSE;
	first_field = FALSE;
	field_order = FALSE;
	objMotBlur = FALSE;
	nBlurFrames = 0;

	pToneOp = NULL;
}

void NodeRenderGlobalContext::SetDefaults()
{
	ilist = NULL;
	instTab.ZeroCount();
	instTab.Shrink();

	mtls.ZeroCount();
	mtls.Shrink();

	nCurNodeID = 0;
	nMaxDepth = 0;
	bReflectEnv = FALSE;
}

//***************************************************************************
// These values can be assumed to be correct.
// See the SDK help for class ViewParams for an explanation.
//***************************************************************************

#define VIEW_DEFAULT_WIDTH ((float)400.0)

void NodeRenderGlobalContext::ComputeViewParams(const ViewParams&vp)
{
	viewParams = vp;
	worldToCam = vp.affineTM;
	camToWorld = Inverse(worldToCam);

	xc = devWidth / 2.0f;
	yc = devHeight / 2.0f;

	scrDUV.x = 1.0f/(float)devWidth;
	scrDUV.y = 1.0f/(float)devHeight;

	projType = vp.projType;

	if (projType == PROJ_PERSPECTIVE) {
		float fac =  -(float)(1.0 / tan(0.5*(double)vp.fov));
		xscale =  fac*xc;
		yscale = -devAspect*xscale;
	}
	else {
		xscale = (float)devWidth/(VIEW_DEFAULT_WIDTH*vp.zoom);
		yscale = -devAspect*xscale;
	}

	// TBD: Do Blowup calculation here.
}

//***************************************************************************
// Calculate the direction of a ray going through pixels sx, sy
//***************************************************************************

Point3 NodeRenderGlobalContext::RayDirection(float sx, float sy)
{
	Point3 p;
	p.x = -(sx-xc)/xscale; 
	p.y = -(sy-yc)/yscale; 
	p.z = -1.0f;
	return Normalize(p);
}

//***************************************************************************
// Render Instances (from RenderGlobalContext)
//***************************************************************************

int NodeRenderGlobalContext::NumRenderInstances()
{
	return instTab.Count();
}

RenderInstance* NodeRenderGlobalContext::GetRenderInstance(int i)
{
	if (i<NumRenderInstances()) {
		return instTab[i];
	}

	return NULL;
}

bool NodeRenderGlobalContext::AddInstance(INode * pNode)
{
	nCurNodeID++;
	Instance* pInst = new Instance(pNode, &mtls, nCurNodeID);

	pInst->next = ilist;
	ilist = pInst;
	// TBD - initialize stuff here

	instTab.Append(1, &pInst, 25);

	return true;
}

NodeRenderGlobalContext::~NodeRenderGlobalContext()
{
	int idx;
	for(idx=0; idx<instTab.Count(); idx++) 
	{
		delete instTab[idx];
	}
	instTab.ZeroCount();
	instTab.Shrink();

	mtls.ZeroCount();
	mtls.Shrink();
}

void NodeRenderGlobalContext::NodeEnum(INode* node)
{
	// For each child of this node, we recurse into ourselves 
	// until no more children are found.
	for (int c = 0; c < node->NumberOfChildren(); c++) {
		NodeEnum(node->GetChildNode(c));
	}

	// Is the node hidden?
	BOOL nodeHidden = node->IsNodeHidden(TRUE);
	
	// Get the ObjectState.
	// The ObjectState is the structure that flows up the pipeline.
	// It contains a matrix, a material index, some flags for channels,
	// and a pointer to the object in the pipeline.
	ObjectState ostate = node->EvalWorldState(0);
	if (ostate.obj==NULL) 
		return;

	// Examine the superclass ID in order to figure out what kind
	// of object we are dealing with.
	switch (ostate.obj->SuperClassID()) {

		// It's a light.
		case LIGHT_CLASS_ID: 
			{ 
			}
			break;
		case SHAPE_CLASS_ID:	// To support renderable shapes
		case GEOMOBJECT_CLASS_ID: {
			// This is an object in the scene

			// If we are not rendering hidden objects, return now
			if (nodeHidden)
				return;

			// If this object cannot render, skip it
			if (!ostate.obj->IsRenderable()) 
				return;
			if (!node->Renderable()) 
				return;

			// Handle motion blur etc...
			
			// Add the node to our list
			AddInstance(node);
			break;
		}
	}
}
