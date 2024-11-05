/*
 * FILE: A3DTypes.cpp
 * 
 * DESCRIPTION: Implements the overloads of some structures
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include <math.h>
#include "A3DTypes.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"

BOOL KeepOrthogonal(A3DMATRIX4 mat)
{
	FLOAT vDot;
	FLOAT vNormal;
	FLOAT error = 1e-5f;

	A3DVECTOR3 x = A3DVECTOR3(mat._11, mat._12, mat._13);
	A3DVECTOR3 y = A3DVECTOR3(mat._21, mat._22, mat._23);
	A3DVECTOR3 z = A3DVECTOR3(mat._31, mat._32, mat._33);

	vNormal = Magnitude(x);
	if( fabs(fabs(vNormal) - 1.0f) > error )
		return FALSE;

	vNormal = Magnitude(y);
	if( fabs(fabs(vNormal) - 1.0f) > error )
		return FALSE;

	vNormal = Magnitude(z);
	if( fabs(fabs(vNormal) - 1.0f) > error )
		return FALSE;

	vDot = DotProduct(x, y);
	if( fabs(vDot) > error )
		return FALSE;

	vDot = DotProduct(y, z);
	if( fabs(vDot) > error )
		return FALSE;

	vDot = DotProduct(x, z);
	if( fabs(vDot) > error )
		return FALSE;
	
	vDot = DotProduct(CrossProduct(x, y), z);
	if( fabs(fabs(vDot) - 1.0f) > error )
		return FALSE;

	return TRUE;
}

A3DVECTOR3 Normalize(const A3DVECTOR3& v)
{
	FLOAT mag = Magnitude(v);
	if( mag < 1e-12	&& mag > -1e-12 )
		return A3DVECTOR3(0.0f);
	else
		return v / mag;
}

FLOAT Normalize(const A3DVECTOR3& v, A3DVECTOR3& vOut)
{
	FLOAT mag = Magnitude(v);
	if( mag < 1e-12	&& mag > -1e-12 )
	{
		vOut = A3DVECTOR3(0.0f);
		return 0;
	}
	else
	{
		vOut = v / mag;
		return mag;
	}
}

//Matrix mul with vector3 and vector4;
A3DVECTOR3 operator * (const A3DMATRIX4& mat, const A3DVECTOR3& v)
{
	A3DVECTOR3 result;
	FLOAT w;

	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43;
	w = 1.0f / (v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + mat._44);
	result.x *= w; result.y *= w; result.z *= w;
	return result;
}

A3DVECTOR3 operator * (const A3DVECTOR3& v, const A3DMATRIX4& mat)
{
	A3DVECTOR3 result;
	FLOAT w;

	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43;
	w = 1.0f / (v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + mat._44);
	
	result.x *= w; result.y *= w; result.z *= w;
	return result;
}

A3DVECTOR4 operator * (const A3DMATRIX4& mat, const A3DVECTOR4& v)
{
	A3DVECTOR4 result;

	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + v.w * mat._41;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + v.w * mat._42;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + v.w * mat._43;
	result.w = v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + v.w * mat._44;
	
	return result;	
}

A3DVECTOR4 operator * (const A3DVECTOR4& v, const A3DMATRIX4& mat)
{
	A3DVECTOR4 result;

	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + v.w * mat._41;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + v.w * mat._42;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + v.w * mat._43;
	result.w = v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + v.w * mat._44;
	
	return result;	
}

A3DVECTOR3 operator * (const A3DMATRIX3& mat, const A3DVECTOR3& v)
{
	A3DVECTOR3 result;

	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33;
	
	return result;	
}

A3DVECTOR3 operator * (const A3DVECTOR3& v, const A3DMATRIX3& mat)
{
	A3DVECTOR3 result;

	result.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31;
	result.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32;
	result.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33;
	
	return result;	
}

//Matrix mul and div a scalar
A3DMATRIX4 operator * (const A3DMATRIX4& mat, FLOAT s)
{
	A3DMATRIX4 result;

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			result.m[i][j] = mat.m[i][j] * s;

	return result;
}

A3DMATRIX4 operator / (const A3DMATRIX4& mat, FLOAT s)
{
	A3DMATRIX4 result;
	s = 1.0f / s;

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			result.m[i][j] = mat.m[i][j] * s;

	return result;
}

//Matrix add, sub, and mul with matrix;
A3DMATRIX4 operator + (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2)
{
	A3DMATRIX4 result;

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			result.m[i][j] = mat1.m[i][j] + mat2.m[i][j];
	
	return result;
}

A3DMATRIX4 operator - (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2)
{
	A3DMATRIX4 result;

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			result.m[i][j] = mat1.m[i][j] - mat2.m[i][j];
	
	return result;
}

A3DMATRIX4 operator * (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2)
{
	A3DMATRIX4	result;

	//KeepOrthogonal(mat1);
	//KeepOrthogonal(mat2);
	memset(&result, 0, sizeof(result));
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			for(int k=0; k<4; k++)
				result.m[i][j] += mat1.m[i][k] * mat2.m[k][j];

	//KeepOrthogonal(result);
	return result;
}

A3DMATRIX3 IdentityMatrix3()
{
	A3DMATRIX3 result;
    memset(&result, 0, sizeof(result));
    result._11 = result._22 = result._33 = 1.0f;
    return result;
}

A3DMATRIX4 IdentityMatrix()
{
    A3DMATRIX4 result;
    memset(&result, 0, sizeof(result));
    result._11 = result._22 = result._33 = result._44 = 1.0f;
    return result;
}

A3DMATRIX4 ZeroMatrix()
{
    A3DMATRIX4 result;
    memset(&result, 0, sizeof(result));
    return result;
}

A3DMATRIX4 ViewMatrix(const A3DVECTOR3& from, const A3DVECTOR3& dir, 
					  const A3DVECTOR3& vecUp, FLOAT roll)
{
    A3DMATRIX4 view = IdentityMatrix();
    A3DVECTOR3 up, right, view_dir;

    view_dir = Normalize(dir);
	right = CrossProduct(vecUp, dir);
    right = Normalize(right);
	up = Normalize(vecUp);
	
    view.m[0][0] = right.x;
    view.m[1][0] = right.y;
    view.m[2][0] = right.z;
    view.m[0][1] = up.x;
    view.m[1][1] = up.y;
    view.m[2][1] = up.z;
    view.m[0][2] = view_dir.x;
    view.m[1][2] = view_dir.y;
    view.m[2][2] = view_dir.z;
	
    view.m[3][0] = -DotProduct(right, from);
    view.m[3][1] = -DotProduct(up, from);
    view.m[3][2] = -DotProduct(view_dir, from);

	// Set roll
	if (roll != 0.0f) {
		view = RotateZ(-roll) * view;
	}

    return view;
} 

/*
A3DMATRIX4 InverseTM(A3DMATRIX4 tm)
{
	//We should check if tm is a unified transform matrix;
	//that is to say: each row's magnitude must be 1.0f;
	A3DMATRIX4 matInv = IdentityMatrix();

	//X axis;
	matInv._11 = tm._11;
	matInv._21 = tm._12;
	matInv._31 = tm._13;

	//Y axis;
	matInv._12 = tm._21;
	matInv._22 = tm._22;
	matInv._32 = tm._23;

	//Z axis;
	matInv._13 = tm._31;
	matInv._23 = tm._32;
	matInv._33 = tm._33;

	//Offset;
	matInv._41 = -(tm._41 * tm._11 + tm._42 * tm._12 + tm._43 * tm._13);
	matInv._42 = -(tm._41 * tm._21 + tm._42 * tm._22 + tm._43 * tm._23);
	matInv._43 = -(tm._41 * tm._31 + tm._42 * tm._32 + tm._43 * tm._33);

	return matInv;
}*/

