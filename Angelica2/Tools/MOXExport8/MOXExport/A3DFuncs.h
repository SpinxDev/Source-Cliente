/*
 * FILE: A3DFuncs.h
 *
 * DESCRIPTION: some data operation routines for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DFUNCS_H_
#define _A3DFUNCS_H_

#include <math.h>
#include "A3DTypes.h"
#include "A3DMacros.h"

// =====================================
// Unary operators
// =====================================
inline A3DVECTOR3 operator + (const A3DVECTOR3& v) {return A3DVECTOR3(v.x, v.y, v.z);}
inline A3DVECTOR3 operator - (const A3DVECTOR3& v) {return A3DVECTOR3(-v.x, -v.y, -v.z);}

// =====================================
// Binary operators
// =====================================
// Addition and subtraction
inline A3DVECTOR3 operator + (const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return A3DVECTOR3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);}
inline A3DVECTOR3 operator - (const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return A3DVECTOR3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);}

// Scalar multiplication and division
inline A3DVECTOR3 operator * (const A3DVECTOR3& v, FLOAT s) {return A3DVECTOR3(s * v.x, s * v.y, s * v.z);}
inline A3DVECTOR3 operator * (FLOAT s, const A3DVECTOR3& v) {return A3DVECTOR3(s * v.x, s * v.y, s * v.z);}
inline A3DVECTOR3 operator / (const A3DVECTOR3& v, FLOAT s) {return A3DVECTOR3(v.x / s, v.y / s, v.z / s);}

// Length-related functions
inline FLOAT SquareMagnitude(const A3DVECTOR3& v) {return v.x * v.x + v.y * v.y + v.z * v.z;}
inline FLOAT Magnitude(const A3DVECTOR3& v) {return (FLOAT)(sqrt(v.x * v.x + v.y * v.y + v.z * v.z));}
inline FLOAT MagnitudeH(const A3DVECTOR3& v) {return (FLOAT)(sqrt(v.x * v.x + v.z * v.z));}

inline A3DVECTOR3 Vector423(const A3DVECTOR4& v) {return A3DVECTOR3(v.x / v.w, v.y / v.w, v.z / v.w);}

// Returns vector with same direction and unit length
A3DVECTOR3 Normalize(const A3DVECTOR3& v);
FLOAT Normalize(const A3DVECTOR3& v, A3DVECTOR3& vOut);

// Return min/max vector composed with min/max component of the input 2 vector
inline A3DVECTOR3 Min(const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return A3DVECTOR3(min2(v1.x, v2.x), min2(v1.y, v2.y), min2(v1.z, v2.z));}
inline A3DVECTOR3 Max(const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return A3DVECTOR3(max2(v1.x, v2.x), max2(v1.y, v2.y), max2(v1.z, v2.z));}

// Return min/max member component of input vectors
inline FLOAT MinMember(const A3DVECTOR3& v) {return min3(v.x, v.y, v.z);}
inline FLOAT MaxMember(const A3DVECTOR3& v) {return max3(v.x, v.y, v.z);}

// Dot and cross product
inline FLOAT DotProduct(const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;}
inline A3DVECTOR3 CrossProduct(const A3DVECTOR3& v1, const A3DVECTOR3& v2) {return A3DVECTOR3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,	v1.x * v2.y - v1.y * v2.x);}

//Matrix mul with vector3 and vector4;
A3DVECTOR3 operator * (const A3DMATRIX4& mat, const A3DVECTOR3& v);
A3DVECTOR3 operator * (const A3DVECTOR3& v, const A3DMATRIX4& mat);
A3DVECTOR4 operator * (const A3DMATRIX4& mat, const A3DVECTOR4& v);
A3DVECTOR4 operator * (const A3DVECTOR4& v, const A3DMATRIX4& mat);
A3DVECTOR3 operator * (const A3DMATRIX3& mat, const A3DVECTOR3& v);
A3DVECTOR3 operator * (const A3DVECTOR3& v, const A3DMATRIX3& mat);

//Matrix mul and div a scalar
A3DMATRIX4 operator * (const A3DMATRIX4& mat, FLOAT s);
A3DMATRIX4 operator / (const A3DMATRIX4& mat, FLOAT s);

//Matrix add, sub, and mul with matrix;
A3DMATRIX4 operator + (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2);
A3DMATRIX4 operator - (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2);
A3DMATRIX4 operator * (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2);

//Matrix initialize;
A3DMATRIX3 IdentityMatrix3();
A3DMATRIX4 IdentityMatrix();
A3DMATRIX4 ZeroMatrix();
A3DMATRIX4 ViewMatrix(const A3DVECTOR3& from, const A3DVECTOR3& dir, const A3DVECTOR3& vecUp, FLOAT roll);
A3DMATRIX4 LookAtMatrix(const A3DVECTOR3& from, const A3DVECTOR3& to, const A3DVECTOR3& vecUp, FLOAT roll);
A3DMATRIX4 TransformMatrix(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp, const A3DVECTOR3& vecPos);

//Matrix manipulation;
A3DMATRIX4 RotateX(FLOAT vRad);
A3DMATRIX4 RotateX(const A3DMATRIX4& mat, FLOAT vRad);
A3DMATRIX4 RotateY(FLOAT vRad);
A3DMATRIX4 RotateY(const A3DMATRIX4& mat, FLOAT vRad);
A3DMATRIX4 RotateZ(FLOAT vRad);
A3DMATRIX4 RotateZ(const A3DMATRIX4& mat, FLOAT vRad);
A3DMATRIX4 Scaling(FLOAT sx, FLOAT sy, FLOAT sz);
A3DMATRIX4 Scaling(const A3DMATRIX4& mat, FLOAT sx, FLOAT sy, FLOAT sz);
A3DMATRIX4 Translate(FLOAT x, FLOAT y, FLOAT z);

//Color Value and Color RGB Convertion;
A3DCOLOR ColorValueToColorRGBA(A3DCOLORVALUE colorValue);
A3DCOLORVALUE ColorRGBAToColorValue(A3DCOLOR colorRGBA);

A3DMATRIX4 MirrorMatrix(const A3DVECTOR3 &p, const A3DVECTOR3 &n);
A3DMATRIX4 InverseTM(A3DMATRIX4 tm);

//Plane operations
void IdentifyPlane(A3DPLANE* pPlane);

//AABB operations
void ClearAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs);
void AddVertexToAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, A3DVECTOR3& vPoint);
void ExpandAABB(A3DAABB * pAABB, const A3DOBB& obb);
void ExpandAABB(A3DAABB * pAABB, const A3DAABB& subAABB);
void CompleteAABB(A3DAABB * pAABB);
void ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DOBB& obb);
void ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DAABB& subAABB);

//OBB operations;
void CompleteOBB(A3DOBB * pOBB);

A3DVECTOR3 RandDirH();

bool A3DGetFileTitle(char * lpszFile, char * lpszTitle, WORD cbBuf);
bool A3DGetFilePath(char * lpszFile, char * lpszPath, WORD cbBuf);
bool A3DRepairFilePath(char * lpszFilePath);
#endif