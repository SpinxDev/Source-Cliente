// FTDriverTestView.cpp : implementation of the CFTDriverTestView class
//

#include "stdafx.h"
#include "FTDriverTest.h"

#include "FTDriverTestDoc.h"
#include "FTDriverTestView.h"
#include "FTInterface.h"
#include "InputDlg.h"
#include <process.h>

typedef struct _tagTT_OFFSET_TABLE{
	USHORT	uMajorVersion;
	USHORT	uMinorVersion;
	USHORT	uNumOfTables;
	USHORT	uSearchRange;
	USHORT	uEntrySelector;
	USHORT	uRangeShift;
}TT_OFFSET_TABLE;

typedef struct _tagTT_TABLE_DIRECTORY{
	char	szTag[4];			//table name
	ULONG	uCheckSum;			//Check sum
	ULONG	uOffset;			//Offset from beginning of file
	ULONG	uLength;			//length of the table in bytes
}TT_TABLE_DIRECTORY;

typedef struct _tagTT_NAME_TABLE_HEADER{
	USHORT	uFSelector;			//format selector. Always 0
	USHORT	uNRCount;			//Name Records count
	USHORT	uStorageOffset;		//Offset for strings storage, from start of the table
}TT_NAME_TABLE_HEADER;

typedef struct _tagTT_NAME_RECORD{
	USHORT	uPlatformID;
	USHORT	uEncodingID;
	USHORT	uLanguageID;
	USHORT	uNameID;
	USHORT	uStringLength;
	USHORT	uStringOffset;	//from start of storage area
}TT_NAME_RECORD;

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SAFE_DELETE(p) {if (p) {delete (p); (p) = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if (p) {delete [] (p); (p) = NULL;}}

/////////////////////////////////////////////////////////////////////////////
// CFTDriverTestView

IMPLEMENT_DYNCREATE(CFTDriverTestView, CView)

BEGIN_MESSAGE_MAP(CFTDriverTestView, CView)
	//{{AFX_MSG_MAP(CFTDriverTestView)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_TEST_MULTI_THREAD, OnTestMultiThread)
	ON_COMMAND(ID_SET_FONT, OnSetFont)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFTDriverTestView construction/destruction

CFTDriverTestView::CFTDriverTestView()
{
	// TODO: add construction code here
	m_bEnterDowned = false;
	m_pManager = CreateFTManager();
	m_pFontFT = m_pManager->CreateFont("simhei.ttf",  0, 500, STYLE_BOLD, 
		1,FONT_CREATE_FREETYPE,L"黑体");
	m_pBuffer = 0;
	m_pFontWT = m_pManager->CreateFont("simhei.ttf",  0, 500, STYLE_BOLD, 
		1,FONT_CREATE_NOFREETYPE,L"DFPShaoNvW5-GB");
}	

CFTDriverTestView::~CFTDriverTestView()
{
	if(m_pFontFT)
		m_pFontFT->Release();
	if(m_pFontWT)
		m_pFontWT->Release();
	m_pManager->Release();
	delete [] m_pBuffer;
}

BOOL CFTDriverTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CFTDriverTestView drawing

void CFTDriverTestView::OnDraw(CDC* pDC)
{
	CFTDriverTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (m_strText.GetLength() == 0)
		return;

	wchar_t *strText = NULL;
	int nLen = 0;
#ifdef _UNICODE
	nLen = m_strText.GetLength();
	strText = m_strText.GetBuffer(0);
#else

	strText = ANSIToUNICODE(m_strText.GetBuffer(0));
	nLen = wcslen(strText);
#endif
	SIZE size;
	BYTE *pByteBuff = NULL;
	BYTE *pByteBuffOutline = NULL;
	DWORD *pDwordBuff = NULL;
	DWORD *pDwordBuffOutline = NULL;
	int w,h;
	FTDSIZE size0, size1, size2;
	FTDSIZE size00, size11, size22;
	
	do{
		m_pFontFT->GetMaxSize(w, h);
		m_pFontFT->GetCharExtent(strText[1], &size0, &size1, &size2);
		m_pFontWT->GetCharExtent(strText[1],&size00, &size11, &size22);
		if (!m_pFontFT->GetTextExtentExPoint(strText, nLen, 0, NULL, NULL, (LPFTDSIZE)&size))
			break;
		if (0 == size.cx || 0 == size.cy)
			break;
			
		pByteBuff = new BYTE[size.cx * size.cy];
		pByteBuffOutline = new BYTE[size.cx * size.cy];
		pDwordBuff = new DWORD[size.cx * size.cy];
		pDwordBuffOutline = new DWORD[size.cx * size.cy];

		if (!m_pFontFT->TextOut(0, 0, strText, nLen, pByteBuff, size.cx, size.cy))
			break;
		//if (!m_pFontFT->TextOutWithOutline(0, 0, strText, nLen, pByteBuff, pByteBuffOutline, size.cx, size.cy))
			//break;
		//if (!m_pFontFT->DrawChar(strText[0], pByteBuff,size.cx, size.cy))
			//break;
		//if (!m_pFontFT->DrawCharWithOutline(strText[0], pByteBuff, pByteBuffOutline, size.cx, size.cy))
			//break;

		BYTE bColor;
		int i, j;
		for (j = 0; j < size.cy; j++)
		{
			for (i = 0; i < size.cx; i++)
			{
				bColor = ~pByteBuff[j * size.cx + i];
				pDwordBuff[j * size.cx + i] = RGB(bColor, bColor, bColor);
			}
		}
		for (j = 0; j < size.cy; j++)
		{
			for (i = 0; i < size.cx; i++)
			{
				bColor = ~pByteBuffOutline[j * size.cx + i];
				pDwordBuffOutline[j * size.cx + i] = RGB(bColor, bColor, bColor);
			}
		}

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(pDC, size.cx, size.cy);
		memDC.SelectObject(&bitmap);

		bitmap.SetBitmapBits(size.cx * size.cy * sizeof(DWORD), pDwordBuff);
		pDC->BitBlt(0, 0, size.cx, size.cy, &memDC, 0, 0, SRCCOPY);
 		bitmap.SetBitmapBits(size.cx * size.cy * sizeof(DWORD), pDwordBuffOutline);
 		pDC->BitBlt(0, size.cy + 1, size.cx, size.cy * 2 + 1, &memDC, 0, 0, SRCCOPY);
		
		SAFE_DELETE_ARRAY(pByteBuff);
		SAFE_DELETE_ARRAY(pDwordBuff);
		SAFE_DELETE_ARRAY(pByteBuffOutline);
		SAFE_DELETE_ARRAY(pDwordBuffOutline);
	} while(false);

	do{
		m_pFontWT->GetMaxSize(w, h);
	//	m_pFontWT->GetCharExtent(strText[0],&size0, &size1, &size2);
		if (!m_pFontWT->GetTextExtentExPoint(strText, nLen, 0, NULL, NULL, (LPFTDSIZE)&size))
			break;
		if (0 == size.cx || 0 == size.cy)
			break;
			
		pByteBuff = new BYTE[size.cx * size.cy];
		pByteBuffOutline = new BYTE[size.cx * size.cy];
		pDwordBuff = new DWORD[size.cx * size.cy];
		pDwordBuffOutline = new DWORD[size.cx * size.cy];

		if (!m_pFontWT->TextOut(0, 0, strText, nLen, pByteBuff, size.cx, size.cy))
			break;
		//if (!m_pFontWT->TextOutWithOutline(0, 0, strText, nLen, pByteBuff, pByteBuffOutline, size.cx, size.cy))
		//	break;
		//if (!m_pFontWT->DrawChar(strText[0], pByteBuff,size.cx, size.cy))
			//break;

		//if (!m_pFontWT->DrawCharWithOutline(strText[0], pByteBuff, pByteBuffOutline, size.cx, size.cy))
			//break;

		BYTE bColor;
		int i, j;
		for (j = 0; j < size.cy; j++)
		{
			for (i = 0; i < size.cx; i++)
			{
				bColor = ~pByteBuff[j * size.cx + i];
				pDwordBuff[j * size.cx + i] = RGB(bColor, bColor, bColor);
			}
		}
		for (j = 0; j < size.cy; j++)
		{
			for (i = 0; i < size.cx; i++)
			{
				bColor = ~pByteBuffOutline[j * size.cx + i];
				pDwordBuffOutline[j * size.cx + i] = RGB(bColor, bColor, bColor);
			}
		}

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(pDC, size.cx, size.cy);
		memDC.SelectObject(&bitmap);

		bitmap.SetBitmapBits(size.cx * size.cy * sizeof(DWORD), pDwordBuff);
		pDC->BitBlt(0, 200, size.cx, size.cy, &memDC, 0, 0, SRCCOPY);
 		bitmap.SetBitmapBits(size.cx * size.cy * sizeof(DWORD), pDwordBuffOutline);
 		pDC->BitBlt(0, 200+size.cy + 1, size.cx, size.cy * 2 + 1, &memDC, 0, 0, SRCCOPY);
		SAFE_DELETE_ARRAY(pByteBuff);
		SAFE_DELETE_ARRAY(pDwordBuff);
		SAFE_DELETE_ARRAY(pByteBuffOutline);
		SAFE_DELETE_ARRAY(pDwordBuffOutline);
	}while(false);
	
	m_strText.ReleaseBuffer();
#ifndef _UNICODE
	SAFE_DELETE_ARRAY(strText);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CFTDriverTestView printing

BOOL CFTDriverTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFTDriverTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFTDriverTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CFTDriverTestView diagnostics

#ifdef _DEBUG
void CFTDriverTestView::AssertValid() const
{
	CView::AssertValid();
}

void CFTDriverTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFTDriverTestDoc* CFTDriverTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFTDriverTestDoc)));
	return (CFTDriverTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFTDriverTestView message handlers

void CFTDriverTestView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (VK_RETURN == nChar)
	{
		m_bEnterDowned = true;
	}
	else
	{
		m_bEnterDowned = false;
	}
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFTDriverTestView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (VK_RETURN == nChar && m_bEnterDowned)
	{

		CInputDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			m_strText = dlg.m_strText;
			Invalidate();
		}
	}
	
	m_bEnterDowned = false;

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