FLOAT Det(FLOAT a11, FLOAT a12, FLOAT a13, 
        FLOAT a21, FLOAT a22, FLOAT a23,
        FLOAT a31, FLOAT a32, FLOAT a33)
{
    return a11 * a22 * a33 + a21 * a32 * a13 + a31 * a12 * a23 -
        a13 * a22 * a31 - a23 * a32 * a11 - a33 * a12 * a21;
}

A3DMATRIX4 InverseTM(A3DMATRIX4 mat)
{
	FLOAT vDet;
	A3DMATRIX4 ret;

	vDet = 1.0f / Det(
        mat._11, mat._12, mat._13,
        mat._21, mat._22, mat._23,
        mat._31, mat._32, mat._33);

    ret._11 = vDet * Det(
        mat._22, mat._23, mat._24,
        mat._32, mat._33, mat._34,
        mat._42, mat._43, mat._44);
    ret._12 = -vDet * Det(
        mat._12, mat._13, mat._14,
        mat._32, mat._33, mat._34,
        mat._42, mat._43, mat._44);
    ret._13 = vDet * Det(
        mat._12, mat._13, mat._14,
        mat._22, mat._23, mat._24,
        mat._42, mat._43, mat._44);
    ret._14 = -vDet * Det(
        mat._12, mat._13, mat._14,
        mat._22, mat._23, mat._24,
        mat._32, mat._33, mat._34);

    ret._21 = -vDet * Det(
        mat._21, mat._23, mat._24,
        mat._31, mat._33, mat._34,
        mat._41, mat._43, mat._44);
    ret._22 = vDet * Det(
        mat._11, mat._13, mat._14,
        mat._31, mat._33, mat._34,
        mat._41, mat._43, mat._44);
    ret._23 = -vDet * Det(
        mat._11, mat._13, mat._14,
        mat._21, mat._23, mat._24,
        mat._41, mat._43, mat._44);
    ret._24 = vDet * Det(
        mat._11, mat._13, mat._14,
        mat._21, mat._23, mat._24,
        mat._31, mat._33, mat._34);

    ret._31 = vDet * Det(
        mat._21, mat._22, mat._24,
        mat._31, mat._32, mat._34,
        mat._41, mat._42, mat._44);
    ret._32 = -vDet * Det(
        mat._11, mat._12, mat._14,
        mat._31, mat._32, mat._34,
        mat._41, mat._42, mat._44);
    ret._33 = vDet * Det(
        mat._11, mat._12, mat._14,
        mat._21, mat._22, mat._24,
        mat._41, mat._42, mat._44);
    ret._34 = -vDet * Det(
        mat._11, mat._12, mat._14,
        mat._21, mat._22, mat._24,
        mat._31, mat._32, mat._34);

	ret._41 = -vDet * Det(
        mat._21, mat._22, mat._23,
        mat._31, mat._32, mat._33,
        mat._41, mat._42, mat._43);
    ret._42 = vDet * Det(
        mat._11, mat._12, mat._13,
        mat._31, mat._32, mat._33,
        mat._41, mat._42, mat._43);
    ret._43 = -vDet * Det(
        mat._11, mat._12, mat._13,
        mat._21, mat._22, mat._23,
        mat._41, mat._42, mat._43);
    ret._44 = vDet * Det(
        mat._11, mat._12, mat._13,
        mat._21, mat._22, mat._23,
        mat._31, mat._32, mat._33);
    return ret;
}

