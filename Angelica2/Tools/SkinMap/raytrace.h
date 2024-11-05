                              #pragma once

#include <windows.h>
#include <max.h>
#include "vector.h"

using namespace abase;

inline void CalcWeights(const Point3& p, const Point3& v0, const Point3& v1, const Point3& v2, float weights[3])
{
	Point3 pv0 = p-v0;
	Point3 pv1 = p-v1;
	Point3 pv2 = p-v2;

	Point3 a0 = CrossProd(pv1, pv2);
	Point3 a1 = CrossProd(pv2, pv0);
	Point3 a2 = CrossProd(pv0, pv1);

	float al0 = a0.Length();
	float al1 = a1.Length();
	float al2 = a2.Length();

	float al = al0+al1+al2;

	if(al<FLT_EPSILON)	// zero area triangle, at least two points overlay
	{
		float pl0 = pv0.Length();
		float pl1 = pv1.Length();
		float pl2 = pv2.Length();

		float pal0 = pl1*pl2;
		float pal1 = pl0*pl2;
		float pal2 = pl0*pl1;
		float pal = pal0+pal1+pal2;
		if(pal<FLT_EPSILON)	// p will locate some place common with the overlayed two points
		{
			float pl = pl0+pl1+pl2;
			if(pl<FLT_EPSILON)		// point triangle, three points overlay
			{
				weights[0] = 1.f/3.f;
				weights[1] = 1.f/3.f;
				weights[2] = 1.f/3.f;
			}
			else	// one pv length nonzero, only two points overlay
			{
				weights[0] = (pl0<FLT_EPSILON)? 0 : 1;
				weights[1] = (pl1<FLT_EPSILON)? 0 : 1;
				weights[2] = (pl2<FLT_EPSILON)? 0 : 1;
				assert( (weights[0]+weights[1]+weights[2]) < 1+FLT_EPSILON);
				assert( (weights[0]+weights[1]+weights[2]) > 1-FLT_EPSILON);
			}
		}
		else	// p will locate some place other than the overlayed two points
		{
			float palv = 1.0/pal;
			weights[0] = pal0*palv;
			weights[1] = pal1*palv;
			weights[2] = pal2*palv;
		}
	}
	else		// nonzero area triangle, no points overlay
	{
		if(al0 < FLT_EPSILON)	al0 = 0.0;
		if(al1 < FLT_EPSILON)	al1 = 0.0;
		if(al2 < FLT_EPSILON)	al2 = 0.0;

		float alv = 1.0/al;
		weights[0] = al0*alv;
		weights[1] = al1*alv;
		weights[2] = al2*alv;
	}

	if(weights[0]<0.001)	weights[0] = 0;
	if(weights[1]<0.001)	weights[1] = 0;
	if(weights[2]<0.001)	weights[2] = 0;

	float wav = 1.0/(weights[0]+weights[1]+weights[2]) ;
	weights[0] = weights[0]*wav;
	weights[1] = weights[1]*wav;
	weights[2] = weights[2]*wav;

	int flags = 0;
	if(weights[0]>0.001) flags++;
	if(weights[1]>0.001) flags++;
	if(weights[2]>0.001) flags++;
	
	if(flags>2)
		int temp = 0;
}

inline bool SamePlane(const Point3& p1, const Point3& p2, const Point3& p3, const Point3& p4)
{
	Point3 cp1 = CrossProd(p4-p3, p1-p3);
	Point3 cp2 = CrossProd(p4-p3, p2-p3);
	Point3 cp = CrossProd(cp1, cp2);
	return (fabs(cp.Length()) < 1e-6);
}

inline bool PointInTriangle(const Point3& p, const Point3& a, const Point3& b, const Point3& c)
{
	Point3 n = CrossProd(b-a, c-a);

	Point3 ap = a - p;
	Point3 bp = b - p;
	float d = DotProd(CrossProd(ap, bp), n);
	if(d<0) return false;
	Point3 cp = c - p;
	d = DotProd(CrossProd(ap, cp), n);
	if(d<0) return false;
	d = DotProd(CrossProd(cp, ap), n);
	if(d<0) return false;
	return true;
}

inline float PointToLineSegmentDistance
(const Point3 & p, const Point3 & v1, const Point3 & v2, Point3 * nearest)
{
	float distance = FLT_MAX;
	Point3 ab =  v2 - v1;
	float abl = ab.Length();
	if(abl < FLT_EPSILON)	// v1, v2 too close to discriminate
	{
		distance = (p-v1).Length();
		*nearest = v1;
	}
	else
	{
		float det = DotProd(ab, p);
		float det1 = DotProd(ab, v1) - det;
		float det2 = DotProd(ab, v2) - det;
		if((det1>=0 && det2>=0) || (det1<=0 && det2<=0))
		{
			if(det1>=0)	// p located outside v1v2
			{
				distance = (p-v1).Length();
				*nearest = v1;
			}
			else
			{
				distance = (p-v2).Length();
				*nearest = v2;
			}
		}
		else
		{
			if(det1<0) det1 = -det1;
			if(det2<0) det2 = -det2;
			*nearest = (v1*det2+v2*det1)/(det1+det2);
			assert(fabs(DotProd(ab, *nearest)-det) < FLT_EPSILON);
			distance = (p-*nearest).Length();
		}
	}
	return distance;
}

inline float PointToTriangleDistance
(const Point3 & p, const Point3 v[3], Point3 * nearest)
{
	float dis2Tri = FLT_MAX;
	bool zeroarea = false;
	float dis2plane = 0;
	Point3 vp;
	Point3 n;
	
	const Point3 &v0 = v[0];
	const Point3 &v1 = v[1];
	const Point3 &v2 = v[2];

	float l01 = (v0-v1).Length();
	float l12 = (v1-v2).Length();
	float l20 = (v2-v0).Length();
	if(l01<FLT_EPSILON || l12<FLT_EPSILON || l20<FLT_EPSILON)
	{
		dis2plane = 0;
		zeroarea = true;
	}
	else
	{
		zeroarea = false;
		n = CrossProd(v1-v0, v2-v0);
		n = n.Normalize();
		float temp = DotProd(n, v0-p);
		vp = p + n*temp;
		assert(fabs(DotProd(n, vp-v0))<1.0e-6);	// vp lines in the plane
		dis2plane = fabs(temp);
	}

	if( (!zeroarea) && PointInTriangle(vp, v0, v1, v2) )	// q lies in the triangle (v0,v1,v2)
	{
		*nearest = vp;
		return dis2plane;
	}
	else
	{
		Point3 nearestThis;
		float disThis = FLT_MAX;
		disThis = PointToLineSegmentDistance(p, v0, v1, &nearestThis);
		if(dis2Tri>disThis)	
		{
			dis2Tri = disThis;
			*nearest = nearestThis;
		}

		disThis = PointToLineSegmentDistance(p, v1, v2, &nearestThis);
		if(dis2Tri>disThis)
		{
			dis2Tri = disThis;
			*nearest = nearestThis;
		}

		disThis = PointToLineSegmentDistance(p, v2, v0, &nearestThis);
		if(dis2Tri>disThis)
		{
			dis2Tri = disThis;
			*nearest = nearestThis;
		}
		
		return dis2Tri;
	}
}
