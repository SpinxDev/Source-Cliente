#include <Windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "A3DObject.h"

#ifndef _A3DTEXTURE_H_
#define _A3DTEXTURE_H_

class A3DTexture : public A3DObject
{
public:
	bool				m_bMissing;
	char				m_szMapFile[MAX_PATH];
	IDirect3DTexture8 * m_pDXTexture;
	IDirect3DDevice8 *	m_pDevice;

	A3DTexture();
	~A3DTexture();

	bool Init(IDirect3DDevice8 * pDevice);
	bool Release();
	bool Appear(int nLayer);
	bool Disappear(int nLayer);
	bool SetMapFile(char * szMapFile);

	bool LoadFromResource(HINSTANCE hRCModule, int nIDResource);

	bool Save(A3DFile * pFileToSave);
	bool Load(IDirect3DDevice8 * pDevice, A3DFile * pFileToLoad);
};

typedef A3DTexture * PA3DTexture;
#endif