#include "A3DMaterial.h"
#include "A3DMacros.h"

A3DMaterial::A3DMaterial()
{
	ZeroMemory(&m_clAmbientValue, sizeof(A3DCOLORVALUE));
	ZeroMemory(&m_clDiffuseValue, sizeof(A3DCOLORVALUE));
	ZeroMemory(&m_clEmissiveValue, sizeof(A3DCOLORVALUE));
	ZeroMemory(&m_clSpecularValue, sizeof(A3DCOLORVALUE));
	m_fShininess = 0.0f;
	m_b2Sided = false;
}

A3DMaterial::~A3DMaterial()
{
}

bool A3DMaterial::Init(IDirect3DDevice8 * pDevice)
{
	m_pDevice = pDevice;

	m_clAmbientValue = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	m_clDiffuseValue = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	m_clEmissiveValue = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 0.0f);
	m_clSpecularValue = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 0.0f);

	m_fShininess = 0.0f;
	return true;
}

bool A3DMaterial::Init(IDirect3DDevice8 * pDevice, A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShininess)
{
	m_pDevice = pDevice;

	return SetValues(clAmbient, clDiffuse, clEmissive, clSpecular, fShininess);
}

bool A3DMaterial::SetValues(A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShininess, bool b2Sided)
{
	m_clAmbientValue = clAmbient;
	m_clDiffuseValue = clDiffuse;
	m_clEmissiveValue = clEmissive;
	m_clSpecularValue = clSpecular;

	m_fShininess = fShininess;
	m_b2Sided = b2Sided;
	return true;
}

bool A3DMaterial::Release()
{
	return true;
}

bool A3DMaterial::Appear()
{
	//If this object is created outside D3D;
	if( m_pDevice == NULL )
		return true;

	D3DMATERIAL8 mtrl;

    ZeroMemory(&mtrl, sizeof(D3DMATERIAL8));
	mtrl.Ambient.r = m_clAmbientValue.r;
	mtrl.Ambient.g = m_clAmbientValue.g;
	mtrl.Ambient.b = m_clAmbientValue.b;
	mtrl.Ambient.a = m_clAmbientValue.a;
    mtrl.Diffuse.r = m_clDiffuseValue.r;
	mtrl.Diffuse.g = m_clDiffuseValue.g;
	mtrl.Diffuse.b = m_clDiffuseValue.b;
	mtrl.Diffuse.a = m_clDiffuseValue.a;
	mtrl.Emissive.r = m_clEmissiveValue.r;
	mtrl.Emissive.g = m_clEmissiveValue.g;
	mtrl.Emissive.b = m_clEmissiveValue.b;
	mtrl.Emissive.a = m_clEmissiveValue.a;
    mtrl.Specular.r = m_clSpecularValue.r;
	mtrl.Specular.g = m_clSpecularValue.g;
	mtrl.Specular.b = m_clSpecularValue.b;
	mtrl.Specular.a = m_clSpecularValue.a;
	mtrl.Power = m_fShininess;

 	m_pDevice->SetMaterial(&mtrl);
	if( m_b2Sided )
	{
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
	else
	{
		//Should check current face culling configuration;
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
	return true;
}

bool A3DMaterial::SetDiffuse(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
{
	m_clDiffuseValue.r = r;
	m_clDiffuseValue.g = g;
	m_clDiffuseValue.b = b;
	m_clDiffuseValue.a = a;

	return true;
}

bool A3DMaterial::Save(A3DFile * pFileToSave)
{
	if( pFileToSave->IsBinary() )
	{
		DWORD		dwWriteLength;
		char		szLineBuffer[2048];

		sprintf(szLineBuffer, "MATERIAL: %s", GetName());
		pFileToSave->Write(szLineBuffer, strlen(szLineBuffer) + 1, &dwWriteLength);

		pFileToSave->Write(&m_clAmbientValue, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_clDiffuseValue, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_clEmissiveValue, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_clSpecularValue, sizeof(A3DCOLORVALUE), &dwWriteLength);
		pFileToSave->Write(&m_fShininess, sizeof(FLOAT), &dwWriteLength);
		pFileToSave->Write(&m_b2Sided, sizeof(bool), &dwWriteLength);
	}
	else
	{
		char szLineBuffer[2048];
		sprintf(szLineBuffer, "MATERIAL: %s\n{", GetName());
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "Ambient:(%f, %f, %f, %f)", m_clAmbientValue.r, m_clAmbientValue.g, m_clAmbientValue.b, m_clAmbientValue.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Diffuse:(%f, %f, %f, %f)", m_clDiffuseValue.r, m_clDiffuseValue.g, m_clDiffuseValue.b, m_clDiffuseValue.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Emissive:(%f, %f, %f, %f)", m_clEmissiveValue.r, m_clEmissiveValue.g, m_clEmissiveValue.b, m_clEmissiveValue.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Specular:(%f, %f, %f, %f)", m_clSpecularValue.r, m_clSpecularValue.g, m_clSpecularValue.b, m_clSpecularValue.a);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Power:(%f)", m_fShininess);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "2Sided:(%d)", m_b2Sided);
		pFileToSave->WriteLine(szLineBuffer);

		//<== "MATERIAL: {"
		pFileToSave->WriteLine("}");
	}
	return true;
}

bool A3DMaterial::Load(IDirect3DDevice8 * pDevice, A3DFile * pFileToLoad)
{
	if( !Init(pDevice) )
		return false;

	if( pFileToLoad->IsBinary() )
	{
		DWORD		dwReadLength;
		char		szLineBuffer[2048];
		char		szResult[2048];

		pFileToLoad->ReadString(szLineBuffer, 2048, &dwReadLength);
		pFileToLoad->GetStringAfter(szLineBuffer, "MATERIAL: ", szResult);
		if( strlen(szResult) >= MAX_NAME )
			return false;
		SetName(szResult);

		pFileToLoad->Read(&m_clAmbientValue, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_clDiffuseValue, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_clEmissiveValue, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_clSpecularValue, sizeof(A3DCOLORVALUE), &dwReadLength);
		pFileToLoad->Read(&m_fShininess, sizeof(FLOAT), &dwReadLength);
		pFileToLoad->Read(&m_b2Sided, sizeof(bool), &dwReadLength);
	}
	else
	{
		char szLineBuffer[A3DFILE_LINEMAXLEN];
		char szResult[A3DFILE_LINEMAXLEN];
		DWORD dwReadLen;

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		pFileToLoad->GetStringAfter(szLineBuffer, "MATERIAL: ", szResult);
		SetName(szResult);

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") )
			return false;

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Ambient:(%f, %f, %f, %f)", &m_clAmbientValue.r, &m_clAmbientValue.g, &m_clAmbientValue.b, &m_clAmbientValue.a);
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Diffuse:(%f, %f, %f, %f)", &m_clDiffuseValue.r, &m_clDiffuseValue.g, &m_clDiffuseValue.b, &m_clDiffuseValue.a);
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Emissive:(%f, %f, %f, %f)", &m_clEmissiveValue.r, &m_clEmissiveValue.g, &m_clEmissiveValue.b, &m_clEmissiveValue.a);
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Specular:(%f, %f, %f, %f)", &m_clSpecularValue.r, &m_clSpecularValue.g, &m_clSpecularValue.b, &m_clSpecularValue.a);

		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Power:(%f)", &m_fShininess);
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "2Sided:(%d)", &m_b2Sided);
		
		//<== "MATERIAL: {"
		pFileToLoad->ReadLine(szLineBuffer, A3DFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
			return false;
	}

	return true;
}