A3DMATRIX4 TransformMatrix(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp, const A3DVECTOR3& vecPos)
{
    A3DMATRIX4   mat;
    A3DVECTOR3   vecXAxis, vecYAxis, vecZAxis;

    vecZAxis = Normalize( vecDir );
    vecYAxis = Normalize( vecUp );
    vecXAxis = CrossProduct( vecYAxis, vecZAxis );

    memset(&mat, 0, sizeof(mat));
    mat.m[0][0] = vecXAxis.x;
    mat.m[0][1] = vecXAxis.y;
    mat.m[0][2] = vecXAxis.z;

    mat.m[1][0] = vecYAxis.x;
    mat.m[1][1] = vecYAxis.y;
    mat.m[1][2] = vecYAxis.z;

    mat.m[2][0] = vecZAxis.x;
    mat.m[2][1] = vecZAxis.y;
    mat.m[2][2] = vecZAxis.z;

    mat.m[3][0] = vecPos.x;
    mat.m[3][1] = vecPos.y;
    mat.m[3][2] = vecPos.z;
    mat.m[3][3] = 1.0f;

    return mat;
}

A3DMATRIX4 RotateX(FLOAT vRad)
{
    A3DMATRIX4 ret = IdentityMatrix();
    ret.m[2][2] = ret.m[1][1] = (FLOAT) cos(vRad);
    ret.m[1][2] = (FLOAT) sin(vRad);
    ret.m[2][1] = (FLOAT) -ret.m[1][2];

	//KeepOrthogonal(ret);
	return ret;
}

