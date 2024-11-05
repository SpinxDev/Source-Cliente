#include "A3DOBBFuncs.h"						   

A3DMATRIX3 GetConvarianceMatrix(A3DVERTEX * pVertex, int nVertCount, WORD * pIndex, int nFaceCount)
{
	int i;
	A3DMATRIX3 Cov;

	double S1[3];
	double S2[3][3];

	S1[0] = S1[1] = S1[2] = 0.0;
	S2[0][0] = S2[1][0] = S2[2][0] = 0.0;
	S2[0][1] = S2[1][1] = S2[2][1] = 0.0;
	S2[0][2] = S2[1][2] = S2[2][2] = 0.0;

	// get center of mass
	for(i=0; i<nVertCount; i++)
	{
		S1[0] += pVertex[i].x;
		S1[1] += pVertex[i].y;
		S1[2] += pVertex[i].z;

		S2[0][0] += pVertex[i].x * pVertex[i].x;
		S2[1][1] += pVertex[i].y * pVertex[i].y;
		S2[2][2] += pVertex[i].z * pVertex[i].z;
		S2[0][1] += pVertex[i].x * pVertex[i].y;
		S2[0][2] += pVertex[i].x * pVertex[i].z;
		S2[1][2] += pVertex[i].y * pVertex[i].z;
	}

	float n = (float)nVertCount;
	// now get covariances
	Cov.m[0][0] = (float)(S2[0][0] - S1[0]*S1[0] / n) / n;
	Cov.m[1][1] = (float)(S2[1][1] - S1[1]*S1[1] / n) / n;
	Cov.m[2][2] = (float)(S2[2][2] - S1[2]*S1[2] / n) / n;
	Cov.m[0][1] = (float)(S2[0][1] - S1[0]*S1[1] / n) / n;
	Cov.m[1][2] = (float)(S2[1][2] - S1[1]*S1[2] / n) / n;
	Cov.m[0][2] = (float)(S2[0][2] - S1[0]*S1[2] / n) / n;
	Cov.m[1][0] = Cov.m[0][1];
	Cov.m[2][0] = Cov.m[0][2];
	Cov.m[2][1] = Cov.m[1][2];

	/*
	for(i=0; i<nFaceCount * 3; i++)
	{
		S1[0] += pVertex[pIndex[i]].x;
		S1[1] += pVertex[pIndex[i]].y;
		S1[2] += pVertex[pIndex[i]].z;
	}

	S1[0] /= nFaceCount * 3; S1[1] /= nFaceCount * 3; S1[2] /= nFaceCount * 3;

	for(i=0; i<nFaceCount * 3; i++)
	{
		S2[0][0] += (pVertex[pIndex[i]].x - S1[0]) * (pVertex[pIndex[i]].x - S1[0]);
		S2[1][1] += (pVertex[pIndex[i]].y - S1[1]) * (pVertex[pIndex[i]].y - S1[1]);
		S2[2][2] += (pVertex[pIndex[i]].z - S1[2]) * (pVertex[pIndex[i]].z - S1[2]);
		S2[0][1] += (pVertex[pIndex[i]].x - S1[0]) * (pVertex[pIndex[i]].y - S1[1]);
		S2[0][2] += (pVertex[pIndex[i]].x - S1[0]) * (pVertex[pIndex[i]].z - S1[2]);
		S2[1][2] += (pVertex[pIndex[i]].y - S1[1]) * (pVertex[pIndex[i]].z - S1[2]);
	}

	Cov.m[0][0] = (float)(S2[0][0] / nFaceCount * 3);
	Cov.m[1][1] = (float)(S2[1][1] / nFaceCount * 3);
	Cov.m[2][2] = (float)(S2[2][2] / nFaceCount * 3);
	Cov.m[0][1] = (float)(S2[0][1] / nFaceCount * 3);
	Cov.m[1][2] = (float)(S2[1][2] / nFaceCount * 3);
	Cov.m[0][2] = (float)(S2[0][2] / nFaceCount * 3);
	Cov.m[1][0] = Cov.m[0][1];
	Cov.m[2][0] = Cov.m[0][2];
	Cov.m[2][1] = Cov.m[1][2];
	*/
	return Cov;
}



#define ROTATE(a,i,j,k,l) g=a.m[i][j]; h=a.m[k][l]; a.m[i][j]=(float)(g-s*(h+g*tau)); a.m[k][l]=(float)(h+s*(g-h*tau));

