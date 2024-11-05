// ColorPickerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "afxcmn.h"
#include "PropDlg_rc.h"
#include "ColorPickerDlg.h"
#include "Common.h"
#include "propertyColorBox.h"

#include "ColorGrid.h"

#include <a3dtypes.h>
#include <a3dmacros.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RADUIS	100

#define IN_CIRCLE	1
#define IN_BRIGHT	2
#define IN_NOTHING	0


/////////////////////////////////////////////////////////////////////////////
// CColorPickerDlg dialog


CColorPickerDlg::CColorPickerDlg(COLORREF c,CWnd* pParent /*=NULL*/)
	: CDialog(CColorPickerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorPickerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Vertex = CPoint(102,108);
	Top = CPoint(102,9);
	Left = CPoint(23,147);
	Right = CPoint(181,147);

	color.r = GetRValue(c);
	color.g = GetGValue(c);
	color.b = GetBValue(c);
	
	m_OldColor = color;
	hsvColor = color.toHSV();
	m_byteAlpha = (LOBYTE((c)>>24));		//alpha
	m_byteOldAlpha = 255;
	m_bInMouse = FALSE;
	m_bInitOver = FALSE;
	m_bInDrawAll = FALSE;
	m_pColorBox = NULL;

	m_BasicColor = new CColorGrid();
	m_DefColor = new CColorGrid(TRUE);
	m_bSetSel = FALSE;
}

void CColorPickerDlg::SetColor(DWORD cl)
{
	if(m_bInitOver)
	{
		color.r = A3DCOLOR_GETRED(cl);
		color.g = A3DCOLOR_GETGREEN(cl);
		color.b = A3DCOLOR_GETBLUE(cl);

		m_OldColor = color;
		hsvColor = color.toHSV();

		m_byteAlpha = A3DCOLOR_GETALPHA(cl); 
		m_byteOldAlpha = m_byteAlpha;

		SetSpinVals();
		SetEditVals();

		DrawAll();
	}
}

DWORD CColorPickerDlg::GetColor()
{
	COLORREF colorRef = color.color();
	return A3DCOLORRGBA(GetRValue(colorRef), GetGValue(colorRef), GetBValue(colorRef), m_byteAlpha);
}

CColorPickerDlg::~CColorPickerDlg()
{
	if(m_RgbBitmap.GetSafeHandle())
	{
		m_RgbBitmap.DeleteObject();
	}
	if(m_HsbBitmap.GetSafeHandle())
	{
		m_HsbBitmap.DeleteObject();
	}
	delete m_BasicColor;
	m_BasicColor = NULL;
	delete m_DefColor;
	m_DefColor = NULL;
}


void CColorPickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPickerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorPickerDlg, CDialog)
	//{{AFX_MSG_MAP(CColorPickerDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_EDIT_BLUE, OnChangeEditBlue)
	ON_EN_CHANGE(IDC_EDIT_GREEN, OnChangeEditGreen)
	ON_EN_CHANGE(IDC_EDIT_HUE, OnChangeEditHue)
	ON_EN_CHANGE(IDC_EDIT_RED, OnChangeEditRed)
	ON_EN_CHANGE(IDC_EDIT_SAT, OnChangeEditSat)
	ON_EN_CHANGE(IDC_EDIT_VAL, OnChangeEditVal)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT_ALPHA, &CColorPickerDlg::OnEnChangeEditAlpha)
	ON_NOTIFY( NM_COLORGRID_NOTIFICATION_MESSAGE, IDC_BASICCOLOR, &CColorPickerDlg::OnBasicColorNotify )
	ON_NOTIFY( NM_COLORGRID_NOTIFICATION_MESSAGE, IDC_DEFCOLOR, &CColorPickerDlg::OnDefColorNotify )
	ON_BN_CLICKED(IDC_ADDCOLOR, &CColorPickerDlg::OnBnClickedAddcolor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPickerDlg message handlers

void CColorPickerDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(hsbRect.PtInRect(point))
	{
		m_bInMouse = FALSE;
		if(InCircle(point))
		{
			m_nMouseIn = IN_CIRCLE;
		}
		else
		if(InBright(point))
		{
			m_nMouseIn = IN_BRIGHT;
		}
		else
		{
			m_nMouseIn = IN_NOTHING;
		}
		if(m_nMouseIn)
		{
			SetCapture();
			TrackPoint(point);
		}
	}
	else
	if(rgbRect.PtInRect(point))
	{
		m_nMouseIn = IN_NOTHING;
		if(rects[X_RED].PtInRect(point))
		{
			SetCapture();
			m_bInMouse = TRUE;
			nIndex = X_RED;
		}
		else
		if(rects[X_GREEN].PtInRect(point))
		{
			SetCapture();
			m_bInMouse = TRUE;
			nIndex = X_GREEN;
		}
		else
		if(rects[X_BLUE].PtInRect(point))
		{
			SetCapture();
			m_bInMouse = TRUE;
			nIndex = X_BLUE;
		}
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CColorPickerDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(GetCapture() == this)
	{
		ReleaseCapture();
		m_bInMouse = FALSE;
	}
	CDialog::OnLButtonUp(nFlags, point);
}

void CColorPickerDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(GetCapture() == this && m_nMouseIn)
	{
		TrackPoint(point);
	}
	else
	if(GetCapture() == this && m_bInMouse)
	{
	double val;
	BOOL bChange = FALSE;
		if(nIndex == X_RED)
		{
			if(point.y > Vertex.y)
			{
				point.y = Vertex.y;
			}
			point.x = Vertex.x;
			val = Distance(point,Vertex);
			if(val > RedLen)
			{
				val = RedLen;
			}
			CClientDC dc(this);
			DrawLines(&dc);
			val = (val/RedLen)*255;
			color.r = (int)val;
			CPoint pt;
			pt = PointOnLine(Vertex,Top,(color.r*RedLen)/255,RedLen);
			rects[X_RED] = CRect(pt.x - RECT_WIDTH ,pt.y-RECT_WIDTH ,pt.x+RECT_WIDTH ,pt.y+RECT_WIDTH );
			CalcCuboid();
			DrawLines(&dc);
			bChange = TRUE;
		}
		else
		if(nIndex == X_GREEN)
		{
			if(point.x > Vertex.x)
			{
				point.x = Vertex.x;
			}
			point.y = rects[X_GREEN].top + RECT_WIDTH;
			val = Distance(point,Vertex);
			if(val > GreenLen)
			{
				val = GreenLen;
			}
			CClientDC dc(this);
			DrawLines(&dc);
			val = (val/GreenLen)*255;
			color.g = (int)val;
			CPoint pt;
			pt = PointOnLine(Vertex,Left,(color.g*GreenLen)/255,GreenLen);
			rects[X_GREEN] = CRect(pt.x - RECT_WIDTH ,pt.y-RECT_WIDTH ,pt.x+RECT_WIDTH ,pt.y+RECT_WIDTH );
			CalcCuboid();
			DrawLines(&dc);
			bChange = TRUE;
		}
		else
		if(nIndex == X_BLUE)
		{
			if(point.x < Vertex.x)
			{
				point.x = Vertex.x;
			}
			point.y = rects[X_BLUE].top + RECT_WIDTH;
			val = Distance(point,Vertex);
			if(val > BlueLen)
			{
				val = BlueLen;
			}
			CClientDC dc(this);
			DrawLines(&dc);
			val = (val/BlueLen)*255;
			color.b = (int)val;
			CPoint pt;
			pt = PointOnLine(Vertex,Right,(color.b*GreenLen)/255,BlueLen);
			rects[X_BLUE] = CRect(pt.x - RECT_WIDTH ,pt.y-RECT_WIDTH ,pt.x+RECT_WIDTH ,pt.y+RECT_WIDTH );
			CalcCuboid();
			DrawLines(&dc);
			bChange = TRUE;
		}
		if(bChange)
		{
			hsvColor = color.toHSV();
			if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
			SetEditVals();
			CClientDC dc(this);
			DrawMarkers(&dc);
			CalcRects();
			SetDIBPalette();

			InvalidateRect(&brightRect,FALSE);
			DrawHSB(&dc);
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CColorPickerDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DrawFilledColor(&dc,OldColorRect,m_OldColor.color());
	DrawHSB(&dc);
	DrawRGB(&dc);
}

BOOL CColorPickerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_STATIC_RGB_RECT)->GetWindowRect(&rgbRect);
	GetDlgItem(IDC_STATIC_HSB_RECT)->GetWindowRect(&hsbRect);
	ScreenToClient(&rgbRect);
	ScreenToClient(&hsbRect);

	int delta_w = (rgbRect.right - rgbRect.left - 190)/2 - 5;
	int delta_h = (rgbRect.bottom - rgbRect.top - 190)/2 - 10;
	rgbRect.OffsetRect(delta_w,delta_h);

	delta_w = (hsbRect.right - hsbRect.left - 202)/2 - 30 ;
	delta_h = (hsbRect.bottom - hsbRect.top - 201)/2 - 20;
	hsbRect.OffsetRect(delta_w,delta_h);

	GetDlgItem(IDC_STATIC_OLD)->GetWindowRect(&OldColorRect);
	GetDlgItem(IDC_STATIC_NEW)->GetWindowRect(&NewColorRect);
	ScreenToClient(&OldColorRect);
	ScreenToClient(&NewColorRect);

	//
	GetDlgItem(IDC_BASICCOLOR)->GetWindowRect(&BasicColorRect);
	GetDlgItem(IDC_DEFCOLOR)->GetWindowRect(&DefColorRect);
	ScreenToClient(&BasicColorRect);
	ScreenToClient(&DefColorRect);

	
CWindowDC dc(NULL);
CSize bmSize;
//	Set Up HSB
	
	memDC.CreateCompatibleDC(&dc);

	LoadMappedBitmap(m_HsbBitmap,IDB_BITMAP_HSB,bmSize);
	hsbWidth = bmSize.cx;
	hsbHeight = bmSize.cy;

	hsbRect.InflateRect(-5,-5);
	hsbRect.top += 20;
	hsbRect.left += 10;

	m_Centre = CPoint(X_RADIUS,X_RADIUS);
	m_Centre += CPoint(hsbRect.left,hsbRect.top);

	brightRect = CRect(hsbRect.left+hsbWidth+20,hsbRect.top,hsbRect.left+hsbWidth+20+20,hsbRect.top + hsbHeight);
	CreateBrightDIB();
	CalcRects();
	SetDIBPalette();

//	Set Up RGB
	LoadMappedBitmap(m_RgbBitmap,IDB_BITMAP_RGB,bmSize);
	rgbWidth = bmSize.cx;
	rgbHeight = bmSize.cy;

	rgbRect.InflateRect(-5,-5);
	rgbRect.top+=10;
	rgbRect.left-=3;
CPoint pt = CPoint(rgbRect.left,rgbRect.top);

	Top += pt;
	Left += pt;
	Right += pt;
	Vertex += pt;
	// TODO: Add your specialized code here and/or call the base class

	RedLen = Distance(Vertex,Top);
	GreenLen = Distance(Vertex,Left);
	BlueLen = Distance(Vertex,Right);

	CalcSlopes();
	CalcCuboid();

	SetSpinVals();
	SetEditVals();

	//
	m_BasicColor->Create( WS_CHILD | WS_VISIBLE, BasicColorRect, this, IDC_BASICCOLOR );
	m_BasicColor->Init( 6, 8 );
	m_BasicColor->FillColor6X8();

	m_DefColor->Create( WS_CHILD | WS_VISIBLE, DefColorRect, this, IDC_DEFCOLOR );
	m_DefColor->Init( 2, 8 );
	m_DefColor->FillColorWhite();

	m_bInitOver = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CColorPickerDlg::DrawMarkers(CDC *pDC)
{
	if(m_CurrentRect.Width())
	{
	CPen *oldPen;
	CBrush *oldBrush;
	int oldMode;
	CRect cr = m_CurrentRect;

		oldPen = (CPen *)pDC->SelectStockObject(WHITE_PEN);
		oldBrush = (CBrush *)pDC->SelectStockObject(NULL_BRUSH);

		oldMode = pDC->SetROP2(R2_XORPEN);

		pDC->Rectangle(&cr);
		CPen pen;
		pen.CreatePen(PS_SOLID,2,RGB(255,255,255));
		pDC->SelectObject(&pen);
		pDC->Rectangle(&brightMark);

		pDC->SelectObject(oldPen);
		pDC->SelectObject(oldBrush);
		pDC->SetROP2(oldMode);
		pen.DeleteObject();

	}

}


BOOL CColorPickerDlg::InCircle(CPoint pt)
{
	return Distance(pt,m_Centre) <= X_RADIUS;
}

BOOL CColorPickerDlg::InBright(CPoint pt)
{
	return brightRect.PtInRect(pt);
}

void CColorPickerDlg::TrackPoint(CPoint pt)
{
	if(m_nMouseIn == IN_CIRCLE)
	{
	CClientDC dc(this);
		DrawMarkers(&dc);
		
		hsvColor.h = (int)X_RAD2DEG(AngleFromPoint(pt,m_Centre));
		if(hsvColor.h < 0)
		{
			hsvColor.h += 360;
		}
		hsvColor.s = (int)X_SCALETOMAX(Distance(pt,m_Centre));
		if(hsvColor.s > 255) hsvColor.s = 255;

		SetDIBPalette();
		CalcRects();


		InvalidateRect(&brightRect,FALSE);

		DrawMarkers(&dc);

		color = hsvColor.toRGB();
		if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
		SetEditVals();
		DrawLines(&dc);
		CalcCuboid();
		DrawRGB(&dc);

	}
	else
	if(m_nMouseIn == IN_BRIGHT)
	{
		double d;
		d = brightRect.bottom - pt.y;
		d *= 255;
		d /= brightRect.Height();
		if(d < 0 ) d = 0;
		if(d > 255) d = 255;
		CClientDC dc(this);
		DrawMarkers(&dc);
		hsvColor.v = (int)d;
		CalcRects();
		DrawMarkers(&dc);

		color = hsvColor.toRGB();
		if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
		SetEditVals();
		DrawLines(&dc);
		CalcCuboid();
		DrawRGB(&dc);

	}
}


void CColorPickerDlg::CreateBrightDIB()
{
CDIB& d = m_BrightDIB;
	d.Create(brightRect.Width(),brightRect.Height(),8);
	for(int i=0; i < d.Height(); i++)
	{
		memset(d.GetLinePtr(i),i,d.Width());
	}
}

void CColorPickerDlg::SetDIBPalette()
{
BYTE palette[768],*p;
HSVType h = hsvColor;
double d;
	d = 255.0/brightRect.Height();
	p = palette;
	for(int i=brightRect.Height()-1; i >= 0 ;i--,p+=3)
	{
		h.v = (int)((double)i * d);
		RGBType rgb = h.toRGB();
		p[0] = rgb.r;
		p[1] = rgb.g;
		p[2] = rgb.b;
	}
	m_BrightDIB.SetPalette(palette);
}

void CColorPickerDlg::CalcRects()
{
CPoint pt;
	pt = PtFromAngle(hsvColor.h,hsvColor.s,m_Centre);
	m_CurrentRect = CRect(pt.x - RECT_WIDTH,pt.y - RECT_WIDTH,pt.x+RECT_WIDTH,pt.y + RECT_WIDTH);	
	
int y;
	y = (int)(((double)hsvColor.v/255)*brightRect.Height()); 	
	y = brightRect.bottom - y;
	brightMark = CRect(brightRect.left - 2, y - 4, brightRect.right+2,y+4);
}


void CColorPickerDlg::DrawHSB(CDC *pDC)
{
	if(m_HsbBitmap.GetSafeHandle())
	{
	CBitmap *pOldBitmap ;
		pOldBitmap  = (CBitmap *)memDC.SelectObject(&m_HsbBitmap);
		pDC->BitBlt(hsbRect.left,hsbRect.top,hsbWidth,hsbHeight,&memDC,0,0,SRCCOPY);
		m_BrightDIB.BitBlt(pDC->m_hDC,brightRect.left,brightRect.top,brightRect.Width(),brightRect.Height(),0,0);
		DrawMarkers(pDC);
		memDC.SelectObject(pOldBitmap);
	}
}

void CColorPickerDlg::DrawRGB(CDC *pDC)
{
	if(m_RgbBitmap.GetSafeHandle())
	{
	CBitmap *pOldBitmap ;
		pOldBitmap  = (CBitmap *)memDC.SelectObject(&m_RgbBitmap);
		pDC->BitBlt(rgbRect.left,rgbRect.top,rgbWidth,rgbHeight,&memDC,0,0,SRCCOPY);
		DrawLines(pDC);
		memDC.SelectObject(pOldBitmap);
	}
}

void CColorPickerDlg::DrawLines(CDC *pDC)
{
CPoint pt[3];
	pt[0] = PointOnLine(Vertex,Top,(color.r*RedLen)/255,RedLen);
	pt[1] = PointOnLine(Vertex,Left,(color.g*GreenLen)/255,GreenLen);
	pt[2] = PointOnLine(Vertex,Right,(color.b*BlueLen)/255,BlueLen);
COLORREF col = RGB(255,255,255);
CRect cr;
	for(int i=0; i < 3; i++)
	{
		cr = CRect(pt[i].x - RECT_WIDTH ,pt[i].y-RECT_WIDTH ,pt[i].x+RECT_WIDTH ,pt[i].y+RECT_WIDTH );
		rects[i] = cr;
		DrawXorRect(pDC,cr);
	}
CPen *oldPen;
int oldMode;
	oldPen = (CPen *)pDC->SelectStockObject(WHITE_PEN);
	oldMode = pDC->SetROP2(R2_XORPEN);
/*
Draw the following lines :
	
	1 -2 
	2 -3
	3 - 4
	4- 5
	5 -2
	5 - 6
	6-7
	7-4
*/
	pDC->MoveTo(m_Cuboid[1]);
	pDC->LineTo(m_Cuboid[2]);
	pDC->LineTo(m_Cuboid[3]);
	pDC->LineTo(m_Cuboid[4]);
	pDC->LineTo(m_Cuboid[5]);
	pDC->LineTo(m_Cuboid[2]);

	pDC->MoveTo(m_Cuboid[5]);
	pDC->LineTo(m_Cuboid[6]);
	pDC->LineTo(m_Cuboid[7]);
	pDC->LineTo(m_Cuboid[4]);
	
	pDC->MoveTo(m_Cuboid[1]);
	pDC->LineTo(m_Cuboid[6]);

	pDC->SelectObject(oldPen);
	pDC->SetROP2(oldMode);

	DrawFilledColor(pDC,NewColorRect,color.color());
}


void CColorPickerDlg::DrawXorRect(CDC *pDC,CRect& cr)
{
CPen pen,*oldPen;
CBrush *oldBrush;
int oldMode;
	pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
	oldPen = (CPen *)pDC->SelectObject(&pen);
	oldBrush = (CBrush *)pDC->SelectStockObject(NULL_BRUSH);
	oldMode =pDC->SetROP2(R2_XORPEN);
	pDC->Rectangle(&cr);
	pDC->SetROP2(oldMode);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pen.DeleteObject();

}

void CColorPickerDlg::CalcSlopes()
{
	lines[X_RED].slope = Slope(Top,Vertex);
	lines[X_GREEN].slope = Slope(Left,Vertex);
	lines[X_BLUE].slope = Slope(Right,Vertex);

int i;
	for(i=0; i < 3; i++)
	{
		lines[i].x = Vertex.x;
		lines[i].y = Vertex.y;
		lines[i].c = FindC(lines[i]);
	}
}


/*

  Cuboid points
   0 = vertex
   1 = Red Axis
   2 = Red Green Intersection
   3 = Green Axis
   4 = Blue Green Intersection
   5 = Blue Green Red Intersection
   6 = Red Blue Intersection
   7 = Blue Axis

  Draw the following lines :
	
	1 -2 
	2 -3
	3 - 4
	4- 5
	5 -2
	5 - 6
	6-7
	7-4
*/

void CColorPickerDlg::CalcCuboid()
{
double rLen,gLen,bLen;
	rLen = (double)(color.r*RedLen)/255;
	gLen = (double)(color.g*GreenLen)/255;
	bLen = (double)(color.b*BlueLen)/255;
LineDesc l[12];

	m_Cuboid[0] = Vertex;
	m_Cuboid[1] = PointOnLine(Vertex,Top,(int)rLen,RedLen);
	m_Cuboid[3] = PointOnLine(Vertex,Left,(int)gLen,GreenLen);
	m_Cuboid[7] = PointOnLine(Vertex,Right,(int)bLen,BlueLen);

	l[0] = lines[X_RED];
	l[1] = lines[X_GREEN];
	l[2] = lines[X_BLUE];

	l[3].slope = lines[X_GREEN].slope;
	l[3].x = m_Cuboid[1].x;
	l[3].y = m_Cuboid[1].y;
	l[3].c = FindC(l[3]);
	
	l[4].slope = lines[X_RED].slope;
	l[4].x = m_Cuboid[3].x;
	l[4].y = m_Cuboid[3].y;
	l[4].c = FindC(l[4]);

	l[5].slope = lines[X_BLUE].slope;
	l[5].x = m_Cuboid[3].x;
	l[5].y = m_Cuboid[3].y;
	l[5].c = FindC(l[5]);
	
	l[6].slope = lines[X_GREEN].slope;
	l[6].x = m_Cuboid[7].x;
	l[6].y = m_Cuboid[7].y;
	l[6].c = FindC(l[6]);



	l[10].slope = lines[X_BLUE].slope;
	l[10].x = m_Cuboid[1].x;
	l[10].y = m_Cuboid[1].y;
	l[10].c = FindC(l[10]);
	
	l[11].slope = lines[X_RED].slope;
	l[11].x = m_Cuboid[7].x;
	l[11].y = m_Cuboid[7].y;
	l[11].c = FindC(l[11]);

	m_Cuboid[2] = Intersection(l[3],l[4]);
	m_Cuboid[4] = Intersection(l[5],l[6]);
	m_Cuboid[6] = Intersection(l[10],l[11]);

	l[7].slope = lines[X_RED].slope;
	l[7].x = m_Cuboid[4].x;
	l[7].y = m_Cuboid[4].y;
	l[7].c = FindC(l[7]);
	
	l[8].slope = lines[X_BLUE].slope;
	l[8].x = m_Cuboid[2].x;
	l[8].y = m_Cuboid[2].y;
	l[8].c = FindC(l[8]);

	m_Cuboid[5] = Intersection(l[7],l[8]);
		
}


void CColorPickerDlg::SetSpinVals()
{
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_RED))->SetRange(0,255);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_GREEN))->SetRange(0,255);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_BLUE))->SetRange(0,255);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_ALPHA))->SetRange(0,255);

	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_HUE))->SetRange(0,360);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_SAT))->SetRange(0,255);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_VAL))->SetRange(0,255);
	
}

