#pragma once

//#ifdef _ANGELICA31
#include "ARenderItem.h"

class CECModel;
class A3DEngine;
class A3DGfxEngine;
class A3DViewport;
class A3DAABB;

class ECMRenderItem : public ARenderItem
{
public:
	ECMRenderItem(const char* szECModel, A3DGfxEngine* pGfxEngine);
	~ECMRenderItem();

private:

	AString m_strECModel;
	CECModel* m_pECModel;
	A3DGfxEngine* m_pGfxEngine;

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