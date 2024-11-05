// SectorEditorDoc.cpp : implementation of the CSectorEditorDoc class
//

#include "Global.h"
#include "SectorEditor.h"
#include "SectorEditorDoc.h"
#include "SectorEditorView.h"
#include "MainFrm.h"

#include "AF.h"
#include "AC.h"
#include "A3D.h"

#include "SceneTexture.h"
#include "SceneFrame.h"
#include "Render.h"
#include "Utility.h"
#include "Sector.h"
#include "Portal.h"
#include "Bound.h"
#include "PT_Interface.h"

#include "OptCreateSector.h"
#include "OptSizeSector.h"
#include "OptCreatePortal.h"
#include "OptSizePortal.h"
#include "OptMoveObject.h"
#include "OptLinkSector.h"
#include "OptSnapPortal.h"
#include "OptZoom.h"
#include "OptCreateSidePortal.h"
#include "OptNamePortal.h"

#include "DlgExportPtFile.h"
#include "DlgMessage.h"
#include "DlgPortalProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CSectorEditorDoc
//
///////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSectorEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CSectorEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CSectorEditorDoc)
	ON_COMMAND(ID_EDIT_IMPORTSCENEFILE, OnEditImportscenefile)
	ON_COMMAND(ID_EDIT_CREATESECTOR, OnEditCreatesector)
	ON_COMMAND(ID_EDIT_SIZESECTOR, OnEditSizesector)
	ON_COMMAND(ID_EDIT_CREATEPORTAL, OnEditCreateportal)
	ON_COMMAND(ID_EDIT_SIZEPORTAL, OnEditSizeportal)
	ON_COMMAND(ID_EDIT_MOVEOBJECT, OnEditMoveobject)
	ON_COMMAND(ID_EDIT_LINKSECTOR, OnEditLinksector)
	ON_COMMAND(ID_EDIT_CHANGELINK, OnEditChangelink)
	ON_COMMAND(ID_VIEW_FORCERENDER, OnViewForcerender)
	ON_COMMAND(ID_FILE_EXPORTPTRFILE, OnFileExportptrfile)
	ON_COMMAND(ID_EDIT_SNAPPORTAL, OnEditSnapportal)
	ON_COMMAND(ID_VIEW_MAGNIFY, OnViewMagnify)
	ON_COMMAND(ID_VIEW_MINIFY, OnViewMinify)
	ON_COMMAND(ID_VIEW_TEST, OnViewTest)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_CREATESIDEPORTAL, OnEditCreatesideportal)
	ON_COMMAND(ID_EDIT_PROPERTY, OnEditProperty)
	ON_COMMAND(ID_EDIT_NAMEPORTAL, OnEditNameportal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorDoc construction/destruction

CSectorEditorDoc::CSectorEditorDoc()
{
	m_pTextureScene		= NULL;
	m_pFrameScene		= NULL;
	m_aVisSurfs			= NULL;	
	m_aVisSurfFlags		= NULL;
	m_iNumVisSurf		= 0;
	m_iNumFace			= 0;
	m_szSceneFile[0]	= '\0';
	m_pSelObj			= NULL;
	m_pCurOpt			= NULL;
	m_aFaceAABBs		= NULL;
}

CSectorEditorDoc::~CSectorEditorDoc()
{
}

BOOL CSectorEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	m_dwCurFileVer = SEDFILE_VERSION;

	return TRUE;
}

BOOL CSectorEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if (!LoadDocument(lpszPathName))
	{
		g_Log.Log("CSectorEditorDoc::OnOpenDocument, Failed to load file %s", lpszPathName);
		return FALSE;
	}

	GetView()->OnNewDocument();

	return TRUE;
}

BOOL CSectorEditorDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if (!SaveDocument(lpszPathName))
	{
		g_Log.Log("CSectorEditorDoc::OnSaveDocument, Failed to save file %s", lpszPathName);
		return FALSE;
	}

	SetModifiedFlag(FALSE);

	return TRUE;
//	return CDocument::OnSaveDocument(lpszPathName);
}

void CSectorEditorDoc::DeleteContents() 
{
	//	Cancel current operation
	EndCurOperation(true);

	ReleaseScene();
	ReleaseEditData();

	m_pSelObj = NULL;

	CDocument::DeleteContents();
}

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorDoc serialization

void CSectorEditorDoc::Serialize(CArchive& ar)
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
// CSectorEditorDoc diagnostics

#ifdef _DEBUG
void CSectorEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSectorEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorDoc commands