A3DMATRIX4 RotateX(const A3DMATRIX4& mat, FLOAT vRad)
{
    return mat * RotateX(vRad);
}

A3DMATRIX4 RotateY(FLOAT vRad)
{
    A3DMATRIX4 ret = IdentityMatrix();
    ret.m[2][2] = ret.m[0][0] = (FLOAT) cos(vRad);
    ret.m[2][0] = (FLOAT) sin(vRad);
    ret.m[0][2] = -ret.m[2][0];
	//KeepOrthogonal(ret);
    return ret;
}

A3DMATRIX4 RotateY(const A3DMATRIX4& mat, FLOAT vRad)
{
    return mat * RotateY(vRad);
}

A3DMATRIX4 RotateZ(FLOAT vRad)
{
    A3DMATRIX4 ret = IdentityMatrix();
    ret.m[1][1] = ret.m[0][0] = (FLOAT) cos(vRad);
    ret.m[0][1] = (FLOAT) sin(vRad);
    ret.m[1][0] = -ret.m[0][1];
	//KeepOrthogonal(ret);
    return ret;
}

A3DMATRIX4 RotateZ(const A3DMATRIX4& mat, FLOAT vRad)
{
    return mat * RotateZ(vRad);
}

A3DMATRIX4 Scaling(FLOAT sx, FLOAT sy, FLOAT sz)
{
    A3DMATRIX4 ret = ZeroMatrix();
    ret.m[0][0] = sx;
    ret.m[1][1] = sy;
    ret.m[2][2] = sz;
    ret.m[3][3] = 1.0f;
    return ret;
}

A3DMATRIX4 Scaling(const A3DMATRIX4& mat, FLOAT sx, FLOAT sy, FLOAT sz)
{
    A3DMATRIX4 ret = mat;
    ret.m[0][0] *= sx;
    ret.m[0][1] *= sx;
    ret.m[0][2] *= sx;
    ret.m[0][3] *= sx;

    ret.m[1][0] *= sy;
    ret.m[1][1] *= sy;
    ret.m[1][2] *= sy;
    ret.m[1][3] *= sy;

    ret.m[2][0] *= sz;
    ret.m[2][1] *= sz;
    ret.m[2][2] *= sz;
    ret.m[2][3] *= sz;
    return ret;
}

A3DMATRIX4 Translate(FLOAT x, FLOAT y, FLOAT z)
{
	A3DMATRIX4 ret = IdentityMatrix();

	ret._41 = x;
	ret._42 = y;
	ret._43 = z;

	return ret;
}

A3DCOLOR ColorValueToColorRGBA(A3DCOLORVALUE colorValue)
{
	BYTE r, g, b, a;

	a = (BYTE)(colorValue.a * 255);
	r = (BYTE)(colorValue.r * 255);
	g = (BYTE)(colorValue.g * 255);
	b = (BYTE)(colorValue.b * 255);

	return A3DCOLORRGBA(r, g, b, a);
}

A3DCOLORVALUE ColorRGBAToColorValue(A3DCOLOR colorRGBA)
{
	BYTE r, g, b, a;
	a = A3DCOLOR_GETALPHA(colorRGBA);
	r = A3DCOLOR_GETRED(colorRGBA);
	g = A3DCOLOR_GETGREEN(colorRGBA);
	b = A3DCOLOR_GETBLUE(colorRGBA);

	FLOAT fTemp = 1.0f / 255.0f;

	return A3DCOLORVALUE(r * fTemp, g * fTemp, b * fTemp, a * fTemp);
}

A3DVECTOR3 RandDirH()
{
	A3DVECTOR3 vecDirH;

	float vRad = (rand() % 10000) / 10000.0f * 2.0f * A3D_PI;
	
	vecDirH.x = (FLOAT) cos(vRad);
	vecDirH.y = 0.0f;
	vecDirH.z = (FLOAT) sin(vRad);

	return vecDirH;
}

