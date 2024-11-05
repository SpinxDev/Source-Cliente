// PropertyFileBox.cpp : implementation file
//

#include "stdafx.h"
#include "PropDlg_rc.h"
#include "AProperty.h"
#include "PropertyFileBox.h"
#include <AString.h>
#include <AFileDialogWrapper.h>
#include "AFI.h"
#include "PropertyList.h"
#include "PropertyInterface.h"

#ifndef _ANGELICA21
#include "EventSelector.h"
#include "FEventSystem.h"
#include "FEventGroup.h"
#include "FEvent.h"

namespace AudioEngine
{
	class Event;
	class EventGroup;
	class EventManager;
}
#endif	//ndef _ANGELICA21


/////////////////////////////////////////////////////////////////////////////
// CPropertyFileBox

CPropertyFileBox::CPropertyFileBox()
{
}

CPropertyFileBox::~CPropertyFileBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyFileBox, CPropertyBox)
	//{{AFX_MSG_MAP(CPropertyFileBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_CONTROL(BN_CLICKED, 101, OnBnClicked)
	ON_CONTROL(BN_CLICKED, 102, OnEditChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyFileBox message handlers

int CPropertyFileBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 创建控制
	BOOL bResult = m_wnd_button.Create(
		_T(".."),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(0,0,0,0),
		this,
		101);

	ASSERT(bResult);
	m_wnd_button.SetFont(GetFont());
	m_wnd_button.ModifyStyle(WS_TABSTOP,0);
	AString tmpV = m_ptr_property->GetValue(m_ptr_data);
//	SL - Need to convert to unicode string
	//m_str_file = (AString)tmpV;
	CSafeString test(tmpV);
	m_str_file = test.GetUnicode();
//	SL - Need to convert to unicode string

	getEventPath();

	if(m_str_file == _T(" "))
		m_str_file = _T("");
	m_str_path = m_ptr_property->GetPath();

	// TODO: 创建控件
	bResult = m_wnd_edit.Create(
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		CRect(0,0,0,0),
		this,
		102);

	ASSERT(bResult);
	m_wnd_edit.SetFont(GetFont());

	UpdateData(FALSE);
	return 0;
}

void CPropertyFileBox::getEventPath()
{
#ifndef _ANGELICA21
	if(m_ptr_property->GetWay() == WAY_AUDIOEVENT)
	{
		if (APropertyInterface* pInterface = APropertyInterface::GetPropertyInterface())
		{
			AudioEngine::Event* pEvent = pInterface->GetEventSystem()->GetEvent(AC2AS(m_str_file));
			if(pEvent)
			{
				m_str_event_path = AS2AC(pEvent->GetFullPath());
			}
			else
			{
				m_str_event_path = _T("");
			}
		}
	}	
#endif
}

void CPropertyFileBox::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	if(pDX->m_bSaveAndValidate)
	{
		
#ifdef _ANGELICA21
		m_wnd_edit.GetWindowText(m_str_file);
		DDX_Text(pDX, 102, m_str_file);
#else
		if(m_ptr_property->GetWay() == WAY_AUDIOEVENT)
		{
			m_wnd_edit.GetWindowText(m_str_event_path);
			DDX_Text(pDX, 102, m_str_event_path);
		}
		else if(m_ptr_property->GetWay() == WAY_FILENAME)
		{
			m_wnd_edit.GetWindowText(m_str_file);
			DDX_Text(pDX, 102, m_str_file);
		}
#endif
		//if(m_str_file.GetLength()==0) 
		//	m_str_file = _T(" ");
		//杨智盈为什么要这样写，还没有搞清除....
		m_ptr_property->SetValue(m_ptr_data, AString(CSafeString(m_str_file)));
		m_wnd_edit.SetModify(FALSE);
	}
	else
	{
//	SL - Need to convert to unicode string
		AString tmpV = m_ptr_property->GetValue(m_ptr_data);
		CSafeString test(tmpV);
		m_str_file = test.GetUnicode();
		//m_str_file = (AString)m_ptr_property->GetValue(m_ptr_data);
//	SL - Need to convert to unicode string

		getEventPath();
#ifdef _ANGELICA21
		DDX_Text(pDX, 102, m_str_file);
#else
		if(m_ptr_property->GetWay() == WAY_AUDIOEVENT)
		{			
			DDX_Text(pDX, 102, m_str_event_path);
		}
		else if(m_ptr_property->GetWay() == WAY_FILENAME)
		{
			DDX_Text(pDX, 102, m_str_file);
		}		
#endif		
	}

	CPropertyBox::DoDataExchange(pDX);
}