//	Get associated view
CSectorEditorView* CSectorEditorDoc::GetView()
{
	POSITION pos = GetFirstViewPosition();
	if (!pos)
		return NULL;

	return (CSectorEditorView*)GetNextView(pos);
}

//	Release scene
void CSectorEditorDoc::ReleaseScene()
{
	//	Stop testing
	if (g_Configs.bTesting)
		OnViewTest();

	if (m_pTextureScene)
	{
		m_pTextureScene->Release();
		delete m_pTextureScene;
		m_pTextureScene = NULL;
	}

	if (m_pFrameScene)
	{
		m_pFrameScene->Release();
		delete m_pFrameScene;
		m_pFrameScene = NULL;
	}

	if (m_aVisSurfs)
	{
		delete [] m_aVisSurfs;
		m_aVisSurfs = NULL;
	}
	
	if (m_aVisSurfFlags)
	{
		delete [] m_aVisSurfFlags;
		m_aVisSurfFlags = NULL;
	}

	if (m_aFaceAABBs)
	{
		delete [] m_aFaceAABBs;
		m_aFaceAABBs = NULL;
	}

	m_iNumVisSurf		= 0;
	m_iNumFace			= 0;
	m_szSceneFile[0]	= '\0';
}

//	Release edit data
void CSectorEditorDoc::ReleaseEditData()
{
	ALISTPOSITION pos;

	//	Release all sectors
	pos = m_SectorList.GetHeadPosition();
	while (pos)
	{
		CSector* pSector = m_SectorList.GetNext(pos);
		pSector->Release();
		delete pSector;
	}

	m_SectorList.RemoveAll();

	//	Release all portals
	pos = m_PortalList.GetHeadPosition();
	while (pos)
	{
		CPortal* pPortal = m_PortalList.GetNext(pos);
		pPortal->Release();
		delete pPortal;
	}

	m_PortalList.RemoveAll();
}

//	Add visible surfaces
bool CSectorEditorDoc::AddVisibleSurfaces(int* aVisSurfs, int iNumSurf)
{
	ASSERT(m_aVisSurfs);

	for (int i=0; i < iNumSurf; i++)
	{
		int iFace = aVisSurfs[i];
		ASSERT(iFace >= 0 && iFace < m_iNumFace);

		if (!m_aVisSurfFlags[iFace])
		{
			m_aVisSurfs[m_iNumVisSurf++] = iFace;
			m_aVisSurfFlags[iFace] = 1;
		}
	}

	return true;
}

//	Change scene file
bool CSectorEditorDoc::ChangeSceneFile(const char* szFile)
{
	//	Release old scene
	ReleaseScene();

	if (!LoadScene(szFile))
	{
		g_Log.Log("CSectorEditorDoc::ChangeSceneFile, Failed to call LoadScene()");
		return false;
	}

	//	Save scene file name
	strcpy(m_szSceneFile, szFile);

	//	Update visible face set
	CBound* pBound = GetView()->GetBound();
	SetBound(pBound->GetMins(), pBound->GetMaxs(), g_Configs.bFullClip);

	m_pTextureScene->SetRenderMode(g_Configs.bForceRender ? CTextureScene::RENDER_ALL : CTextureScene::RENDER_SELECT);
	m_pFrameScene->ForceToRenderAll(g_Configs.bForceRender);

	return true;
}

void CSectorEditorDoc::OnEditImportscenefile() 
{
	//	Let user select a BSP file
	char szFilter[] = "场景模型文件(*.ils)|*.ils||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
	CFileDialog	FileDia(TRUE, "*.exe", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDCANCEL)
		return;

	//	Restore current directory
	if (!ChangeSceneFile(FileDia.GetPathName()))
		AUX_MessageBox(MB_OK, "场景加载失败!");

	SetModifiedFlag(TRUE);
}

//	Add sector to list
bool CSectorEditorDoc::AddSector(CSector* pSector)
{
	m_SectorList.AddTail(pSector);
	return true;
}

//	Remove sector from list, Note, this function doesn't delete pSector
bool CSectorEditorDoc::RemoveSector(CSector* pSector, bool bUnLink)
{
	if (!pSector)
		return true;

	if (bUnLink)
		UnlinkSector(pSector);

	if (pSector == m_pSelObj)
		SelectEditObject(NULL);

	ALISTPOSITION pos = m_SectorList.Find(pSector);
	if (pos)
	{
		m_SectorList.RemoveAt(pos);
		return true;
	}

	return false;
}