void CColorPickerDlg::SetEditVals()
{
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_RED))->SetPos(color.r);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_GREEN))->SetPos(color.g);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_BLUE))->SetPos(color.b);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_ALPHA))->SetPos(m_byteAlpha);

	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_HUE))->SetPos(hsvColor.h);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_SAT))->SetPos(hsvColor.s);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_VAL))->SetPos(hsvColor.v);
	

}

void CColorPickerDlg::OnChangeEditBlue() 
{
int b;
	b = GetDlgItemInt(IDC_EDIT_BLUE);
	if( b != color.b && m_bInitOver)
	{
		color.b = b;
		if(color.b < 0) color.b = 0;
		if(color.b > 255) color.b = 255;
		hsvColor = color.toHSV();
		DrawAll();

		if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
	}
}

void CColorPickerDlg::OnChangeEditGreen() 
{
int g;
	
	g = GetDlgItemInt(IDC_EDIT_GREEN);
	if(g != color.g && m_bInitOver)
	{
		color.g = g;
		if(color.g < 0) color.g = 0;
		if(color.g > 255) color.g = 255;
		hsvColor = color.toHSV();
		DrawAll();

		if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
	}
}


void CColorPickerDlg::OnChangeEditRed() 
{
int r;
	r = GetDlgItemInt(IDC_EDIT_RED);
	if(r != color.r && m_bInitOver)
	{
		color.r = r;
		if(color.r < 0) color.r = 0;
		if(color.r > 255) color.r = 255;
		hsvColor = color.toHSV();
		DrawAll();

		if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
	}
}

