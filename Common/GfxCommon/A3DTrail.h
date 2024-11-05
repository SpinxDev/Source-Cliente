/*
 * FILE: A3DTrail.h
 *
 * DESCRIPTION: 3D Trail
 *
 * CREATED BY: ZhangYu, 2004/7/15
 *
 * HISTORY:
 *
 */

#ifndef _A3DTRAIL_H_
#define _A3DTRAIL_H_

#include "A3DGFXElement.h"
#include "A3DGFXEditorInterface.h"

#define		SEG_SIZE_DEFAULT	256

class ITrailList;
class A3DTrail : public A3DGFXElement
{
public:
	enum TRAILMODE
	{
		LINE_MODE		= 0,		//	Old Line Mode
		SPLINE_MODE		= 1,		//	Spline Mode
		LINE_KP_MODE	= 2,		//	Key Point Line Mode (a keypoint a key vertex)
	};

public:

	explicit A3DTrail(A3DGFXEx* pGfx);
	virtual ~A3DTrail();

protected:
	//RotList<TRAIL_DATA> m_TrailLst;
	A3DVECTOR3		m_vOrgPos[2];	// start point
	bool			m_bBind;
	int				m_nSegLife;
	A3DQUATERNION	m_quLastDir;
	A3DVECTOR3		m_vLastPos;
	bool			m_bFirstSeg;
	
	//	Spline mode means m_pTrailImpl = new SplineTrailList
	//	Otherwise, m_pTrailImpl = new LineTrailList
	//bool			m_bSplineMode;
	int				m_iSplineMode;
	ITrailList*		m_pTrailImpl;

	// param settings
	bool			m_bOrgPos1Enable;
	bool			m_bOrgPos2Enable;
	bool			m_bMatrixEnable;
	bool			m_bOrgPos1Set;
	bool			m_bOrgPos2Set;
	bool			m_bOrgMatSet;
	bool			m_bHostMatSet;
	DWORD			m_dwDeltaTm;
	A3DMATRIX4		m_matHost;
	bool			m_bAddOutData;
	int				m_iCurActKeyPoint;

protected:
	
	bool IsSplineMode() const { return m_iSplineMode == SPLINE_MODE; }
	void AddTrailSeg(
		const A3DMATRIX4& matTran,
		DWORD dwTickTime,
		A3DCOLOR cl = _white_diffuse,
		float fScale = 1.f);
	bool InnerTick(DWORD dwTickTime);
	void RemoveOldData(DWORD dwTickTime);

	// Override base class' interface
	virtual void ResumeLoop();
	// Space wrap support
	virtual int DrawToBuffer(A3DViewport* pView, A3DTLWARPVERTEX* pVerts, int nMaxVerts, float rw, float rh);
	virtual int DrawToBuffer(A3DViewport* pView, A3DWARPVERTEX* pVerts, int nMaxVerts);

protected:
	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual bool Play();
	virtual bool Render(A3DViewport*);
	virtual int GetVertsCount();
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView);	
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
	virtual bool IsParamEnable(int nParamId) const;
	virtual void EnableParam(int nParamId, bool bEnable);
	virtual bool NeedUpdateParam(int nParamId) const;
	virtual void UpdateParam(int nParamId, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetParam(int nParamId) const;
	virtual GfxValueType GetParamType(int nParamId) const;

public:
	bool Init(A3DDevice* pDevice);
	A3DTrail& operator = (const A3DTrail& src);
	void SetOriginPos(const A3DVECTOR3& v1, const A3DVECTOR3& v2)
	{
		m_vOrgPos[0] = v1;
		m_vOrgPos[1] = v2;
	}
	void SetSegLife(int nSegLife) { m_nSegLife = nSegLife; }
	virtual void DummyTick(DWORD dwTick);
};

#endif