//	Add portal to list
bool CSectorEditorDoc::AddPortal(CPortal* pPortal)
{
	m_PortalList.AddTail(pPortal);
	return true;
}

//	Remove portal from list, Note, this function doesn't delete pPortal
bool CSectorEditorDoc::RemovePortal(CPortal* pPortal, bool bUnLink)
{
	if (!pPortal)
		return true;

	if (bUnLink)
		pPortal->UnlinkSectors();

	if (pPortal == m_pSelObj)
		SelectEditObject(NULL);

	ALISTPOSITION pos = m_PortalList.Find(pPortal);
	if (pos)
	{
		m_PortalList.RemoveAt(pos);
		return true;
	}

	return false;
}

/*	Begin a new operation

	Return true for success, otherwise return false.

	iOperation: operation ID.
	dwParam: params used to begine specified operation
*/
bool CSectorEditorDoc::BeginOperation(int iOperation, DWORD dwParam)
{
	//	Finish current operation
	EndCurOperation(false);

	switch (iOperation)
	{
	case OPT_CREATESECTOR:
	{
		COptCreateSector* pOpt = new COptCreateSector(OPT_CREATESECTOR);
		m_pCurOpt = pOpt;
		break;
	}
	case OPT_SIZESECTOR:
	{
		if (!PromptToSelect(EOID_SECTOR))
			return false;

		COptSizeSector* pOpt = new COptSizeSector(OPT_SIZESECTOR);
		pOpt->SetSector((CSector*)m_pSelObj);
		m_pCurOpt = pOpt;
		break;
	}
	case OPT_CREATEPORTAL:
	{
		COptCreatePortal* pOpt = new COptCreatePortal(OPT_CREATEPORTAL);
		m_pCurOpt = pOpt;
		break;
	}
	case OPT_SIZEPORTAL:
	{
		if (!PromptToSelect(EOID_PORTAL))
			return false;

		COptSizePortal* pOpt = new COptSizePortal(OPT_SIZEPORTAL);
		pOpt->SetPortal((CPortal*)m_pSelObj);
		m_pCurOpt = pOpt;
		break;
	}
	case OPT_LINKSECTOR:
	{
		if (!PromptToSelect(EOID_PORTAL))
			return false;

		COptLinkSector* pOpt = new COptLinkSector(OPT_LINKSECTOR);
		pOpt->SetPortal((CPortal*)m_pSelObj);
		m_pCurOpt = pOpt;
		break;
	}
	case OPT_MOVEOBJECT:
	{
		if (!PromptToSelect(EOID_UNKNOWN))
			return false;

		COptMoveObject* pOpt = new COptMoveObject(OPT_MOVEOBJECT);
		pOpt->SetObject(m_pSelObj);
		m_pCurOpt = pOpt;
		break;
	}
	case OPT_SNAPPORTAL:
	{
		if (!PromptToSelect(EOID_PORTAL))
			return false;

		COptSnapPortal* pOpt = new COptSnapPortal(OPT_SNAPPORTAL);
		pOpt->SetPortal((CPortal*)m_pSelObj);
		m_pCurOpt = pOpt;
		break;
	}
	case OPT_ZOOMVIEWPORT:
	{
		COptZoom* pOpt = new COptZoom(OPT_ZOOMVIEWPORT);
		pOpt->SetMagnifyFlag(dwParam ? true : false);
		m_pCurOpt = pOpt;
		break;
	}
	case OPT_CREATESIDEPORTAL:
	{
		COptCreateSidePortal* pOpt = new COptCreateSidePortal(OPT_CREATESIDEPORTAL);
		m_pCurOpt = pOpt;
		break;
	}
	case OPT_NAMEPORTAL:
	{
		COptNamePortal* pOpt = new COptNamePortal(OPT_NAMEPORTAL);
		m_pCurOpt = pOpt;
		break;
	}
	default:
		return false;
	}

	m_pCurOpt->Begin();
	
	return true;
}

