/*
 * FILE: MAXNullView.h
 *
 * DESCRIPTION: 3DMAX null view class
 *
 * CREATED BY: duyuxin, 2003/9/16
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _MAXNULLVIEW_H_
#define _MAXNULLVIEW_H_

#include "max.h"

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
//	Class CMAXNullView
//
///////////////////////////////////////////////////////////////////////////

class CMAXNullView : public View
{
public:		//	Types

public:		//	Constructions and Destructions

	CMAXNullView() {}
	virtual ~CMAXNullView() {}

public:		//	Attributes

public:		//	Operaitons

	Point2 ViewToScreen(Point3 p) { return Point2(p.x, p.y); }

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_MAXNULLVIEW_H_