wchar_t * CFTDriverTestView::ANSIToUNICODE(const char *strANSI)
{
	int nLen = 0;	
	if (!(nLen = MultiByteToWideChar(CP_ACP, 0, strANSI, -1, NULL, 0)))
		return NULL;
	wchar_t *strUnicode = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, 0, strANSI, -1, strUnicode, nLen);
	return strUnicode;

}

void CFTDriverTestView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
	CView::OnLButtonDown(nFlags, point);
}

void CFTDriverTestView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// test render 
	static const wchar_t str[] = L"我想起和我女儿同名的四川省会．那里也有一位都人士。成都武侯祠里的请葛亮，可谓是中国历史上秀外慧中的传奇人物。在小说家罗贯中的笔下．诸葛亮以他特有的潇洒机智，将所有厉害的对手(包括天才的军事家曹操和周瑜)戏弄得如同小丑．好像天地间的风云变化，早巳在他的掌握巾般。可是．许多刻薄的读者却发出了这样的质疑：既然诸葛亮这般了得，为什么六出祁山．屡战不胜，最后在秋风五丈原中，落得“出师未捷身先死．长使英雄泪满襟”的悲剧收场?诸葛亮一生的输赢，可能会成为一十永久的历史争议。就像诗人陆游所咏叹的那样：“斜阳古道赵家庄．负鼓盲翁正作场。身后是非谁管得。满村听说蔡中郎。”造个蔡中郎．就是才女蔡文姬的父亲、大文豪蔡邕如皋追溯到设有《三国演义》的年代．无论诸葛亮生括的那个时期．以及此后的两晋南北朝和唐米．无论文如“光焰万丈长”的李白杜甫、武如“精忠报国”的岳飞，几乎无人不对诸葛丞相推崇有加。即使是将近1800年之后的今天，透对这些“满村听说”的是非，他老人家的智慧、道德、文章仍然散发着阳光一样的魅力。我从韭于管理咨询和营销策划多年．一直把诸葛亮奉为我古代的老师．同时也把手抄前后《出师表》的岳飞视为典范。我在同行们中间是出了名的三国迷．现在也算是客串了一回说书匠。与以往说书先生讲的内容有所不同的是，这本书是用管理学做底料，水煮的系列三国故事。许多经理人身份的读者认为，这种另类三国的确有如一道名叫“水煮鱼片”的川菜，别有一番风味。写作的过程常常有一种快意口舌的享受。然而．我也常常诧异于自己的胆大妄为。我个人对管理学的体验毕竟还是很粗浅．对于诸葛亮这样的千古完人．写得不好也怕挨骂。此外．值得注意的是，中国历代的说书人和写书人．都有语不惊人死不休的毛病，我也未能例外。我想．诸葛亮以及他所辅佐的皇叔刘备等人的形象，就是这样在众说纷纭中变得光怪陆离起来的吧?";

	static const int count = sizeof(str) / sizeof(str[0]);

	SIZE size;

	DWORD dwStart, dwEnd;
	dwStart = GetTickCount();

	for (int i = 0; i < count; i++)
	{
		m_pFontFT->GetTextExtentExPoint(&str[i], 1, 0, NULL, NULL, (LPFTDSIZE) &size);
		BYTE *pBuffer = new BYTE[size.cx * size.cy];
		BYTE *pBuffer1 = new BYTE[size.cx * size.cy];
		//m_pFont->TextOut(0, 0, &str[i], 1, pBuffer, size.cx, size.cy);
		m_pFontFT->TextOutWithOutline(0, 0, &str[i], 1, pBuffer, pBuffer1, size.cx, size.cy);
		delete [] pBuffer;
		delete [] pBuffer1;
	}

	dwEnd = GetTickCount();
	
	CString msg;
	msg.Format(_T("Time used : %u milliseconds"), dwEnd - dwStart);
	AfxMessageBox(msg);


	// test reliability