/*	End current operation

	Return true for success, otherwise return false.

	bCancel: true, cancel operation. false, finish operation
*/
bool CSectorEditorDoc::EndCurOperation(bool bCancel)
{
	if (!m_pCurOpt)
		return true;

	m_pCurOpt->End(bCancel);

	if (bCancel)
	{
		//	Release operation object
		delete m_pCurOpt;
		m_pCurOpt = NULL;
		return true;
	}

	bool bModified = true;

	switch (m_pCurOpt->GetOptID())
	{
	case OPT_CREATESECTOR:
	{
		CSector* pSector = ((COptCreateSector*)m_pCurOpt)->GetSector();
		if (pSector)
			SelectEditObject(pSector);

		break;
	}
	case OPT_CREATEPORTAL:
	{
		CPortal* pPortal = ((COptCreatePortal*)m_pCurOpt)->GetPortal();
		if (pPortal)
			SelectEditObject(pPortal);

		break;
	}
	case OPT_ZOOMVIEWPORT:

		bModified = false;
		break;

	case OPT_SIZESECTOR:
	case OPT_SIZEPORTAL:
	case OPT_MOVEOBJECT:
	case OPT_LINKSECTOR:
	case OPT_SNAPPORTAL:
	case OPT_CREATESIDEPORTAL:
	case OPT_NAMEPORTAL:

		break;
	}

	//	Release operation object
	delete m_pCurOpt;
	m_pCurOpt = NULL;

	if (bModified)
		SetModifiedFlag(TRUE);

	return true;
}

//	Create sector
void CSectorEditorDoc::OnEditCreatesector() 
{
	BeginOperation(OPT_CREATESECTOR, 0);
}

//	Size sector
void CSectorEditorDoc::OnEditSizesector() 
{
	BeginOperation(OPT_SIZESECTOR, 0);
}

void CSectorEditorDoc::OnEditCreateportal() 
{
	BeginOperation(OPT_CREATEPORTAL, 0);
}

void CSectorEditorDoc::OnEditSizeportal() 
{
	BeginOperation(OPT_SIZEPORTAL, 0);
}

void CSectorEditorDoc::OnEditLinksector() 
{
	BeginOperation(OPT_LINKSECTOR, 0);
}

void CSectorEditorDoc::OnEditChangelink() 
{
	if (!PromptToSelect(EOID_PORTAL))
		return;

	((CPortal*)m_pSelObj)->ChangeLinkMode();
}

void CSectorEditorDoc::OnEditSnapportal() 
{
	BeginOperation(OPT_SNAPPORTAL, 0);
}

void CSectorEditorDoc::OnEditNameportal() 
{
	BeginOperation(OPT_NAMEPORTAL, 0);
}

void CSectorEditorDoc::OnEditMoveobject() 
{
	BeginOperation(OPT_MOVEOBJECT, 0);
}

//	Magnify viewport
void CSectorEditorDoc::OnViewMagnify() 
{
	BeginOperation(OPT_ZOOMVIEWPORT, 1);
}

//	Minify viewport
void CSectorEditorDoc::OnViewMinify() 
{
	BeginOperation(OPT_ZOOMVIEWPORT, 0);
}

void CSectorEditorDoc::OnEditCreatesideportal() 
{
	BeginOperation(OPT_CREATESIDEPORTAL, 0);
}

//	Render routine
bool CSectorEditorDoc::Render(bool bOnlySel/* false */)
{
	//	Render all sectors
	ALISTPOSITION pos;

	//	Delay render array
	static CEditObject* aDelayObjs[10];
	static int iNumDelay;

	iNumDelay = 0;

	if (g_Configs.bShowSector)
	{
		//	Render all sectors
		pos = m_SectorList.GetHeadPosition();
		while (pos)
		{
			CSector* pSector = m_SectorList.GetNext(pos);
			if (pSector == m_pSelObj)
				continue;
			else if (pSector->GetDelayRenderFlag())
			{
				aDelayObjs[iNumDelay++] = pSector;
				continue;
			}

			if (!bOnlySel)
				pSector->Render();
		}
	}

	if (g_Configs.bShowPortal)
	{
		//	Render all portals
		pos = m_PortalList.GetHeadPosition();
		while (pos)
		{
			CPortal* pPortal = m_PortalList.GetNext(pos);
			if (pPortal == m_pSelObj)
				continue;

			if (!bOnlySel)
				pPortal->Render();
		}
	}

	//	Render delay object
	for (int i=0; i < iNumDelay; i++)
	{
		switch (aDelayObjs[i]->GetID())
		{
		case EOID_SECTOR: 	((CSector*)aDelayObjs[i])->Render();	break;
		case EOID_PORTAL:	((CPortal*)aDelayObjs[i])->Render();	break;
		}
	}

	//	Render selected object separately
	if (m_pSelObj)
	{
		switch (m_pSelObj->GetID())
		{
		case EOID_SECTOR: 	((CSector*)m_pSelObj)->Render();	break;
		case EOID_PORTAL:	((CPortal*)m_pSelObj)->Render();	break;
		}
	}

	return true;
}

