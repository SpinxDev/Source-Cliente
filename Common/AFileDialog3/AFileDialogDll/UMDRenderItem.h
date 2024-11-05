#pragma once

//#ifdef _ANGELICA31
#include "ARenderItem.h"

class A3DDevice;
class A3DUnlitModel;
class A3DEngine;
class A3DViewport;
class A3DAABB;

class UMDRenderItem : public ARenderItem
{
public:
	UMDRenderItem(const char* szUMD, A3DDevice* pA3DDevice);
	~UMDRenderItem();
private:
	AString m_strUMD;
	A3DDevice* m_pA3DDevice;
	A3DUnlitModel* m_pUMD;
protected:
	virtual bool Init();
	virtual void Release();
	virtual void Play();
	virtual void Tick(DWORD dwTick);
	virtual void Render(A3DEngine* pEngine, A3DViewport* pA3DViewport);
	virtual void Stop();
	virtual void GetAABB(A3DAABB* aabb) const;
	virtual A3DVECTOR3 GetPos() const;
};

//#endif
