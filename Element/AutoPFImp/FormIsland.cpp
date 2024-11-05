// FormIsland.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPFImp.h"
#include "FormIsland.h"
#include "Global.h"
#include "MainFrm.h"
#include "GridRender.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormIsland

IMPLEMENT_DYNCREATE(CFormIsland, CFormView)

CFormIsland::CFormIsland()
	: CFormView(CFormIsland::IDD)
{
	//{{AFX_DATA_INIT(CFormIsland)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bEditMode = false;
}

CFormIsland::~CFormIsland()
{
}

void CFormIsland::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormIsland)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormIsland, CFormView)
	//{{AFX_MSG_MAP(CFormIsland)
	ON_BN_CLICKED(IDC_ISLAND_NEW, OnIslandNew)
	ON_BN_CLICKED(IDC_ISLAND_SELECT, OnIslandSelect)
	ON_BN_CLICKED(IDC_ISLAND_ADDBORDER, OnIslandAddborder)
	ON_BN_CLICKED(IDC_ISLAND_DELBORDER, OnIslandDelborder)
	ON_BN_CLICKED(IDC_ISLAND_DEL, OnIslandDel)
	ON_BN_CLICKED(IDC_ISLAND_CHECKPASS, OnIslandCheckpass)
	ON_BN_CLICKED(IDC_ISLAND_CREATEPASS, OnIslandCreatepass)
	ON_BN_CLICKED(IDC_ISLAND_PASSTHROUGH, OnIslandPassthrough)
	ON_EN_KILLFOCUS(IDC_EDIT_PASSTHROUGH, OnKillfocusEditPassthrough)
	ON_CBN_SELCHANGE(IDC_COMBO_PASSES, OnSelchangeComboPasses)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormIsland diagnostics

#ifdef _DEBUG
void CFormIsland::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormIsland::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFormIsland message handlers

void CFormIsland::OnIslandNew() 
{
	// TODO: Add your control notification handler code here
	
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap || !pLayerMap->GetRMap())
	{
		return;
	}

	if (!pLayerMap->GetIslandList())
	{
		pLayerMap->CreateIslandList();
	}

	_RestoreBtn();
	CGridRender * pGrid = AUX_GetMainFrame()->GetView()->GetGridRender();
	pGrid->SetOp(CGridRender::OP_NONE);

	if (!_LeaveEditMode())
	{
		return;
	}

	
	CIsland *	pIsl = pLayerMap->GetIslandList()->NewIsland();
	pLayerMap->GetIslandList()->SetCurIsland(pIsl);
	m_bEditMode = true;

	//	发送添加边界消息
	SendMessage(WM_COMMAND, MAKEWPARAM(IDC_ISLAND_ADDBORDER, BN_CLICKED));
}


void CFormIsland::_RestoreBtn()
{
	CButton * pBtn;
	pBtn = (CButton *)GetDlgItem(IDC_ISLAND_NEW);
	if (pBtn)
	{
		pBtn->SetCheck(0);
	}

	pBtn = (CButton *)GetDlgItem(IDC_ISLAND_ADDBORDER);
	if (pBtn)
	{
		pBtn->SetCheck(0);
	}

	pBtn = (CButton *)GetDlgItem(IDC_ISLAND_DELBORDER);
	if (pBtn)
	{
		pBtn->SetCheck(0);
	}

	pBtn = (CButton *)GetDlgItem(IDC_ISLAND_SELECT);
	if (pBtn)
	{
		pBtn->SetCheck(0);
	}

	pBtn = (CButton *)GetDlgItem(IDC_ISLAND_DEL);
	if (pBtn)
	{
		pBtn->SetCheck(0);
	}

	pBtn = (CButton *)GetDlgItem(IDC_ISLAND_CREATEPASS);
	if (pBtn)
	{
		pBtn->SetCheck(0);
	}

	pBtn = (CButton *)GetDlgItem(IDC_ISLAND_PASSTHROUGH);
	if (pBtn)
	{
		pBtn->SetCheck(0);
	}

	pBtn = (CButton *)GetDlgItem(IDC_ISLAND_CHECKPASS);
	if (pBtn)
	{
		pBtn->SetCheck(0);
	}

}

