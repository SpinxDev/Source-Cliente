// FTDriverTestDoc.cpp : implementation of the CFTDriverTestDoc class
//

#include "stdafx.h"
#include "FTDriverTest.h"

#include "FTDriverTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFTDriverTestDoc

IMPLEMENT_DYNCREATE(CFTDriverTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CFTDriverTestDoc, CDocument)
	//{{AFX_MSG_MAP(CFTDriverTestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFTDriverTestDoc construction/destruction

CFTDriverTestDoc::CFTDriverTestDoc()
{
	// TODO: add one-time construction code here

}

CFTDriverTestDoc::~CFTDriverTestDoc()
{
}

BOOL CFTDriverTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CFTDriverTestDoc serialization

void CFTDriverTestDoc::Serialize(CArchive& ar)
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
// CFTDriverTestDoc diagnostics

#ifdef _DEBUG
void CFTDriverTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFTDriverTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFTDriverTestDoc commands
