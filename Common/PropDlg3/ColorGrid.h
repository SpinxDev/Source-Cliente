#pragma once

#define COLORGRID_CLASSNAME _T("CColorGrid")

typedef struct _tagCOLORGRID_NOTIFY_INFO
{
	NMHDR   hdr;
	int nRow;
	int nCol;
	COLORREF color;
}COLORGRID_NOTIFY_INFO;

#define NM_COLORGRID_NOTIFICATION_MESSAGE	0x1113

// CColorGrid

class CColorGrid : public CWnd
{
	DECLARE_DYNAMIC(CColorGrid)

public:
	CColorGrid( BOOL bStaticData = FALSE );
	virtual ~CColorGrid();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	
public:
	void Init( int nRows, int nCols );
	BOOL Create( DWORD dwStyle , const RECT& rect ,  CWnd* pParentWnd = NULL ,  UINT nID = 0  );
	
	void FillColor6X8();
	void FillColorWhite();

	void SetColor( int nRow, int nCol, COLORREF color );
	COLORREF GetColor( int nRow, int nCol );

public:
	int m_nRow;
	int m_nCol;
	CSize m_GridSize;
	CSize m_GridMargin;
	int m_nRowSel;
	int m_nColSel;

private:
	CArray<COLORREF, COLORREF> m_Colors;
	static CArray<COLORREF, COLORREF> m_ColorsStatic;
	COLORGRID_NOTIFY_INFO m_NotifyInfo;
	BOOL m_bStatic;

private:
	void DrawControl( CDC* pDC ); 
	LRESULT SendParentNotifyMessage();
	
protected:
	BOOL RegisterWindowClass();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