bool CFormIsland::_LeaveEditMode()
{
	if (!m_bEditMode)
	{
		return true;
	}
	//can leave the edit mode
	CIsland * pIsl;
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();

	pIsl = pLayerMap->GetIslandList()->GetCurIsland();
	AString   msg;
	if (pIsl && (!pLayerMap->GetIslandList()->Update(pIsl, msg)))
	{
		AString errormsg;
		errormsg = msg + ", try to fix it";
		AfxMessageBox(AS2AC(errormsg));
		return false;
	}
	m_bEditMode = false;
	return true;
}


void CFormIsland::OnIslandSelect() 
{
	// TODO: Add your control notification handler code here
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap || !pLayerMap->GetIslandList())
	{
		return;
	}

	_RestoreBtn();
	CGridRender * pGrid = AUX_GetMainFrame()->GetView()->GetGridRender();


	//can switch the current island
	if (!_LeaveEditMode())
	{
		return;
	}
	
	CButton * pBtn = (CButton *)GetDlgItem(IDC_ISLAND_SELECT);
	assert(pBtn);
	if (pGrid->GetOp() != CGridRender::OP_ISLAND_SELECT)
	{
		pGrid->SetOp(CGridRender::OP_ISLAND_SELECT);
		pBtn->SetCheck(1);
	}
	else
	{
		pGrid->SetOp(CGridRender::OP_NONE);
	}
}

void CFormIsland::OnIslandAddborder() 
{
	// TODO: Add your control notification handler code here
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap|| !pLayerMap->GetIslandList()|| !pLayerMap->GetIslandList()->GetCurIsland())
	{
		return;
	}

	_RestoreBtn();

	m_bEditMode = true;
	CButton * pBtn = (CButton *)GetDlgItem(IDC_ISLAND_ADDBORDER);
	assert(pBtn);
	CGridRender * pGrid =  AUX_GetMainFrame()->GetView()->GetGridRender();
	if (pGrid->GetOp() != CGridRender::OP_ISLAND_ADDBORDER)
	{
		pGrid->SetOp(CGridRender::OP_ISLAND_ADDBORDER);
		pBtn->SetCheck(1);
	}
	else
	{
		pGrid->SetOp(CGridRender::OP_NONE);
	}
}


bool CFormIsland::AddBorderLine(const A3DPOINT2& pt1, const A3DPOINT2& pt2, bool &bStillInvalid)
{
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();

	if (!pLayerMap 
		|| !pLayerMap->GetIslandList()
		|| !pLayerMap->GetIslandList()->GetCurIsland())
	{
		return false;
	}

	//discard too close
	if (GetManhDist(pt1.x, pt1.y, pt2.x, pt2.y) < 5.0f)
	{
		return false;
	}
	pLayerMap->GetIslandList()->GetCurIsland()->AddBorderLine(pt1, pt2);
	AString strDummy;
	bStillInvalid = !pLayerMap->GetIslandList()->GetCurIsland()->IsValid(strDummy);
	return true;
}

void CFormIsland::SelectIsland(const A3DPOINT2& pt)
{
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();

	if (!pLayerMap
		|| !pLayerMap->GetIslandList())
	{
		return;
	}

	CIsland * pIsl = pLayerMap->GetIslandList()->SelectIsland(pt);
	pLayerMap->GetIslandList()->SetCurIsland(pIsl);
}


void CFormIsland::DelBorderPt(const A3DPOINT2& pt)
{
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap 
		|| !pLayerMap->GetIslandList()
		|| !pLayerMap->GetIslandList()->GetCurIsland())
	{
		return;
	}

	pLayerMap->GetIslandList()->GetCurIsland()->DelBorderPoint(pt);
}

void CFormIsland::OnIslandDelborder() 
{
	// TODO: Add your control notification handler code here
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap
		|| !pLayerMap->GetIslandList()
		|| !pLayerMap->GetIslandList()->GetCurIsland())
	{
		return;
	}

	_RestoreBtn();

	m_bEditMode = true;
	CButton * pBtn = (CButton *)GetDlgItem(IDC_ISLAND_DELBORDER);
	assert(pBtn);
	CGridRender * pGrid = AUX_GetMainFrame()->GetView()->GetGridRender();
	if (pGrid->GetOp() != CGridRender::OP_ISLAND_DELBORDER)
	{
		pGrid->SetOp(CGridRender::OP_ISLAND_DELBORDER);
		pBtn->SetCheck(1);
	}
	else
	{
		pGrid->SetOp(CGridRender::OP_NONE);
	}
}

