#include <Windows.h>
#include <d3d8.h>
#include "A3DTypes.h"
#include "A3DTexture.h"
#include "A3DVertex.h"

#ifndef _A3DFONT_H_
#define _A3DFONT_H_

#define MAX_STRING		1024

class A3DFont
{
private:
	IDirect3DDevice8		* m_pDevice;
	IDirect3DVertexBuffer8	* m_pVertexBuffer;
	IDirect3DIndexBuffer8	* m_pIndexBuffer;	

	A3DTexture			m_Texture;

	int					m_nCharWidth;
	int					m_nCharHeight;
	int					m_nCellWidth;
	int					m_nCellHeight;
	int					m_nMapWidth;
	int					m_nMapHeight;

	float				m_fCellWidth;
	float				m_fCellHeight;

	char				m_szFontMap[MAX_PATH];
	int					m_nStringLength;

public:
	A3DFont();
	~A3DFont();
	
	bool Init(HINSTANCE hInstance, IDirect3DDevice8 * pDevice, char * szFontMap, int chWidth, int chHeight, int cellWidth, int cellHeight);
	bool Release();
	bool Render();
	bool TextOut(int x, int y, char * szText, A3DCOLOR color);
};

typedef A3DFont * PA3DFont;

#endif