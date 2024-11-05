#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include "shadgen.h"

class BoundingSphere {
public:
	void	Set(Point3 center, float radsq) { bsCenter = center; bsRadSq = radsq; }

	Point3	bsCenter;
	float	bsRadSq;
};

typedef Tab<BoundingSphere*> BoundingSphereTab;

class Instance : public RenderInstance {
public:

	Instance(INode* node, MtlBaseLib* mtls, int nodeID);
	~Instance();

	// Methods inherited from RenderInstance
	RenderInstance*	Next();
	Interval	MeshValidity();
	int			NumLights();
	LightDesc*	Light(int n);
	int			NumShadLights();
	LightDesc*	ShadLight(int n);
	INode*		GetINode();
	Object*		GetEvalObject();
	unsigned long	MtlRequirements(int mtlNum, int faceNum);
	Point3		GetFaceNormal(int faceNum);
	Point3		GetFaceVertNormal(int faceNum, int vertNum);
	void		GetFaceVertNormals(int faceNum, Point3 n[3]);
	Point3		GetCamVert(int vertNum);
	void		GetObjVerts(int fnum, Point3 obp[3]);
	void		GetCamVerts(int fnum, Point3 cp[3]);

	// New for R3
	int			CastsShadowsFrom(const ObjLightDesc& lt);

	// Access methods
	TCHAR*		GetName();

	int			Update(TimeValue t, View& gview);
	void		UpdateViewTM(Matrix3 affineTM);
	void		TransformGeometry(Matrix3 pointMat, Matrix3 vecMat);
	void		CalcBoundingSphere();
	void		FreeAll();

	Mtl*		GetMtl(int faceNum);

	Matrix3		normalCamToObj;
	INode*		pNode;
	Object*		pObject;
	//Matrix3		objToWorld;
	Box3		camBox;
	Instance*	next;
	BoundingSphereTab	faceBoundSpheres;
};

class NodeRenderGlobalContext : public RenderGlobalContext {
public:
	RendType	rendType;				// View, blowup, region etc.
	int			nMinx;
	int			nMiny;
	int			nMaxx;
	int			nMaxy;
	int			nNumDefLights;			// The default lights passed into the renderer
	int			nRegxmin;				// Coords for render blowup etc.
	int			nRegxmax;				// Coords for render blowup etc.
	int			nRegymin;				// Coords for render blowup etc.
	int			nRegymax;				// Coords for render blowup etc.
	Point2		scrDUV;
	BitArray	gbufChan;				// The G buffer channels (bitflags)
	DefaultLight*	pDefaultLights;
	FrameRendParams*	pFrp;			// Frame specific members
	RendProgressCallback*	progCallback;	// Render progress callback
	ViewParams	viewParams;

	GBufReader*	gbufReader;
	GBufWriter*	gbufWriter;

	// Custom options
	// These options are specific to the sample renderer
	int			nMaxDepth;
	int			nAntiAliasLevel;
	BOOL		bReflectEnv;

	// Standard options
	// These options are configurable for all plugin renderers
	BOOL		bVideoColorCheck;
	BOOL		bForce2Sided;
	BOOL		bRenderHidden;
	BOOL		bSuperBlack;
	BOOL		bRenderFields;
	BOOL		bNetRender;

	// Render effects
	Effect*		effect;

	NodeRenderGlobalContext();
	~NodeRenderGlobalContext();
	void		SetDefaults();
	void		ComputeViewParams(const ViewParams&vp);
	Point3		RayDirection(float sx, float sy);

	virtual void IntersectRay(RenderInstance *inst, Ray& ray, ISect &isct, ISectList &xpList, BOOL findExit) 
	{
	}
	virtual BOOL IntersectWorld(Ray &ray, int skipID, ISect &hit, ISectList &xplist, int blurFrame = NO_MOTBLUR) 
	{ 
		return FALSE; 
	}

	// added by Hedi for shadowing in nodeshador
	Instance*			ilist;
	MtlBaseLib			mtls;
	Tab<Instance *>		instTab;		// Keep track of instances
	int					nCurNodeID;
	bool				AddInstance(INode * pNode);
	void				NodeEnum(INode* node);

	ViewParams *	GetViewParams() 
	{
		return &viewParams; 
	}
	int				NumRenderInstances();
	RenderInstance*	GetRenderInstance(int i);
};

class NullView : public View 
{
public:
	Point2 ViewToScreen(Point3 p)
	{ return Point2(p.x,p.y); }

	NullView()
	{
		worldToView.IdentityMatrix();
		screenW=640.0f; screenH = 480.0f;
	}
};



#endif//_INSTANCE_H_
