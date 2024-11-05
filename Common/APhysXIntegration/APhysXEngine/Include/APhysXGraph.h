/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008


	APhysXGraph: define some light-weight Graph data structure stuff

----------------------------------------------------------------------*/

#ifndef	_APHYSXGRAPH_H_
#define _APHYSXGRAPH_H_

// typedef APhysXArray<APhysXU32> APhysXIDArray;
typedef APhysXArray<int> APhysXIDArray;

class APHYSX_DLL_EXPORT APhysXGraphVertex : public APhysXSerializerBase
{
	DeclareVersion(0xcc000001);

public:

	APhysXGraphVertex()
	{	
		mData = NULL;
	}

	//----------------------------------------------
	// data member...
	
	void* mData;							// runtime data which will not be saved to (/loaded from) the stream
	APhysXIDArray mNeighbors;
};


//-----------------------------------------------------------------------
// both directed/undirected graphs are use this common interface

class APHYSX_DLL_EXPORT APhysXGraph : public APhysXRefCountable, public APhysXSerializerBase
{

	DeclareVersion(0xcc000001);

public:

	APhysXGraph()
	{
		m_Vertices = NULL;
		m_iVertexNum = 0;
	}
	virtual ~APhysXGraph()
	{
		Release();
	}
	
	APhysXGraph(int iVertexNum)
	{
		m_Vertices = new APhysXGraphVertex [iVertexNum];
		m_iVertexNum = iVertexNum;
	}


	void Init(int iVertexNum)
	{
		if(m_Vertices) delete [] m_Vertices;

		m_Vertices = new APhysXGraphVertex [iVertexNum];
		m_iVertexNum = iVertexNum;
	}

	void Release()
	{
		if(m_Vertices) delete [] m_Vertices;
		m_iVertexNum = 0;
	}


	bool CheckVID(int vid)
	{
		return (vid >= 0) && ( vid < m_iVertexNum);
	}

	void SetVertexData(int vid, void* vData)
	{
		if(m_Vertices && CheckVID(vid))
		{
			m_Vertices[vid].mData = vData;
		}
	}

	void* GetVertexData(int vid)
	{
		if(m_Vertices && CheckVID(vid))
		{
			return m_Vertices[vid].mData;
		}
		else
			return NULL;
	}

	void SetVertexNeighbors(int vid, const APhysXIDArray& vNeighbors)
	{
		if(m_Vertices && CheckVID(vid))
		{
			m_Vertices[vid].mNeighbors = vNeighbors;
		}

	}

	void GetVertexNeighbors(int vid, APhysXIDArray& vNeighbors)
	{
		if(m_Vertices && CheckVID(vid))
		{
			vNeighbors = m_Vertices[vid].mNeighbors;
		}
	}


	const APhysXGraph& operator= (const APhysXGraph& other);

protected:

	APhysXGraphVertex* m_Vertices;
	int m_iVertexNum;
};


#endif