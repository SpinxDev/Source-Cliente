#ifndef _LIGHTEXTRA3DWINDOW_H_
#define _LIGHTEXTRA3DWINDOW_H_

#include <Windows.h>
#include <StdIO.h>
#include <StdLib.h>

#include "Engine3DWindow.h"

class LightExtra3DWindow : public Engine3DWindow
{
private:
	AList			m_ListIBLMesh;
	A3DIBLScene		* m_pIBLScene;
	A3DModel		* m_pModelBall;

protected:
	virtual bool BeforeRenderScene(A3DViewport * pCurrentViewport);
	virtual bool AfterRenderScene(A3DViewport * pCurrentViewport);

public:
	virtual bool InitEngine();
	virtual bool ReleaseEngine();

	bool Release();

	bool LoadAllLightMaps();

	LightExtra3DWindow();
	~LightExtra3DWindow();
	bool AddIBLMesh(A3DIBLMesh * pIBLMesh);
	bool SetIBLScene(A3DIBLScene * pIBLScene);
};

#endif//_LIGHTEXTRA3DWINDOW_H_