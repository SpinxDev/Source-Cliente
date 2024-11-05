#include "A3DBox.h"
#include "A3DFuncs.h"

A3DBox::A3DBox()
{
	m_pA3DMesh = NULL;
}

A3DBox::~A3DBox()
{
}

bool A3DBox::Init(IDirect3DDevice8 * pDevice, const A3DOBB& a3dOBB)
{
	m_a3dOBB = a3dOBB;
	m_pDevice = pDevice;

	m_pA3DMesh = new A3DMesh();
	if( NULL == m_pA3DMesh )
		return false;

	if( !m_pA3DMesh->Init(pDevice, 1, 8, 12 * 2, true) )
	{
		return false;
	}

	A3DVERTEX	verts[8];
	static WORD index[] = {0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 6, 6, 7, 7, 4};

	//Up 4 vertex;
	//Left Up corner;
	A3DVECTOR3 vecPos;
	vecPos = m_a3dOBB.Center - m_a3dOBB.Extents.x * m_a3dOBB.XAxis +
		m_a3dOBB.Extents.y * m_a3dOBB.YAxis + m_a3dOBB.Extents.z * m_a3dOBB.ZAxis;
	verts[0].x = vecPos.x; verts[0].y = vecPos.y; verts[0].z = vecPos.z;
	verts[0].nx = verts[0].ny = verts[0].nz = 1.0f;

	//right up corner;
	vecPos = vecPos + 2.0f * m_a3dOBB.Extents.x * m_a3dOBB.XAxis;
	verts[1] = verts[0];
	verts[1].x = vecPos.x; verts[1].y = vecPos.y; verts[1].z = vecPos.z;
		
	//right bottom corner;
	vecPos = vecPos - 2.0f * m_a3dOBB.Extents.z * m_a3dOBB.ZAxis;
	verts[2] = verts[0];
	verts[2].x = vecPos.x; verts[2].y = vecPos.y; verts[2].z = vecPos.z;
	
	//left bottom corner;
	vecPos = vecPos - 2.0f * m_a3dOBB.Extents.x * m_a3dOBB.XAxis;
	verts[3] = verts[0];
	verts[3].x = vecPos.x; verts[3].y = vecPos.y; verts[3].z = vecPos.z;

	//Down 4 vertex;
	//Left up corner;
	vecPos = vecPos - 2.0f * m_a3dOBB.Extents.y * m_a3dOBB.YAxis + 2.0f * m_a3dOBB.Extents.z * m_a3dOBB.ZAxis;
	verts[4] = verts[0];
	verts[4].x = vecPos.x; verts[4].y = vecPos.y; verts[4].z = vecPos.z;

	//right up corner;
	vecPos = vecPos + 2.0f * m_a3dOBB.Extents.x * m_a3dOBB.XAxis;
	verts[5] = verts[0];
	verts[5].x = vecPos.x; verts[5].y = vecPos.y; verts[5].z = vecPos.z;

	//right bottom corner;
	vecPos = vecPos - 2.0f * m_a3dOBB.Extents.z * m_a3dOBB.ZAxis;
	verts[6] = verts[0];
	verts[6].x = vecPos.x; verts[6].y = vecPos.y; verts[6].z = vecPos.z;

	//left bottom corner;
	vecPos = vecPos - 2.0f * m_a3dOBB.Extents.x * m_a3dOBB.XAxis;
	verts[7] = verts[0];
	verts[7].x = vecPos.x; verts[7].y = vecPos.y; verts[7].z = vecPos.z;

	if( !m_pA3DMesh->SetVerts(0, verts, 8) )
		return false;
	if( !m_pA3DMesh->SetIndices(index, 24) )
		return false;
							 
	if( !m_pA3DMesh->SetMaterial(A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), 
		A3DCOLORVALUE(0.0f, 1.0f, 0.0f, 1.0f), A3DCOLORVALUE(0.0f, 1.0f, 0.0f, 1.0f), 
		A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, false) )
		return false;
	return true;
}

bool A3DBox::Release()
{
	if( m_pA3DMesh )
	{
		m_pA3DMesh->Release();
		delete m_pA3DMesh;
		m_pA3DMesh = NULL;
	}
	return true;
}

bool A3DBox::Render()
{
	return m_pA3DMesh->Render();
}