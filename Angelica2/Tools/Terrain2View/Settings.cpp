//	Settings.cpp

#include "Global.h"
#include "Settings.h"
#include "MainFrm.h"
#include "Terrain2ViewDoc.h"
#include "Render.h"
#include "A3DTerrain2Wrapper.h"

#include <A3DTerrain2Cull.h>

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
//	Implement CSettings
//	
///////////////////////////////////////////////////////////////////////////

CSettings::CSettings()
{
	m_fViewDist		= 500.0f;
	m_fLODDist1		= 80.0f;
	m_fLODDist2		= 150.0f;
	m_fCameraSpeed	= 80.0f;
	m_dwBackCol		= 0xff000000;
	m_bWireFrame	= false;
	m_iBlockQuad	= BQS_ALL;
	m_bHoriLine		= true;
	m_bRenderTrn	= true;
	m_bUseHoriLine	= true;
	m_bShowPrompt	= true;
	m_iLODType		= 1;
}

CSettings::~CSettings()
{
}

//	Set view distance
void CSettings::SetViewDist(float fDist)
{
	m_fViewDist = fDist;

	A3DTerrain2Wrapper* pTerrain = glb_GetMainFrame()->GetDocument()->m_pA3DTerrain;
	if (pTerrain)
	{
		pTerrain->SetActRadius(m_fViewDist);
		pTerrain->SetViewRadius(m_fViewDist);
	}
}

//	Set LOD distance
void CSettings::SetLODDist(float f1, float f2)
{
	m_fLODDist1	= f1;
	m_fLODDist2	= f2;

	A3DTerrain2Wrapper* pTerrain = glb_GetMainFrame()->GetDocument()->m_pA3DTerrain;
	if (pTerrain)
		pTerrain->SetLODDist(f1, f2);
}

void CSettings::SetLODType(int iType)
{ 
	m_iLODType = iType;

	A3DTerrain2Wrapper* pTerrain = glb_GetMainFrame()->GetDocument()->m_pA3DTerrain;
	if (pTerrain)
		pTerrain->SetLODType((A3DTerrain2::LODTYPE)iType);
}

void CSettings::SetHoriLineUse(bool bTrue)
{ 
	m_bUseHoriLine = bTrue;

	A3DTerrain2Wrapper* pTerrain = glb_GetMainFrame()->GetDocument()->m_pA3DTerrain;
	if (pTerrain)
	{
		A3DTerrain2Cull* pTrnCuller = pTerrain->GetTerrainCuller();
		if (pTrnCuller)
			pTrnCuller->Enable(bTrue);
	}
}



