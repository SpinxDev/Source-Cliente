//	Settings.cpp

#include <windows.h>
#include "Settings.h"
#include "Game.h"
#include "Render.h"

#include "A3D.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define FOG_COL_UNDERWATER	0xff22394C
#define FOG_COL_CLOUDY		0xffA0B6B0
#define FOG_COL_RAIN		0xff737A82

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
//	Implement CSettings
//	
///////////////////////////////////////////////////////////////////////////

CSettings::CSettings()
{
	m_bSimpleWater	= false;
	m_bRenderWater	= true;
	m_iViewDist		= 1;
	m_iNewObjOpt	= NEW_OBJ_NONE;
	m_iWeather		= WEATHER_CLOUDY;
	m_fCameraSpeed	= 30.0f;
	m_fCameraFOV	= 56.0f;

	m_FogCol.Reset(A3DCOLORVALUE((A3DCOLOR)FOG_COL_CLOUDY));
}

CSettings::~CSettings()
{
}

//	Apply current all settings
bool CSettings::ApplyCurSettings()
{
	SetRenderWaterFlag(m_bRenderWater);
	SetSimpleWaterFlag(m_bSimpleWater);
	SetViewDist(m_iViewDist);
	SetWeather(m_iWeather);
	SetCameraSpeed(m_fCameraSpeed);
	return true;
}

void CSettings::SetSimpleWaterFlag(bool bTrue)
{
	m_bSimpleWater = bTrue;

	A3DTerrainWater* pWater = g_Game.GetTerrainWater();
	if (pWater)
		pWater->SetSimpleWaterFlag(bTrue);
}

void CSettings::SetRenderWaterFlag(bool bTrue)
{
	m_bRenderWater = bTrue;
}

void CSettings::SetViewDist(int iDist)
{
	m_iViewDist = iDist;

	float fViewDist, fLODDist1, fLODDist2;

	switch (iDist)
	{
	case 0:

		fViewDist	= 150.0f;
		fLODDist1	= 100.0f;
		fLODDist2	= 150.0f;
		break;

	case 1:

		fViewDist	= 250.0f;
		fLODDist1	= 100.0f;
		fLODDist2	= 180.0f;
		break;

	case 2:

		fViewDist	= 350.0f;
		fLODDist1	= 120.0f;
		fLODDist2	= 200.0f;
		break;

	default:
		return;
	}

	A3DTerrain2* pTerrain = g_Game.GetTerrain();
	if (pTerrain)
	{
		pTerrain->SetActRadius(fViewDist);
		pTerrain->SetViewRadius(fViewDist);
		pTerrain->SetLODDist(fLODDist1, fLODDist2);
	}

	//	Change fog distance
	ApplyFogDist();
}

//	Restore fog distance
void CSettings::ApplyFogDist()
{
	float fFogStart, fFogEnd;

	switch (m_iViewDist)
	{
	case 0:

		fFogStart	= 100.0f;
		fFogEnd		= 200.0f;
		break;

	case 1:

		fFogStart	= 180.0f;
		fFogEnd		= 320.0f;
		break;

	case 2:

		fFogStart	= 300.0f;
		fFogEnd		= 500.0f;
		break;

	default:
		return;
	}

	//	Change fog distance
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	if (pA3DDevice)
	{
		pA3DDevice->SetFogStart(fFogStart);
		pA3DDevice->SetFogEnd(fFogEnd);
	}
}

//	Update fog parameters
void CSettings::UpdateFogParams(DWORD dwDeltaTime)
{
	A3DTerrainWater* pWater = g_Game.GetTerrainWater();

	bool bUnderWater;
	if (!m_bRenderWater)
		bUnderWater = false;
	else
		bUnderWater = pWater->IsUnderWater(g_Render.GetCamera()->GetPos());

	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();

//	m_FogCol.Shade((float)dwDeltaTime);
	DWORD dwFogCol = GetFogColor(bUnderWater);

	if (bUnderWater)
	{
		pA3DDevice->SetFogStart(0.0f);
		pA3DDevice->SetFogEnd(20.0f);

		pWater->SetFogWater(dwFogCol, 0.0f, 20.0f);
	}
	else
	{
		ApplyFogDist();

	//	pWater->SetFogAir(dwFogCol, pA3DDevice->GetFogStart(), pA3DDevice->GetFogEnd());
	}

	pA3DDevice->SetFogColor(dwFogCol);
}

//	Get fog color
DWORD CSettings::GetFogColor(bool bUnderWater)
{
	DWORD dwCol = 0xffffffff;
		
	if (bUnderWater)
		dwCol = FOG_COL_UNDERWATER;
	else
		dwCol = m_FogCol.cur.ToRGBAColor();

	return dwCol;
}

void CSettings::SetWeather(int iWeather)
{
	m_iWeather = iWeather;

	A3DSkySphere* pSky = g_Game.GetSky();
	DWORD dwTransTime = 1000;

	static char* aRainFiles[] = {"Rain\\Roof.bmp", "Rain\\01.bmp", "Rain\\02.bmp"};
	static char* aCloudyFiles[] = {"Cloudy\\Roof.bmp", "Cloudy\\01.bmp", "Cloudy\\02.bmp"};

	if (iWeather == WEATHER_RAIN)
	{
		if (pSky->NeedTrans(aRainFiles[0], aRainFiles[1], aRainFiles[2]))
			pSky->TransSky(aRainFiles[0], aRainFiles[1], aRainFiles[2], dwTransTime);

		g_Game.GetRain()->StartRain();

		m_FogCol.SetTarget(A3DCOLORVALUE((A3DCOLOR)FOG_COL_RAIN), 1.0f / dwTransTime);
	}
	else if (iWeather == WEATHER_SNOW)
	{
		if (pSky->NeedTrans(aRainFiles[0], aRainFiles[1], aRainFiles[2]))
			pSky->TransSky(aRainFiles[0], aRainFiles[1], aRainFiles[2], dwTransTime);

		g_Game.GetSnow()->StartSnow();

		m_FogCol.SetTarget(A3DCOLORVALUE((A3DCOLOR)FOG_COL_RAIN), 1.0f / dwTransTime);
	}
	else
	{
		if (pSky->NeedTrans(aCloudyFiles[0], aCloudyFiles[1], aCloudyFiles[2]))
			pSky->TransSky(aCloudyFiles[0], aCloudyFiles[1], aCloudyFiles[2], dwTransTime);

		m_FogCol.SetTarget(A3DCOLORVALUE((A3DCOLOR)FOG_COL_CLOUDY), 1.0f / dwTransTime);
	}
}

void CSettings::SetCameraFOV(float fFOV)
{
	m_fCameraFOV = fFOV;
	A3DCamera* pA3DCamera = g_Render.GetCamera();
	pA3DCamera->SetFOVSet(DEG2RAD(m_fCameraFOV));
}


