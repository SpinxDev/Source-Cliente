// FormClu.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPFImp.h"
#include "FormClu.h"

#include "Global.h"
#include "MainFrm.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////
//class CCheckDiag
class CCheckDiag
{
public:
	CCheckDiag(){};
	~CCheckDiag() {};
	void SetLayer(CLayerMap * pLayerMap)
	{
		m_pLayerMap = pLayerMap;
	}
	bool Check();
	void RetrieveFailPair(A3DPOINT2& diag1, A3DPOINT2& diag2) const
	{
		diag1 = m_Diag1;
		diag2 = m_Diag2;
	}
private:
	bool _CheckHorizEntrances(int start, int end, int latitude);
	bool  _CheckVertEntrances(int start, int end, int longitude);
	bool  _IsDiagonalPattern1(int x, int z, A3DPOINT2& diag);
	bool  _IsDiagonalPattern2(int x, int z, A3DPOINT2& diag);
	bool  _IsDiagonalPattern3(int x, int z, A3DPOINT2& diag);
	bool  _IsDiagonalPattern4(int x, int z, A3DPOINT2& diag);

private:
	CLayerMap *    m_pLayerMap;
	A3DPOINT2      m_Diag1;
	A3DPOINT2      m_Diag2;
};


bool CCheckDiag::Check()
{
	int mapW, mapH;
	m_pLayerMap->GetRMap()->GetImageSize(mapW, mapH);
	int iClusterSize =	m_pLayerMap->GetCluGraph()->GetCluSize();
	int horizSize,vertSize;


	for (int j = 0; j < mapH; j+=iClusterSize)
	{
		for (int i = 0; i< mapW; i+=  iClusterSize)
		{
			horizSize = a_Min(iClusterSize, mapW-i);
			vertSize  = a_Min(iClusterSize, mapH-j);
			if (j > 0 && j < mapH )
			{
				
				if(!_CheckHorizEntrances(i,i+horizSize-1,j-1))
				{
					
					return false; 
				}
			}
			if (i > 0 && i < mapW)
			{
				
				if(!_CheckVertEntrances(j,j+vertSize-1,i-1))
				{
					return false;
				}
			}

		}

	}  

	return true;

}

bool CCheckDiag::_CheckHorizEntrances(int start, int end, int latitude)
{
	CBitImage * pRMap = m_pLayerMap->GetRMap();

	int cur = latitude +1;
	for (int i=start; i<=end; i++)
	{
		if (!pRMap->GetPixel(i, cur))
		{
			continue;
		}
		
		A3DPOINT2 diag;
		if (_IsDiagonalPattern1(i, cur, diag) || _IsDiagonalPattern2(i, cur, diag))
		{
			// todo: additional check? [6/3/2008 kuiwu]
			m_Diag1.x = i;
			m_Diag1.y = cur;
			m_Diag2 = diag;
			return false;
		}
		
	}

	return true;
}

bool CCheckDiag::_CheckVertEntrances(int start, int end, int longitude)
{
	CBitImage * pRMap = m_pLayerMap->GetRMap();


	int cur = longitude +1;
	for (int i=start; i<=end; i++)
	{
		if (!pRMap->GetPixel(cur, i))
		{
			continue;
		}
		
		A3DPOINT2 diag;
		if (_IsDiagonalPattern3(cur, i, diag) || _IsDiagonalPattern4(cur, i, diag))
		{
			// todo: additional check? [6/3/2008 kuiwu]
			m_Diag1.x = cur;
			m_Diag1.y = i;
			m_Diag2 = diag;
			return false;
		}
		
	}

	return true;
}

/**
 *     x  o <- (x,z)
 *     o  x
 */
bool CCheckDiag::_IsDiagonalPattern1(int x, int z, A3DPOINT2& diag)
{
	diag.x = x-1;
	diag.y = z-1;
	CBitImage * pRMap = m_pLayerMap->GetRMap();

	if ((!pRMap->GetPixel(x-1, z))
		&& (pRMap->GetPixel(x, z))
		&& (pRMap->GetPixel(x-1, z-1))
		&& (!pRMap->GetPixel(x, z-1))
		)
	{
		return true;
	}
		

	return false;
}

/**
 *  (x,z)-> o  x 
 *			x  o
 */
bool CCheckDiag::_IsDiagonalPattern2(int x, int z, A3DPOINT2& diag)
{
	diag.x = x+1;
	diag.y = z-1;

	CBitImage * pRMap = m_pLayerMap->GetRMap();
	if ((pRMap->GetPixel(x, z))
		&& (!pRMap->GetPixel(x+1, z))
		&& (!pRMap->GetPixel(x, z-1))
		&& (pRMap->GetPixel(x+1, z-1))
		)
	{
		return true;
	}
		

	return false;
}

