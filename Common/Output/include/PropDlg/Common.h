#ifndef __COMMON_H_
#define __COMMON_H_

#define X_RADIUS		100
#define X_PI			3.14159265358

#define RECT_WIDTH	5

#define X_RAD2DEG(x)  ((180.0 * (x))/X_PI)
#define X_DEG2RAD(x)	(((x) * X_PI)/180.0)
#define X_TOSCALE(x)	(((x)*X_RADIUS)/255.0)
#define X_SCALETOMAX(x) (((x)*255.0)/X_RADIUS)


#define X_RED	0
#define X_GREEN 1
#define X_BLUE 2


#define BAD_SLOPE	1000000.0


struct HSVType;

struct RGBType
{
	COLORREF color() { return RGB(r,g,b); };
	HSVType toHSV();
	int r,g,b;
};

struct HSVType
{
	RGBType toRGB();
	int h,s,v;
};

struct LineDesc
{
	double x,y;
	double slope;
	double c;
};


int Distance(CPoint pt1,CPoint pt2);
CPoint PointOnLine(CPoint pt1,CPoint p2,int len,int maxlen);
double Slope(CPoint pt1,CPoint pt2);
double FindC(LineDesc& l); 
CPoint Intersection(LineDesc desc1,LineDesc desc2);
double AngleFromPoint(CPoint pt,CPoint center);
CPoint PtFromAngle(double angle,double sat,CPoint center);



#endif