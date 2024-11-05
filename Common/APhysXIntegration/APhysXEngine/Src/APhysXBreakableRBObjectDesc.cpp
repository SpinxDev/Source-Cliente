
#include "APhysXStdAfx.h"


//-----------------------------------------------------------------------------------------
// APhysXSimpleBreakableSkeletonRBObjectDesc

Begin_Save(APhysXSimpleBreakableSkeletonRBObjectDesc, APhysXSkeletonRBObjectDesc)

	pNxStream->storeFloat(mBreakLimit);

End_Save()


Begin_Load(APhysXSimpleBreakableSkeletonRBObjectDesc, APhysXSkeletonRBObjectDesc)

	if(m_StreamObjVer >= 0xcc000001)
	{
		mBreakLimit = pNxStream->readFloat();		
	}


End_Load()




//-----------------------------------------------------------------------------------------
// APhysXBreakableSkeletonRBObjectDesc

Begin_Save(APhysXBreakableSkeletonRBObjectDesc, APhysXSkeletonRBObjectDesc)

	// save the neighbor and support graph
	if(m_pActorNeighborGraph)
	{
		pNxStream->storeDword(1);
		m_pActorNeighborGraph->Save(pNxStream);
	}
	else
		pNxStream->storeDword(0);


	if(m_pActorSupportGraph)
	{
		pNxStream->storeDword(1);
		m_pActorSupportGraph->Save(pNxStream);
	}
	else
		pNxStream->storeDword(0);

	
	pNxStream->storeFloat(mActorBreakLimit);
	pNxStream->storeFloat(mBreakFallOffFactor);


End_Save()


Begin_Load(APhysXBreakableSkeletonRBObjectDesc, APhysXSkeletonRBObjectDesc)

	if(m_StreamObjVer >= 0xcc000001)
	{
		if(pNxStream->readDword())
		{
			m_pActorNeighborGraph = new APhysXGraph;
			if(!m_pActorNeighborGraph->Load(pNxStream))
			{
				delete m_pActorNeighborGraph;
				m_pActorNeighborGraph = NULL;
				return false;
			}
			m_pActorNeighborGraph->AddRef();
		}
		
		if(pNxStream->readDword())
		{
			m_pActorSupportGraph = new APhysXGraph;
			if(!m_pActorSupportGraph->Load(pNxStream))
			{
				delete m_pActorSupportGraph;
				m_pActorSupportGraph = NULL;
				return false;
			}
			m_pActorSupportGraph->AddRef();
		}

		mActorBreakLimit = pNxStream->readFloat();
		mBreakFallOffFactor = pNxStream->readFloat();
	}

End_Load()


const APhysXBreakableSkeletonRBObjectDesc& APhysXBreakableSkeletonRBObjectDesc::operator= (const APhysXBreakableSkeletonRBObjectDesc& other)
{
	if(this == &other) return *this;

	// release old data...
	Release();

	// clone parents' data
	(APhysXSkeletonRBObjectDesc&) *this = (APhysXSkeletonRBObjectDesc&) other;

	SetActorNeighborGraph(other.m_pActorNeighborGraph);
	SetActorSupportGraph(other.m_pActorSupportGraph);

	mActorBreakLimit = other.mActorBreakLimit;
	mBreakFallOffFactor = other.mBreakFallOffFactor;

	return *this;
}


void APhysXBreakableSkeletonRBObjectDesc::SetActorNeighborGraph(APhysXGraph* pActorNeighborGraph)
{
	// remove the reference first...
	if(m_pActorNeighborGraph)
	{
		m_pActorNeighborGraph->RemoveRef();
	}

	m_pActorNeighborGraph = pActorNeighborGraph;
	if(m_pActorNeighborGraph) m_pActorNeighborGraph->AddRef();
}

void APhysXBreakableSkeletonRBObjectDesc::SetActorSupportGraph(APhysXGraph* pActorSupportGraph)
{
	// remove the reference first...
	if(m_pActorSupportGraph)
	{
		m_pActorSupportGraph->RemoveRef();
	}

	m_pActorSupportGraph = pActorSupportGraph;
	if(m_pActorSupportGraph) m_pActorSupportGraph->AddRef();
}


void APhysXBreakableSkeletonRBObjectDesc::Release()
{
	if(m_pActorNeighborGraph)
	{
		m_pActorNeighborGraph->RemoveRef();
		m_pActorNeighborGraph = NULL;
	}

	if(m_pActorSupportGraph)
	{
		m_pActorSupportGraph->RemoveRef();
		m_pActorSupportGraph = NULL;
	}


}