void CColorPickerDlg::OnChangeEditHue() 
{
int h;
	h = GetDlgItemInt(IDC_EDIT_HUE);
	if(h != hsvColor.h && m_bInitOver)
	{
		hsvColor.h = h;
		if(hsvColor.h < 0) hsvColor.h = 0;
		if(hsvColor.h > 359) hsvColor.h = 359;
		color = hsvColor.toRGB();
		DrawAll();

		if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
	}
}

void CColorPickerDlg::OnChangeEditSat() 
{
int s;
	s = GetDlgItemInt(IDC_EDIT_SAT);
	if(s != hsvColor.s && m_bInitOver)
	{
		hsvColor.s = s;
		if(hsvColor.s < 0) hsvColor.s = 0;
		if(hsvColor.s > 255) hsvColor.s = 255;
		color = hsvColor.toRGB();
		DrawAll();

		if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
	}
}

void CColorPickerDlg::OnChangeEditVal() 
{
int v;
	v = GetDlgItemInt(IDC_EDIT_VAL);
	if(v != hsvColor.v && m_bInitOver)
	{
		hsvColor.v = v;
		if(hsvColor.v < 0) hsvColor.v = 0;
		if(hsvColor.v > 255) hsvColor.v = 255;
		color = hsvColor.toRGB();
		DrawAll();

		if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
	}
}

