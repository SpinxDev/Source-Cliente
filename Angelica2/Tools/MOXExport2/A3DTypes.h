/*
 * FILE: A3DTypes.h
 *
 * DESCRIPTION: Fundermental data types for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTYPES_H_
#define _A3DTYPES_H_

//Common Headers;
#include <StdIO.h>
#include <StdLib.h>
#include <Memory.h>
#include <String.h>

//return code data type;
//32-bit;
typedef long				A3DRESULT;

//unsigned value
//32-bit;
typedef unsigned long		DWORD;
//16-bit;
typedef unsigned short 		WORD;
//8-bit;
typedef unsigned char		BYTE;

typedef int					BOOL;
#define TRUE				1
#define FALSE				0

//typedef int				INT
//commone integer ---- int, but the size undetermined, so not defined, use carefully;

//signed value
//32-bit;
typedef long				LONG;
//16-bit;
typedef short				SHORT;
//8-bit;
typedef char				CHAR;

//float
typedef float				FLOAT;

//double
typedef double				DOUBLE;

//color
//32-bit;
typedef DWORD				A3DCOLOR;

typedef struct _A3DCOLORVALUE		
{
	FLOAT	r;
	FLOAT	g;
	FLOAT	b;
	FLOAT	a;
public:
	_A3DCOLORVALUE() {}
	inline _A3DCOLORVALUE(FLOAT _r, FLOAT _g, FLOAT _b, FLOAT _a) {r = _r; g = _g; b = _b; a = _a;}
} A3DCOLORVALUE, * PA3DCOLORVALUE;

//2D Point
typedef struct _A3DPOINT2
{
	LONG x;
	LONG y;
public:
	_A3DPOINT2() {}
	inline _A3DPOINT2(LONG _x, LONG _y) { x = _x; y = _y; }
} A3DPOINT2, * PA3DPOINT2;

//Homogeneous vector and quad
typedef struct _A3DVECTOR4
{
	union
	{
		struct{
			FLOAT x, y, z, w;
		};
		FLOAT m[4];
	};

public:
	_A3DVECTOR4() {}
	inline _A3DVECTOR4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w) {x = _x; y = _y; z = _z; w = _w;}
	inline _A3DVECTOR4(FLOAT _f) {x = y = z = w = _f;}
	inline _A3DVECTOR4(const FLOAT	f[4]){x = f[0]; y = f[1]; z = f[2]; w = f[3];}
} A3DVECTOR4, * PA3DVECTOR4;

//3D vector 
typedef struct _A3DVECTOR3
{
	union
	{
		struct{
			FLOAT x, y, z;
		};
		FLOAT m[3];
	};

public:
	_A3DVECTOR3() {}
	inline _A3DVECTOR3(FLOAT _x, FLOAT _y, FLOAT _z) {x = _x; y = _y; z = _z;}
	inline _A3DVECTOR3(FLOAT _f) {x = y = z = _f;}
	inline _A3DVECTOR3(const FLOAT	f[3]){x = f[0]; y = f[1]; z = f[2];}
	inline operator = (A3DVECTOR4& vec4) {x = vec4.x / vec4.w; y = vec4.y / vec4.w; z = vec4.z / vec4.w;}
} A3DVECTOR3, * PA3DVECTOR3;

// Matrix;
typedef struct _A3DMATRIX3
{
	union
	{
		struct
		{
			FLOAT _11, _12, _13;
			FLOAT _21, _22, _23;
			FLOAT _31, _32, _33;
		};
		FLOAT m[3][3];
	};
	inline void T() 
	{ 
		FLOAT t; 
		t = m[0][1]; m[0][1] = m[1][0]; m[1][0] = t; 
		t = m[0][2]; m[0][2] = m[2][0];	m[2][0] = t;
		t = m[1][2]; m[1][2] = m[2][1]; m[2][1] = t;
	}
	inline A3DVECTOR3 GetRow(int i){ return A3DVECTOR3(m[i][0], m[i][1], m[i][2]); }
	inline A3DVECTOR3 GetCol(int i){ return A3DVECTOR3(m[0][i], m[1][i], m[2][i]); }
} A3DMATRIX3, * PA3DMATRIX3;

typedef struct _A3DMATRIX4
{
	union
	{
		struct
		{
			FLOAT _11, _12, _13, _14;
			FLOAT _21, _22, _23, _24;
			FLOAT _31, _32, _33, _34;
			FLOAT _41, _42, _43, _44;
		};
		FLOAT m[4][4];
	};
	inline void T()
	{
		FLOAT t;
		t = m[0][1]; m[0][1] = m[1][0]; m[1][0] = t; 
		t = m[0][2]; m[0][2] = m[2][0];	m[2][0] = t;
		t = m[0][3]; m[0][3] = m[3][0]; m[3][0] = t;
		t = m[1][2]; m[1][2] = m[2][1]; m[2][1] = t;
		t = m[1][3]; m[1][3] = m[3][1]; m[3][1] = t;
		t = m[2][3]; m[2][3] = m[3][2]; m[3][2] = t;
	}
} A3DMATRIX4, * PA3DMATRIX4;

//Rects;
typedef struct _A3DRECT
{
	LONG		left;
	LONG		top;
	LONG		right;
	LONG		bottom;
} A3DRECT, * PA3DRECT;

//Device Material and Texture Surface related structures;
typedef enum _A3DFORMAT {
    A3DFMT_UNKNOWN              =   0,

    A3DFMT_R8G8B8               =  20,
    A3DFMT_A8R8G8B8             =  21,
    A3DFMT_X8R8G8B8             =  22,
    A3DFMT_R5G6B5               =  23,
    A3DFMT_X1R5G5B5             =  24,
    A3DFMT_A1R5G5B5             =  25,
    A3DFMT_A4R4G4B4             =  26,
    A3DFMT_R3G3B2               =  27,
    A3DFMT_A8                   =  28,
    A3DFMT_A8R3G3B2             =  29,
    A3DFMT_X4R4G4B4             =  30,

    A3DFMT_A8P8                 =  40,
    A3DFMT_P8                   =  41,

    A3DFMT_L8                   =  50,
    A3DFMT_A8L8                 =  51,
    A3DFMT_A4L4                 =  52,

    A3DFMT_V8U8                 =  60,
    A3DFMT_L6V5U5               =  61,
    A3DFMT_X8L8V8U8             =  62,
    A3DFMT_Q8W8V8U8             =  63,
    A3DFMT_V16U16               =  64,
    A3DFMT_W11V11U10            =  65,

    A3DFMT_D16_LOCKABLE         =  70,
    A3DFMT_D32                  =  71,
    A3DFMT_D15S1                =  73,
    A3DFMT_D24S8                =  75,
    A3DFMT_D16                  =  80,
    A3DFMT_D24X8                =  77,
    A3DFMT_D24X4S4              =  79,

    A3DFMT_FORCE_DWORD          = 0xFFFFFFFF
} A3DFORMAT;

typedef struct _A3DDEVFMT
{
	bool		bWindowed;
	int			nWidth;
	int			nHeight;
	A3DFORMAT	fmtTarget;
	A3DFORMAT	fmtDepth;
} A3DDEVFMT, * PA3DDEVFMT;

typedef enum _A3DBLEND {
    A3DBLEND_ZERO               =  1,
    A3DBLEND_ONE                =  2,
    A3DBLEND_SRCCOLOR           =  3,
    A3DBLEND_INVSRCCOLOR        =  4,
    A3DBLEND_SRCALPHA           =  5,
    A3DBLEND_INVSRCALPHA        =  6,
    A3DBLEND_DESTALPHA          =  7,
    A3DBLEND_INVDESTALPHA       =  8,
    A3DBLEND_DESTCOLOR          =  9,
    A3DBLEND_INVDESTCOLOR       = 10,
    A3DBLEND_SRCALPHASAT        = 11,
    A3DBLEND_BOTHSRCALPHA       = 12,
    A3DBLEND_BOTHINVSRCALPHA    = 13,

    A3DBLEND_FORCE_DWORD        = 0x7fffffff
} A3DBLEND;

typedef struct _A3DMATERIALPARAM {
    A3DCOLORVALUE   Diffuse;
    A3DCOLORVALUE   Ambient;
    A3DCOLORVALUE   Specular;
    A3DCOLORVALUE   Emissive;
    FLOAT           Power;
} A3DMATERIALPARAM;

typedef enum _A3DCMPFUNC {
    A3DCMP_NEVER                = 1,
    A3DCMP_LESS                 = 2,
    A3DCMP_EQUAL                = 3,
    A3DCMP_LESSEQUAL            = 4,
    A3DCMP_GREATER              = 5,
    A3DCMP_NOTEQUAL             = 6,
    A3DCMP_GREATEREQUAL         = 7,
    A3DCMP_ALWAYS               = 8,

    A3DCMP_FORCE_DWORD          = 0x7fffffff
} A3DCMPFUNC;

//Light Param;
typedef enum _A3DLIGHTTYPE {
    A3DLIGHT_POINT          = 1,
    A3DLIGHT_SPOT           = 2,
    A3DLIGHT_DIRECTIONAL    = 3,

	A3DLIGHT_AMBIENT		= 100000,
    A3DLIGHT_FORCE_DWORD    = 0x7fffffff
} A3DLIGHTTYPE;

typedef struct _A3DLIGHTPARAM {
    A3DLIGHTTYPE    Type;
    A3DCOLORVALUE   Diffuse;
    A3DCOLORVALUE   Specular;
    A3DCOLORVALUE   Ambient;
    A3DVECTOR3      Position;
    A3DVECTOR3      Direction;
    FLOAT           Range;
    FLOAT           Falloff;
    FLOAT           Attenuation0;
    FLOAT           Attenuation1;
    FLOAT           Attenuation2;
    FLOAT           Theta;
    FLOAT           Phi;
} A3DLIGHTPARAM;

typedef enum _A3DPRIMITIVETYPE {
    A3DPT_POINTLIST             = 1,
    A3DPT_LINELIST              = 2,
    A3DPT_LINESTRIP             = 3,
    A3DPT_TRIANGLELIST          = 4,
    A3DPT_TRIANGLESTRIP         = 5,
    A3DPT_TRIANGLEFAN           = 6,

    A3DPT_FORCE_DWORD           = 0x7fffffff
} A3DPRIMITIVETYPE;

//Viewport Parameters;
typedef struct _A3DVIEWPORTPARAM
{	
    DWORD       X;
    DWORD       Y;
    DWORD       Width;
    DWORD       Height;
    FLOAT       MinZ;
    FLOAT       MaxZ;
} A3DVIEWPORTPARAM;

//Face Culling;
typedef enum _A3DCULLTYPE
{
	A3DCULL_CW = 0,
	A3DCULL_CCW,
	A3DCULL_NONE
} A3DCULLTYPE;

//Fill Mode;
typedef enum _A3DFILLMODE {
    A3DFILL_POINT               = 1,
    A3DFILL_WIREFRAME           = 2,
    A3DFILL_SOLID               = 3,
} A3DFILLMODE;

//Filter Type;
typedef enum _A3DTEXTUREFILTERTYPE {
    A3DTEXF_NONE            = 0,
    A3DTEXF_POINT           = 1,
    A3DTEXF_LINEAR          = 2,
    A3DTEXF_ANISOTROPIC     = 3,
    A3DTEXF_FLATCUBIC       = 4,
    A3DTEXF_GAUSSIANCUBIC   = 5,

    A3DTEXF_FORCE_DWORD     = 0x7fffffff
} A3DTEXTUREFILTERTYPE;

//Texture operation type
typedef enum _A3DTEXTUREOP {
    A3DTOP_DISABLE                   =  1,
    A3DTOP_SELECTARG1                =  2,
    A3DTOP_SELECTARG2                =  3,
    A3DTOP_MODULATE                  =  4,
    A3DTOP_MODULATE2X                =  5,
    A3DTOP_MODULATE4X                =  6,
    A3DTOP_ADD                       =  7,
    A3DTOP_ADDSIGNED                 =  8,
    A3DTOP_ADDSIGNED2X               =  9,
    A3DTOP_SUBTRACT                  = 10,
    A3DTOP_ADDSMOOTH                 = 11,
    A3DTOP_BLENDDIFFUSEALPHA         = 12,
    A3DTOP_BLENDTEXTUREALPHA         = 13,
    A3DTOP_BLENDFACTORALPHA          = 14,
    A3DTOP_BLENDTEXTUREALPHAPM       = 15,
    A3DTOP_BLENDCURRENTALPHA         = 16,
    A3DTOP_PREMODULATE               = 17,
    A3DTOP_MODULATEALPHA_ADDCOLOR    = 18,
    A3DTOP_MODULATECOLOR_ADDALPHA    = 19,
    A3DTOP_MODULATEINVALPHA_ADDCOLOR = 20,
    A3DTOP_MODULATEINVCOLOR_ADDALPHA = 21,
    A3DTOP_BUMPENVMAP                = 22,
    A3DTOP_BUMPENVMAPLUMINANCE       = 23,
    A3DTOP_DOTPRODUCT3               = 24,
    A3DTOP_MULTIPLYADD               = 25,
    A3DTOP_LERP                      = 26,

    A3DTOP_FORCE_DWORD               = 0x7fffffff,
} A3DTEXTUREOP;

//Action;
typedef struct _A3DACTION
{
	char    szName[32];
	int		nAnimStart;
	int		nAnimEnd;
	bool	bAnimLoop;
} A3DACTION, * PA3DACTION;

//Group Action;
#define A3D_GROUP_ACTION_MAX_ELEMENT	6
typedef struct _A3DGROUPACTION
{
	char			szName[32];
	int				nActionNum;
	A3DACTION *		pActionElement[A3D_GROUP_ACTION_MAX_ELEMENT];
} A3DGROUPACTION, * PA3DGROUPACTION;

//Oriented Bounding Box
typedef struct _A3DOBB
{
	A3DVECTOR3	Center;
	A3DVECTOR3	XAxis;
	A3DVECTOR3	YAxis;
	A3DVECTOR3	ZAxis;
	A3DVECTOR3	ExtX;
	A3DVECTOR3	ExtY;
	A3DVECTOR3	ExtZ;
	A3DVECTOR3	Extents;

} A3DOBB, * PA3DOBB;

//Axis-Aligned Bounding Box
typedef struct _A3DAABB
{
	A3DVECTOR3	Center;
	A3DVECTOR3	Extents;
	A3DVECTOR3	Mins;
	A3DVECTOR3	Maxs;

} A3DAABB, * PA3DAABB;

//Plane
typedef struct _A3DPLANE
{
	A3DVECTOR3	vNormal;		//	Normal
	FLOAT		fDist;			//	d parameter
	BYTE		byType;			//	Type of plane
	BYTE		bySignBits;		//	Sign flags

} A3DPLANE, * PA3DPLANE;

// Mesh and frame obb property;
enum A3DMATERIALTYPE
{
	A3DMTL_CONCRETE = 0,
	A3DMTL_WOOD,
	A3DMTL_METALSOLID,
	A3DMTL_METALBARREL,
	A3DMTL_GLASS,
	A3DMTL_RUBBER,
	A3DMTL_FLESH,
	A3DMTL_WATER,
	A3DMTL_EARTH,
};

/*
typedef struct _A3DMESH_PROP
{
	bool			b2Sided;
	bool			bCanShootThrough;
	bool			bNoMark;
	bool			bCanPickThrough;
	MATERIAL_TYPE	nMaterialType;
	bool			bOnGround;
	bool			bUsedForPVS;

	char			foo[30];//This is used to make some reserved room for expanding new proerpty;

} A3DMESH_PROP, * PA3DMESH_PROP;
*/

