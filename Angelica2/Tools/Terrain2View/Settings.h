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

	//	Block quad showing flag
	enum
	{
		BQS_OCCLUSION	= 0x0001,
		BQS_TEST		= 0x0002,
		BQS_PICKED		= 0x0004,	//	Picked by cursor
		BQS_ALL			= 0x0007,
	};

public:		//	Constructor and Destructor

	CSettings();
	virtual ~CSettings();
	
public:		//	Attributes

public:		//	Operations

	//	Set settings
	void SetViewDist(float fDist);
	void SetLODDist(float f1, float f2);
	void SetCameraSpeed(float fSpeed) { m_fCameraSpeed = fSpeed; }
	void SetBackColor(DWORD dwCol) { m_dwBackCol = dwCol; }
	void SetWireFrame(bool bTrue) { m_bWireFrame = bTrue; }
	void SetBlockQuadShow(int iType) { m_iBlockQuad = iType; }
	void SetHoriLineShow(bool bTrue) { m_bHoriLine = bTrue; }
	void SetTerrainRender(bool bTrue) { m_bRenderTrn = bTrue; }
	void SetHoriLineUse(bool bTrue);
	void SetPromptShow(bool bTrue) { m_bShowPrompt = bTrue; }
	void SetLODType(int iType);

	//	Get settings
	float GetViewDist() { return m_fViewDist; }
	float GetLODDist1() { return m_fLODDist1; }
	float GetLODDist2() { return m_fLODDist2; }
	float GetCameraSpeed() { return m_fCameraSpeed; }
	DWORD GetBackColor() { return m_dwBackCol; }
	bool GetWireFrame() { return m_bWireFrame; }
	int GetBlockQuadShow() { return m_iBlockQuad; }
	bool GetHoriLineShow() { return m_bHoriLine; }
	bool GetTerrainRender() { return m_bRenderTrn; }
	bool GetHoriLineUse() { return m_bUseHoriLine; }
	bool GetPromptShow() { return m_bShowPrompt; }
	int GetLODType() { return m_iLODType; }

protected:	//	Attributes

	float	m_fViewDist;		//	View distance
	float	m_fLODDist1;		//	LOD distance 1
	float	m_fLODDist2;		//	LOD distance 2
	float	m_fCameraSpeed;		//	Camera moving speed
	DWORD	m_dwBackCol;		//	Background color
	bool	m_bWireFrame;		//	true, wire frame mode
	int		m_iBlockQuad;		//	Block quad show type
	bool	m_bHoriLine;		//	true, show horizon occlusion line
	bool	m_bRenderTrn;		//	true, render terrain
	bool	m_bUseHoriLine;		//	true, use horizon algorithm
	bool	m_bShowPrompt;		//	true, draw prompt info
	int		m_iLODType;			//	Terrain LOD type

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

