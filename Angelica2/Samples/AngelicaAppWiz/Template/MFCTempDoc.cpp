// $$root$$Doc.cpp : implementation of the C$$root$$Doc class
//

#include "stdafx.h"
#include "$$root$$.h"

#include "$$root$$Doc.h"

/////////////////////////////////////////////////////////////////////////////
// C$$root$$Doc

IMPLEMENT_DYNCREATE(C$$root$$Doc, CDocument)

BEGIN_MESSAGE_MAP(C$$root$$Doc, CDocument)
	//{{AFX_MSG_MAP(C$$root$$Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C$$root$$Doc construction/destruction

C$$root$$Doc::C$$root$$Doc()
{
	// TODO: add one-time construction code here

}

C$$root$$Doc::~C$$root$$Doc()
{
}

BOOL C$$root$$Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// C$$root$$Doc serialization

void C$$root$$Doc::Serialize(CArchive& ar)
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
// C$$root$$Doc diagnostics

#ifdef _DEBUG
void C$$root$$Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void C$$root$$Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// C$$root$$Doc commands
