//	RayTrace.cpp

#include <windows.h>
#include "RayTrace.h"
#include "Game.h"
#include "Render.h"
#include "NPC.h"

#include <AList2.h>
#include <A3DViewport.h>
#include <A3DTerrain2.h>
#include <A3DGeometry.h>
#include <A3DCamera.h>
#include <A3DCollision.h>

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
//	Implement CRayTrace
//	
///////////////////////////////////////////////////////////////////////////

CRayTrace::CRayTrace()
{
	m_iEntity	= ENT_NONE;
	m_fFaction	= 0.0f;
	m_dwObject	= 0;
}

//	Do ray trace
bool CRayTrace::DoRayTrace(int x, int y, DWORD dwFilter/* FILTER_ALL */)
{
	A3DVECTOR3 vDest((float)x, (float)y, 1.0f);
	A3DViewport* pView = g_Render.GetViewport();
	pView->InvTransform(vDest, vDest);

	A3DVECTOR3 vStart = g_Render.GetCamera()->GetPos();
	return DoRayTrace(vStart, vDest, dwFilter);
}

bool CRayTrace::DoRayTrace(const A3DVECTOR3& vFrom, const A3DVECTOR3& vTo, DWORD dwFilter/* FILTER_ALL */)
{
	float fMinFraction = 1.1f;
	A3DVECTOR3 vDelta = vTo - vFrom;

	if (dwFilter & FILTER_TERRAIN)
	{
		//	Trace with terrain
		A3DTerrain2* pTerrain = g_Game.GetTerrain();
		if (pTerrain)
		{
			RAYTRACERT TraceRt;

			if (pTerrain->RayTrace(vFrom, vDelta, 1.0f, &TraceRt) && 
				TraceRt.fFraction < fMinFraction)
			{
				fMinFraction	= TraceRt.fFraction;

				m_fFaction		= TraceRt.fFraction;
				m_vHitNormal	= TraceRt.vNormal;
				m_iEntity		= ENT_TERRAIN;
				m_dwObject		= 0;
			}
		}
	}

	if (dwFilter & FILTER_NPC)
	{
		A3DVECTOR3 vTempPos, vTempNormal;
		float fTempFrac;

		//	Trace NPCs
		APtrList<CNPC*>& NPCList = g_Game.GetNPCMan()->GetNPCList();
		ALISTPOSITION pos = NPCList.GetHeadPosition();
		while (pos)
		{
			CNPC* pNPC = NPCList.GetNext(pos);
			const A3DAABB& aabb = pNPC->GetAABB();
			if (CLS_RayToAABB3(vFrom, vDelta, aabb.Mins, aabb.Maxs, vTempPos, &fTempFrac, vTempNormal) &&
				fTempFrac < fMinFraction)
			{
				fMinFraction	= fTempFrac;

				m_fFaction		= fTempFrac;
				m_vHitNormal	= vTempNormal;
				m_iEntity		= ENT_NPC;
				m_dwObject		= (DWORD)pNPC;
			}
		}
	}

	if (fMinFraction != 1.1f)
	{
		m_vHitPos = vFrom + vDelta * m_fFaction;
		return true;
	}

	return false;
}

