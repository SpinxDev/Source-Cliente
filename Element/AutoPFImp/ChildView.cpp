// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "AutoPFImp.h"
#include "ChildView.h"
#include "MainFrm.h"
#include "Global.h"
#include "Render.h"
#include "GridRender.h"
#include "FormMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//local var
vector<AString>    l_szRMapNames;
//////////////////////////////////////////////////////////////////////////
//local func
void    l_CreateClu(bool bForce = false)
{
	int i;

	for (i = 0; i < g_AutoMoveAgent.GetMoveMap()->GetLayerCount(); ++i)
	{
		CLayerMap * pLayerMap = g_AutoMoveAgent.GetMoveMap()->GetLayer(i);
		if (bForce)
		{
			pLayerMap->ReleaseCluGraph();
		}
		if (!pLayerMap->GetCluGraph())
		{
			pLayerMap->CreateClu(g_AutoMoveAgent.GetPfAlg(), g_iClusterSize, g_iEntranceWidth);
		}
	}
}

void l_RetrieveRMapName(int i, AString& fileName)
{
	assert(i >=0 && i < (int)l_szRMapNames.size());

	char name[MAX_PATH];

	_splitpath(l_szRMapNames[i], NULL, NULL, name, NULL);

	fileName = name;
}




/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	m_pCurLayer = NULL;
	m_pGrid = NULL;
}

CChildView::~CChildView()
{
	_ReleaseGame();	
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_UPDATE_COMMAND_UI ( ID_INDICATOR_POS, OnUpdatePane )
	ON_COMMAND(ID_SET_START, OnSetStart)
	ON_COMMAND(ID_SET_GOAL, OnSetGoal)
	ON_COMMAND(ID_SEARCH, OnSearch)
	ON_COMMAND(ID_MOVE, OnMove)
	ON_COMMAND(ID_ALG_CLU_G, OnAlgCluG)
	ON_UPDATE_COMMAND_UI(ID_ALG_CLU_G, OnUpdateAlgCluG)
	ON_COMMAND(ID_ALG_CLU_MA, OnAlgCluMa)
	ON_UPDATE_COMMAND_UI(ID_ALG_CLU_MA, OnUpdateAlgCluMa)
	ON_COMMAND(ID_ALG_CLU_MH, OnAlgCluMh)
	ON_UPDATE_COMMAND_UI(ID_ALG_CLU_MH, OnUpdateAlgCluMh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}

void CChildView::_ReleaseGame()
{
	A3DRELEASE(m_pGrid);
	m_pCurLayer = NULL;
}


bool CChildView::Render()
{
	if (!g_Render.BeginRender())
		return false;

	A3DViewport* pA3DViewport = g_Render.GetA3DViewport();

	//	Activate viewport and clear it
	pA3DViewport->Active();
	pA3DViewport->ClearDevice();


	if (m_pCurLayer && m_pGrid)
	{
		m_pGrid->Render();
	}


	_DrawPath();
	

	if (g_AutoMoveAgent.GetState() == PF_STATE_FOUND)
	{
		ACString str;
		str.Format(_AL("%d %d %d"), m_Stat.t_setup, m_Stat.t_search, m_Stat.t_optimize);
		g_Render.TextOut(8,40,str,A3DCOLORRGB(255,255,255));
	}
	 

	//	End render
	g_Render.EndRender();

	return true;
}

void CChildView::Tick(DWORD dwTickTime)
{
	CWnd * pWnd = GetActiveWindow();
	if (m_pCurLayer && m_pGrid && (pWnd == GetParent()))
	{
		m_pGrid->Tick(dwTickTime);
	}
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return true;
	//return CWnd ::OnEraseBkgnd(pDC);
}

void CChildView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd ::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;

	//	Resize device
	g_Render.ResizeDevice(cx, cy);

	if (m_pGrid)
	{
		m_pGrid->Resize(cx, cy);
	}
}