//	Select edit object
void CSectorEditorDoc::SelectEditObject(CEditObject* pObject)
{
	if (m_pSelObj == pObject)
		return;

	if (m_pSelObj)
		m_pSelObj->Select(false);
	
	m_pSelObj = pObject;

	if (m_pSelObj)
		m_pSelObj->Select(true);
}

//	Get first sector
CSector* CSectorEditorDoc::GetFirstSector()
{
	if ((m_SectorPos = m_SectorList.GetHeadPosition()))
		return m_SectorList.GetNext(m_SectorPos);
	else
		return NULL;
}

//	Get next sector
CSector* CSectorEditorDoc::GetNextSector()
{
	if (m_SectorPos)
		return m_SectorList.GetNext(m_SectorPos);
	else
		return NULL;
}

//	Get first portal
CPortal* CSectorEditorDoc::GetFirstPortal()
{
	if ((m_PortalPos = m_PortalList.GetHeadPosition()))
		return m_PortalList.GetNext(m_PortalPos);
	else
		return NULL;
}

//	Get next portal
CPortal* CSectorEditorDoc::GetNextPortal()
{
	if (m_PortalPos)
		return m_PortalList.GetNext(m_PortalPos);
	else
		return NULL;
}

/*	Check sector collision

	vMins, vMaxs: position of sector which has been snaped on grid
	pExcept: skip this sector.
*/
bool CSectorEditorDoc::CheckSectorCollision(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, CSector* pExcept)
{
	ALISTPOSITION pos = m_SectorList.GetHeadPosition();
	while (pos)
	{
		CSector* pSector = m_SectorList.GetNext(pos);
		if (pSector == pExcept)
			continue;

		if (pSector->Collision(vMins, vMaxs))
			return true;
	}

	return false;
}

/*	Sector collision checking routine used to resize sector
	
	vMins, vMaxs (in): position of sector which has been snaped on grid
				 (out): 
	pExcept: skip this sector.
	iAxis: resize along this axis
	iSign: 1, resize to positive direction. -1 resize to negative direction
*/
bool CSectorEditorDoc::SizeSectorCollision(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, 
										   CSector* pExcept, int iAxis, int iSign)
{
	ALISTPOSITION pos = m_SectorList.GetHeadPosition();
	while (pos)
	{
		CSector* pSector = m_SectorList.GetNext(pos);
		if (pSector == pExcept)
			continue;

		if (!pSector->Collision(vMins, vMaxs))
			continue;

		if (iSign < 0)
			vMins.m[iAxis] = pSector->GetMaxs().m[iAxis];
		else
			vMaxs.m[iAxis] = pSector->GetMins().m[iAxis];
	}

	return false;
}

/*	Sector collision checking routine used to resize sector
	
	vMins, vMaxs (in): position of sector which has been snaped on grid
				 (out): 
	pExcept: skip this sector.
	vDelta: moving delta
*/
bool CSectorEditorDoc::MoveSectorCollision(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, CSector* pExcept, const A3DVECTOR3& vDelta)
{
	float fFrac, fMinFrac=1.0f;
	A3DVECTOR3 vAreaMins, vAreaMaxs;

	vAreaMins = vMins + vDelta;
	vAreaMaxs = vMaxs + vDelta;
	vAreaMins = a3d_VecMin(vMins, vAreaMins);
	vAreaMaxs = a3d_VecMax(vMaxs, vAreaMaxs);

	ALISTPOSITION pos = m_SectorList.GetHeadPosition();
	while (pos)
	{
		CSector* pSector = m_SectorList.GetNext(pos);
		if (pSector == pExcept)
			continue;

		if (!pSector->Collision(vAreaMins, vAreaMaxs))
			continue;

		AABBMoveToAABB(vMins, vMaxs, pSector->GetMins(), pSector->GetMaxs(), vDelta, &fFrac);

		if (fFrac < fMinFrac)
			fMinFrac = fFrac;
	}

	vMins += vDelta * fMinFrac;
	vMaxs += vDelta * fMinFrac;

	SnapVector(&vMins, &vMins, MIN_GRID);
	SnapVector(&vMaxs, &vMaxs, MIN_GRID);

	return true;
}

/*	Set world bound

	vMins, vMaxs: bound's volume
	bWindow: true, window mode; false, crossing mode
*/
void CSectorEditorDoc::SetBound(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, bool bWindow)
{
	ALISTPOSITION pos;

	//	Bound sectors
	pos = m_SectorList.GetHeadPosition();
	while (pos)
	{
		CSector* pSector = m_SectorList.GetNext(pos);
		pSector->Bound(vMins, vMaxs, bWindow);
	}

	//	Bound portals
	pos = m_PortalList.GetHeadPosition();
	while (pos)
	{
		CPortal* pPortal = m_PortalList.GetNext(pos);
		pPortal->Bound(vMins, vMaxs, bWindow);
	}

	//	Bound scene
	BoundScene(vMins, vMaxs, bWindow);
}