class A3DMESHPROP
{
public:		//	Types

	//	Property flags
	enum
	{
		FG_2SIDES			= 0x0001,		//	Two sides
		FG_SHOOTTHROUGH		= 0x0002,		//	Can be shoot through
		FG_NOMARK			= 0x0004,		//	No mark
		FG_PICKTHROUGH		= 0x0008,		//	Can be picked through
		FG_ONGROUND			= 0x0010,		//	On ground flag used by BSP
		FG_BSPPVS			= 0x0020,		//	Used for BSP PVS calculation
		FG_NOTRENDER		= 0x0040,		//	Not render
		FG_WALKIGNORE		= 0x0080,		//	Ignore when character walks
		FG_NOLYFORWALK		= 0x0100,		//	Only used for character walking
	};

public:		//	Constructors and Destructors

	A3DMESHPROP() { dwProps = 0; MtlType = A3DMTL_CONCRETE; }
	A3DMESHPROP(DWORD _dwProps, A3DMATERIALTYPE _MtlType) { dwProps = _dwProps; MtlType = _MtlType; }
	A3DMESHPROP(const A3DMESHPROP& mp) { dwProps = mp.dwProps; MtlType = mp.MtlType; }

public:		//	Attributes

	DWORD			dwProps;
	A3DMATERIALTYPE	MtlType;

public:		//	Operations