/**
 *			o  x 
 *			x  o  <-(x,z)
 */
bool CCheckDiag::_IsDiagonalPattern3(int x, int z, A3DPOINT2& diag)
{
	diag.x = x-1;
	diag.y = z+1;
	CBitImage * pRMap = m_pLayerMap->GetRMap();

	if ((pRMap->GetPixel(x-1, z+1))
		&& (!pRMap->GetPixel(x, z+1))
		&& (!pRMap->GetPixel(x-1, z))
		&& (pRMap->GetPixel(x, z))
		)
	{
		return true;
	}
		

	return false;
}


/**
 *			x  o <-(x,z)	
 *			o  x  
 */
bool CCheckDiag::_IsDiagonalPattern4(int x, int z, A3DPOINT2& diag)
{
	diag.x = x-1;
	diag.y = z-1;
	CBitImage * pRMap = m_pLayerMap->GetRMap();
	if ((!pRMap->GetPixel(x-1, z))
		&& (pRMap->GetPixel(x, z))
		&& (pRMap->GetPixel(x-1, z-1))
		&& (!pRMap->GetPixel(x, z-1))
		)
	{
		return true;
	}
		

	return false;
}


//////////////////////////////////////////////////////////////////////////
//class CCheckIslet

class CCheckIslet
{
public:
	CCheckIslet();
	~CCheckIslet();
	void SetRMap(CBitImage * pRMap);
	bool Check();

	vector<A3DPOINT2> * GetFailList()
	{
		return &m_FailList;
	}
	
private:
	bool _Check(int x, int z);
	bool _Add2NeighborList(A3DPOINT2& pt);

	bool _IsChecked(int x, int z)
	{
		return m_CheckMask[z * m_Width + x];
	}
	void _SetChecked(int x, int z, bool check)
	{
		m_CheckMask[z*m_Width + x] = check;
	}
private:
	int m_Width;
	int m_Height;
	bool * m_CheckMask;
	vector<A3DPOINT2> m_NeighborList;
	vector<A3DPOINT2> m_FailList;
	CBitImage  * m_pRMap;
};


CCheckIslet::CCheckIslet()
:m_pRMap(NULL), m_CheckMask(NULL)
{

}

CCheckIslet::~CCheckIslet()
{
	if (m_CheckMask)
	{
		delete[] m_CheckMask;
	}
}

void CCheckIslet::SetRMap(CBitImage * pRMap)
{
	m_pRMap = pRMap;
	m_pRMap->GetImageSize(m_Width, m_Height);
	if (m_CheckMask)
	{
		delete[] m_CheckMask;
	}
	m_CheckMask = new bool[m_Width*m_Height];
}

bool CCheckIslet::Check()
{
	memset(m_CheckMask, 0, sizeof(bool)*m_Width*m_Height);
	m_FailList.clear();
	int x, z;
	for (z = 0; z < m_Height; z++)
	{
		for (x = 0; x < m_Width; x++)
		{
			if (!_IsChecked(x, z) && m_pRMap->GetPixel(x, z) && !_Check(x, z))
			{
				m_FailList.push_back(A3DPOINT2(x, z));
			}
		}
	}

	return m_FailList.empty();
}

bool CCheckIslet::_Check(int x, int z)
{
	//const int LOCAL_ISLET_SIZE = 10;  //large size may be inefficient
	const int LOCAL_ISLET_SIZE = 200;  //large size may be inefficient
	
	m_NeighborList.clear();
	m_NeighborList.push_back(A3DPOINT2(x, z));
	A3DPOINT2 pt;
	int current = 0;
	while ((current != m_NeighborList.size()) && (m_NeighborList.size() < LOCAL_ISLET_SIZE))
	{
		pt = m_NeighborList[current];
		++current;
		
		//the neighbors
		for (int i = 0; i < 8; i++)
		{
			A3DPOINT2 neighbor;
			neighbor.x = pt.x + PF2D_NeighborD[i*2];
			neighbor.y = pt.y + PF2D_NeighborD[i*2+1];
			if (m_pRMap->GetPixel(neighbor.x, neighbor.y))
			{
				_Add2NeighborList(neighbor);
				_SetChecked(neighbor.x, neighbor.y, true);
			}

		}
	}

	return (m_NeighborList.size() >= LOCAL_ISLET_SIZE);

}

