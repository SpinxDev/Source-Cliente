#include "A3DObject.h"
#include "A3DVertex.h"
#include "A3DMaterial.h"

#include "A3DTypes.h"
#include "A3DMesh.h"

#ifndef _A3DBOX_H_
#define _A3DBOX_H_

class A3DBox
{
private:
	IDirect3DDevice8 *	m_pDevice;
	A3DMesh *			m_pA3DMesh;
	A3DOBB				m_a3dOBB;

public:
	A3DBox();
	~A3DBox();

	bool Init(IDirect3DDevice8 * pDevice, const A3DOBB& a3dObb);
	bool Release();
	bool Render();
};
#endif