	//	== operator
	friend bool operator == (const A3DMESHPROP& mp1, const A3DMESHPROP& mp2) { return mp1.dwProps == mp2.dwProps && mp1.MtlType == mp2.MtlType; }
	//	!= operator
	friend bool operator != (const A3DMESHPROP& mp1, const A3DMESHPROP& mp2) { return mp1.dwProps != mp2.dwProps || mp1.MtlType != mp2.MtlType; }

	//	= operator
	const A3DMESHPROP& operator = (const A3DMESHPROP& mp) { dwProps = mp.dwProps; MtlType = mp.MtlType; return *this; }

	//	Get / Set material type
	void SetMaterialType(A3DMATERIALTYPE _MtlType) { MtlType = _MtlType; }
	A3DMATERIALTYPE GetMaterialType() { return MtlType; }

	//	Get / Set property flags
	void SetPropFlags(DWORD _dwProps) { dwProps = _dwProps; }
	DWORD GetPropFlags() { return dwProps; }

	void Set2SidesFlag(bool bTrue) { if (bTrue) dwProps |= FG_2SIDES; else dwProps &= ~FG_2SIDES; }
	bool Get2SidesFlag() { return (dwProps & FG_2SIDES) ? true : false; }
	void SetShootThroughFlag(bool bTrue) { if (bTrue) dwProps |= FG_SHOOTTHROUGH; else dwProps &= ~FG_SHOOTTHROUGH; }
	bool GetShootThroughFlag() { return (dwProps & FG_SHOOTTHROUGH) ? true : false; }
	void SetNoMarkFlag(bool bTrue) { if (bTrue) dwProps |= FG_NOMARK; else dwProps &= ~FG_NOMARK; }
	bool GetNoMarkFlag() { return (dwProps & FG_NOMARK) ? true : false; }
	void SetPickThroughFlag(bool bTrue) { if (bTrue) dwProps |= FG_PICKTHROUGH; else dwProps &= ~FG_PICKTHROUGH; }
	bool GetPickThroughFlag() { return (dwProps & FG_PICKTHROUGH) ? true : false; }
	void SetOnGroundFlag(bool bTrue) { if (bTrue) dwProps |= FG_ONGROUND; else dwProps &= ~FG_ONGROUND; }
	bool GetOnGroundFlag() { return (dwProps & FG_ONGROUND) ? true : false; }
	void SetBSPPVSFlag(bool bTrue) { if (bTrue) dwProps |= FG_BSPPVS; else dwProps &= ~FG_BSPPVS; }
	bool GetBSPPVSFlag() { return (dwProps & FG_BSPPVS) ? true : false; }
	void SetNotRenderFlag(bool bTrue) { if (bTrue) dwProps |= FG_NOTRENDER; else dwProps &= ~FG_NOTRENDER; }
	bool GetNotRenderFlag() { return (dwProps & FG_NOTRENDER) ? true : false; }
	void SetWalkIgnoreFlag(bool bTrue) { if (bTrue) dwProps |= FG_WALKIGNORE; else dwProps &= ~FG_WALKIGNORE; }
	bool GetWalkIgnoreFlag() { return (dwProps & FG_WALKIGNORE) ? true : false; }
	void SetOnlyForWalkFlag(bool bTrue) { if (bTrue) dwProps |= FG_NOLYFORWALK; else dwProps &= ~FG_NOLYFORWALK; }
	bool GetOnlyForWalkFlag() { return (dwProps & FG_NOLYFORWALK) ? true : false; }
};