A3DMATRIX4 MirrorMatrix(const A3DVECTOR3& p, // IN: point on the plane
						const A3DVECTOR3& n) // IN: plane perpendicular direction
{
	A3DMATRIX4 ret;

	//V' = V - 2((V - P)*N)N)
	FLOAT dot = p.x*n.x + p.y*n.y + p.z*n.z;
	
	ret.m[0][0] = 1 - 2*n.x*n.x;
	ret.m[1][0] = - 2*n.x*n.y;
	ret.m[2][0] = - 2*n.x*n.z;
	ret.m[3][0] = 2*dot*n.x;

	ret.m[0][1] = - 2*n.y*n.x;
	ret.m[1][1] = 1 - 2*n.y*n.y;
	ret.m[2][1] = - 2*n.y*n.z;
	ret.m[3][1] = 2*dot*n.y;

	ret.m[0][2] = - 2*n.z*n.x;
	ret.m[1][2] = - 2*n.z*n.y;
	ret.m[2][2] = 1 - 2*n.z*n.z;
	ret.m[3][2] = 2*dot*n.z;

	ret.m[0][3] = 0;
	ret.m[1][3] = 0;
	ret.m[2][3] = 0;
	ret.m[3][3] = 1;

	return ret;
}

//	Set plane's properties.
void IdentifyPlane(A3DPLANE* pPlane)
{
	pPlane->byType = 0;

	if (pPlane->vNormal.x == 1.0f)
		pPlane->byType = A3DPLANE_PX;
	else if (pPlane->vNormal.x == -1.0f)
		pPlane->byType = A3DPLANE_NX;
	else if (pPlane->vNormal.y == 1.0f)
		pPlane->byType = A3DPLANE_PY;
	else if (pPlane->vNormal.y == -1.0f)
		pPlane->byType = A3DPLANE_NY;
	else if (pPlane->vNormal.z == 1.0f)
		pPlane->byType = A3DPLANE_PZ;
	else if (pPlane->vNormal.z == -1.0f)
		pPlane->byType = A3DPLANE_NZ;
	else
	{
		//	Get normal's major axis
		float fTemp, fMax = (float)fabs(pPlane->vNormal.x);
		pPlane->byType = A3DPLANE_MAJORX;
		
		fTemp = (float)fabs(pPlane->vNormal.y);
		if (fMax < fTemp)
		{
			fMax = fTemp;
			pPlane->byType = A3DPLANE_MAJORY;
		}
		
		if (fMax < (float)fabs(pPlane->vNormal.z))
			pPlane->byType = A3DPLANE_MAJORZ;
	}

	//	Sign bits
	pPlane->bySignBits = 0;

	for (int i=0; i < 3; i++)
	{
		if (pPlane->vNormal.m[i] < 0.0f)
			pPlane->bySignBits |= (1 << i);
	}
}

//	Clear AABB
void ClearAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs)
{
	vMins = A3DVECTOR3(999999, 999999, 999999);
	vMaxs = A3DVECTOR3(-999999, -999999, -999999);
}

//Use a vertex point to expand an AABB data;
void AddVertexToAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, A3DVECTOR3& vPoint)
{
	for (int i=0; i < 3; i++)
	{
		if (vPoint.m[i] < vMins.m[i])
			vMins.m[i] = vPoint.m[i];
	
		if (vPoint.m[i] > vMaxs.m[i])
			vMaxs.m[i] = vPoint.m[i];
	}
}