//	for (unsigned char i = 0; i < 255; i++)
//		for (unsigned char j = 0; j < 255; j++)
//		{
//			m_strText = "国"; 
//			m_strText.SetAt(0, i);
//			m_strText.SetAt(1, j);
//			Invalidate(FALSE);
//			UpdateWindow();
//		}
	
	CView::OnLButtonDblClk(nFlags, point);
}




void CFTDriverTestView::OnTestMultiThread() 
{
	static const int threadCount = 5;
	unsigned long threadHandles[threadCount];

	for (int i = 0; i < threadCount; i++)
	{
		IFTFont * pFont =m_pManager->CreateFont((const char *)m_pBuffer,  0, 500, STYLE_BOLD|STYLE_ITALIC, 
		1,FONT_CREATE_NOFREETYPE,L"DFPShaoNvW5-GB");
		threadHandles[i] = _beginthreadex(NULL, 0, MultiThreadTestFunc, pFont, CREATE_SUSPENDED, NULL);
		if (threadHandles[i] <= 1)
		{
			AfxMessageBox(_T("failed to create font"));
			return;
		}
	}
	for (i = 0; i < threadCount; i++)
	{
		ResumeThread((HANDLE)threadHandles[i]);
	}
	
}


unsigned __stdcall CFTDriverTestView::MultiThreadTestFunc(void * pData)
{
	IFTFont * pFont = static_cast<IFTFont *>(pData);

	for (int k = 0; k < 10; k++)
	{
		wchar_t tmpChar = 0;
		FTDSIZE size;
		for (int i = 0; i < 6553; i++)
		{
			pFont->GetTextExtentExPoint(&tmpChar, 1, 0, NULL, NULL, &size);
		}
	}

	pFont->Release();

	return 0;
}

