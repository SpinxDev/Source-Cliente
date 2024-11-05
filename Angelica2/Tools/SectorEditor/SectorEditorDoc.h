// SectorEditorDoc.h : interface of the CSectorEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTOREDITORDOC_H__FB7C4C2D_3643_4192_81F3_88C24EC2C13B__INCLUDED_)
#define AFX_SECTOREDITORDOC_H__FB7C4C2D_3643_4192_81F3_88C24EC2C13B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AList2.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class CTextureScene;
class CFrameScene;
class CSectorEditorView;
class CSector;
class CPortal;
class CEditObject;
class COperation;

struct PTBUILDPARAMS;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CSectorEditorDoc
//
///////////////////////////////////////////////////////////////////////////

class CSectorEditorDoc : public CDocument
{
protected: // create from serialization only

	CSectorEditorDoc();
	DECLARE_DYNCREATE(CSectorEditorDoc)

public:		//	Types

	struct FILEHEADER
	{
		DWORD	dwFlags;		//	File flag
		DWORD	dwVersion;		//	File version, current version is SEDFILE_VERSION
		int		iNumSector;		//	Number of sector
		int		iNumPortal;		//	Number of portal
	};

	struct FACEAABB
	{
		A3DVECTOR3 vMins;
		A3DVECTOR3 vMaxs;
	};

public:		//	Attributes

public:		//	Operations

	//	Get scene interface
	CTextureScene* GetTextureScene() { return m_pTextureScene; }
	CFrameScene* GetFrameScene() { return m_pFrameScene; }

	//	Get associated view
	CSectorEditorView* GetView();

	//	Add visible surfaces
	bool AddVisibleSurfaces(int* aVisSurfs, int iNumSurf);
	//	Clear all visible surface
	void ClearVisibleSurfaces()
	{
		if (m_aVisSurfFlags)
			memset(m_aVisSurfFlags, 0, m_iNumFace);

		m_iNumVisSurf = 0;
	}

	//	Change scene file
	bool ChangeSceneFile(const char* szFile);

	//	Add sector to list
	bool AddSector(CSector* pSector);
	//	Remove sector from list
	bool RemoveSector(CSector* pSector, bool bUnLink);
	//	Add portal to list
	bool AddPortal(CPortal* pPortal);
	//	Remove portal from list
	bool RemovePortal(CPortal* pPortal, bool bUnLink);
	//	Enumerate sector and portal
	CSector* GetFirstSector();
	CSector* GetNextSector();
	CPortal* GetFirstPortal();
	CPortal* GetNextPortal();
	//	Get sector's index
	int GetSectorIndex(CSector* pSector);
	//	Get portal's index
	int GetPortalIndex(CPortal* pPortal);
	//	Get sector by index
	CSector* GetSectorByIndex(int iIndex);
	//	Get portal by index
	CPortal* GetPortalByIndex(int iIndex);
	//	Get portal by name
	CPortal* GetPortalByName(const char* szName);
	//	Generate a random unique portal name
	void GeneratePortalName(char* szNameBuf);
	//	Unlink sector
	void UnlinkSector(CSector* pSector);
	//	Get portal number
	int GetPortalNum() { m_PortalList.GetCount(); }
	//	Get sector number
	int GetSectorNum() { m_SectorList.GetCount(); }

	//	Check sector collision
	bool CheckSectorCollision(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, CSector* pExcept);
	bool SizeSectorCollision(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, CSector* pExcept, int iAxis, int iSign);
	bool MoveSectorCollision(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, CSector* pExcept, const A3DVECTOR3& vDelta);

	//	Begin a new operation
	bool BeginOperation(int iOperation, DWORD dwParam);
	//	End current operation
	bool EndCurOperation(bool bCancel);
	//	Get current operation
	COperation* GetCurOperation() { return m_pCurOpt; }

	//	Render routine
	bool Render(bool bOnlySel=false);
	//	Select edit object
	void SelectEditObject(CEditObject* pObject);
	//	Bound edit object
	void SetBound(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, bool bWindow);
	//	Get selected edit object
	CEditObject* GetSelectedEditObject() { return m_pSelObj; }

	//	Get currently load file's version
	DWORD GetCurFileVersion() { return m_dwCurFileVer; }
	//	Get scene file
	const char* GetSceneFile() { return m_szSceneFile; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSectorEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSectorEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:	//	Attributes

	CTextureScene*		m_pTextureScene;
	CFrameScene*		m_pFrameScene;
	char				m_szSceneFile[MAX_PATH];
	FACEAABB*			m_aFaceAABBs;

	APtrList<CSector*>	m_SectorList;	//	Sector list
	APtrList<CPortal*>	m_PortalList;	//	Portal list
	CEditObject*		m_pSelObj;		//	Current selected edit object	
	COperation*			m_pCurOpt;		//	Current operation
	ALISTPOSITION		m_SectorPos;	//	Used to enumerate sectors
	ALISTPOSITION		m_PortalPos;	//	Used to enumerate portals

	int*	m_aVisSurfs;		//	Visible surface array
	BYTE*	m_aVisSurfFlags;	//	Visible surfaces' flags
	int		m_iNumVisSurf;		//	Number of visible surface
	int		m_iNumFace;			//	Number of total face

	int		m_dwCurFileVer;		//	Currently loaded file's version

protected:	//	Operations

	//	Load document
	bool LoadDocument(const char* szFile);
	//	Save document
	bool SaveDocument(const char* szFile);
	//	Load scene
	bool LoadScene(const char* szFile);
	//	Build face AABB
	bool BuildFaceAABB();
	//	Release scene
	void ReleaseScene();
	//	Load edit data
	bool LoadEditData(FILE* fp);
	//	Save edit data
	bool SaveEditData(FILE* fp);
	//	Release edit data
	void ReleaseEditData();

	//	Export portal file
	bool ExportPortalFile(const char* szFile, PTBUILDPARAMS* pParams);
	//	Emit sectors
	bool EmitSectors();
	//	Emit portals
	bool EmitPortals();
	//	Emit scene
	bool EmitScene();
	//	Portal file export checking
	bool ExportPortalChecking();

	//	Bound scene
	void BoundScene(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, bool bWindow);
	//	Prompt to select a sector or portal
	bool PromptToSelect(int iObject);	

// Generated message map functions
protected:
	//{{AFX_MSG(CSectorEditorDoc)
	afx_msg void OnEditImportscenefile();
	afx_msg void OnEditCreatesector();
	afx_msg void OnEditSizesector();
	afx_msg void OnEditCreateportal();
	afx_msg void OnEditSizeportal();
	afx_msg void OnEditMoveobject();
	afx_msg void OnEditLinksector();
	afx_msg void OnEditChangelink();
	afx_msg void OnViewForcerender();
	afx_msg void OnFileExportptrfile();
	afx_msg void OnEditSnapportal();
	afx_msg void OnViewMagnify();
	afx_msg void OnViewMinify();
	afx_msg void OnViewTest();
	afx_msg void OnEditDelete();
	afx_msg void OnEditCreatesideportal();
	afx_msg void OnEditProperty();
	afx_msg void OnEditNameportal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECTOREDITORDOC_H__FB7C4C2D_3643_4192_81F3_88C24EC2C13B__INCLUDED_)
