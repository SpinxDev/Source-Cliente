//	DemoMenu.h

#pragma once

#include "AArray.h"
#include "AString.h"

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

typedef void (*LPMENUHANDLER)();

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CDemoMenuItem
//	
///////////////////////////////////////////////////////////////////////////

class CDemoMenuItem
{
public:		//	Types

	enum
	{
		TYPE_ITEM = 0,
		TYPE_SUBMENU,
	};

	friend class CDemoMenu;

public:		//	Constructor and Destructor

	CDemoMenuItem(int iType, const char* szText, CDemoMenu* pParent, DWORD dwHandler);
	virtual ~CDemoMenuItem();
	
public:		//	Attributes

public:		//	Operations

	//	Get menu item type
	int GetType() { return m_iType; }
	//	Get parent menu
	CDemoMenu* GetParent() { return m_pParent; }
	//	Get menu item handler
	DWORD GetHandler() { return m_dwHandler; }
	//	Get menu item text
	const char* GetText() { return m_strText; }

	//	When menu item is clicked, this function is called
	void OnEvent();

protected:	//	Attributes

	int			m_iType;		//	Menu type
	CDemoMenu*	m_pParent;		//	Parent menu
	DWORD		m_dwHandler;	//	Menu item handler
	AString		m_strText;		//	Menu item text

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CDemoMenu
//	
///////////////////////////////////////////////////////////////////////////

class CDemoMenu
{
public:		//	Types

public:		//	Constructor and Destructor

	CDemoMenu(const char* szText);
	virtual ~CDemoMenu();

public:		//	Attributes

	static CDemoMenu*	m_pCurMenu;

public:		//	Operations

	//	Get menu item text
	const char* GetText() { return m_strText; }

	//	Get item number
	int GetItemNum() { return m_aItems.GetSize(); }
	//	Get item
	CDemoMenuItem* GetItem(int n) { return m_aItems[n]; }
	//	Add item
	void AddItem(CDemoMenuItem* pItem) { m_aItems.Add(pItem); }

	//	Render routine
	bool Render();

protected:	//	Attributes

	AString		m_strText;		//	Menu text

	APtrArray<CDemoMenuItem*>	m_aItems;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Menu handler functions
//	
///////////////////////////////////////////////////////////////////////////

//	Main menu
void menu_OnExitGame();

//	Terrain menu
void menu_trn_OnWaterQuality();
void menu_trn_OnRenderWater();
void menu_trn_OnFarDist();
void menu_trn_OnMiddleDist();
void menu_trn_OnNearDist();

//	Model menu
void menu_mod_OnCreateCat();
void menu_mod_OnCreateFox();
void menu_mod_OnCreateRabbit();

//	GFX menu
void menu_gfx_OnCreateGFX1();
void menu_gfx_OnCreateGFX2();

//	Weather menu
void menu_wea_OnCloudy();
void menu_wea_OnRain();
void menu_wea_OnSnow();

//	Camera menu
void menu_cam_OnMoveFast();
void menu_cam_OnMoveNormal();
void menu_cam_OnMoveSlow();
void menu_cam_OnTelescope();