void CColorPickerDlg::OnEnChangeEditAlpha()
{
	int v;
	v = GetDlgItemInt(IDC_EDIT_ALPHA);
	if(v != m_byteAlpha && m_bInitOver)
	{
		m_byteAlpha = v;
		if(m_byteAlpha < 0) m_byteAlpha = 0;
		if(m_byteAlpha > 255) m_byteAlpha = 255;
	}
}

void CColorPickerDlg::DrawAll()
{
	if(m_bInitOver && !m_bInDrawAll)
	{
	CClientDC dc(this);
		DrawMarkers(&dc);
		DrawLines(&dc);
		m_bInDrawAll = TRUE;
		CalcCuboid();
		CalcRects();
		SetDIBPalette();
		DrawRGB(&dc);
		DrawHSB(&dc);
		SetEditVals();
		m_bInDrawAll = FALSE;
	}
}


void CColorPickerDlg::DrawFilledColor(CDC *pDC,CRect cr,COLORREF c)
{
	pDC->FillSolidRect(&cr,c);
	pDC->Draw3dRect(&cr,RGB(0,0,0),RGB(0,0,0));
	cr.InflateRect(-1,-1);
	pDC->Draw3dRect(&cr,RGB(192,192,192),RGB(128,128,128));
}



void CColorPickerDlg::LoadMappedBitmap(CBitmap& bitmap,UINT nIdResource,CSize& size)
{
	CBitmap *pOldBitmap;
	
	if(bitmap.GetSafeHandle()) bitmap.DeleteObject();
	
	if(bitmap.LoadBitmap(nIdResource))
	{

	int width,height;
	BITMAP bmInfo;
		::GetObject(bitmap.m_hObject,sizeof(bmInfo),&bmInfo);	
		width = bmInfo.bmWidth;
		height = bmInfo.bmHeight;

	COLORREF colorWindow = ::GetSysColor(COLOR_3DFACE);
	COLORREF sourceColor = RGB(192,192,192);

		pOldBitmap = (CBitmap *)memDC.SelectObject(&bitmap);
		/*
		int i,j;
		
		for(i=0; i < height; i++)
		{
			for(j=0; j < width; j++)
			{
				if(memDC.GetPixel(j,i) == sourceColor)
				{
					memDC.SetPixel(j,i,colorWindow);
				}
			}
		}*/

		memDC.SelectObject(&pOldBitmap);
		size = CSize(width,height);
	}

}

