//	DemoMenu.cpp

#include <windows.h>
#include "DemoMenu.h"
#include "Game.h"
#include "Render.h"
#include "Settings.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CDemoMenuItem
//	
///////////////////////////////////////////////////////////////////////////

CDemoMenuItem::CDemoMenuItem(int iType, const char* szText, CDemoMenu* pParent, DWORD dwHandler)
{
	m_iType		= iType;
	m_pParent	= pParent;
	m_dwHandler	= dwHandler;
	m_strText	= szText;
}

CDemoMenuItem::~CDemoMenuItem()
{
	if (m_dwHandler)
	{
		if (m_iType == TYPE_SUBMENU)
		{
			CDemoMenu* pMenu = (CDemoMenu*)m_dwHandler;
			delete pMenu;
		}
	}
}

//	When menu item is clicked, this function is called
void CDemoMenuItem::OnEvent()
{
	if (m_iType == TYPE_ITEM)
	{
		if (m_dwHandler)
			((LPMENUHANDLER)m_dwHandler)();
	}
	else if (m_iType == TYPE_SUBMENU)
	{
		CDemoMenu::m_pCurMenu = (CDemoMenu*)m_dwHandler;
	}
	else
	{
		ASSERT(0);
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CDemoMenu
//	
///////////////////////////////////////////////////////////////////////////

CDemoMenu* CDemoMenu::m_pCurMenu = NULL;

CDemoMenu::CDemoMenu(const char* szText)
{
	m_strText	= szText;
}

CDemoMenu::~CDemoMenu()
{
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		delete m_aItems[i];
	}
}

//	Render routine
bool CDemoMenu::Render()
{
	int i, iNumItem = m_aItems.GetSize();
	int x = 10, y = 10;
	DWORD dwColor = 0xffffff00;

	g_Render.TextOut(x, y, m_strText, dwColor);
	y += 16;
	g_Render.TextOut(x, y, "------------------------------", dwColor);
	y += 16;

	for (i=0; i < iNumItem; i++)
	{
		CDemoMenuItem* pItem = m_aItems[i];
		g_Render.TextOut(x, y, pItem->GetText(), dwColor);

		y += 16;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Menu handler functions
//	
///////////////////////////////////////////////////////////////////////////

//	Exit game
void menu_OnExitGame()
{
	::PostMessage(g_Game.GetWindow(), WM_CLOSE, 0, 0);
}

void menu_trn_OnWaterQuality()
{
	CSettings& s = g_Game.GetSettings();
	s.SetSimpleWaterFlag(!s.GetSimpleWaterFlag());
}

void menu_trn_OnRenderWater()
{
	CSettings& s = g_Game.GetSettings();
	s.SetRenderWaterFlag(!s.GetRenderWaterFlag());
}

void menu_trn_OnFarDist()
{
	CSettings& s = g_Game.GetSettings();
	s.SetViewDist(2);
}

void menu_trn_OnMiddleDist()
{
	CSettings& s = g_Game.GetSettings();
	s.SetViewDist(1);
}

void menu_trn_OnNearDist()
{
	CSettings& s = g_Game.GetSettings();
	s.SetViewDist(0);
}

void menu_mod_OnCreateCat()
{
	CSettings& s = g_Game.GetSettings();
	s.SetNewObjectOpt(CSettings::NEW_OBJ_NPC_CAT);
}

void menu_mod_OnCreateFox()
{
	CSettings& s = g_Game.GetSettings();
	s.SetNewObjectOpt(CSettings::NEW_OBJ_NPC_FOX);
}

void menu_mod_OnCreateRabbit()
{
	CSettings& s = g_Game.GetSettings();
	s.SetNewObjectOpt(CSettings::NEW_OBJ_NPC_RABBIT);
}

void menu_gfx_OnCreateGFX1()
{
	CSettings& s = g_Game.GetSettings();
	s.SetNewObjectOpt(CSettings::NEW_OBJ_GFX_1);
}

void menu_gfx_OnCreateGFX2()
{
	CSettings& s = g_Game.GetSettings();
	s.SetNewObjectOpt(CSettings::NEW_OBJ_GFX_2);
}

void menu_wea_OnCloudy()
{
	CSettings& s = g_Game.GetSettings();
	s.SetWeather(CSettings::WEATHER_CLOUDY);
}

void menu_wea_OnRain()
{
	CSettings& s = g_Game.GetSettings();
	s.SetWeather(CSettings::WEATHER_RAIN);
}

void menu_wea_OnSnow()
{
	CSettings& s = g_Game.GetSettings();
	s.SetWeather(CSettings::WEATHER_SNOW);
}

void menu_cam_OnMoveFast()
{
	CSettings& s = g_Game.GetSettings();
	s.SetCameraSpeed(60.0f);
}

void menu_cam_OnMoveNormal()
{
	CSettings& s = g_Game.GetSettings();
	s.SetCameraSpeed(30.0f);
}

void menu_cam_OnMoveSlow()
{
	CSettings& s = g_Game.GetSettings();
	s.SetCameraSpeed(15.0f);
}

void menu_cam_OnTelescope()
{
	CSettings& s = g_Game.GetSettings();
	if (s.GetCameraFOV() == 56.0f)
		s.SetCameraFOV(10.0f);
	else
		s.SetCameraFOV(56.0f);
}