//Use a OBB data to expand an AABB data;
void ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DOBB& obb)
{
	A3DVECTOR3 v[8];

	//Up 4 corner;
	v[0] = obb.Center + obb.ExtY - obb.ExtX + obb.ExtZ;
	v[1] = v[0] + obb.ExtX + obb.ExtX;	//	+ obb.ExtX * 2.0f;
	v[2] = v[1] - obb.ExtZ - obb.ExtZ;	//	+ obb.ExtZ * (-2.0f);
	v[3] = v[2] - obb.ExtX - obb.ExtX;	//	+ obb.ExtX * (-2.0f);

	//Down 4 corner;
	v[4] = obb.Center - obb.ExtY - obb.ExtX + obb.ExtZ;
	v[5] = v[4] + obb.ExtX + obb.ExtX;	//	+ obb.ExtX * 2.0f;
	v[6] = v[5] - obb.ExtZ - obb.ExtZ;	//	+ obb.ExtZ * (-2.0f);
	v[7] = v[6] - obb.ExtX - obb.ExtX;	//	+ obb.ExtX * (-2.0f);

	for(int i=0; i<3; i++)
	{
		for(int j=0; j<8; j++)
		{
			if( vecMins.m[i] > v[j].m[i] )
				vecMins.m[i] = v[j].m[i];

			if( vecMaxs.m[i] < v[j].m[i] )
				vecMaxs.m[i] = v[j].m[i];
		}
	}
}

//Use a OBB data to expand an AABB data;
void ExpandAABB(A3DAABB * pAABB, const A3DOBB& obb)
{
	ExpandAABB(pAABB->Mins, pAABB->Maxs, obb);
	pAABB->Center	= (pAABB->Mins + pAABB->Maxs) * 0.5f;
	pAABB->Extents	= pAABB->Maxs - pAABB->Center;
}

//Use a sub AABB data to expand an AABB data;
void ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DAABB& subAABB)
{
	for(int i=0; i<3; i++)
	{
		if( vecMins.m[i] > subAABB.Mins.m[i] )
			vecMins.m[i] = subAABB.Mins.m[i];
		if( vecMaxs.m[i] < subAABB.Maxs.m[i] )
			vecMaxs.m[i] = subAABB.Maxs.m[i];
	}
}

//Use a sub AABB data to expand an AABB data;
void ExpandAABB(A3DAABB * pAABB, const A3DAABB& subAABB)
{
	pAABB->Mins = Min(pAABB->Mins, subAABB.Mins);
	pAABB->Maxs = Max(pAABB->Maxs, subAABB.Maxs);
	pAABB->Center  = (pAABB->Mins + pAABB->Maxs) * 0.5f;
	pAABB->Extents = pAABB->Maxs - pAABB->Center;
}

//Fill in the unset data memeber in an OBB structure;
void CompleteAABB(A3DAABB * pAABB)
{
	pAABB->Center  = (pAABB->Mins + pAABB->Maxs) * 0.5f;
	pAABB->Extents = (pAABB->Maxs - pAABB->Mins) * 0.5f;
}

//Fill in the unset data memeber in an OBB structure after a operation;
void CompleteOBB(A3DOBB * pOBB)
{
	pOBB->ExtX = pOBB->XAxis * pOBB->Extents.x;
	pOBB->ExtY = pOBB->YAxis * pOBB->Extents.y;
	pOBB->ExtZ = pOBB->ZAxis * pOBB->Extents.z;
}

bool A3DGetFileTitle(char * lpszFile, char * lpszTitle, WORD cbBuf)
{
    char * pszTemp;						
    
    pszTemp = (char *)lpszFile + strlen(lpszFile);

    --pszTemp;
    if ('\\' == (* pszTemp)) return false;
    while (true) {
        if ('\\' == (* pszTemp)) {
            ++pszTemp;
            break;
        }
        if (pszTemp == lpszFile) break;
        --pszTemp;
    }
    strcpy(lpszTitle, pszTemp);
    return true;
}

bool A3DGetFilePath(char * lpszFile, char * lpszPath, WORD cbBuf)
{
    char * pszTemp;
    
	strncpy(lpszPath, lpszFile, cbBuf);
    pszTemp = (char *)lpszPath + strlen(lpszPath);

    --pszTemp;
    while (true) {
        if ('\\' == (* pszTemp)) {
            break;
        }
        if (pszTemp == lpszPath) break;
        --pszTemp;
    }
    *pszTemp = '\0';
    return true;
}

bool A3DRepairFilePath(char * lpszFilePath)
{
	int i = 0;
	while(lpszFilePath[i])
	{
		if( lpszFilePath[i] == '/' )
			lpszFilePath[i] = '\\';	   

		i ++;
	}

	return true;
}