/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXDEBUGRENDERER_H_
#define _APHYSXDEBUGRENDERER_H_

class APhysXEngine;
class APhysXCCBase;

class APHYSX_DLL_EXPORT APhysXDebugRenderer
{

public:
	
	enum APhysXDebugRenderMode
	{
		APX_DRMODE_WIREFRAME,
		APX_DRMODE_SOLID,
	};

public:

	APhysXDebugRenderer()
	{
		m_DebugRenderMode = APX_DRMODE_WIREFRAME;
		m_pA3DEngine = NULL;
		m_pFont = NULL;
		m_iFontHeight = 0;

		SetTextStartXY();
		PreRender();
	}

	void Init(APhysXEngine* pPhysXEngine);

	void RenderData(const NxDebugRenderable* pDRData);
	void RenderForceField(NxForceField& ff, DWORD dwColor = 0xff5fff5f);
	void RenderCC(const APhysXCCBase& cc, DWORD dwColor);

	void SetDebugRenderMode(APhysXDebugRenderMode DebugRenderMode)
	{
		m_DebugRenderMode = DebugRenderMode;
	}

	void RenderPerfItem(const APhysXPerfItem* pPerfItem, DWORD dwColor);
	void RenderPerfStats(const APhysXPerfStats* pPerfStats, DWORD dwColor = 0xffffff00);

	void RenderLineText(const ACHAR* szOutput, DWORD dwColor);

	void RenderSceneStats(NxScene* pNxScene, DWORD dwColor = 0xffff0000);
	
	void RenderPhysXSDKStats(NxPhysicsSDK* pSDK, DWORD dwColor = 0xff00ff00);

	void SetTextStartXY(int iX = 10, int iY = 10)
	{
		m_iTextStartX = iX;
		m_iTextStartY = iY;
	}

	int GetCurTextStartX() const { return m_iTextCurX; }
	int GetCurTextStartY() const { return m_iTextCurY; }

	void PreRender();	// called before render
	
	// if we have some extra data for debug rendering, add them to this function
	void RenderExtraDebugData(APhysXScene* pScene);

private:
	void RenderForceFieldShape(NxForceFieldShape& ffs, DWORD dwColor, NxMat34* pPose = 0);


protected:
	// ....
	A3DEngine* m_pA3DEngine;
	APhysXDebugRenderMode m_DebugRenderMode;

	int m_iTextStartX;
	int m_iTextStartY;

	int m_iTextCurX;
	int m_iTextCurY;

	int m_iFontHeight;

	A3DFont* m_pFont;

};

class APHYSX_DLL_EXPORT APhysXDrawBasic
{
public:
	enum DrawFlag
	{
		DF_DRAW_WHOLE_SPHERE		= (1<<0),
		DF_DRAW_TOP_SEMISPHERE		= (1<<1),
		DF_DRAW_BOTTOM_SEMISPHERE	= (1<<2),
		DF_DRAW_DIM_XOY				= (1<<3),
		DF_DRAW_DIM_XOZ				= (1<<4),
		DF_DRAW_DIM_ZOY				= (1<<5),
		DF_DRAW_DIM_ALL				= DF_DRAW_DIM_XOY | DF_DRAW_DIM_XOZ | DF_DRAW_DIM_ZOY,
		DF_NODRAW_DIM_XOY			= DF_DRAW_DIM_XOZ | DF_DRAW_DIM_ZOY,
		DF_NODRAW_DIM_XOZ			= DF_DRAW_DIM_XOY | DF_DRAW_DIM_ZOY,
		DF_NODRAW_DIM_ZOY			= DF_DRAW_DIM_XOY | DF_DRAW_DIM_XOZ,
	};

public:
	static void DrawBox(A3DWireCollector& a3dwc, const NxBox& box, NxU32 color);
	static void DrawSphere(A3DWireCollector& a3dwc, float radius, NxU32 color, int flags = 0xffffffff, const NxMat34* pPose = 0);
	static void DrawCapsule(A3DWireCollector& a3dwc, float radius, float height, NxU32 color, int flags = 0xffffffff, const NxMat34* pPose = 0);
	static void DrawConvex(A3DWireCollector& a3dwc, const NxConvexMeshDesc& meshDesc, NxU32 color, const NxMat34* pPose = 0);
	static void DrawTriangle(A3DWireCollector& a3dwc, const NxTriangleMeshDesc& meshDesc, NxU32 color, const NxMat34* pPose = 0);

private:
	static void InitSinCos();
	static void GenerateVertices(const void* points, const NxU32 pointStrideBytes, const NxU32 numVertices, APhysXArray<A3DVECTOR3>& outvtxArray, const NxMat34* pMat = 0);
	static void GenerateIndices(const void* triangles, const NxU32 triangleStrideBytes, const NxU32 numTriangles, APhysXArray<WORD>& outvtxArray, const bool is16BitIndices = false);
	
private:
	#define nSegmentInSemiCircle 8
	#define nAllSegment  (nSegmentInSemiCircle * 2)
	
	static float cosVal[nAllSegment];
	static float sinVal[nAllSegment];
};

#endif