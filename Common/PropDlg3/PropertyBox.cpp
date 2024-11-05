// PropertyBox.cpp : implementation file
//

#include "stdafx.h"
#include "PropDlg_rc.h"
#include "AProperty.h"

#include "PropertyBox.h"
#include "PropertyList.h"
#include "PropertyBooleanBox.h"
#include "PropertyIntegerBox.h"
#include "PropertyStringBox.h"
#include "PropertyFloatBox.h"
#include "PropertyFileBox.h"
#include "PropertyVectorBox.h"
#include "PropertyIDBox.h"
#include "PropertyColorBox.h"
#include "PropertyMemoBox.h"
#include "PropertyFontBox.h"
#include "PropertyCustomBox.h"
#include "PropertyDoubleBox.h"
#include "PropertySliderBox.h"
#include "PropertyInteger64Box.h"
#include "AObject.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyBox
A3DEngine* CPropertyBox::g_pA3DEngine = NULL;

CPropertyBox::CPropertyBox()
{
	m_ptr_property	= NULL;
	m_ptr_data	= NULL;
	m_defaultState  = 0;
	m_property_index = -1;
	m_bReadOnly = false;
}

CPropertyBox::~CPropertyBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyBox, CWnd)
	//{{AFX_MSG_MAP(CPropertyBox)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GETFONT, OnGetFont)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyBox message handlers


BOOL CPropertyBox::Create(LPCSTR title, DWORD style, CRect &rect, CWnd *pParent, UINT nID, APropertyObject * pData, AProperty * pProperty)
{
//	ASSERT(pProperty);

	BOOL bReadOnly = (pProperty->GetWay() & WAY_READONLY) != 0;
	LPCTSTR szClass = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW | CS_PARENTDC, 
		LoadCursor(NULL, IDC_ARROW),
		GetSysColorBrush(COLOR_WINDOW),
		//GetSysColorBrush(bReadOnly ? COLOR_HIGHLIGHT : COLOR_WINDOW),
		NULL);

	m_ptr_data = pData;
	m_ptr_property = pProperty;
	if(pProperty == NULL)
	{
		m_defaultState = -1;
	}
	else
	{
		int index = -1;
		if(pData->GetProperty(pProperty->GetName(),&index) == NULL)
		{
			assert(false);
			pParent->MessageBox(_T("寻找属性索引时失败"));
			return FALSE;
		}
		m_property_index = index;
		m_defaultState = pData->GetState(index);
	}

	return CWnd::CreateEx(
		0,
		CSafeString(szClass),
		CSafeString(title),
		style,
		rect,
		pParent,
		nID,
		NULL);
}

#define INNER_WAY_DOUBLE	0xffff

