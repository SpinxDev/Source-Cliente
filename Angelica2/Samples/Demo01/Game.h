//	Game.h

#pragma once

#include "ABaseDef.h"
#include "A3DVector.h"
#include "Settings.h"
#include "RayTrace.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2;
class A3DTerrainWater;
class A3DSkySphere;
class A3DViewport;
class CDemoMenu;
class CNPCMan;
class A3DRain;
class A3DSnow;
class AMSoundStream;
class CGFXModule;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CGame
//	
///////////////////////////////////////////////////////////////////////////

class CGame
{
public:		//	Types

public:		//	Constructor and Destructor

	CGame();
	virtual ~CGame();

public:		//	Attributes

	static A3DVECTOR3	m_vOrigin;
	static A3DVECTOR3	m_vAxisX;
	static A3DVECTOR3	m_vAxisY;
	static A3DVECTOR3	m_vAxisZ;

public:		//	Operations

	//	Initialize game
	bool Init(HINSTANCE hInst, HWND hWnd);
	//	Release game
	void Release();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render();
	//	Render for reflect
	bool RenderForReflected(A3DViewport* pA3DViewport);

	//	Windows message handler
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//	Play 3D-sound at specified position
	void Play3DSound(const A3DVECTOR3& vPos, const char* szFile);

	//	Get instance handle
	HINSTANCE GetInstance() { return m_hInstance; }
	//	Get window handle
	HWND GetWindow() { return m_hWnd; }
	//	Get root menu
	CDemoMenu* GetRootMenu() { return m_pRootMenu; }
	//	Get game settings
	CSettings& GetSettings() { return m_Settings; }
	//	Get terrain
	A3DTerrain2* GetTerrain() { return m_pA3DTerrain; }
	//	Get terrain water
	A3DTerrainWater* GetTerrainWater() { return m_pA3DTerrainWater; }
	//	Get sky
	A3DSkySphere* GetSky() { return m_pA3DSky; }
	//	Get NPC manager
	CNPCMan* GetNPCMan() { return m_pNPCMan; }
	//	Get rain object
	A3DRain* GetRain() { return m_pRain; }
	//	Get snow object
	A3DSnow* GetSnow() { return m_pSnow; }
	//	Get GFX module
	CGFXModule* GetGFXModule() { return m_pGFXModule; }

protected:	//	Attributes

	A3DTerrain2*		m_pA3DTerrain;
	A3DTerrainWater*	m_pA3DTerrainWater;
	A3DSkySphere*		m_pA3DSky;
	CDemoMenu*			m_pRootMenu;
	CNPCMan*			m_pNPCMan;
	A3DRain*			m_pRain;
	A3DSnow*			m_pSnow;
	AMSoundStream*		m_pBackMusic;		//	Sound stream object
	CGFXModule*			m_pGFXModule;

	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	bool		m_bInit;			//	Initialize flag
	bool		m_bRightPressed;	//	Right button pressed flag
	POINT		m_ptMouseLast;		//	Last mouse position
	DWORD		m_dwTickTime;		//	Current tick time

	CSettings	m_Settings;			//	Game settings
	CRayTrace	m_RayTrace;			//	Ray trace controller

protected:	//	Operations

	//	Load terrain
	bool LoadTerrain(const char* szFile);
	//	Load terrain water
	bool LoadTerrainWater(const char* szFile);
	//	Load sky
	bool LoadSky();
	//	Load music
	bool LoadMusic(const char* szFile);
	//	Create nature objects
	bool CreateNatureObjects();
	//	Create menu
	bool CreateMenu();

	//	Move camera
	void MoveCamera(DWORD dwTime);
	//	Render prompts
	void DrawPrompts();
	//	Create new object
	void CreateNewObject(int sx, int sy);

	void OnLButtonDown(HWND hWnd, UINT nFlags, int x, int y);
	void OnLButtonUp(HWND hWnd, UINT nFlags, int x, int y);
	void OnMouseMove(HWND hWnd, UINT nFlags, int x, int y);
	void OnRButtonDown(HWND hWnd, UINT nFlags, int x, int y);
	void OnRButtonUp(HWND hWnd, UINT nFlags, int x, int y);
};

extern CGame g_Game;

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

