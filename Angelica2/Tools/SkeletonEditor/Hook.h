/*
 * FILE: Hook.h
 *
 * DESCRIPTION: Hook class
 *
 * CREATED BY: Duyuxin, 2003/10/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_HOOK_H_
#define _HOOK_H_

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

class CBox3D;
class CStaff;
class A3DSkeleton;
class A3DSkeletonHook;
class CRender;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CHook
//
///////////////////////////////////////////////////////////////////////////

class CHook
{
public:		//	Types

public:		//	Constructor and Destructor

	CHook();
	virtual ~CHook() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize box
	bool Init(CRender* pRender, A3DSkeletonHook* pA3DHook);
	//	Release box
	void Release();

	//	Frame move
	bool FrameMove(const A3DMATRIX4& matBoneToWorld);
	//	Render routine
	bool Render();

	//	Set box size and staff radius
	void SetBoxAndStaffSize(float fBoxSize, float fStaffRadius);

	//	Set / Get visible flag
	void SetVisibleFlag(bool bVisible) { m_bVisible = bVisible; }
	bool IsVisible() { return m_bVisible; }

	//	Set transform matrix
	void SetTM(const A3DMATRIX4& mat);

	//	Get A3D hook object
	A3DSkeletonHook* GetA3DHook() { return m_pA3DHook; }

protected:	//	Attributes

	CRender*			m_pRender;
	A3DSkeletonHook*	m_pA3DHook;

	bool		m_bVisible;		//	Visible flag
	CBox3D*		m_pBox;			//	Box represents coordinates origin
	CStaff*		m_aAxis[3];		//	Staffs represents coordinates axises

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_HOOK_H_