void CFormIsland::OnIslandDel() 
{
	// TODO: Add your control notification handler code here
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap
		|| !pLayerMap->GetIslandList()
		|| !pLayerMap->GetIslandList()->GetCurIsland())
	{
		return;
	}

	_RestoreBtn();
	CGridRender * pGrid = AUX_GetMainFrame()->GetView()->GetGridRender();
	pGrid->SetOp(CGridRender::OP_NONE);

	CIsland * pIsl = pLayerMap->GetIslandList()->GetCurIsland();
	pLayerMap->GetIslandList()->DelIsland(pIsl);
	_ResetPassList();
}

void CFormIsland::OnIslandCheckpass() 
{
	// TODO: Add your control notification handler code here
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap
		|| !pLayerMap->GetIslandList())
	{
		return;
	}

	_RestoreBtn();
	CGridRender * pGrid = AUX_GetMainFrame()->GetView()->GetGridRender();
	pGrid->SetOp(CGridRender::OP_NONE);
	
	_CheckAllPasses();
}

void CFormIsland::OnIslandCreatepass() 
{
	// TODO: Add your control notification handler code here
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();

	if (!pLayerMap
		|| !pLayerMap->GetIslandList())
	{
		return;
	}

	_RestoreBtn();
	CGridRender * pGrid = AUX_GetMainFrame()->GetView()->GetGridRender();
	pGrid->SetOp(CGridRender::OP_NONE);

	//can leave the edit mode
	if (!_LeaveEditMode())
	{
		return;
	}

	pLayerMap->GetIslandList()->CreateAllPasses();

	_ResetPassList();
}

void CFormIsland::_ResetPassList()
{
	CComboBox  * pCmb = (CComboBox *)GetDlgItem(IDC_COMBO_PASSES);
	assert(pCmb);
	pCmb->ResetContent();

	vector<CIsland::Pass> * pPassList;
	unsigned int i;
	CIsland::Pass * pPass;
	AString msg;
	int  cur;
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	pPassList = pLayerMap->GetIslandList()->GetMainlandPasses();
	cur = -1;
	for (i = 0; i < pPassList->size(); ++i)
	{
		pPass = &(pPassList->at(i));
		msg.Format("%d -> %d", cur, pPass->dest);
		int index =	pCmb->AddString(AS2AC(msg));
		DWORD dwId;
		dwId = MAKE_DWORD(i, cur);
		pCmb->SetItemData(index, dwId);
	}


	unsigned int j;
	vector<CIsland*> * pAllIsland;
	pAllIsland = pLayerMap->GetIslandList()->GetAllIslands();
	for (j = 0; j < pAllIsland->size(); ++j)
	{
		CIsland * pIsl = pAllIsland->at(j);
		cur = j;
		pPassList = pIsl->GetAllPasses();
		for (i = 0; i < pPassList->size(); ++i)
		{
			pPass = &(pPassList->at(i));
			msg.Format("%d -> %d", cur, pPass->dest);
			int index =	pCmb->AddString(AS2AC(msg));
			DWORD dwId;
			dwId = MAKE_DWORD(i,  cur);
			pCmb->SetItemData(index, dwId);
		}
	}
}

void CFormIsland::OnIslandPassthrough() 
{
	// TODO: Add your control notification handler code here
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap
		|| !pLayerMap->GetIslandList())
	{
		return;
	}

	_RestoreBtn();
	CGridRender * pGrid = AUX_GetMainFrame()->GetView()->GetGridRender();

	//can leave the edit mode
	if (!_LeaveEditMode())
	{
		pGrid->SetOp(CGridRender::OP_NONE);
		return;
	}
	
	CButton * pBtn = (CButton *)GetDlgItem(IDC_ISLAND_PASSTHROUGH);
	assert(pBtn);
	if (pGrid->GetOp() != CGridRender::OP_ISLAND_SETPASSTHROUGH)
	{
		pGrid->SetOp(CGridRender::OP_ISLAND_SETPASSTHROUGH);
		pBtn->SetCheck(1);
	}
	else
	{
		pGrid->SetOp(CGridRender::OP_NONE);
	}
	
}


