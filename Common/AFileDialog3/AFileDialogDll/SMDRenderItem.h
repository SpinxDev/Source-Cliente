#pragma once

//#ifdef _ANGELICA31
#include "ARenderItem.h"

class A3DDevice;
class A3DSkinModel;
class A3DEngine;
class A3DViewport;
class A3DAABB;

class SMDRenderItem : public ARenderItem
{
public:
	SMDRenderItem(const char* szSMD, A3DDevice* pA3DDevice);
	~SMDRenderItem();

private:

	AString m_strSMD;
	A3DSkinModel* m_pSMD;
	A3DDevice* m_pA3DDevice;

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