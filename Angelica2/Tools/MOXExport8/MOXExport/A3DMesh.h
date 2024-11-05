#include <Windows.h>

#include "A3DObject.h"
#include "A3DVertex.h"
#include "A3DMaterial.h"
#include "A3DTexture.h"

#ifndef _A3DMESH_H_
#define _A3DMESH_H_

typedef class A3DBox * PA3DBox;
class A3DMesh : public A3DObject
{
public:
	IDirect3DVertexBuffer8 * m_pVertexBuffer;
	IDirect3DIndexBuffer8 *	 m_pIndexBuffer;

	bool			m_bWire;
	A3DVERTEX **	m_ppVertsBuffer;
	WORD *			m_pIndices;
	int				m_nFrameCount;
	int				m_nFrame;
	int				m_nVertCount;
	int				m_nIndexCount;
	A3DTexture 		m_Texture;
	A3DTexture 		m_DetailTexture;
	A3DMaterial 	m_Material;

	A3DOBB *		m_pOBBs;
	A3DBox *		m_pBoxOBB;

	bool			m_bHasLOD;
	FLOAT			m_vLODMinDis;		//	The minimum distance that this mesh begin to collapse vertex;
	FLOAT			m_vLODMaxDis;		//  The maximum distance that this mesh will reach its iLODLimit vert
	A3DMESHPROP		m_Property;

	IDirect3DDevice8 * m_pDevice;

	char			m_szDetailTexture[MAX_PATH];
	A3DMATRIX4		m_matDetailTM;

	A3DMesh();
	~A3DMesh();

	bool Init(IDirect3DDevice8 * pDevice, int nFrameCount, int nVertCount, int nIndexCount, bool bWire=false);

	bool CleanMesh();

	bool SetMaterial(A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShiness, bool b2Sided=false);
	bool SetTexture(char * szTextureName);
	bool SetDetailTexture(char * szDetailTextureName, A3DMATRIX4 matDetailTM);
	bool SetIndices(WORD * pIndices, int nIndexCount);
	bool SetVerts(int nFrame, A3DVERTEX * pVerts, int nVertCount);
	bool AddMoreVertex(A3DVERTEX * pVerts, int nVertCount,	WORD * pIndices, int nIndexCount);
	bool Release();
	
	inline bool GetWireFrame() { return m_bWire; }
	inline A3DMaterial * GetMaterial() { return &m_Material; }
	inline A3DTexture * GetTexture() { return &m_Texture; }
	inline int GetVertexCount() { return m_nVertCount; }
	inline int GetIndexCount() { return m_nIndexCount; }
	inline void SetProperty(const A3DMESHPROP& Prop) { m_Property = Prop; }
	inline const A3DMESHPROP& GetProperty() { return m_Property; }
	inline bool Is2Sided() { return m_Property.Get2SidesFlag(); }
	inline bool CanShootThrough() { return m_Property.GetShootThroughFlag(); }
	inline bool CanPickThrough() { return m_Property.GetPickThroughFlag(); }

	bool Render();
	bool TickAnimation();

	bool BuildOBBs();
	bool CreateVertexBuffer(int nVerts, IDirect3DVertexBuffer8 ** ppVertexBuffer);
	bool CreateIndexBuffer(int nIndices, IDirect3DIndexBuffer8 **ppIndexBuffer);
	bool UpdateVertexBuffer();
	bool UpdateIndexBuffer();

	bool Save(A3DFile * pFileToSave);
	bool Load(IDirect3DDevice8 * pDevice, A3DFile * pFileToLoad);

	WORD*		m_pMapTable;		//	Index map tab used to do LOD
	WORD*		m_pRDIndices;		//	Array used to store indices passed to rendering
	int			m_iLODLimit;		//	The minimum number of vertex should be rendered
	
protected:

	//	Loading function for version 2 mesh's property
	bool LoadMeshPropV2(A3DFile* pFile, bool bBinary);
	//	Saving function for version 2 mesh's property
	bool SaveMeshPropV2(A3DFile* pFile, bool bBinary);
};

typedef A3DMesh * PA3DMesh;
#endif