void CFTDriverTestView::OnSetFont() 
{
	char szFile[MAX_PATH];
	wchar_t szFace[MAX_PATH];

	OPENFILENAMEA ofn;

	szFile[0] = '\0';
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hInstance = NULL;
	ofn.hwndOwner = NULL;
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "(*.ttf)\0*.ttf\0(*.ttc)\0*.ttc\0";
	ofn.lpstrDefExt = ".ttf";

	if( !GetOpenFileNameA(&ofn) )
		return;

	if( m_pFontFT )
	{
		m_pFontFT->Release();
		m_pFontFT = NULL;
	}
	if( m_pFontWT )
	{
		m_pFontWT->Release();
		m_pFontWT = NULL;
	}
	if( m_pBuffer )
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
	if( m_pManager )
	{
		m_pManager->Release();
		m_pManager = NULL;
	}

	m_pManager = CreateFTManager();
	/*
	CFile fileFont(szFile, CFile::modeRead | CFile::shareDenyWrite);
	m_dwFontFileLength = fileFont.GetLength();
	m_pBuffer = new BYTE[m_dwFontFileLength];
	fileFont.ReadHuge(m_pBuffer, m_dwFontFileLength);
	*/	
	GetFaceNameTTF(szFile,szFace);
	m_pFontFT = m_pManager->CreateFont(szFile, 0, 500, STYLE_BOLD|STYLE_ITALIC, 1);
	m_pFontWT = m_pManager->CreateFont(szFile,  0, 500, STYLE_BOLD|STYLE_ITALIC, 
		1,FONT_CREATE_NOFREETYPE,szFace);
	m_pBuffer = new BYTE[strlen(szFile)+1];
	strcpy((char *)m_pBuffer,szFile);

}