/*
//A3DFrame's bounding box element;
typedef struct _A3DFRAMEOBB_PROP
{
	bool			bCanShootThrough;
	bool			bNoMark;
	bool			bCanPickThrough;
	bool			bCanNotDestroy;
	A3DMATERIALTYPE	nMaterialType;
	char			foo[32];//This is used to make some reserved room for expanding new proerpty;

} A3DFRAMEOBB_PROP, * PA3DFRAMEOBB_PROP;

typedef struct _A3DFRAMEOBB
{
	char					szName[32];
	A3DOBB					a3dOBB;
	A3DFRAMEOBB_PROP		property;

} A3DFRAMEOBB, * PA3DFRAMEOBB;
*/

class A3DFRAMEOBB
{
public:		//	Types

	//	Property flags
	enum
	{
		FG_SHOOTTHROUGH		= 0x0001,		//	Can be shoot through
		FG_NOMARK			= 0x0002,		//	No mark
		FG_PICKTHROUGH		= 0x0004,		//	Can be picked through
		FG_CANNOTDESTROY	= 0x0008,		//	Can not be destroyed
	};

public:		//	Constructors and Destructors

	A3DFRAMEOBB() { szName[0] = '\0'; dwProps = 0; }
	A3DFRAMEOBB(const A3DFRAMEOBB& fo)
	{ 
		memcpy(szName, fo.szName, 32);
		a3dOBB	= fo.a3dOBB;
		dwProps = fo.dwProps; 
		MtlType = fo.MtlType;
	}

public:		//	Attributes

