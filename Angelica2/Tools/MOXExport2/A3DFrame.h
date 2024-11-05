#include <Windows.h>
#include <d3d8.h>
#include "A3DObject.h"
#include "A3DTypes.h"
#include "A3DMesh.h"
#include "A3DBox.h"
#include "AList.h"

#ifndef _A3DFRAME_H_
#define _A3DFRAME_H_

#define MAX_FRAME	30000

class A3DFrame : public A3DObject
{
protected:
	IDirect3DDevice8 *  m_pDevice;
	A3DFrame *			m_pParent;

	AList				m_ChildList;
	AList				m_MeshList;
	
	ALISTELEMENT *		m_pLastFrameElement;
	ALISTELEMENT *		m_pLastMeshElement;

	A3DMATRIX4 *		m_pRelativeTM;
	A3DMATRIX4 *		m_pAbsoluteTM;
	int					m_nFrameCount;
	int					m_nFrame;

	int					m_nBoundingBoxNum;
	A3DFRAMEOBB *		m_pBoundingBox;
	A3DBox **			m_ppA3DBox;

public:
	A3DFrame(); 
	~A3DFrame();
	
	A3DFrame * GetFirstChildFrame();
	A3DFrame * GetNextChildFrame();
	A3DMesh * GetFirstMesh();
	A3DMesh * GetNextMesh();

	inline int GetNumChilds() { return m_ChildList.GetSize(); }
	inline int GetNumMeshes() { return m_MeshList.GetSize(); }
	inline int GetBoundingBoxNum() { return m_nBoundingBoxNum; }
	inline A3DFrame * GetParent() { return m_pParent; }
	inline void SetParent(A3DFrame * pParent) { m_pParent = pParent; }
	A3DMATRIX4 GetAbsoluteTM(int nFrame);
	A3DMATRIX4 GetRelativeTM(int nFrame);
	A3DMATRIX4 GetAbsoluteTM();
	A3DMATRIX4 GetRelativeTM();

	bool FindChildByName(char * szChildName, A3DFrame ** ppFrame);

	bool SetTM(A3DMATRIX4 TM, bool bRelative=true, int nFrame=0);
	bool SetPos(A3DFrame * pRefFrame, A3DVECTOR3 vecPos, int nFrame=0);
	bool SetOrientation(A3DFrame * pRefFrame, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp, int nFrame=0);
	bool AddRotation(A3DVECTOR3 vecAxis, FLOAT fDegree, int nFrame=0);
	bool AddTranslation(A3DVECTOR3 vecOffset, int nFrame=0);
	bool Scale(FLOAT fScale, int nFrame=0);
	A3DVECTOR3 Transform(A3DVECTOR3 vec, int nFrame=0);
	A3DVECTOR3 InverseTransform(A3DVECTOR3 vec, int nFrame=0);

	bool GetPolygonInfo(int * pMeshCount, int * pVertCount, int * pIndexCount, int * pFrameCount);

	virtual bool Init(IDirect3DDevice8 * pDevice, int nFrameCount=1);
	virtual bool Release();
	virtual bool Render();
	virtual bool TickAnimation();

	bool AddChild(A3DFrame * pChild);
	bool AddMesh(A3DMesh * pMesh);
	bool AddBoundingBox(const A3DFRAMEOBB& FrameOBB);

	bool Save(A3DFile * pFileToSave);
	bool Load(IDirect3DDevice8 * pDevice, A3DFile * pFileToLoad);
	
protected:

	//	Load version 2 frame obb
	bool LoadFrameOBBV2(int iBoxNum, A3DFile* pFile, bool bBinary);
	//	Save version 2 frame obb
	bool SaveFrameOBBV2(A3DFile* pFile, bool bBinary);
};

typedef A3DFrame * PA3DFrame;

#endif
