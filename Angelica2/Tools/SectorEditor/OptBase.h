/*
 * FILE: OptBase.h
 *
 * DESCRIPTION: Operation base class
 *
 * CREATED BY: Duyuxin, 2003/7/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _OPTBASE_H_
#define _OPTBASE_H_

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Operation ID
enum
{
	OPT_NONE = 0,			//	None
	OPT_CREATESECTOR,		//	Create sector
	OPT_SIZESECTOR,			//	Size sector
	OPT_CREATEPORTAL,		//	Create portal
	OPT_SIZEPORTAL,			//	Size portal
	OPT_MOVEOBJECT,			//	Move object
	OPT_LINKSECTOR,			//	Link sector
	OPT_SNAPPORTAL,			//	Snap portal to sector
	OPT_ZOOMVIEWPORT,		//	Zoom viewport
	OPT_CREATESIDEPORTAL,	//	Create portal on sector side
	OPT_NAMEPORTAL,			//	Name portal automatically
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class CViewportWnd;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class COperation
//
///////////////////////////////////////////////////////////////////////////

class COperation
{
public:		//	Types

	//	Operation states
	enum
	{
		STATE_READY		= 0,	//	Ready to begin
		STATE_DOING		= 1,	//	Doing
		STATE_FINISHED	= 2,	//	Finished
		STATE_CANCELED	= 3		//	Canceled	
	};

public:		//	Constructor and Destructor

	COperation(int iID)
	{	
		m_iState	= STATE_READY;	
		m_iOptID	= iID;
		m_bMouseMsg = true;
		m_bFixView	= false;
	}

	virtual ~COperation() {}

public:		//	Attributes

public:		//	Operations

	//	Begin operation
	virtual bool Begin();
	//	End operation
	virtual bool End(bool bCancel);

	//	Tick routinue
	virtual bool FrameMove(DWORD dwTime) { return true; }
	//	Render routinue
	virtual bool Render(CViewportWnd* pView) { return true; }

	//	Mouse operations
	virtual void OnMouseMove(int x, int y, DWORD dwFlags) {}
	virtual void OnLButtonDown(int x, int y, DWORD dwFlags) {}
	virtual void OnRButtonDown(int x, int y, DWORD dwFlags) {}
	virtual void OnLButtonUp(int x, int y, DWORD dwFlags) {}
	virtual void OnRButtonUp(int x, int y, DWORD dwFlags) {}
	virtual void OnLButtonDbClk(int x, int y, DWORD dwFlags) {}
	virtual void OnRButtonDbClk(int x, int y, DWORD dwFlags) {}

	//	Get operation state
	int	GetState() { return m_iState; }
	//	Get operation ID
	int	GetOptID() { return m_iOptID; }
	//	Mouse message handling flag
	bool HandleMouseMsg() { return m_bMouseMsg; }
	//	Forbid viewport size change
	bool FixViewport() { return m_bFixView; }

protected:	//	Attributes

	int		m_iState;		//	Operation state
	int		m_iOptID;		//	Operation ID
	bool	m_bMouseMsg;	//	true, this operation will handle mouse operation
	bool	m_bFixView;		//	true, fix viewport

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_EDITORROUTE_H_



