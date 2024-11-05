//	NPC.h

#pragma once

#include <AList2.h>
#include <A3DVector.h>

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

class A3DSkinModel;
class A3DViewport;
class A3DAABB;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CNPC
//	
///////////////////////////////////////////////////////////////////////////

class CNPC
{
public:		//	Types

	//	State ID
	enum
	{
		STATE_NONE = 0,
		STATE_STAND,
		STATE_WALK,
	};

public:		//	Constructor and Destructor

	CNPC();
	virtual ~CNPC();
	
public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(const char* szModelFile, const A3DVECTOR3& vPos, const A3DVECTOR3& vDir);
	//	Release object
	void Release();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(A3DViewport* pViewport);
	
	//	Set position
	void SetPos(const A3DVECTOR3& vPos);
	//	Set absolute forward and up direction
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Get NPC aabb
	const A3DAABB& GetAABB();

protected:	//	Attributes

	A3DSkinModel*	m_pSkinModel;	//	Skin model object

	int		m_iState;		//	State ID
	int		m_iStTimeCnt;	//	State time counter

protected:	//	Operations

	//	Load model
	bool LoadModel(const char* szFile);

	//	Enter state
	void EnterState(int iState);
	//	State tick
	void Tick_StateStand(DWORD dwDeltaTime);
	void Tick_StateWalk(DWORD dwDeltaTime);
	
	//	Render with bright edge
	bool RenderWithEdge(A3DViewport* pViewport);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CNPCMan
//	
///////////////////////////////////////////////////////////////////////////

class CNPCMan
{
public:		//	Types

public:		//	Constructor and Destructor

	CNPCMan();
	virtual ~CNPCMan();
	
public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init();
	//	Release object
	void Release();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(A3DViewport* pViewport);

	//	Create a NPC at specified position
	bool CreateNPC(const A3DVECTOR3& vPos, const char* szModelFile);

	//	Get NPC list
	APtrList<CNPC*>& GetNPCList() { return m_NPCList; }

protected:	//	Attributes

	APtrList<CNPC*>		m_NPCList;		//	NPC list

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

