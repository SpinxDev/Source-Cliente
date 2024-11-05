#pragma once

//#ifdef _ANGELICA31
#include "ARenderItem.h"

class A3DGfxEngine;
class A3DEngine;
class A3DAABB;
class A3DViewport;
class A3DGFXEx;

class GfxRenderItem : public ARenderItem
{
public:

	GfxRenderItem(const char* szGfx, A3DGfxEngine* pGfxEngine);
	~GfxRenderItem();


private:

	AString m_strGfxPath;
	A3DGfxEngine* m_pGfxEngine;
	A3DGFXEx* m_pGfx;

protected:

	virtual bool Init();
	virtual void Release();
	virtual void Play();
	virtual void Stop();
	virtual void Tick(DWORD dwTick);
	virtual void Render(A3DEngine* pEngine, A3DViewport* pA3DViewport);
	virtual void GetAABB(A3DAABB* aabb) const;
	virtual A3DVECTOR3 GetPos() const;
};

//#endif