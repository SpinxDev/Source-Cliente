#include <Windows.h>
#include "A3DFile.h"
#include <d3d8.h>

#ifndef _A3DOBJECT_H_
#define _A3DOBJECT_H_

#define MAX_NAME 32

class A3DObject
{
protected:
	char m_szName[MAX_NAME + 1];
public:
	A3DObject();
	~A3DObject();
	inline void SetName(char * pszName) { strncpy(m_szName, pszName, MAX_NAME); }
	inline char * GetName() { return m_szName; }
	virtual bool Save(A3DFile * pFileToSave);
	virtual bool Load(IDirect3DDevice8 * pDevice, A3DFile * pFileToLoad);
};

typedef A3DObject * PA3DObject;

#endif