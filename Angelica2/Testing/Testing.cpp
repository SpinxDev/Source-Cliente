// Testing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "A3DFuncs.h"

int _tmain(int argc, _TCHAR* argv[])
{
	BYTE b1 = 0xBE;
	BYTE b2 = 0x23;
	BYTE* pDst = new BYTE [2];
	A3DVECTOR3 v = a3d_DecompressDir(b1, b2);
	v.Normalize();
	a3d_CompressDir(v,pDst[0], pDst[1]);
	BYTE d1 = pDst[0];
	BYTE d2 = pDst[1];

	A3DVECTOR3 v2 = a3d_DecompressDir(d1, d2);

	delete pDst;
	return 0;
}