void CFormIsland::SetPassThrough(const A3DPOINT2& pt)
{
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();

	if (!pLayerMap
		|| !pLayerMap->GetIslandList())
	{
		return;
	}

	CIsland::Pass * pPass = pLayerMap->GetIslandList()->GetCurPass();
	if (pPass)
	{
		pPass->through = pt;
		_SetEditThrough(pt);
	}

}

void CFormIsland::_SetEditThrough(const A3DPOINT2& pt)
{
	AString  str;
	str.Format("%d,%d", pt.x, pt.y);
	SetDlgItemText(IDC_EDIT_PASSTHROUGH, AS2AC(str));
}

bool CFormIsland::_CheckAllPasses()
{
	CIsland * pIsl;
	int       iPass;
	AString   msg;
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap->GetIslandList()->CheckAllPasses(pIsl, iPass, msg))
	{
		AfxMessageBox(AS2AC(msg));
		pLayerMap->GetIslandList()->SetCurIsland(pIsl);
		pLayerMap->GetIslandList()->SetCurPass(iPass);
		CString   str;
		int cur = (pIsl)?(pIsl->GetId()):(-1);
		int dest = -9999;
		CIsland::Pass * pPass = pLayerMap->GetIslandList()->GetCurPass();
		if (pPass)
		{
			dest = pPass->dest;
			_SetEditThrough(pPass->through);
		}
		str.Format(_T("%d -> %d"), cur, dest );
		CComboBox  * pCmb = (CComboBox *)GetDlgItem(IDC_COMBO_PASSES);
		assert(pCmb);
		int index =	pCmb->FindString(0, str);
		if (index != LB_ERR)
		{
			pCmb->SetCurSel(index);
		}
		return false;
	}

	return true;

}

void CFormIsland::OnKillfocusEditPassthrough() 
{
	// TODO: Add your control notification handler code here
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap
		|| !pLayerMap->GetIslandList())
	{
		return;
	}

	_RestoreBtn();
	CGridRender * pGrid = AUX_GetMainFrame()->GetView()->GetGridRender();
	pGrid->SetOp(CGridRender::OP_NONE);

	//can leave the edit mode
	if (!_LeaveEditMode())
	{
		return;
	}

	CIsland::Pass * pPass = pLayerMap->GetIslandList()->GetCurPass();
	if (pPass)
	{
		pPass->through = _GetEditThrough();
	}
}


A3DPOINT2 CFormIsland::_GetEditThrough()
{
	static const int invalid = -9999;

	CString str;
	GetDlgItemText(IDC_EDIT_PASSTHROUGH, str);
	
	int comma = str.Find(',');
	if (comma == -1)
	{
		return A3DPOINT2(invalid, invalid);
	}
	
	CString xStr, yStr;
	xStr = str.Left(comma);
	yStr = str.Right(str.GetLength() - comma - 1);

	A3DPOINT2   pt;
	pt.x = atoi(AC2AS(xStr));
	pt.y = atoi(AC2AS(yStr));

	return pt;
}

void CFormIsland::OnSelchangeComboPasses() 
{
	// TODO: Add your control notification handler code here
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();
	if (!pLayerMap
		|| !pLayerMap->GetIslandList())
	{
		return;
	}

	_RestoreBtn();
	CGridRender * pGrid = AUX_GetMainFrame()->GetView()->GetGridRender();
	pGrid->SetOp(CGridRender::OP_NONE);

	if (!_LeaveEditMode())
	{
		return;
	}
	CComboBox  * pCmb = (CComboBox *)GetDlgItem(IDC_COMBO_PASSES);
	assert(pCmb);

	int index = pCmb->GetCurSel();
	if (index == CB_ERR)
	{
		return;
	}
	DWORD dwId = pCmb->GetItemData(index);
	int isl_index = (short)GET_HIWORD(dwId);
	int pass_index = GET_LOWORD(dwId);
	
	pLayerMap->GetIslandList()->SetCurIsland(isl_index);
	pLayerMap->GetIslandList()->SetCurPass(pass_index);
	
	CIsland::Pass * pPass = pLayerMap->GetIslandList()->GetCurPass();
	if (pPass)
	{
		_SetEditThrough(pPass->through);
	}
}
