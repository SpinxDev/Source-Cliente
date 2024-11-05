//	Settings.h

#pragma once

#include "A3DTypes.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CSettings
//	
///////////////////////////////////////////////////////////////////////////

class CSettings
{
public:		//	Types

	//	'New Object' operation
	enum
	{
		NEW_OBJ_NONE = 0,
		NEW_OBJ_NPC_CAT,
		NEW_OBJ_NPC_FOX,
		NEW_OBJ_NPC_RABBIT,
		NEW_OBJ_GFX_1,
		NEW_OBJ_GFX_2,
	};

	//	Weather
	enum
	{
		WEATHER_CLOUDY = 0,
		WEATHER_RAIN,
		WEATHER_SNOW,
	};

	class CFloatShader
	{
	public:

		float cur;		//	Current value
		float target;	//	Target value
		float delta;	//	Value change delta

	public:

		void Reset(float v) { cur = target = v; delta = 0.0f; }

		void SetTarget(float t, float fInvTime)
		{
			if (!fInvTime)
				Reset(t);
			else
			{
				target = t;
				delta  = (t - cur) * fInvTime;
			}
		}

		static void Shade(float& f, float f1, float fDelta, float fTime)
		{
			if (!fDelta || f == f1)
				return;

			f += fDelta * fTime;
			if ((fDelta > 0.0f && f > f1) || (fDelta < 0.0f && f < f1))
				f = f1;
		}

		void Shade(float fTime) { Shade(cur, target, delta, fTime); }
	};

	class CColorShader
	{
	public:

		A3DCOLORVALUE cur;		//	Current value
		A3DCOLORVALUE target;	//	Target value
		A3DCOLORVALUE delta;	//	Value change delta

	public:

		void Reset(const A3DCOLORVALUE& v) { cur = target = v; delta.Set(0.0f, 0.0f, 0.0f, 0.0f); }

		void SetTarget(const A3DCOLORVALUE& t, float fInvTime)
		{
			if (!fInvTime)
				Reset(t);
			else
			{
				target = t;
				delta  = (t - cur) * fInvTime;
			}
		}

		void Shade(float fTime)
		{
			CFloatShader::Shade(cur.r, target.r, delta.r, fTime);
			CFloatShader::Shade(cur.g, target.g, delta.g, fTime);
			CFloatShader::Shade(cur.b, target.b, delta.b, fTime);
			CFloatShader::Shade(cur.a, target.a, delta.a, fTime);
		}
	};

public:		//	Constructor and Destructor

	CSettings();
	virtual ~CSettings();
	
public:		//	Attributes

public:		//	Operations

	//	Apply current all settings
	bool ApplyCurSettings();
	//	Apply fog distance
	void ApplyFogDist();

	//	Update fog parameters
	void UpdateFogParams(DWORD dwDeltaTime);

	//	Set settings
	void SetSimpleWaterFlag(bool bTrue);
	void SetRenderWaterFlag(bool bTrue);
	void SetViewDist(int iDist);
	void SetNewObjectOpt(int iOpt) { m_iNewObjOpt = iOpt; }
	void SetWeather(int iWeather);
	void SetCameraSpeed(float fSpeed) { m_fCameraSpeed = fSpeed; }
	void SetCameraFOV(float fFOV);

	//	Get settings
	bool GetSimpleWaterFlag() { return m_bSimpleWater; }
	bool GetRenderWaterFlag() { return m_bRenderWater; }
	int GetViewDist() { return m_iViewDist; }
	int GetNewObjectOpt() { return m_iNewObjOpt; }
	int GetWeather() { return m_iWeather; }
	float GetCameraSpeed() { return m_fCameraSpeed; }
	float GetCameraFOV() { return m_fCameraFOV; }

	CColorShader& GetCurFogCol() { return m_FogCol; }

protected:	//	Attributes

	bool	m_bSimpleWater;		//	Simple water flag
	bool	m_bRenderWater;		//	Render water flag
	int		m_iViewDist;		//	View distance
	int		m_iNewObjOpt;		//	'New Object' operation
	int		m_iWeather;			//	Current weather
	float	m_fCameraSpeed;		//	Camera moving speed
	float	m_fCameraFOV;		//	Camera FOV

	CColorShader	m_FogCol;	//	Fog color above water

protected:	//	Operations

	//	Get fog color
	DWORD GetFogColor(bool bUnderWater);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