bool CChildView::LoadMapBin(const char * szPath)
{
	g_AutoMoveAgent.GetMoveMap()->Release();
	_ReleaseGame();
	l_szRMapNames.clear();
	
	
	AString msg;

	if (!g_AutoMoveAgent.GetMoveMap()->Load(szPath, &l_szRMapNames))
	{
		msg.Format("load %s fail", szPath);
		AfxMessageBox(AS2AC(msg));
		return false;
	}
	g_AutoMoveAgent.GetMoveMap()->CalcOrigin();
	g_AutoMoveAgent.CreateOptimizer();

	msg.Format("size %d,%d layer %d",  
		g_AutoMoveAgent.GetMoveMap()->GetMapWidth(),
		g_AutoMoveAgent.GetMoveMap()->GetMapLength(),
		g_AutoMoveAgent.GetMoveMap()->GetLayerCount());

	AUX_SetStatusbar(msg);

	m_pGrid = new CGridRender;
	m_pGrid->Init(g_AutoMoveAgent.GetMoveMap()->GetMapWidth(), g_AutoMoveAgent.GetMoveMap()->GetMapLength());

	return true;
}

bool CChildView::LoadMapCfg(const char * szPath, bool bConnectAndSave)
{


	g_AutoMoveAgent.GetMoveMap()->Release();
	
	_ReleaseGame();
	l_szRMapNames.clear();

	BeginWaitCursor();

	AString msg;
	FILE * fp = fopen(szPath, "r");
	char seps[]   = " =\t\n";
	CLayerMap * pCurLayer = NULL;

	if (!fp)
	{
		msg.Format("open %s fail", szPath);
		goto Err;
	}

	char line[1000];
	char line_bak[1000];
	char * token;

	g_AutoMoveAgent.GetMoveMap()->CreateJointList();

	while (fgets(line, 1000, fp))
	{
		strcpy(line_bak, line);
		token = strtok(line, seps);
		if (!token || token[0] == '#')
		{
			continue;
		}

		if (strcmp(token, "layer") == 0)
		{
			pCurLayer = g_AutoMoveAgent.GetMoveMap()->CreateLayer();
		}
		else if (strcmp(token, "rmap") == 0 && pCurLayer)
		{
			token = strtok(NULL, seps);
			AString path = g_szWorkDir;
			path += token;
			if (!pCurLayer->LoadRMap(path))
			{
				msg.Format("load %s fail", path);
				goto Err;
			}
			l_szRMapNames.push_back(path);
		}
		else if (strcmp(token, "dhmap") == 0 && pCurLayer)
		{
			token = strtok(NULL, seps);
			AString path = g_szWorkDir;
			path += token;
			if (!pCurLayer->LoadDHMap(path))
			{
				msg.Format("load %s fail", path);
				goto Err;
			}
		}
		else if (strcmp(token, "clu") == 0 && pCurLayer)
		{
			token = strtok(NULL, seps);
			AString path = g_szWorkDir;
			path += token;
			if (!pCurLayer->LoadClu(path))
			{
				msg.Format("load %s fail", path);
				goto Err;
			}
		}
		else if (strcmp(token, "isl") == 0 && pCurLayer)
		{
			token = strtok(NULL, seps);
			AString path = g_szWorkDir;
			path += token;
			if (!pCurLayer->LoadIsl(path))
			{
				msg.Format("load %s fail", path);
				goto Err;
			}
		}
		else if (strcmp(token, "end") == 0)
		{
			pCurLayer = NULL;
		}
		else if (strcmp(token, "joint") == 0)
		{
			char * c = strchr(line_bak, '=');
			if (c)
			{
				 AUX_GetFormMap()->AddJoint(c+1);
			}
			else
			{
				msg.Format("%s, unknown format", line_bak);
				AfxMessageBox(AS2AC(msg));
			}
			
		}
		else if (strcmp(token, "clu_param") == 0)
		{
			token = strtok(NULL, seps);
			g_iClusterSize = atoi(token);
			token = strtok(NULL, seps);
			g_iEntranceWidth = atoi(token);
		}

	}
	
	if (g_AutoMoveAgent.GetMoveMap()->GetLayerCount() ==0)
	{
		msg.Format("error! no layer");
		goto Err;
	}


	g_AutoMoveAgent.GetMoveMap()->CalcOrigin();
	l_CreateClu();
	if (bConnectAndSave){
		if (g_AutoMoveAgent.GetMoveMap()->CreateMultiCluGraph(g_AutoMoveAgent.GetPfAlg())){
			Save();
		}else{
			AfxMessageBox(AS2AC("connet fail"));
		}
	}

	g_AutoMoveAgent.CreateOptimizer();

	msg.Format("size %d,%d layer %d",  
		g_AutoMoveAgent.GetMoveMap()->GetMapWidth(),
		g_AutoMoveAgent.GetMoveMap()->GetMapLength(),
		g_AutoMoveAgent.GetMoveMap()->GetLayerCount());

	AUX_SetStatusbar(msg);

	m_pGrid = new CGridRender;
	m_pGrid->Init(g_AutoMoveAgent.GetMoveMap()->GetMapWidth(), g_AutoMoveAgent.GetMoveMap()->GetMapLength());

	fclose(fp);
	EndWaitCursor();
	return true;

Err:
	fclose(fp);
	AfxMessageBox(AS2AC(msg));
	g_AutoMoveAgent.GetMoveMap()->Release();
	EndWaitCursor();
	return false;
}


