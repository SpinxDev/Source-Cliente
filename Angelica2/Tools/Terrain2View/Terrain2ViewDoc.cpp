// Terrain2ViewDoc.cpp : implementation of the CTerrain2ViewDoc class
//

#include "Global.h"
#include "Terrain2View.h"
#include "Terrain2ViewDoc.h"
#include "Render.h"
#include "Settings.h"
#include "A3DTerrain2Wrapper.h"
#include "DlgTerrainInfo.h"

#include <A3DCamera.h>
#include <A3DEngine.h>

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewDoc

IMPLEMENT_DYNCREATE(CTerrain2ViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CTerrain2ViewDoc, CDocument)
	//{{AFX_MSG_MAP(CTerrain2ViewDoc)
	ON_COMMAND(ID_VIEW_TRNINFO, OnViewTrninfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewDoc construction/destruction

CTerrain2ViewDoc::CTerrain2ViewDoc()
{
	// TODO: add one-time construction code here
	m_pA3DTerrain	= NULL;
}

CTerrain2ViewDoc::~CTerrain2ViewDoc()
{
}

BOOL CTerrain2ViewDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


BOOL CTerrain2ViewDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	if (!LoadTerrain(lpszPathName))
	{
		glb_MessageBox(MB_OK, "Failed to load terrain file.");
		return FALSE;
	}
	
	A3DVECTOR3 vPos = g_vOrigin;
	m_pA3DTerrain->SetLoadCenter(vPos);
	float fHei = m_pA3DTerrain->GetPosHeight(vPos);
	vPos.y = fHei + 5.0f;

	//	Reset camera's position and orientation
	A3DCamera* pCamera = g_Render.GetCamera();
	pCamera->SetPos(vPos);
	pCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);

//	pCamera->SetPos(A3DVECTOR3(125.820f, 402.898f, -87.211f));
//	pCamera->SetDirAndUp(A3DVECTOR3(0.5f, -0.5f, 0.0f), g_vAxisY);

	return TRUE;
}

void CTerrain2ViewDoc::DeleteContents() 
{
	A3DRELEASE(m_pA3DTerrain);

	m_strTrnFile = "";
	
	CDocument::DeleteContents();
}

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewDoc serialization

void CTerrain2ViewDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewDoc diagnostics

#ifdef _DEBUG
void CTerrain2ViewDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTerrain2ViewDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewDoc commands

//	Load terrain
bool CTerrain2ViewDoc::LoadTerrain(const char* szFile)
{
	//	Load terrain
	if (!(m_pA3DTerrain = new A3DTerrain2Wrapper))
	{
		a_LogOutput(1, "CTerrain2ViewDoc::LoadTerrain", "Not enough memory");
		return false;
	}

	CSettings& s = glb_GetApp()->GetSettings();
	if (!m_pA3DTerrain->Init(g_Render.GetA3DEngine(), s.GetViewDist(), s.GetViewDist()))
	{
		a_LogOutput(1, "CTerrain2ViewDoc::LoadTerrain", "Failed to initialize terrain");
		return false;
	}

	if (!m_pA3DTerrain->Load(szFile, 0.0f, 0.0f))
	{
		a_LogOutput(1, "CTerrain2ViewDoc::LoadTerrain, Failed to load A3DTerrain2.");
		return false;
	}

	m_pA3DTerrain->SetLODDist(s.GetLODDist1(), s.GetLODDist2());
	m_pA3DTerrain->SetLODType((A3DTerrain2::LODTYPE)s.GetLODType());

	m_strTrnFile = szFile;

	return true;
}

//	Logic tick
bool CTerrain2ViewDoc::Tick(DWORD dwTime)
{
	return true;
}

void CTerrain2ViewDoc::OnViewTrninfo()
{
	CDlgTerrainInfo dlg(m_pA3DTerrain);
	dlg.DoModal();
}
