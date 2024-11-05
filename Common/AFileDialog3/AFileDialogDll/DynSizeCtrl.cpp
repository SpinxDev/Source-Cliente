// DynSizeCtrl.cpp: implementation of the DynSizeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DynSizeCtrl::DynSizeCtrl(CWnd* pCtrl, CWnd* pParent,bool bToTopMove, bool bToBottomMove, bool bToLeftMove, bool bToRightMove, bool bIsWDelta, bool bIsHDelta,  bool bIsWScale, bool bIsHScale)
: pWnd(pCtrl)
, m_bToTopMove(bToTopMove)
, m_bToBottomMove(bToBottomMove)
, m_bToLeftMove(bToLeftMove)
, m_bToRightMove(bToRightMove)
, m_bIsWDelta(bIsWDelta)
, m_bIsHDelta(bIsHDelta)
, m_bIsWScale(bIsWScale)
, m_bIsHScale(bIsHScale)
{
	ASSERT(pCtrl && pParent);
	CRect rcCtrl;
	pCtrl->GetWindowRect(&rcCtrl);
	CRect rcParent;
	pParent->GetClientRect(&rcParent);
	pParent->ScreenToClient(&rcCtrl);
	float fDlgWidth = static_cast<float>(rcParent.Width());
	float fDlgHeight = static_cast<float>(rcParent.Height());
	fXPercent = rcCtrl.left / fDlgWidth;
	fYPercent = rcCtrl.top / fDlgHeight;
	fWidthPercent = rcCtrl.Width() / fDlgWidth;
	fHeightPercent = rcCtrl.Height() / fDlgHeight;

	m_CtrlPos.x = rcCtrl.left;
	m_CtrlPos.y = rcCtrl.top;
}

void DynSizeCtrl::ResizeCtrl(int cx, int cy, CPoint lastwsize, CPoint origwsize)
{
	CRect rcCtrl;

	float detParentW = cx - lastwsize.x;
	float detParentH = cy - lastwsize.y;

	pWnd->GetWindowRect(&rcCtrl);

	float ctrlposx = 0, ctrlposy = 0;
	if (!m_bToTopMove && m_bToBottomMove)
		ctrlposy = m_CtrlPos.y;
	else if (!m_bToBottomMove && m_bToTopMove)
		ctrlposy = cy +  m_CtrlPos.y - origwsize.y;		
	else
		ctrlposy = fYPercent * cy;

	if (!m_bToLeftMove && m_bToRightMove)
		ctrlposx = m_CtrlPos.x; 
	else if (!m_bToRightMove && m_bToLeftMove)
		ctrlposx = cx +  m_CtrlPos.x - origwsize.x;		
	else
		ctrlposx = fXPercent * cx;	

	bool scaleflag = false;
	if ( m_bIsHScale || m_bIsWScale )
		scaleflag = true;
	
	if (scaleflag)
		pWnd->MoveWindow(static_cast<int>(ctrlposx), static_cast<int>(ctrlposy), static_cast<int>(m_bIsWScale ? fWidthPercent * cx : rcCtrl.Width()), static_cast<int>(m_bIsHScale ? fHeightPercent * cy : rcCtrl.Height()));
	else
		pWnd->MoveWindow(static_cast<int>(ctrlposx), static_cast<int>(ctrlposy), static_cast<int>(m_bIsWDelta ? rcCtrl.Width() + detParentW : rcCtrl.Width()), static_cast<int>(m_bIsHDelta ? rcCtrl.Height() + detParentH : rcCtrl.Height()));

	pWnd->Invalidate();

}