//	Bound scene
void CSectorEditorDoc::BoundScene(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, bool bWindow)
{
	if (!m_pTextureScene)
		return;

	int i, iNumFace = m_pTextureScene->GetFaceCount();

	m_iNumVisSurf = 0;

	if (bWindow)
	{
		FACEAABB* pAABB;

		for (i=0; i < iNumFace; i++)
		{
			pAABB = &m_aFaceAABBs[i];

			if (AABBInAABB(pAABB->vMins, pAABB->vMaxs, vMins, vMaxs))
				m_aVisSurfs[m_iNumVisSurf++] = i;
		}
	}
	else
	{
		FACEAABB* pAABB;

		for (i=0; i < iNumFace; i++)
		{
			pAABB = &m_aFaceAABBs[i];

			if (AABBToAABB(pAABB->vMins, pAABB->vMaxs, vMins, vMaxs))
				m_aVisSurfs[m_iNumVisSurf++] = i;
		}
	}

	m_pTextureScene->SetVisibleSurfs(m_aVisSurfs, m_iNumVisSurf);
	m_pTextureScene->RefillStream();

	m_pFrameScene->SetVisibleSurfs(m_aVisSurfs, m_iNumVisSurf);
	m_pFrameScene->RefillStream();
}

void CSectorEditorDoc::OnViewForcerender() 
{
	g_Configs.bForceRender = !g_Configs.bForceRender;

	if (m_pTextureScene)
	{
		if (g_Configs.bForceRender)
			m_pTextureScene->SetRenderMode(CTextureScene::RENDER_ALL);
		else if (g_Configs.bTesting)
			m_pTextureScene->SetRenderMode(CTextureScene::RENDER_PORTAL);
		else
			m_pTextureScene->SetRenderMode(CTextureScene::RENDER_SELECT);
	}
	
	if (m_pFrameScene)
		m_pFrameScene->ForceToRenderAll(g_Configs.bForceRender);
}

//	Get sector's index
int CSectorEditorDoc::GetSectorIndex(CSector* pSector)
{
	int iIndex = 0;

	ALISTPOSITION pos = m_SectorList.GetHeadPosition();
	while (pos)
	{
		CSector* p = m_SectorList.GetNext(pos);
		if (p == pSector)
			return iIndex;

		iIndex++;
	}

	return -1;
}

//	Get portal's index
int CSectorEditorDoc::GetPortalIndex(CPortal* pPortal)
{
	int iIndex = 0;

	ALISTPOSITION pos = m_PortalList.GetHeadPosition();
	while (pos)
	{
		CPortal* p = m_PortalList.GetNext(pos);
		if (p == pPortal)
			return iIndex;

		iIndex++;
	}

	return -1;
}

//	Get sector by index
CSector* CSectorEditorDoc::GetSectorByIndex(int iIndex)
{
	int i = 0;

	ALISTPOSITION pos = m_SectorList.GetHeadPosition();
	while (pos)
	{
		CSector* p = m_SectorList.GetNext(pos);
		if (i++ == iIndex)
			return p;
	}

	return NULL;
}

//	Get portal by index
CPortal* CSectorEditorDoc::GetPortalByIndex(int iIndex)
{
	int i = 0;

	ALISTPOSITION pos = m_PortalList.GetHeadPosition();
	while (pos)
	{
		CPortal* p = m_PortalList.GetNext(pos);
		if (i++ == iIndex)
			return p;
	}

	return NULL;
}

//	Get portal by name
CPortal* CSectorEditorDoc::GetPortalByName(const char* szName)
{
	if (!szName || !szName[0])
		return NULL;

	ALISTPOSITION pos = m_PortalList.GetHeadPosition();
	while (pos)
	{
		CPortal* p = m_PortalList.GetNext(pos);
		const char* szTemp = p->GetName();
		if (szTemp[0] && !stricmp(szTemp, szName))
			return p;
	}

	return NULL;
}

