// DynSizeCtrl.h: interface for the DynSizeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYNSIZECTRL_H__A2D933E5_2AA0_4634_86F2_E97B9202EFDE__INCLUDED_)
#define AFX_DYNSIZECTRL_H__A2D933E5_2AA0_4634_86F2_E97B9202EFDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct FixedFrame
{
	FixedFrame()
		: m_iRightBlank(256) {}

	int m_iRightBlank;
};

class DynSizeCtrl : public FixedFrame  
{
public:
public:
	DynSizeCtrl(CWnd* pCtrl, CWnd* pParent,bool bToTopMove = true, bool bToBottomMove = true, bool bToLeftMove = true, bool bToRightMove = true, bool bIsWDelta = false, bool bIsHDelta = false, bool bIsWScale = true, bool bIsHScale = true);
	void ResizeCtrl(int cx, int cy, CPoint lastwsize, CPoint origwsize);
protected:
	CWnd* pWnd;
	bool m_bIsWScale;
	bool m_bIsHScale;

	float fXPercent;		// x / dlg's length
	float fYPercent;		// y / dlg's height
	float fWidthPercent;	// width / dlg's length
	float fHeightPercent;	// height / dlg's height

	bool m_bIsWDelta;		
	bool m_bIsHDelta;
	
	bool m_bToTopMove;
	bool m_bToLeftMove;
	bool m_bToBottomMove;
	bool m_bToRightMove;
	CPoint m_CtrlPos;

};

typedef AFVector<DynSizeCtrl> DynSizeCtrlArray;
typedef DynSizeCtrlArray::iterator DynSizeCtrlIterator;

#define DECLARE_DYNSIZECTRL() \
	DynSizeCtrlArray m_dynSizeArray;

#define ON_INIT_DYNSIZECTRL(pCtrl, pParent) \
	m_dynSizeArray.push_back(DynSizeCtrl(pCtrl, pParent));

#define ON_SIZE_DYNSIZECTRL(cx, cy) \
	for (DynSizeCtrlIterator itr = m_dynSizeArray.begin(); itr != m_dynSizeArray.end(); ++itr) \
		itr->ResizeCtrl(cx, cy, lastwsize, origwsize);

#endif // !defined(AFX_DYNSIZECTRL_H__A2D933E5_2AA0_4634_86F2_E97B9202EFDE__INCLUDED_)
