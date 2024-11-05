/**********************************************************************
 *<
	FILE: SkinMap.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "SkinMap.h"
#include "SkinMapper.h"

#define SKINMAP_CLASS_ID	Class_ID(0x1c3bacf9, 0x2095e2a4)

class SkinMap : public UtilityObj 
{
public:
	HWND			hPanel;
	IUtil			*iu;
	Interface		*ip;
	
	void BeginEditParams(Interface *ip,IUtil *iu);
	void EndEditParams(Interface *ip,IUtil *iu);
	
	void Init(HWND hWnd);
	void Destroy(HWND hWnd);
	
	void DeleteThis() { }		
	//Constructor/Destructor
	
	SkinMap();
	~SkinMap();		
	
	void UpdateSrcList();
	void OnListDblClick();
	void UpdateStragyRadioButtons();
	
	CSkinMapper		theMapper;
	void AddSrc();
	void DeleteSrc();
	void ClearSrc();
	void DoMap();
};

static SkinMap theSkinMap;

class SkinMapClassDesc:public ClassDesc2 
{
public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return &theSkinMap; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return SKINMAP_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }
	
	const TCHAR*	InternalName() { return _T("SkinMap"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
	
};

static SkinMapClassDesc SkinMapDesc;
ClassDesc2* GetSkinMapDesc() { return &SkinMapDesc; }


static BOOL CALLBACK SkinMapDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			theSkinMap.Init(hWnd);			
			break;

		case WM_DESTROY:
			theSkinMap.Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_BUTTONADDSRC:
				theSkinMap.AddSrc();
				break;

			case IDC_BUTTONDELETESRC:
				theSkinMap.DeleteSrc();
				break;

			case IDC_BUTTONCLEARSRC:
				theSkinMap.ClearSrc();
				break;

			case IDC_BUTTONDOMAP:
				theSkinMap.DoMap();
				break;

			case IDC_CLOSEST_VERTEX:
				theSkinMap.theMapper.SetMappingStategy(CLOSEST_VERTEX);
				break;
			case IDC_CLOSEST_EDGE:
				theSkinMap.theMapper.SetMappingStategy(CLOSEST_EDGE);
				break;
			case IDC_CLOSEST_TRIANGLE:
				theSkinMap.theMapper.SetMappingStategy(CLOSEST_TRIANGLE);
				break;

			case IDC_LISTSRCNODES:
				if( HIWORD(wParam) == LBN_DBLCLK )
					theSkinMap.OnListDblClick();
				break;
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			theSkinMap.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}
	return TRUE;
}



//--- SkinMap -------------------------------------------------------
SkinMap::SkinMap()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
}

SkinMap::~SkinMap()
{

}

void SkinMap::UpdateSrcList()
{
	int nNumSrc = theMapper.GetNumSourceNode();
	if( nNumSrc == 0 )
	{
		::EnableWindow(GetDlgItem(hPanel, IDC_BUTTONDOMAP), FALSE);
		return;
	}

	::EnableWindow(GetDlgItem(hPanel, IDC_BUTTONDOMAP), TRUE);
	HWND hList = GetDlgItem(hPanel, IDC_LISTSRCNODES);
	int nCurSel = ::SendMessage(hList, LB_GETCURSEL, 0, 0);
	::SendMessage(hList, LB_RESETCONTENT, 0, 0);

	for(int i=0; i<nNumSrc; i++)
		::SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)theMapper.GetSourceNode(i)->GetName());

	if( nCurSel >= nNumSrc )
		::SendMessage(hList, LB_SETCURSEL, nNumSrc - 1, 0);
	else
		::SendMessage(hList, LB_SETCURSEL, nCurSel, 0);
}

void SkinMap::OnListDblClick()
{
	HWND hList = GetDlgItem(hPanel, IDC_LISTSRCNODES);
	int nCurSel = ::SendMessage(hList, LB_GETCURSEL, 0, 0);
	if( nCurSel == -1 )
		return;

	theHold.Begin();
	GetCOREInterface()->SelectNode(theMapper.GetSourceNode(nCurSel));
	TSTR undostr; undostr.printf("Select");
	theHold.Accept(undostr); 

	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
}

void SkinMap::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		SkinMapDlgProc,
		GetString(IDS_PARAMS),
		0);

	UpdateSrcList();
	UpdateStragyRadioButtons();
}
	
void SkinMap::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
	theMapper.RemoveAllSourceNodes();
}

void SkinMap::Init(HWND hWnd)
{
}

void SkinMap::UpdateStragyRadioButtons()
{
	HWND radio;
	radio = GetDlgItem(hPanel, IDC_CLOSEST_VERTEX);
	if(theMapper.GetMappingStrategey() == CLOSEST_VERTEX)
		::SendMessage(radio, BM_SETCHECK, BST_CHECKED, 0);
	else
		::SendMessage(radio, BM_SETCHECK, BST_UNCHECKED, 0);

	radio = GetDlgItem(hPanel, IDC_CLOSEST_EDGE);
	if(theMapper.GetMappingStrategey() == CLOSEST_EDGE)
		::SendMessage(radio, BM_SETCHECK, BST_CHECKED, 0);
	else
		::SendMessage(radio, BM_SETCHECK, BST_UNCHECKED, 0);

	radio = GetDlgItem(hPanel, IDC_CLOSEST_TRIANGLE);
	if(theMapper.GetMappingStrategey() == CLOSEST_TRIANGLE)
		::SendMessage(radio, BM_SETCHECK, BST_CHECKED, 0);
	else
		::SendMessage(radio, BM_SETCHECK, BST_UNCHECKED, 0);
}

void SkinMap::Destroy(HWND hWnd)
{

}

class MyHitByNameDlgCallback : public HitByNameDlgCallback
{
	TCHAR *dialogTitle() { return _T("Select objects you want to map weight from:"); }
	TCHAR *buttonText() { return _T("Select"); }
	BOOL singleSelect() { return TRUE; }
	BOOL useProc() { return FALSE; }
	BOOL filter(INode * pNode); 
};

BOOL MyHitByNameDlgCallback::filter(INode * pNode)
{
	if( !FindSkinModifier(pNode) )
		return FALSE;

	if( pNode->IsGroupHead() )
		return FALSE;

	ObjectState os = pNode->EvalWorldState(GetCOREInterface()->GetTime());
	if( !os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
		return FALSE;

	return TRUE;
}

class MyHitByNameDlgCallbackDest : public HitByNameDlgCallback
{
	TCHAR *dialogTitle() { return _T("Select object you want to map to"); }
	TCHAR *buttonText() { return _T("DoMap"); }
	BOOL singleSelect() { return TRUE; }
	BOOL useProc() { return FALSE; }
	BOOL filter(INode * pNode); 
};

BOOL MyHitByNameDlgCallbackDest::filter(INode * pNode)
{
	if( !FindSkinModifier(pNode) )
		return FALSE;

	if( pNode->IsGroupHead() )
		return FALSE;

	ObjectState os = pNode->EvalWorldState(GetCOREInterface()->GetTime());
	if( !os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
		return FALSE;

	// filter out the source nodes.
	for(int i=0; i<theSkinMap.theMapper.GetNumSourceNode(); i++)
	{
		if( pNode == theSkinMap.theMapper.GetSourceNode(i) )
			return FALSE;
	}

	return TRUE;
}

void SkinMap::AddSrc()
{
	MyHitByNameDlgCallback hitCallBack;
	if( !GetCOREInterface()->DoHitByNameDialog(&hitCallBack) )
		return;

	int nSelCount = GetCOREInterface()->GetSelNodeCount();
	if( nSelCount == 0 )
	{
		MessageBox(NULL, "Nothing has been selected!\nProcess Aborted!", "ABORTED", MB_ICONINFORMATION | MB_OK);
		return;
	}

	INode * pNode;
	pNode = GetCOREInterface()->GetSelNode(0);
	if( !theMapper.AddSourceNode(pNode) )
		return;

	UpdateSrcList();
}

void SkinMap::DeleteSrc()
{
	HWND hList = ::GetDlgItem(hPanel, IDC_LISTSRCNODES);
	int nCurSel = ::SendMessage(hList, LB_GETCURSEL, 0, 0);
	if( nCurSel == -1 )
		return;

	theMapper.RemoveSourceNode(nCurSel);
	UpdateSrcList();
}

void SkinMap::ClearSrc()
{
	theMapper.RemoveAllSourceNodes();
	UpdateSrcList();
}

void SkinMap::DoMap()
{
	INode * pNode;

	MyHitByNameDlgCallbackDest hitCallBack;
	if( !GetCOREInterface()->DoHitByNameDialog(&hitCallBack) )
		return;

	int nSelCount = GetCOREInterface()->GetSelNodeCount();
	if( nSelCount == 0 )
	{
		MessageBox(NULL, "Nothing has been selected!\nProcess Aborted!", "ABORTED", MB_ICONINFORMATION | MB_OK);
		return;
	}

	pNode = GetCOREInterface()->GetSelNode(0);
	if( !theMapper.DoMapping(hPanel, pNode) )
		return;
}