	char			szName[32];
	A3DOBB			a3dOBB;
	DWORD			dwProps;
	A3DMATERIALTYPE	MtlType;

public:		//	Operations

	void SetShootThroughFlag(bool bTrue) { if (bTrue) dwProps |= FG_SHOOTTHROUGH; else dwProps &= ~FG_SHOOTTHROUGH; }
	bool GetShootThroughFlag() { return (dwProps & FG_SHOOTTHROUGH) ? true : false; }
	void SetNoMarkFlag(bool bTrue) { if (bTrue) dwProps |= FG_NOMARK; else dwProps &= ~FG_NOMARK; }
	bool GetNoMarkFlag() { return (dwProps & FG_NOMARK) ? true : false; }
	void SetPickThroughFlag(bool bTrue) { if (bTrue) dwProps |= FG_PICKTHROUGH; else dwProps &= ~FG_PICKTHROUGH; }
	bool GetPickThroughFlag() { return (dwProps & FG_PICKTHROUGH) ? true : false; }
	void SetCanNotDestroyFlag(bool bTrue) { if (bTrue) dwProps |= FG_CANNOTDESTROY; else dwProps &= ~FG_CANNOTDESTROY; }
	bool GetCanNotDestroyFlag() { return (dwProps & FG_CANNOTDESTROY) ? true : false; }
};

enum A3DTRANSPARENT_MODE 
{
	A3DTRANSPARENT_COLORKEY = 0,
	A3DTRANSPARENT_COLORALPHA,
	A3DTRANSPARENT_SRCALPHA_DESTINVSRCALPHA,
	A3DTRANSPARENT_SRCONE_DESTINVSRCALPHA,
	A3DTRANSPARENT_SRCALPHA_DESTONE,
};

typedef struct _A3DSURFACE_PARAM
{
	//Output param;
	int					width;
	int					height;
} A3DSURFACE_PARAM, * PA3DSURFACE_PARAM;

#endif