void CColorPickerDlg::OnSysColorChange()
{
	CSize size;
	LoadMappedBitmap(m_HsbBitmap,IDB_BITMAP_HSB,size);
	LoadMappedBitmap(m_RgbBitmap,IDB_BITMAP_RGB,size);
}

void CColorPickerDlg::OnOK()
{
	if(m_pColorBox) m_pColorBox->UpdateColorData(GetColor(),true);
	
	CDialog::OnOK();
}

void CColorPickerDlg::OnCancel()
{
	COLORREF colorRef = m_OldColor.color();
	DWORD oldclr = A3DCOLORRGBA(GetRValue(colorRef), GetGValue(colorRef), GetBValue(colorRef), m_byteOldAlpha);
	if(m_pColorBox) m_pColorBox->UpdateColorData(oldclr,false);
	
	CDialog::OnCancel();
}

void CColorPickerDlg::OnBasicColorNotify( NMHDR* hdr, LRESULT* result )
{
	COLORGRID_NOTIFY_INFO* notifyInfo = (COLORGRID_NOTIFY_INFO*)hdr;

	COLORREF colorRef = notifyInfo->color;
	//DWORD _color =  A3DCOLORRGBA(GetRValue(colorRef), GetGValue(colorRef), GetBValue(colorRef), m_byteAlpha);
	
	if(m_bInitOver)
	{
		color.r = GetRValue(colorRef);
		color.g = GetGValue(colorRef);
		color.b = GetBValue(colorRef);
		hsvColor = color.toHSV();
		DrawAll();

		if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
	}
}

