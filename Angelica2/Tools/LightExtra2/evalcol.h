/**********************************************************************
 *<
	FILE:			EvalCpolor.cpp
	DESCRIPTION:	Vertex Color Renderer
	CREATED BY:		Christer Janson
	HISTORY:		Created Monday, December 12, 1996

 *>	Copyright (c) 1997 Kinetix, All Rights Reserved.
 **********************************************************************/
//
// Description:
// These functions calculates the diffuse or ambient color at each vertex
// or face of an INode.
//
// Exports:
// BOOL calcMixedVertexColors(INode*, TimeValue, int, ColorTab&, EvalColProgressCallback* callb = NULL);
//      This function calculates the interpolated diffuse or ambient 
//      color at each vertex of an INode.
//      Usage: Pass in a node pointer and the TimeValue to generate
//      a list of Colors corresponding to each vertex in the mesh
//      Use the int flag to specify if you want to have diffuse or 
//      ambient colors, or if you want to use the lights in the scene.
//      Note: 
//        You are responsible for deleting the Color objects in the table.
//      Additional note:
//        Since materials are assigned by face, this function renders each
//        face connected to the specific vertex (at the point of the vertex)
//        and then mixes the colors.
//
//***************************************************************************

#include "instance.h"
#include <A3D.h>

#define LIGHT_AMBIENT		0x00
#define LIGHT_DIFFUSE		0x01
#define LIGHT_SCENELIGHT	0x02

typedef Tab<Color*> ColorTab;

class FaceColor {
public:
	Color colors[3];
	};

typedef Tab<FaceColor*> FaceColorTab;
typedef BOOL (*EVALCOL_PROGRESS)(float);

class EvalColProgressCallback {
public:
	virtual BOOL progress(float prog) = 0;
};

BOOL calcMixedVertexColors(INode* node, TimeValue t, int lightModel, BOOL castShadows, BOOL useMaps, ColorTab& vxColTab, EvalColProgressCallback* callb = NULL);
BOOL calcFaceColors(INode* node, TimeValue t, int lightModel, BOOL castShadows, BOOL useMaps, FaceColorTab& faceColTab, EvalColProgressCallback* callb = NULL);

struct NodeShadorOptions
{
	bool		bUseMaps;		// true if we use texture mapping when we are not using lightonly
	bool		bCastShadows;	// true if we need cast raytraced shadows
	bool		bDefaultLights;	// true if we want to add 2 default lights when there are no lights
	bool		bSmoothGrid;	// true if we need to sample 9 point at each light grid point
	float		vScale;			// scale factor for colors, we will multiply this factor after shading
};

struct NodeBoxShadeInfo
{
	int		nTexWidth;	// width of texture image
	int		nTexHeight;	// height of texture image
	int		x[6];		// top-left corner position of each box face
	int		y[6];	
	int		width[6];	// width and height of each box face
	int		height[6]; 
	Point3 	normal[6];	// box face's normal
	Point3	lt[6];		// left-top position
	Point3	r[6];		// right axis
	Point3	d[6];		// down axis  
	Point3  c[6];		// center of each rectangle
};

class MeshInstance;
class LightDescInstance;
typedef Tab<MeshInstance*> MeshInstanceTab;
typedef Tab<LightDescInstance*> LightDescInstanceTab;
class SContext;

class LightDescInstance
{
public:
	INode			* pNode;
	LightObject		* pLightObject;
	ObjLightDesc	* pObjLightDesc;

	LightDescInstance(INode* node, MtlBaseLib* mtls);
	~LightDescInstance();
};

/*******************************************************************************
*	Class used to shade a node's all faces								       *
*																               *
*	Created by Hedi for Angelica 3D light maps generating routine by using Max *
********************************************************************************/
class NodeShador
{
public:
	NodeShador(NodeShadorOptions shadeOptions);

	~NodeShador();	

	// Global init section, done in constructor;
	NodeShadorOptions		m_shadeOptions;
	bool					m_bCastShadows;
	NodeRenderGlobalContext	m_NodeRenderGlobalContext;
	MeshInstanceTab			* m_pInstanceList; // we use pointer because we should always call the destructor of these objects to release resource
	LightDescInstanceTab	* m_pLightList;

	// Node related section, init in BeginShade();
	SContext				* m_pSC;
	Mtl						* m_pMtl;
	MtlBaseLib				* m_pMtls;
	MtlBaseLib				* m_pLightMtls;

	Mesh					* m_pMesh;
	INode					* m_pNode;
	
	RendParams				m_RendParams;
	TimeValue				m_time;

	int						m_nFaceCount;

	void enumLights(INode * node, MtlBaseLib* mtls);
	void AddShadeLights(INode * pRecvNode, SContext * sc, MtlBaseLib * mtls, int * numLight);
	void ReleaseLights();

protected:
	bool DetermineBoxShadeInfo(float vPrecision, int * pnDimension, NodeBoxShadeInfo * pInfo);

public:
	// Methods;
	// method called to begin the shade procedure
	bool BeginShade(INode * pNode, TimeValue t);

	// Get a point's light sample
	bool ShadeLightGrid(Point3& minPos, Point3& maxPos, float vGridSize, float vGridDim, A3DIBLLightGrid * pIBLLightGrid);

	// Shade one triangle;
	bool ShadeTriangle(int nFaceIndex, LPBYTE * ppFaceColors, float vPrecision, int * pWidth, int * pHeight, int * pnVMap0, int * pnVMap1, int * pnVMap2);

	// Shade into triangle;
	bool ShadeIntoTriangle(int nFaceIndex, LPBYTE * ppFaceColors, int nWidth, int nHeight, int nVMap0, int nVMap1, int nVMap2);

	// Shade the node's bounding box, there will be six shade rectangles, they will be put into one texture and pInfo will contain the information;
	bool ShadeBoundingBox(LPBYTE * ppFaceColors, float vPrecision, int * pWidth, int * pHeight, NodeBoxShadeInfo * pInfo);

	// Shade entire mesh into one line image, one pixel for each vertex;
	bool ShadeVertexColors(bool bHasUV, LPBYTE * ppVertexColors, int * pVertexCount);

	// methods called after the shade
	bool AfterShade();

public:
	inline int GetFaceCount() { return m_nFaceCount; }

	inline bool SetOptions(const NodeShadorOptions& options);
};

bool NodeShador::SetOptions(const NodeShadorOptions& options)
{
	m_shadeOptions = options;
	return true;
}