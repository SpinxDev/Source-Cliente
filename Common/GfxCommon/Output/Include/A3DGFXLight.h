/*
 * FILE: A3DGFXLight.h
 *
 * DESCRIPTION: Light
 *
 * CREATED BY: ZhangYu, 2004/8/12
 *
 * HISTORY:
 *
 */

#ifndef _A3DGFXLIGHT_H_
#define _A3DGFXLIGHT_H_

#include "A3DGFXElement.h"

class A3DGFXLight : public A3DGFXElement
{
public:
	A3DGFXLight(A3DGFXEx* pGfx);
	virtual ~A3DGFXLight();

protected:
	A3DLIGHTPARAM m_LightParam;
	bool m_bAddedToLightQueue;

public:
	A3DGFXLight& operator = (const A3DGFXLight& src);

public:
	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual void Release() {}
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual bool Render(A3DViewport*);
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;

	A3DLIGHTPARAM& GetLightParam() { return m_LightParam; }
	void SetAddedToLightQueue(bool b) { m_bAddedToLightQueue = b; }
	bool GetAddedToLightQueue() const { return m_bAddedToLightQueue; }
};

#endif
