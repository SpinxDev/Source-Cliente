 /*
 * FILE: MaxExFunc.h
 *
 * DESCRIPTION: my Max functions to used in max plug-in
 *
 * CREATED BY: Hedi, 2004/7/3
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _MAXEXFUNC_H_
#define _MAXEXFUNC_H_

#include "Max.h"

inline Point3 GetMatrixScale(const Matrix3& mat)
{
	Point3 scale;
	scale.x = mat.GetRow(0).Length();
	scale.y = mat.GetRow(1).Length();
	scale.z = mat.GetRow(2).Length();
	return scale;
}

#endif