bool CFTDriverTestView::GetFaceNameTTF(const char *pFile,wchar_t * face)
{
	CFile f;
	BOOL bRetVal = FALSE;

	//char   a[2048]//   to   'unsigned   short   *'   
	BSTR   path;   
	CString   c(pFile);   
	CString facename,family;
	path=c.AllocSysString();   

	if(f.Open(path, CFile::modeRead|CFile::shareDenyWrite)){
		TT_OFFSET_TABLE ttOffsetTable;
		f.Read(&ttOffsetTable, sizeof(TT_OFFSET_TABLE));
		ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
		ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
		ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

		//check is this is a true type font and the version is 1.0
		if(ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
			return bRetVal;
		
		TT_TABLE_DIRECTORY tblDir;
		BOOL bFound = FALSE;
		CString csTemp;
		
		for(int i=0; i< ttOffsetTable.uNumOfTables; i++){
			f.Read(&tblDir, sizeof(TT_TABLE_DIRECTORY));
			if(strncmp("name",tblDir.szTag,4) == 0){
				bFound = TRUE;
				tblDir.uLength = SWAPLONG(tblDir.uLength);
				tblDir.uOffset = SWAPLONG(tblDir.uOffset);
				break;
			}
		}
		
		if(bFound){
			f.Seek(tblDir.uOffset, CFile::begin);
			TT_NAME_TABLE_HEADER ttNTHeader;
			f.Read(&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER));
			ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
			ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);
			TT_NAME_RECORD ttRecord;
			bFound = FALSE;
			
			for(int i=0; 
				i<ttNTHeader.uNRCount;i++){
				f.Read(&ttRecord, sizeof(TT_NAME_RECORD));
				ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);
				ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
				ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);

				if(ttRecord.uNameID == 1 || ttRecord.uNameID == 0 || ttRecord.uNameID == 7){
					int nPos = f.GetPosition();
					f.Seek(tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset, CFile::begin);
					csTemp.Empty();
					f.Read(csTemp.GetBuffer(ttRecord.uStringLength), ttRecord.uStringLength);
					csTemp.ReleaseBuffer();
					if(csTemp.GetLength() > 0){
						switch(ttRecord.uNameID){
						case 1:
							family.IsEmpty() ? family = csTemp : void(0);
							bRetVal = TRUE;
							break;
						case 0:
							//Copyright
							break;
						case 7:
							//Trademark.IsEmpty()
							break;
						case 4:
							facename.IsEmpty() ? facename = csTemp : void(0);break;
						default:
							break;
						}
					}
					f.Seek(nPos, CFile::begin);
				}
			}			
		}
		f.Close();
	}
	if(facename.IsEmpty())
			facename = family;
	int i;
	char*   pszTest   =  (char*)facename.GetBuffer(0);   
	for(i=0;i<facename.GetLength();i++)
	{
		face[i] = pszTest[i];
	}
	face[i]= 0;

	return bRetVal;
}