bool CCheckIslet::_Add2NeighborList(A3DPOINT2& pt)
{
	//check exist
	bool exist = false;
	vector<A3DPOINT2>::iterator it;
	for (it = m_NeighborList.begin(); it != m_NeighborList.end(); it++)
	{
		if (*it == pt)
		{
			exist = true;
			break;
		}
	}

	if (!exist)
	{
		m_NeighborList.push_back(pt);
	}

	return !exist;

}



/////////////////////////////////////////////////////////////////////////////
// CFormClu

IMPLEMENT_DYNCREATE(CFormClu, CFormView)

CFormClu::CFormClu()
	: CFormView(CFormClu::IDD)
{
	//{{AFX_DATA_INIT(CFormClu)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CFormClu::~CFormClu()
{
}

void CFormClu::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormClu)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormClu, CFormView)
	//{{AFX_MSG_MAP(CFormClu)
	ON_BN_CLICKED(IDC_BUTTON_INFO_CLU, OnButtonInfoClu)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT_CLU, OnButtonConnectClu)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_CLU, OnButtonCreateClu)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_CLU, OnButtonCheckClu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormClu diagnostics

#ifdef _DEBUG
void CFormClu::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormClu::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFormClu message handlers
void CFormClu::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
}

void CFormClu::OnButtonInfoClu() 
{
	// TODO: Add your control notification handler code here
	CLayerMap * pLayerMap = AUX_GetMainFrame()->GetView()->GetCurLayer();	
	if (!pLayerMap || !pLayerMap->GetCluGraph())
	{
		return;
	}
	
// 	CString msg;
// 	msg.Format("c %d n %d e %d", stat.clusters, stat.nodes, stat.edges);
// 	
// 	AUX_SetStatusbar(msg);
}

void CFormClu::OnButtonConnectClu() 
{
	// TODO: Add your control notification handler code here
	if (!g_AutoMoveAgent.GetMoveMap()->CreateMultiCluGraph(g_AutoMoveAgent.GetPfAlg()))
	{
		AfxMessageBox(AS2AC("connet fail"));
	}
	
}

void CFormClu::OnButtonCreateClu() 
{
	// TODO: Add your control notification handler code here
	g_AutoMoveAgent.GetMoveMap()->ReleaseMultiCluGraph();

	AUX_GetMainFrame()->GetView()->CreateClu();
}

void CFormClu::_AddInfo(const char * szTxt, bool bAppend)
{
	CString  str;
	
	if (bAppend)
	{
		GetDlgItemText(IDC_EDIT_INFO, str);
	}

	str += szTxt;
	str += "\r\n";
	SetDlgItemText(IDC_EDIT_INFO, str);
}

void CFormClu::_Check()
{
	CCheckDiag  chk_diag;
	CCheckIslet chk_islet;

	AString msg;
	int i;
	for (i = 0; i < g_AutoMoveAgent.GetMoveMap()->GetLayerCount(); ++i)
	{
		CLayerMap * pLayerMap = g_AutoMoveAgent.GetMoveMap()->GetLayer(i);
		if (!pLayerMap->GetCluGraph())
		{
			msg.Format("layer %d no clu", i);
			AUX_SetStatusbar(msg);
			AfxMessageBox(AS2AC(msg));
			return;
		}
		chk_diag.SetLayer(pLayerMap);
		if (!chk_diag.Check())
		{
			A3DPOINT2 pt1,pt2;
			chk_diag.RetrieveFailPair(pt1, pt2);
			msg.Format("layer %d check (%d,%d) and (%d,%d) fail", i, pt1.x, pt1.y, pt2.x, pt2.y);
			//AUX_SetStatusbar(msg);
			_AddInfo(msg, false);
			AfxMessageBox(AS2AC(msg));
			return;
		}

		chk_islet.SetRMap(pLayerMap->GetRMap());
		if (!chk_islet.Check())
		{
			vector<A3DPOINT2>* failList = chk_islet.GetFailList();
			int j;
			for (j = 0; j < (int)failList->size(); ++j)
			{
				msg.Format("layer %d islet (%d,%d)", i, failList->at(j).x, failList->at(j).y);
				_AddInfo(msg, true);
			}
			//return;
		}

	}

	AfxMessageBox(AS2AC("Done!"));
}

void CFormClu::OnButtonCheckClu() 
{
	// TODO: Add your control notification handler code here
	BeginWaitCursor();
	_Check();
	EndWaitCursor();
}