void CColorPickerDlg::OnDefColorNotify( NMHDR* hdr, LRESULT* result )
{
	COLORGRID_NOTIFY_INFO* notifyInfo = (COLORGRID_NOTIFY_INFO*)hdr;

	COLORREF colorRef = notifyInfo->color;
	//DWORD _color =  A3DCOLORRGBA(GetRValue(colorRef), GetGValue(colorRef), GetBValue(colorRef), m_byteAlpha);
	
	if(m_bInitOver)
	{
		color.r = GetRValue(colorRef);
		color.g = GetGValue(colorRef);
		color.b = GetBValue(colorRef);
		hsvColor = color.toHSV();
		DrawAll();

		if(m_pColorBox) m_pColorBox->TestUpdateColorData(GetColor());
	}

	m_bSetSel = TRUE;
}

void CColorPickerDlg::OnBnClickedAddcolor()
{
	static int nRow = 0; 
	static int nCol = 0;

	if ( m_DefColor->m_nRowSel != -1 && m_DefColor->m_nColSel != -1 && m_bSetSel )
	{
		nRow = m_DefColor->m_nRowSel;
		nCol = m_DefColor->m_nColSel;
		m_bSetSel = FALSE;
	}
	
	DWORD color = GetColor();
	COLORREF colorRef = RGB(A3DCOLOR_GETRED(color), A3DCOLOR_GETGREEN(color), A3DCOLOR_GETBLUE(color));
	
	m_DefColor->SetColor( nRow, nCol, colorRef );
	m_DefColor->Invalidate(TRUE);

	++nRow;
	if ( nRow >= m_DefColor->m_nRow )
	{
		nRow = 0 ;
		++nCol;
		if ( nCol >= m_DefColor->m_nCol )
		{
			nCol = 0;
		}
	}
}