CPropertyBox * CPropertyBox::CreatePropertyBox(CWnd * pParent, UINT nID, CRect & rc, APropertyObject * pData, AProperty *pProperty)
{
	// 根据AProperty的类型选择具体的编辑器
	ASSERT(pParent);
	ASSERT(pProperty);

	int way = pProperty->GetWay();

	BOOL bReadOnly = way & WAY_READONLY;	// 取只读标识
	way = way & ~WAY_READONLY;

	if(way == WAY_DEFAULT)
	{
		switch(pProperty->GetValue(pData).GetType())
		{
		case AVariant::AVT_INVALIDTYPE:	ASSERT(FALSE);		break;
		case AVariant::AVT_BOOL:		way = WAY_BOOLEAN;	break;
		case AVariant::AVT_CHAR:		way = WAY_INTEGER;	break;
		case AVariant::AVT_UCHAR:		way = WAY_INTEGER;	break;
		case AVariant::AVT_SHORT:		way = WAY_INTEGER;	break;
		case AVariant::AVT_USHORT:		way = WAY_INTEGER;	break;
		case AVariant::AVT_INT:			way = WAY_INTEGER;	break;
		case AVariant::AVT_UINT:		way = WAY_INTEGER;	break;
		case AVariant::AVT_LONG:		way = WAY_INTEGER;	break;
		case AVariant::AVT_ULONG:		way = WAY_INTEGER;	break;
		case AVariant::AVT_INT64:		way = WAY_INTEGER64;	break;
		case AVariant::AVT_UINT64:		way = WAY_INTEGER64;	break;
		case AVariant::AVT_FLOAT:		way = WAY_FLOAT;	break;
		case AVariant::AVT_DOUBLE:		way = INNER_WAY_DOUBLE;	break;
		case AVariant::AVT_POBJECT:		way = WAY_OBJECT;	break;
		case AVariant::AVT_PSCRIPT:		way = WAY_SCRIPT;	break;	
		case AVariant::AVT_APointF:		way = WAY_VECTOR2;	break;		// AVT_APointF, AVT_A3DVECTOR3, AVT_A3DVECTOR4 用同一个对话框编辑(数目不同)
		case AVariant::AVT_A3DVECTOR3:	way = WAY_VECTOR3;	break;
		case AVariant::AVT_A3DVECTOR4:	way = WAY_VECTOR4;	break;
		case AVariant::AVT_A3DMATRIX4:	break; // 不支持对矩阵的编辑
		case AVariant::AVT_STRING:		way = WAY_STRING;	break;
		case AVariant::AVT_PABINARY:	way = WAY_BINARY;	break;
		case AVariant::AVT_PACLASSINFO:	break; // 不支持编辑类信息
		case AVariant::AVT_PASET:		break; // 不支持编辑集合
		case AVariant::AVT_FONT:		way = WAY_FONT;		break;
		}
	}

	CPropertyBox * pBox = NULL;

	switch(way)
	{
	case WAY_BOOLEAN:	pBox = new CPropertyBooleanBox();break;
	case WAY_INTEGER:	pBox = new CPropertyIntegerBox();break;
	case WAY_INTEGER64: pBox = new CPropertyInteger64Box();break;
	case WAY_FLOAT:		pBox = new CPropertyFloatBox();break;
	case WAY_STRING:	pBox = new CPropertyStringBox();break;
	case WAY_MEMO:		pBox = new CPropertyMemoBox();break;
	case WAY_FILENAME:	
	case WAY_AUDIOEVENT:
		pBox = new CPropertyFileBox();break;
	case WAY_COLOR:		pBox = new CPropertyColorBox();break;
	case WAY_VECTOR2:	pBox = new CPropertyVectorBox(2);break;
	case WAY_VECTOR3:	pBox = new CPropertyVectorBox(3);break;
	case WAY_VECTOR4:	pBox = new CPropertyVectorBox(4);break;
	case WAY_FONT:		pBox = new CPropertyFontBox(); break;
	case WAY_CUSTOM:	pBox = new CPropertyCustomBox(); break;
	case INNER_WAY_DOUBLE:	pBox = new CPropertyDoubleBox(); break;
	case WAY_SLIDER:		pBox = new CPropertySliderBox();break;
//	case WAY_SCRIPT:	pBox = new CPropertyScriptBox();break;
//	case WAY_BINARY:	pBox = new CPropertyBox(); break;
//	case WAY_OBJECT:	pBox = new CPropertyObjectBox();break;
//	case WAY_WAYPOINTID:	pBox = new CPropertyIDBox(_T("路点"));break;
//	case WAY_IDOBJECT:	pBox = new CPropertyIDBox(_T(""));break;
//	case WAY_TRIGGEROBJECT:	pBox = new CPropertyIDBox(_T("触发器"));break;
//	case WAY_NAME:		pBox = new CSpecStringBox("",CSpecStringBox::NAME);break;
//	case WAY_UNITID:	pBox = new CPropertyIDBox(_T("单位"));break;
//	case WAY_PATHID:	pBox = new CPropertyIDBox(_T("路线"));break;
//	case WAY_STRID:		pBox = new CPropertyIDBox(_T("字串"));break;
//	case WAY_SFXID:		pBox = new CPropertyIDBox(_T("音效"));break;
	default: 
		assert(false && "尚不被支持的属性");
		break;
	}

	if(pBox)
	{
		BOOL bResult = pBox->Create(CSafeString(pProperty->GetName()), WS_CHILD| WS_TABSTOP , rc, pParent, nID, pData, pProperty);
		ASSERT(bResult);
		pBox->SetReadOnly(bReadOnly);
	}

	return pBox;
}

void CPropertyBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DrawSelf(&dc);
}

void CPropertyBox::DrawSelf(CDC *pDC)
{
	
}

void CPropertyBox::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	// TODO: 显示窗口时自动将焦点置向第一个字窗口
	if(bShow)
	{
		UpdateData(FALSE);
		CWnd * pChild = GetWindow(GW_CHILD);
		if(pChild)
		{
			pChild->SetFocus();
		}
	}
	else
	{
		if(IsModified())
		{
			UpdateData(TRUE);
		}
	}
}

int CPropertyBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: 创建字体
	CFont * pFont = GetParent()->GetFont();
	LOGFONT info;
	pFont->GetLogFont(&info);
	m_gdi_font.CreateFontIndirect(&info);
	if(m_defaultState != -1) EnableWindow(!m_defaultState);
	return 0;
}

LRESULT CPropertyBox::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	return LRESULT(m_gdi_font.m_hObject);
}

BOOL CPropertyBox::IsModified()
{
	return FALSE;
}

BOOL CPropertyBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}