void CSectorEditorDoc::OnFileExportptrfile() 
{
	//	Scene has been loaded ?	
	if (!m_szSceneFile[0])
		return;

	//	Check protal and sector
	if (!ExportPortalChecking())
		return;

	static PTBUILDPARAMS Params = 
	{
		{2, 2, 2}, 40.0f, 12.0f
	};

	CDlgExportPtFile ExportDlg;

	ExportDlg.m_iDivX		= Params.aSectorDiv[0];
	ExportDlg.m_iDivY		= Params.aSectorDiv[1];
	ExportDlg.m_iDivZ		= Params.aSectorDiv[2];
	ExportDlg.m_fMinLimit	= Params.fPartMin;
	ExportDlg.m_fMaxLimit	= Params.fPartMax;

	if (ExportDlg.DoModal() != IDOK)
		return;

	Params.aSectorDiv[0]	= ExportDlg.m_iDivX;
	Params.aSectorDiv[1]	= ExportDlg.m_iDivY;
	Params.aSectorDiv[2]	= ExportDlg.m_iDivZ;
	Params.fPartMin			= ExportDlg.m_fMinLimit;
	Params.fPartMax			= ExportDlg.m_fMaxLimit;

	int iCursor = g_iCurCursor;
	AUX_SetCursor(CURSOR_WAIT);

//	g_Log.Log("%s", ExportDlg.m_strPortalFile);

	if (!ExportPortalFile(ExportDlg.m_strPortalFile, &Params))
		AUX_MessageBox(MB_OK, "导出 portal 文件失败！");

	AUX_SetCursor(iCursor);
}

void CSectorEditorDoc::OnViewTest() 
{
	if (!m_pTextureScene)
		return;

	bool bTesting = !g_Configs.bTesting;

	if (bTesting)
	{
		//	Let user select a portal (.prt) file
		char szFilter[] = "Portal 文件(*.prt)|*.prt||";
		DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
		CFileDialog	FileDia(TRUE, "*.prt", NULL, dwFlags, szFilter, NULL);

		//	Use scene file's path as default path
		char szDir[MAX_PATH];
		af_GetFilePath(m_szSceneFile, szDir, MAX_PATH);
		FileDia.m_ofn.lpstrInitialDir = szDir;
		
		if (FileDia.DoModal() == IDCANCEL)
			return;
		
		if (!m_pTextureScene->LoadPortalFile((LPCTSTR)FileDia.GetPathName()))
		{
			AUX_MessageBox(MB_OK, "加载 portal 文件失败！");
			return;
		}
	}

	g_Configs.bTesting = bTesting;

	if (bTesting)
		m_pTextureScene->SetRenderMode(g_Configs.bForceRender ? CTextureScene::RENDER_ALL : CTextureScene::RENDER_PORTAL);
	else
		m_pTextureScene->SetRenderMode(g_Configs.bForceRender ? CTextureScene::RENDER_ALL : CTextureScene::RENDER_SELECT);
}

void CSectorEditorDoc::OnEditDelete() 
{
	if (!m_pSelObj)
		return;

	if (AUX_MessageBox(MB_YESNO, "确定要删除选中的物体吗？") == IDNO)
		return;

	//	End current operation
	EndCurOperation(false);

	if (m_pSelObj->GetID() == EOID_SECTOR)
	{
		CSector* pSector = (CSector*)m_pSelObj;

		RemoveSector(pSector, true);
		pSector->Release();
		delete pSector;
	}
	else if (m_pSelObj->GetID() == EOID_PORTAL)
	{
		CPortal* pPortal = (CPortal*)m_pSelObj;

		RemovePortal(pPortal, true);
		pPortal->Release();
		delete pPortal;
	}
}

//	Unlink sector
void CSectorEditorDoc::UnlinkSector(CSector* pSector)
{
	if (!pSector)
		return;

	//	Release all portals
	ALISTPOSITION pos = m_PortalList.GetHeadPosition();
	while (pos)
	{
		CPortal* pPortal = m_PortalList.GetNext(pos);

		int iMode;
		if (pSector == pPortal->GetLinkedSector(0, &iMode) ||
			pSector == pPortal->GetLinkedSector(1, &iMode))
		{
			pPortal->UnlinkSectors();
			return;
		}
	}
}

//	Prompt to select a sector or portal
bool CSectorEditorDoc::PromptToSelect(int iObject)
{
	if (iObject == EOID_PORTAL)
	{
		if (!m_pSelObj || m_pSelObj->GetID() != EOID_PORTAL)
		{
			AUX_MessageBox(MB_OK, "请先选中一个 portal");
			return false;
		}
	}
	else if (iObject == EOID_SECTOR)
	{
		if (!m_pSelObj || m_pSelObj->GetID() != EOID_SECTOR)
		{
			AUX_MessageBox(MB_OK, "请先选中一个 sector");
			return false;
		}
	}
	else
	{
		if (!m_pSelObj)
		{
			AUX_MessageBox(MB_OK, "请先选中一个物品");
			return false;
		}
	}

	return true;
}

