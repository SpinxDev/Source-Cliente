#include <Windows.h>
#include "A3DObject.h"
#include "A3DFrame.h"
#include "A3DMesh.h"
#include "A3DTexture.h"

#ifndef _A3DMODEL_H_
#define _A3DMODEL_H_

#define A3DMODEL_PROPERTY_SIZE	32

class A3DModel : public A3DFrame
{
private:
	A3DFrame * m_pChildFrame;

public:
	A3DModel();
	~A3DModel();
	bool AddFrame(A3DFrame ** ppFrame);
	bool Init(IDirect3DDevice8 * pDevice);
	bool Release();
	bool Render();
	bool TickAnimation();
	bool Save(A3DFile * pFileToSave);
	bool Load(A3DFile * pFileToLoad);

	void inline AddChildFrame(A3DFrame * pChildFrame) { m_pChildFrame = pChildFrame; }
};

typedef A3DModel * PA3DModel;

#endif