bool CChildView::ViewLayer(int iLayer)
{

	m_pCurLayer = g_AutoMoveAgent.GetMoveMap()->GetLayer(iLayer);

	if (!m_pCurLayer)
	{
		return false;
	}

	assert(m_pGrid);
	m_iCurLayer = iLayer;
	m_pGrid->SetLayerMap(m_pCurLayer, iLayer);
	m_pGrid->UpdateMap();

	return true;
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_pGrid && m_pCurLayer)
	{
		m_pGrid->OnLButtonDown(point.x, point.y, nFlags);
	}

	CWnd ::OnLButtonDown(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_pGrid && m_pCurLayer)
	{
		m_pGrid->OnLButtonUp(point.x, point.y, nFlags);
	}


	CWnd ::OnLButtonUp(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_pGrid && m_pCurLayer)
	{
		m_pGrid->OnMouseMove(point.x, point.y, nFlags);
		
		A3DPOINT2 ptMap;
		TransScr2Map(A3DPOINT2(point.x, point.y), ptMap);
		AString msg;
		msg.Format("(%d,%d)", ptMap.x, ptMap.y);
		CStatusBar * pStatusBar = AUX_GetMainFrame()->GetStatusBar();
		int index = pStatusBar->CommandToIndex(ID_INDICATOR_POS);
		pStatusBar->SetPaneText(index, AS2AC(msg));
		//AUX_GetMainFrame()->GetStatusBar()->SetPaneText(0, msg);
	}
	CWnd ::OnMouseMove(nFlags, point);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_pGrid && m_pCurLayer)
	{
		m_pGrid->OnRButtonDown(point.x, point.y, nFlags);
	}
	CWnd ::OnRButtonDown(nFlags, point);
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_pGrid && m_pCurLayer)
	{
		m_pGrid->OnRButtonUp(point.x, point.y, nFlags);
	}

	CWnd ::OnRButtonUp(nFlags, point);
}

