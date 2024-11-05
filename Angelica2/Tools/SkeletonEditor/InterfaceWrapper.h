/*
 * FILE: InterfaceWrapper.h
 *
 * DESCRIPTION:
 *
 * CREATED BY: duyuxin, 2006/7/11
 *
 * HISTORY:
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#ifndef _INTERFACEWRAPPER_H_
#define _INTERFACEWRAPPER_H_

#include <A3DSkeleton.h>
#include <A3DSkin.h>
#include <A3DTrackMan.h>
#include <A3DSkinModel.h>

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
//	Class CSkinModel
//
///////////////////////////////////////////////////////////////////////////

class CSkinModel : public A3DSkinModel
{
public:		//	Types

public:		//	Constructions and Destructions

	CSkinModel();
	virtual ~CSkinModel() {}

public:		//	Attributes

public:		//	Operaitons

	//	Update routine
	bool FrameUpdate(int iDeltaTime);

	//	Set version
	void SetVersion(DWORD dwVer) { m_dwVersion = dwVer; }
	//	Set one step flag
	void SetOneStep(bool bTrue) { m_bOneStep = bTrue; }
	//	Get one step flag
	bool GetOneStep() { return m_bOneStep; }
	//	Set current frame
	void SetCurFrame(float fFrame) { m_fCurFrame = fFrame; }
	//	Get current frame
	float GetCurFrame() { return m_fCurFrame; }
	//	Step frame
	void StepFrame(bool bInc, const char* szAction);
	//	Step frame to specified frame
	void StepFrameTo(float fFrame, const char* szAction);

	//	Get initial skin mesh obb
	const A3DOBB& GetInitMeshOBB() { return m_obbInitMesh; }

protected:	//	Attributes

	bool	m_bOneStep;		//	true, on step playing
	float	m_fCurFrame;	//	Current frame
	
protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class CSkeletonWrapper
//
///////////////////////////////////////////////////////////////////////////

class CSkeletonWrapper : public A3DSkeleton
{
public:		//	Types

public:		//	Constructions and Destructions

	CSkeletonWrapper() {}
	virtual ~CSkeletonWrapper() {}

public:		//	Attributes

public:		//	Operaitons

	//	Initialize object
	bool Init();
	//	Release object
	void Release();

	//	Load skeleton data form file
	bool Load(const char* szFile);

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class CSkinWrapper
//
///////////////////////////////////////////////////////////////////////////

class CSkinWrapper : public A3DSkin
{
public:		//	Types

public:		//	Constructions and Destructions

	CSkinWrapper() {}
	virtual ~CSkinWrapper() {}

public:		//	Attributes

public:		//	Operaitons

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release object
	void Release();

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class CTrackSetWrapper
//
///////////////////////////////////////////////////////////////////////////

class CTrackSetWrapper : public A3DSklTrackSet
{
public:		//	Types

public:		//	Constructions and Destructions

	CTrackSetWrapper() : A3DSklTrackSet(false) {}
	virtual ~CTrackSetWrapper() {}

public:		//	Attributes

public:		//	Operaitons

	//	Create a empty bone track
	BONETRACK* CreateBoneTrack(int id);

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_INTERFACEWRAPPER_H_

