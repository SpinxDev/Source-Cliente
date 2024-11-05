#include <Windows.h>

#ifndef _A3DVERTEX_H_
#define _A3DVERTEX_H_

typedef struct _A3DVERTEX
{
    FLOAT		x, y, z;
	FLOAT		nx, ny, nz;
	FLOAT       tu, tv;
} A3DVERTEX, * PA3DVERTEX;

typedef struct _A3DLVERTEX
{
	FLOAT x, y, z;
	DWORD color;
	FLOAT tu, tv;
} A3DLVERTEX, * PA3DLVERTEX;

typedef struct _A3DTLVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
	FLOAT tu, tv;
} A3DTLVERTEX, * PA3DTLVERTEX;

typedef struct _A3DLIGHTMAPCOORD
{
	float u, v;
}A3DLIGHTMAPCOORD, * PA3DLIGHTMAPCOORD;

#define D3DFVF_A3DVERTEX  (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_A3DLVERTEX  (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_A3DTLVERTEX  (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#endif
