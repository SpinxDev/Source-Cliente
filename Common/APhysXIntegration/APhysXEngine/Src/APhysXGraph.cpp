
#include "APhysXStdAfx.h"


//------------------------------------------------------------------
// APhysXGraphVertex's member functions

Begin_Save(APhysXGraphVertex, APhysXSerializerBase)

	pNxStream->storeDword(mNeighbors.size());

	for(unsigned int i =0; i < mNeighbors.size(); i++)
	{
		pNxStream->storeDword(mNeighbors[i]);	
	}


End_Save()


Begin_Load(APhysXGraphVertex, APhysXSerializerBase)

	if(m_StreamObjVer >= 0xcc000001)
	{
		mNeighbors.clear();
		int iNeighborNum = pNxStream->readDword();

		for(int i =0; i < iNeighborNum; i++)
		{
			mNeighbors.push_back(pNxStream->readDword());	
		}

	}


End_Load()



//------------------------------------------------------------------
// APhysXGraph's member functions

Begin_Save(APhysXGraph, APhysXSerializerBase)

	pNxStream->storeDword(m_iVertexNum);

	for(int i =0; i < m_iVertexNum; i++)
	{
		if(!m_Vertices[i].Save(pNxStream))
			return false;
	}

End_Save()


Begin_Load(APhysXGraph, APhysXSerializerBase)

	if(m_StreamObjVer >= 0xcc000001)
	{

		m_iVertexNum = pNxStream->readDword();
		Init(m_iVertexNum);

		for(int i = 0; i < m_iVertexNum; i++)
		{
			if(!m_Vertices[i].Load(pNxStream))
				return false;
		}

	}


End_Load()


const APhysXGraph& APhysXGraph::operator= (const APhysXGraph& other)
{
	if(this == &other) return *this;

	// clone parents' data
	(APhysXSerializerBase& ) *this = (APhysXSerializerBase& )other;

	// Note here: do not copy the Reference Count info!!!
	// (APhysXRefCountable& ) *this = (APhysXRefCountable& )other;

	if(other.m_iVertexNum)
	{
		Init(other.m_iVertexNum);
		
		for(int i=0; i<m_iVertexNum; i++)
		{
			m_Vertices[i] = other.m_Vertices[i];
		}
	}
	else
		Release();

	return *this;
	
}