//	Portal file export checking
bool CSectorEditorDoc::ExportPortalChecking()
{
	int iNumUnlinked = 0, iPortal = -1;
	bool bWarning = false, bError = false;

	static float fEpsilon = 0.001f;
	static char szMsg[256];

	CDlgMessage* pMsgWnd = AUX_GetMainFrame()->GetMessageWnd();
	pMsgWnd->ClearAllMessages();

	ALISTPOSITION pos = m_PortalList.GetHeadPosition();
	while (pos)
	{
		iPortal++;
		CPortal* pPortal = m_PortalList.GetNext(pos);

		//	Position check
		A3DVECTOR3 aVerts[4];
		pPortal->GetVertices(aVerts);

		A3DPLANE Plane;
		Plane.CreatePlane(aVerts[0], aVerts[1], aVerts[2]);

		float fPos = aVerts[0].m[pPortal->GetMajorAxis()];
		int aSides[2];

		for (int i=0; i < 2; i++)
		{
			int iLinkMode;
			CSector* s = pPortal->GetLinkedSector(i, &iLinkMode);

			//	Linking check
			if (!s)
			{
				iNumUnlinked++;
				bWarning = true;
				break;
			}

			A3DVECTOR3 vMins = s->GetMins();
			A3DVECTOR3 vMaxs = s->GetMaxs();

			//	Sector shouldn't cross portal
			float d1 = DotProduct(vMins, Plane.vNormal) - Plane.fDist;
			float d2 = DotProduct(vMaxs, Plane.vNormal) - Plane.fDist;

			if ((d1 > fEpsilon && d2 < -fEpsilon) || (d1 < -fEpsilon && d2 > fEpsilon))
			{
				bError = true;
				aSides[i] = 0;

				sprintf(szMsg, "<!> Sector (%d) 跨越了与它连接的 portal (%d)", GetSectorIndex(s), iPortal);
				pMsgWnd->AddMessage(szMsg);
			}
			else
			{
				bool bGap = false;

				if (d1 < fEpsilon && d2 < fEpsilon)
				{
					//	On back side
					if (d1 < -fEpsilon && d2 < -fEpsilon)
						bGap = true;

					aSides[i] = -1;
				}
				else	//	On front side
				{
					if (d1 > fEpsilon && d2 > fEpsilon)
						bGap = true;

					aSides[i] = 1;
				}

				if (bGap)
				{
					sprintf(szMsg, "<!> Sector (%d) 和它连接的 portal (%d) 之间有缝隙", GetSectorIndex(s), iPortal);
					pMsgWnd->AddMessage(szMsg);
					bError = true;
				}
			}
		}
	
		//	Portal is unlinked ?
		if (i < 2)
			continue;

		if (aSides[0] * aSides[1] > 0)
		{
			sprintf(szMsg, "<!> 和 portal (%d) 连接的两个 sector 处于同一侧", iPortal);
			pMsgWnd->AddMessage(szMsg);
			bError = true;
		}
	}

	if (iNumUnlinked)
	{
		sprintf(szMsg, "<+> 一共有 %d portal 没有和 sector 连接", iNumUnlinked);
		pMsgWnd->AddMessage(szMsg);
	}

	if (bError || bWarning)
		AUX_GetMainFrame()->ShowMessageWnd(true);

	return !bError;
}

void CSectorEditorDoc::OnEditProperty() 
{
	if (!m_pSelObj || m_pSelObj->GetID() != EOID_PORTAL)
		return;
	
	CDlgPortalProp PropDlg((CPortal*)m_pSelObj);

	if (PropDlg.DoModal() == IDOK)
		SetModifiedFlag(TRUE);
}

//	Generate a random unique portal name
void CSectorEditorDoc::GeneratePortalName(char* szNameBuf)
{
	while (1)
	{
		//	Generate random name
		sprintf(szNameBuf, "portal_%d", rand() % 50000);

		bool bUnique = true;

		//	Ensure name is unique
		ALISTPOSITION pos = m_PortalList.GetHeadPosition();
		while (pos)
		{
			CPortal* pPortal = m_PortalList.GetNext(pos);
			if (!stricmp(szNameBuf, pPortal->GetName()))
			{
				bUnique = false;
				break;
			}
		}

		if (bUnique)
			return;
	}	
}



