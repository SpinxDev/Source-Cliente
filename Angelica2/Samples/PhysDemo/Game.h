//	Game.h

#pragma once

#include <ABaseDef.h>
#include <A3DVector.h>
#include <AString.h>

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

class CPhysTerrain;
class CPhysObjectMan;
class A3DSkySphere;
class A3DSysCursor;
class APhysXScene;
class CPhysObject;

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

public:		//	Operations

	//	Initialize game
	bool Init();
	//	Release game
	void Release();
	//	Reset game
	bool Reset();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render();

	//	Show or hide cursor
	void ShowCursor(bool bShow);

	//	Load terrain
	bool LoadTerrain(const char* szFile);
	//	Load physX terrain data
	bool LoadPhysXTerrain(const char* szFile);

	//	Set picked object
	void SetPickedObject(CPhysObject* pObject);
	//	Get picked object
	CPhysObject* GetPickedObject() { return m_pPickedObject; }
	//	Set up picked object
	bool SetupPickedObject(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);

	//	Get instance handle
	HINSTANCE GetInstance() { return m_hInstance; }
	//	Get window handle
	HWND GetWindow() { return m_hWnd; }
	//	Get terrain initialized flag
	bool IsTerrainInit() { return m_pTerrain ? true : false; }
	//	Get terrain
	CPhysTerrain* GetTerrain() { return m_pTerrain; }
	//	Get sky
	A3DSkySphere* GetSky() { return m_pA3DSky; }
	//	Get physical scene
	APhysXScene* GetPhysXScene() { return m_pPhysXScene; }
	//	Get current terrain file name
	const char* GetTerrainFile() { return m_strTrnFile; }

	//	Set / Get wire mode flag
	void SetWireModeFlag(bool bWire) { m_bWireMode = bWire; }
	bool GetWireModeFlag() { return m_bWireMode; }
	//	Set / Get physical state
	void SetPhysState(int iState);
	int GetPhysState() { return m_iPhysState; }

protected:	//	Attributes

	CPhysTerrain*		m_pTerrain;
	CPhysObjectMan*		m_pObjectMan;
	A3DSkySphere*		m_pA3DSky;
	A3DSysCursor*		m_pCursor;
	APhysXScene*		m_pPhysXScene;
	CPhysObject*		m_pPickedObject;	//	Picked object

	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	DWORD		m_dwTickTime;		//	Current tick time
	AString		m_strTrnFile;		//	Current terrain file

	bool		m_bWireMode;		//	Wire mode
	int			m_iPhysState;		//	Physical state

protected:	//	Operations

	//	Load sky
	bool LoadSky();
	//	Initialize physical module
	bool InitPhysX();
	//	Release physical module
	void ReleasePhysX();
};

extern CGame g_Game;

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