void CPropertyFileBox::DrawSelf(CDC *pDC)
{
	ASSERT(pDC);
	ASSERT(m_ptr_property);

	int edge_cx = GetSystemMetrics(SM_CXDLGFRAME),
		edge_cy = GetSystemMetrics(SM_CYDLGFRAME);

	CString text;

#ifdef _ANGELICA21
	text = m_str_file;
#else	
	if(m_ptr_property->GetWay() == WAY_AUDIOEVENT)
	{			
		text = m_str_event_path;
	}
	else if(m_ptr_property->GetWay() == WAY_FILENAME)
	{
		text = m_str_file;
	}
#endif

	CRect rc;
	GetClientRect(rc);

	rc.top += edge_cy;
	rc.left += edge_cx + 1;
	rc.bottom -= edge_cy;

	int iSave = pDC->SaveDC();
	pDC->SelectObject(GetFont());
	pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));

	pDC->DrawText(text, rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	pDC->RestoreDC(iSave);
}

BOOL CPropertyFileBox::IsModified()
{
	return m_wnd_edit.GetModify();
}

void CPropertyFileBox::OnBnClicked()
{
	SetCurrentDirectoryA(af_GetBaseDir());
	OutputDebugStringA(af_GetBaseDir());
	OutputDebugStringA("\n");

	if (m_ptr_property->GetWay() == WAY_FILENAME)
	{
		AFileDialogWrapper dlg(g_pA3DEngine->GetA3DDevice(), TRUE, CSafeString(m_str_path), _T("Select File:"), _T(""), AFileDialogWrapper::AFD_USEDEFAULT, this);
		if (dlg.DoModal() != IDOK)
			return;

		m_str_file = dlg.GetRelativePathName();
	}
	else if (m_ptr_property->GetWay() == WAY_AUDIOEVENT)
	{
#ifdef _ANGELICA21
		ASSERT(0 && "should never get here");
#else	//ifndef _ANGELICA21
		if (APropertyInterface* pInterface = APropertyInterface::GetPropertyInterface())
		{
			A3DVECTOR3 vCamPos(pInterface->GetCamPos());
			EventSelector esdlg(pInterface->GetEventSystem(), VECTOR(vCamPos.x, vCamPos.y, vCamPos.z));
			ACString strTmp(m_str_file);
			esdlg.SetInitSelectEvent(AC2AS(strTmp));
			if (esdlg.DoModal() != IDOK)
				return;

			m_str_file = esdlg.GetSelectEventGuid();
			getEventPath();
		}
#endif	//ndef _ANGELICA21
	}
#ifdef _ANGELICA21
	m_wnd_edit.SetWindowText(m_str_file);
#else	
	if(m_ptr_property->GetWay() == WAY_AUDIOEVENT)
	{			
		m_wnd_edit.SetWindowText(m_str_event_path);
	}
	else if(m_ptr_property->GetWay() == WAY_FILENAME)
	{
		m_wnd_edit.SetWindowText(m_str_file);
	}
#endif
	m_wnd_edit.SetModify(TRUE);
	m_ptr_property->SetValue(m_ptr_data, AString(CSafeString(m_str_file)));
	CPropertyList *pList = (CPropertyList*)GetParent();
	GetParent()->GetParent()->PostMessage(WM_LIST_DATA_CHANGED,(WPARAM)pList,pList->m_int_selected);
}

void CPropertyFileBox::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyBox::OnSize(nType, cx, cy);
	
	// TODO: 将按钮定位在右侧
	m_wnd_edit.MoveWindow(4,4,cx - cy -4,cy - 4);
	m_wnd_button.MoveWindow(
		cx - cy + 1, 1,
		cy - 2, cy - 1);
}

void CPropertyFileBox::OnEditChanged()
{
	m_wnd_edit.SetModify();
}

void CPropertyFileBox::SetReadOnly( BOOL bReadOnly )
{
	if (m_wnd_edit.GetSafeHwnd())
	{
		m_wnd_edit.SetReadOnly(bReadOnly);
	}

	if (m_wnd_button.GetSafeHwnd())
	{
		m_wnd_button.EnableWindow(!bReadOnly);
	}

	CPropertyBox::SetReadOnly(bReadOnly);
}
