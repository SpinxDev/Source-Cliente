#pragma once

//#ifdef _ANGELICA31
#include "ARenderItem.h"

class A3DDevice;
class A3DUnlitModelSet;
class A3DEngine;
class A3DViewport;
class A3DAABB;

class UMDSRenderItem : public ARenderItem
{
public:
	UMDSRenderItem(const char* szUMDS, A3DDevice* pA3DDevice);
	~UMDSRenderItem();
private:
	AString m_strUMDS;
	A3DDevice* m_pA3DDevice;
	A3DUnlitModelSet* m_pUMDS;
protected:
	virtual bool Init();
	virtual void Release();
	virtual void Play();
	virtual void Tick(DWORD dwTick);
	virtual void Render(A3DEngine* pEngine, A3DViewport* pA3DViewport);
	virtual void Stop();
	virtual void GetAABB(A3DAABB* aabbset) const;
	virtual A3DVECTOR3 GetPos() const;
};

//#endif
