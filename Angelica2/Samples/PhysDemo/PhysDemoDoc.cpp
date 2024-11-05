// PhysDemoDoc.cpp : implementation of the CPhysDemoDoc class
//

#include "Global.h"
#include "PhysDemo.h"
#include "PhysDemoDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoDoc

IMPLEMENT_DYNCREATE(CPhysDemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CPhysDemoDoc, CDocument)
	//{{AFX_MSG_MAP(CPhysDemoDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoDoc construction/destruction

CPhysDemoDoc::CPhysDemoDoc()
{
	// TODO: add one-time construction code here

}

CPhysDemoDoc::~CPhysDemoDoc()
{
}

BOOL CPhysDemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPhysDemoDoc serialization

void CPhysDemoDoc::Serialize(CArchive& ar)
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
// CPhysDemoDoc diagnostics

#ifdef _DEBUG
void CPhysDemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPhysDemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoDoc commands
