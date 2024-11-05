#include <Windows.h>
#include <d3d8.h>
#include "A3DTypes.h"
#include "A3DObject.h"

#ifndef _A3DMATERIAL_H_
#define _A3DMATERIAL_H_

class A3DMaterial : public A3DObject
{
private:
	IDirect3DDevice8 * m_pDevice;
public:
	A3DCOLORVALUE	m_clAmbientValue;
	A3DCOLORVALUE	m_clDiffuseValue;
	A3DCOLORVALUE	m_clEmissiveValue;
	A3DCOLORVALUE	m_clSpecularValue;

	FLOAT			m_fShininess;
	bool			m_b2Sided;

	A3DMaterial();
	~A3DMaterial();

	bool SetValues(A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShininess, bool b2Sided=false);
	bool SetDiffuse(FLOAT r, FLOAT g, FLOAT b, FLOAT a);
	bool Init(IDirect3DDevice8 * pDevice);
	bool Init(IDirect3DDevice8 * pDevice, A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShiness);
	bool Release();
	bool Appear();

	bool Save(A3DFile * pFileToSave);
	bool Load(IDirect3DDevice8 * pDevice, A3DFile * pFileToLoad);
};

typedef A3DMaterial * PA3DMaterial;

#endif