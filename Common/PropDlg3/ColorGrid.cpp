// ColorGrid.cpp : implementation file
//

#include "stdafx.h"
#include "ColorGrid.h"
#include "MemDC.h"


// CColorGrid

IMPLEMENT_DYNAMIC(CColorGrid, CWnd)

CArray<COLORREF, COLORREF> CColorGrid::m_ColorsStatic;

CColorGrid::CColorGrid( BOOL bStaticData )
{
	m_GridSize = CSize(20, 18);
	m_GridMargin = CSize(3,3);
	m_nRowSel = -1;
	m_nColSel = -1;
	m_bStatic = bStaticData;

	RegisterWindowClass();

//	FillColor6X8();
}

CColorGrid::~CColorGrid()
{
	DestroyWindow();

	WNDCLASS wndcls = { 0 };

	if( ( ::GetClassInfo( AfxGetResourceHandle() , COLORGRID_CLASSNAME , &wndcls ) ) ) 
	{
		::UnregisterClass( wndcls.lpszClassName , AfxGetResourceHandle() );
	}
}

void CColorGrid::Init( int nRows, int nCols )
{
	m_nRow = nRows;
	m_nCol = nCols;
	if ( m_bStatic && m_ColorsStatic.IsEmpty() )
	{
		m_ColorsStatic.SetSize( m_nRow * m_nCol );
	}
	else
	{
		m_Colors.SetSize( m_nRow * m_nCol );
	}
	
}

BEGIN_MESSAGE_MAP(CColorGrid, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CColorGrid message handlers



void CColorGrid::PreSubclassWindow()
{
	//ModifyStyleEx( 0, WS_EX_CLIENTEDGE );
	SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER );
	CWnd::PreSubclassWindow();
}

BOOL CColorGrid::Create( DWORD dwStyle , const RECT& rect , CWnd* pParentWnd /*= NULL */, UINT nID /*= 0 */ )
{
	ASSERT(pParentWnd->GetSafeHwnd());

	if (!CWnd::Create( COLORGRID_CLASSNAME , NULL, dwStyle, rect, pParentWnd, nID ) )
		return FALSE;

	return TRUE;
}

BOOL CColorGrid::RegisterWindowClass()
{
	WNDCLASS wndcls;

	HINSTANCE hInst = AfxGetResourceHandle();

	if ( !( ::GetClassInfo( hInst, COLORGRID_CLASSNAME , &wndcls ) ) )
	{

		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = ::DefWindowProc;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = hInst;
		wndcls.hIcon            = NULL;

		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

		wndcls.hbrBackground    = (HBRUSH)( COLOR_3DFACE + 1 );
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = COLORGRID_CLASSNAME;

		if( !AfxRegisterClass( &wndcls ) )
		{
			AfxThrowResourceException();
			return FALSE;
		}

	}

	return TRUE;
}
BOOL CColorGrid::OnEraseBkgnd(CDC* pDC)
{
	CMemDC memDC( pDC );
	CRect rect;
	GetClientRect( &rect );

	memDC.FillSolidRect( &rect, GetSysColor(COLOR_BTNFACE) );

	DrawControl( &memDC );

	return TRUE;
}

void CColorGrid::DrawControl( CDC* pDC )
{
	int iRow, iCol;
	CPoint pt(2,2);
	CRect rcGrid;
	for ( iRow = 0; iRow < m_nRow; ++iRow )
	{
		for ( iCol = 0; iCol < m_nCol; ++iCol )
		{		
			//
			rcGrid = CRect( pt, m_GridSize );
			pDC->FillSolidRect( &rcGrid, GetColor(iRow,iCol) );

			if ( iRow == m_nRowSel && iCol == m_nColSel )
			{
				rcGrid.InflateRect(2,2);
				pDC->DrawFocusRect( &rcGrid );
				rcGrid.DeflateRect(2,2);
				pDC->Draw3dRect( &rcGrid, RGB(0,0,0), RGB(0,0,0) );
				rcGrid.DeflateRect(1,1);
				pDC->Draw3dRect( &rcGrid, RGB(10,10,10), RGB(255,255,255) );
			}
			else
			{
				pDC->Draw3dRect( &rcGrid, GetSysColor(COLOR_BTNFACE), RGB(255,255,255) );
				rcGrid.DeflateRect(1,1);
				pDC->Draw3dRect( &rcGrid, RGB(10,10,10), RGB(255,255,255) );
			}	
		
			pt.x += ( m_GridSize.cx + m_GridMargin.cx );
		}
		pt.x = 2;
		pt.y += ( m_GridSize.cy + m_GridMargin.cy );
	}
}

void CColorGrid::SetColor( int nRow, int nCol, COLORREF color )
{
	if ( nRow >= m_nRow || nCol >= m_nCol )
	{
		return;
	}
	if ( m_bStatic )
	{
		m_ColorsStatic[nRow*m_nCol + nCol] = color;
	}
	else
	{
		m_Colors[nRow*m_nCol + nCol] = color;
	}
	
}

