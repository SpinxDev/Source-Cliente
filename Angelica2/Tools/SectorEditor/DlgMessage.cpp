// DlgMessage.cpp : implementation file
//

#include "stdafx.h"
#include "SectorEditor.h"
#include "DlgMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMessage dialog

IMPLEMENT_DYNCREATE(CDlgMessage, CDialogBar)

CDlgMessage::CDlgMessage() : CDialogBar()
{
	//{{AFX_DATA_INIT(CDlgMessage)
	//}}AFX_DATA_INIT
}


void CDlgMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMessage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMessage, CDialogBar)
	//{{AFX_MSG_MAP(CDlgMessage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMessage message handlers

void CDlgMessage::SetWindowText(LPCTSTR szText)
{
	GetDockingFrame()->SetWindowText(szText);
	m_pDockBar->SetWindowText(szText);
	CDialogBar::SetWindowText(szText);
}

//	Clear all message
void CDlgMessage::ClearAllMessages()
{
	CListBox* pMsgList = GetMessageList();
	pMsgList->ResetContent();
}

//	Add a message
void CDlgMessage::AddMessage(char* szMsg)
{
	CListBox* pMsgList = GetMessageList();
	pMsgList->InsertString(-1, szMsg);
}

//	Get message list
CListBox* CDlgMessage::GetMessageList()
{
	CListBox* pMsgList = (CListBox*)GetDlgItem(IDC_LIST_MESSAGE);
	ASSERT(pMsgList);
	return pMsgList;
}