void GetEigenVectors(A3DMATRIX3 *vout, A3DVECTOR3 *dout, A3DMATRIX3 a)
{
	int n = 3;
	int j,iq,ip,i;
	double tresh,theta,tau,t,sm,s,h,g,c;
	int nrot;
	A3DVECTOR3 b;
	A3DVECTOR3 z;
	A3DMATRIX3 v;
	A3DVECTOR3 d;

	v = IdentityMatrix3();
	for(ip=0; ip<n; ip++) 
	{
		b.m[ip] = a.m[ip][ip];
		d.m[ip] = a.m[ip][ip];
		z.m[ip] = 0.0;
	}
  
	nrot = 0;

	for(i=0; i<50; i++)
	{
		sm=0.0;
		for(ip=0; ip<n; ip++) for(iq=ip+1;iq<n;iq++) sm+=fabs(a.m[ip][iq]);
		if( sm == 0.0 )
		{
			v.T();
			*vout = v;
			*dout = d;
			return;
		}
      
		if (i < 3) tresh = 0.2 * sm / (n*n);
		else tresh=0.0;
      
		for(ip=0; ip<n; ip++) 
		{
			for(iq=ip+1; iq<n; iq++)
			{
				g = 100.0*fabs(a.m[ip][iq]);
				if( i>3 && fabs(d.m[ip])+g==fabs(d.m[ip]) && fabs(d.m[iq])+g==fabs(d.m[iq]) )
					a.m[ip][iq]=0.0;
				else if (fabs(a.m[ip][iq])>tresh)
				{
					h = d.m[iq]-d.m[ip];
					if (fabs(h)+g == fabs(h)) 
						t=(a.m[ip][iq])/h;
					else
					{
						theta=0.5*h/(a.m[ip][iq]);
						t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0/sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a.m[ip][iq];
					z.m[ip] -= (float)h;
					z.m[iq] += (float)h;
					d.m[ip] -= (float)h;
					d.m[iq] += (float)h;
					a.m[ip][iq]=0.0;
					for(j=0;j<ip;j++) { ROTATE(a,j,ip,j,iq); } 
					for(j=ip+1;j<iq;j++) { ROTATE(a,ip,j,j,iq); } 
					for(j=iq+1;j<n;j++) { ROTATE(a,ip,j,iq,j); } 
					for(j=0;j<n;j++) { ROTATE(v,j,ip,j,iq); } 
					nrot++;
				}
			}
		}
	    for(ip=0;ip<n;ip++)
		{
			b.m[ip] += z.m[ip];
			d.m[ip] = b.m[ip];
			z.m[ip] = 0.0;
		}
    }

	v.T();
	*vout = v;
	*dout = d;
	return;
}

/* 
	return an OBB extracing from the vertices;
*/
A3DMATRIX3 GetOBBOrientation(A3DVERTEX * pVertex, int nVertCount, WORD * pIndex, int nFaceCount)
{
	A3DMATRIX3 Cov;

	if( nFaceCount <= 0 ) return IdentityMatrix3();

	Cov = GetConvarianceMatrix(pVertex, nVertCount, pIndex, nFaceCount);

	// now get eigenvectors
	A3DMATRIX3 Evecs;
	A3DVECTOR3 Evals;
	GetEigenVectors(&Evecs, &Evals, Cov);

	return Evecs;
}

A3DOBB GetOBB(A3DVERTEX * pVertex, int nVertCount, WORD * pIndex, int nFaceCount)
{
	A3DOBB obb;
	A3DMATRIX3 matTransform = GetOBBOrientation(pVertex, nVertCount, pIndex, nFaceCount);

	ZeroMemory(&obb, sizeof(obb));
	if( nFaceCount <= 0 ) return obb;

	//For matTransform is orthogonal, so the inverse matrix is just rotate it;
 	matTransform.T();

	A3DVECTOR3 vecMax, vecMin;
	
	vecMax = vecMin = A3DVECTOR3(pVertex[0].x, pVertex[0].y, pVertex[0].z) * matTransform;
	
	for(int i=1; i<nVertCount; i++)
	{
		A3DVECTOR3 vecThis = A3DVECTOR3(pVertex[i].x, pVertex[i].y, pVertex[i].z) * matTransform;		

		vecMax = Max(vecMax, vecThis);
		vecMin = Min(vecMin, vecThis);
	}

	matTransform.T();

	A3DVECTOR3 row0, row1, row2;
	row0 = matTransform.GetRow(0);
	row1 = matTransform.GetRow(1);
	row2 = matTransform.GetRow(2);

	obb.Center = 0.5f * (vecMax + vecMin) * matTransform;
	obb.XAxis = Normalize(row0);
	obb.YAxis = Normalize(row1);
	obb.ZAxis = Normalize(row2);
	obb.Extents = 0.5f * (vecMax - vecMin);
	CompleteOBB(&obb);
	return obb;
}