void CChildView::OnUpdatePane(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

void CChildView::OnSetStart() 
{
	// TODO: Add your command handler code here
	if (m_pGrid)
	{
		m_pGrid->SetOp(CGridRender::OP_SETSTART);
	}
}

void CChildView::OnSetGoal() 
{
	// TODO: Add your command handler code here

	if (m_pGrid)
	{
		m_pGrid->SetOp(CGridRender::OP_SETGOAL);
	}
	
}

void CChildView::OnSearch() 
{
	// TODO: Add your command handler code here
	m_CurMoveIndex = 0;
	memset(&m_Stat, 0, sizeof(Stat));
	if (m_pGrid)
	{
		m_pGrid->SetOp(CGridRender::OP_NONE);
	}
	CString msg;
	if (!g_AutoMoveAgent.IsReady())
	{
		msg.Format(_T("not ready"));
		AfxMessageBox(msg);
		return;
	}
	// todo: check ret [5/30/2008 kuiwu]
	unsigned long tstart, tend;
	tstart = (unsigned long)(ACounter::GetMicroSecondNow()/1000);
	g_AutoMoveAgent.SetStartEnd(g_ptStart, g_iLayerStart, g_ptGoal, g_iLayerGoal);
	tend = (unsigned long)(ACounter::GetMicroSecondNow()/1000);
	m_Stat.t_setup = tend - tstart;
	
	tstart = tend;
	if (!g_AutoMoveAgent.Search(&g_InitPath))
	{
		msg.Format(_T("from(%d %d,%d)to(%d %d,%d) no path"), g_iLayerStart, g_ptStart.x, g_ptStart.y, g_iLayerGoal, g_ptGoal.x, g_ptGoal.y);
		AUX_SetStatusbar(AC2AS(msg));
		AfxMessageBox(msg);
	}
	tend = (unsigned long)(ACounter::GetMicroSecondNow()/1000);
	m_Stat.t_search = tend - tstart;
	if (m_pGrid)
	{
		m_pGrid->UpdateMap();
	}
	
	
}


void CChildView::_DrawPath()
{
	if (g_AutoMoveAgent.GetState() != PF_STATE_FOUND || !g_AutoMoveAgent.Get2DPath())
	{
		return;
	}

	vector<PathNode> * path = g_AutoMoveAgent.Get2DPath();
	int i;
	for (i = 0; i < (int)path->size(); i++)
	{
		A3DPOINT2  ptScr;
		TransMap2Scr(path->at(i).ptMap, ptScr);
		A3DCOLOR  col;
		if (i == m_CurMoveIndex)
		{
			col = A3DCOLORRGB(g_Color[COL_MOVE_POS*3], g_Color[COL_MOVE_POS*3+1], g_Color[COL_MOVE_POS*3+2]);
		}
		else if (path->at(i).layer == m_iCurLayer)
		{
			col = A3DCOLORRGB(g_Color[COL_OPT_PATH0*3], g_Color[COL_OPT_PATH0*3+1], g_Color[COL_OPT_PATH0*3+2]);
		}
		else
		{
			col = A3DCOLORRGB(g_Color[COL_OPT_PATH1*3], g_Color[COL_OPT_PATH1*3+1], g_Color[COL_OPT_PATH1*3+2]);
		}
		g_pA3DGDI->Draw2DPoint(ptScr, col, 4.0f);			
	}

}

void CChildView::OnMove() 
{
	// TODO: Add your command handler code here
	if (g_AutoMoveAgent.GetState() != PF_STATE_FOUND || m_CurMoveIndex >= (int)g_AutoMoveAgent.Get2DPath()->size())
	{
		return;
	}
	unsigned long tstart, tend;
	tstart = (unsigned long)(ACounter::GetMicroSecondNow()/1000);
	g_AutoMoveAgent.Optimize(m_CurMoveIndex, NULL);
	tend = (unsigned long)(ACounter::GetMicroSecondNow()/1000);
	m_Stat.t_optimize = tend - tstart;
	++m_CurMoveIndex;
}

void CChildView::Save()
{
	AString msg;
	if (!_SaveTxt())
	{
		msg.Format("save %s fail", g_szCfgPath);
		AfxMessageBox(AS2AC(msg));
		return;
	}

	_SaveBin();
	
}

bool CChildView::_SaveTxt()
{
	FILE * fp = fopen(g_szCfgPath, "wt");
	if (!fp)
	{
		return false;
	}
	fprintf(fp, "#config for pf map\n");	
	fprintf(fp, "clu_param= %d %d\n", g_iClusterSize, g_iEntranceWidth);
	
	int i;
	for (i = 0; i < g_AutoMoveAgent.GetMoveMap()->GetLayerCount(); ++i)
	{
		CLayerMap * pLayerMap = g_AutoMoveAgent.GetMoveMap()->GetLayer(i);
		fprintf(fp, "#layer %d\n", i);
		fprintf(fp, "layer\n");
		AString  szFileName;
		l_RetrieveRMapName(i, szFileName);
		fprintf(fp, "\trmap= %s.prmap\n", szFileName);
		fprintf(fp, "\tdhmap= %s.pdhmap\n", szFileName);
		if (pLayerMap->GetCluGraph())
		{
			AString path(g_szWorkDir);
			path += szFileName;
			path += ".clu";
			pLayerMap->GetCluGraph()->Save(path);
			fprintf(fp, "\tclu= %s.clu\n", szFileName);
		}
		if (pLayerMap->GetIslandList())
		{
			AString path(g_szWorkDir);
			path += szFileName;
			path += ".isl";
			pLayerMap->GetIslandList()->Save(path);
			fprintf(fp, "\tisl= %s.isl\n", szFileName);
		}
		fprintf(fp, "end\n");
	}
	
	if (g_AutoMoveAgent.GetMoveMap()->GetJointList())
	{
		for (i = 0; i < g_AutoMoveAgent.GetMoveMap()->GetJointList()->GetCount(); ++i)
		{
			const Joint * pJoint = g_AutoMoveAgent.GetMoveMap()->GetJointList()->GetJoint(i);
			fprintf(fp, "joint=%d %d %d %d\n", pJoint->layer1, pJoint->layer2, pJoint->ptMap.x, pJoint->ptMap.y);
		}
	}

	fflush(fp);
	fclose(fp);
	return true;
}


bool CChildView::_SaveBin()
{
	char drive[MAX_PATH];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(g_szCfgPath, drive, dir, fname, ext);

	AString path;
	path += drive;
	path += dir;
	path += fname;
	path += ".cfg";

	int i;
	vector<AString> layerNames;
	for (i =0; i < g_AutoMoveAgent.GetMoveMap()->GetLayerCount(); ++i)
	{
		AString name;
		l_RetrieveRMapName(i, name);
		layerNames.push_back(name);
	}

	AString mluName = fname;	
	if (g_AutoMoveAgent.GetMoveMap()->GetMultiCluGraph())
	{
		AString  mluPath;
		mluPath += drive;
		mluPath += dir;
		mluPath += mluName;
		mluPath += ".mlu";
		g_AutoMoveAgent.GetMoveMap()->GetMultiCluGraph()->Save(mluPath);
	}

	g_AutoMoveAgent.GetMoveMap()->Save(path, layerNames, mluName);
	return true;
}

void CChildView::CreateClu()
{
	BeginWaitCursor();
	l_CreateClu(true);
	EndWaitCursor();
}

void CChildView::OnAlgCluG() 
{
	// TODO: Add your command handler code here
	g_AutoMoveAgent.SetPfAlg(PF_ALGORITHM_CLU_G);
}

void CChildView::OnUpdateAlgCluG(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_AutoMoveAgent.GetPfAlg() == PF_ALGORITHM_CLU_G);
}

void CChildView::OnAlgCluMa() 
{
	// TODO: Add your command handler code here
	g_AutoMoveAgent.SetPfAlg(PF_ALGORITHM_CLU_MA);
}

void CChildView::OnUpdateAlgCluMa(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_AutoMoveAgent.GetPfAlg()== PF_ALGORITHM_CLU_MA);
}

void CChildView::OnAlgCluMh() 
{
	// TODO: Add your command handler code here
	g_AutoMoveAgent.SetPfAlg(PF_ALGORITHM_CLU_MH);
}

void CChildView::OnUpdateAlgCluMh(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_AutoMoveAgent.GetPfAlg() == PF_ALGORITHM_CLU_MH);
}