COLORREF CColorGrid::GetColor( int nRow, int nCol )
{
	if ( nRow >= m_nRow || nCol >= m_nCol )
	{
		return RGB(0,0,0);
	}
	if ( m_bStatic )
	{
		return m_ColorsStatic[nRow*m_nCol + nCol];
	}
	else
	{
		return m_Colors[nRow*m_nCol + nCol];
	}
	
}
void CColorGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	int iRow, iCol;
	CPoint pt(2,2);
	CRect rcGrid;
	for ( iRow = 0; iRow < m_nRow; ++iRow )
	{
		for ( iCol = 0; iCol < m_nCol; ++iCol )
		{		
			rcGrid = CRect( pt, m_GridSize );
			if ( PtInRect( &rcGrid, point ) )
			{
				m_nRowSel = iRow;
				m_nColSel = iCol;
				SendParentNotifyMessage();
				Invalidate(TRUE);
				break;
			}
			pt.x += ( m_GridSize.cx + m_GridMargin.cx );
		}
		pt.x = 2;
		pt.y += ( m_GridSize.cy + m_GridMargin.cy );
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CColorGrid::FillColor6X8()
{
	SetColor( 0, 0, RGB(255,128,128) );
	SetColor( 0, 1, RGB(255,255,128) );
	SetColor( 0, 2, RGB(128,255,128) );
	SetColor( 0, 3, RGB(0, 255, 128) );
	SetColor( 0, 4, RGB(128, 255, 255) );
	SetColor( 0, 5, RGB(0, 128, 255 ) );
	SetColor( 0, 6, RGB(255, 128, 192) );
	SetColor( 0, 7, RGB(255, 128, 255) );

	SetColor( 1, 0, RGB(255,0,0) );
	SetColor( 1, 1, RGB(255,255,0) );
	SetColor( 1, 2, RGB(128,255,0) );
	SetColor( 1, 3, RGB(0, 255, 64) );
	SetColor( 1, 4, RGB(0, 255, 255) );
	SetColor( 1, 5, RGB(0, 128, 192 ) );
	SetColor( 1, 6, RGB(128, 128, 192) );
	SetColor( 1, 7, RGB(255, 0, 255) );

	SetColor( 2, 0, RGB(128,64,64) );
	SetColor( 2, 1, RGB(255,128,64) );
	SetColor( 2, 2, RGB(0,255,0) );
	SetColor( 2, 3, RGB(0, 128, 128) );
	SetColor( 2, 4, RGB(0, 64, 128) );
	SetColor( 2, 5, RGB(128, 128, 255 ) );
	SetColor( 2, 6, RGB(128, 0, 64) );
	SetColor( 2, 7, RGB(255, 0, 128) );

	SetColor( 3, 0, RGB(128,0,0) );
	SetColor( 3, 1, RGB(255,128,0) );
	SetColor( 3, 2, RGB(0,128,0) );
	SetColor( 3, 3, RGB(0, 128, 64) );
	SetColor( 3, 4, RGB(0, 0, 255) );
	SetColor( 3, 5, RGB(0, 0, 160 ) );
	SetColor( 3, 6, RGB(128, 0, 128) );
	SetColor( 3, 7, RGB(128, 0, 255) );

	SetColor( 4, 0, RGB(64,0,0) );
	SetColor( 4, 1, RGB(128,64,0) );
	SetColor( 4, 2, RGB(0,64,0) );
	SetColor( 4, 3, RGB(0, 64, 64) );
	SetColor( 4, 4, RGB(0, 0, 128) );
	SetColor( 4, 5, RGB(0, 0, 64 ) );
	SetColor( 4, 6, RGB(64, 0, 64) );
	SetColor( 4, 7, RGB(64, 0, 128) );

	SetColor( 5, 0, RGB(0,0,0) );
	SetColor( 5, 1, RGB(128,128,0) );
	SetColor( 5, 2, RGB(128,128,64) );
	SetColor( 5, 3, RGB(128, 128, 128) );
	SetColor( 5, 4, RGB(64, 128, 128) );
	SetColor( 5, 5, RGB(192, 192, 192 ) );
	SetColor( 5, 6, RGB(64, 0, 64) );
	SetColor( 5, 7, RGB(255, 255, 255) );
}

void CColorGrid::FillColorWhite()
{
	static BOOL bFirst = TRUE;

	int iRow, iCol;
	for ( iRow = 0; iRow < m_nRow; ++iRow )
	{
		for ( iCol = 0; iCol < m_nCol; ++iCol )
		{
			if ( m_bStatic && !bFirst )
			{
				SetColor( iRow, iCol , GetColor(iRow, iCol) );
			}
			else
			{
				SetColor( iRow, iCol, RGB(255,255,255) );
			}		
		}
	}

	bFirst = FALSE;
}

LRESULT CColorGrid::SendParentNotifyMessage()
{
	memset( &m_NotifyInfo , 0 , sizeof( m_NotifyInfo ) );

	//Header
	m_NotifyInfo.hdr.hwndFrom = m_hWnd;
	m_NotifyInfo.hdr.idFrom   = GetDlgCtrlID();
	m_NotifyInfo.hdr.code     = NM_COLORGRID_NOTIFICATION_MESSAGE;

	m_NotifyInfo.nRow = m_nRowSel;
	m_NotifyInfo.nCol = m_nColSel;
	m_NotifyInfo.color = GetColor(m_nRowSel, m_nColSel);

	CWnd *pWnd = GetParent();
	if( pWnd != NULL && IsWindow( pWnd->m_hWnd ) )
		return pWnd->SendMessage( WM_NOTIFY , (WPARAM)m_NotifyInfo.hdr.idFrom , (LPARAM)&m_NotifyInfo );

	return -1;
}