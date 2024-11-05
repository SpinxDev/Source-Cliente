#if !defined(AFX_AFILEDIALOG_H__E37FAFE0_0835_420C_87BA_8A0291469A70__INCLUDED_)
#define AFX_AFILEDIALOG_H__E37FAFE0_0835_420C_87BA_8A0291469A70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AFileDialog.h : header file
//


#include "ximage.h"
#include "DynSizeCtrl.h"
#include "AFileImageWrapper.h"
#include "afxwin.h"
#include "AListCtrl.h"
#include "IAFileDialog.h"

//	These constants defines the minimize value of dialog size which would not cause the controls display error
#define AFILE_DLG_MIN_HEIGHT 450
#define AFILE_DLG_MIN_WIDTH 600

#define WM_FOLDER_PROC_COMPLETED	(WM_USER + 0x100)
#define WM_UPDATE_ONE_IMAGE			(WM_USER + 0x101)
#define WM_UPDATE_FIRST_FOLDER		(WM_USER + 0x102)
#define WM_ONDBCLK_SELFILE			(WM_USER + 0x103)

class AListCtrl;
class A3DDevice;
struct AFSoundPlayItem;
class AMSoundBufferMan;
class A3DGfxEngine;
class AFileGfxInterface;

class ICLThreadPool;
class ICLFile;
class ICLNotifier;

class AFilePreviewWnd;
class AFilePreviewItem;

#ifdef _USE_BCGP

typedef CBCGPDialog BaseDialog;
typedef CBCGPButton BaseButton;
typedef CBCGPStatic BaseStatic;
typedef CBCGPEdit BaseEdit;
typedef CBCGPComboBox BaseComboBox;
typedef CBCGPComboBox BaseComboBoxEx;

#else

typedef CDialog BaseDialog;
typedef CButton BaseButton;
typedef CStatic BaseStatic;
typedef CEdit BaseEdit;
typedef CComboBox BaseComboBox;
typedef CComboBoxEx BaseComboBoxEx;

#endif

class AComboBox : public BaseComboBox
{
public:
	void DrawItem(LPDRAWITEMSTRUCT) {}
};

/////////////////////////////////////////////////////////////////////////////
// AFileDialog dialog

class AFileDialog : public BaseDialog, public IAFileDialog
{

public:

	typedef BaseDialog base_class;

	enum AFFilter
	{
		AFF_TEXTUREFILE		= 0,
		AFF_SOUNDFILE,
		AFF_TEXTFILE,
		AFF_CUSTOMFILE,					//	If user passed a custom extension in, we add this
		AFF_ALL,
	};

	friend class AFileDialogMsgReciever;
	friend class AListCtrlListener;

	enum AFItemType
	{
		AFT_IMG,
		AFT_DIR,
		AFT_SOUND,
		AFT_TEXT,
		AFT_CUSTOM,
		AFT_DEFAULT,
		AFT_OTHER,
	};

	struct AFITEM
	{
		virtual ~AFITEM() = 0 {}
		int GetType() const { return nType; }
		time_t GetTime() const;
		void SetFileTime(const FILETIME& ft);
		void SetFileSize(ULONGLONG dwFs);
		ULONGLONG GetFileSize() const { return dwFilesize; }
		void SetFullName(const TCHAR * szFullname);
		const TCHAR * GetFullName() const { return strFullname; }
		virtual CBitmap* GetBitmap(bool bIsAlphaBlend = false) = 0;
	protected:
		void SetType(int type) { nType = type; }
	private:
		CString strFullname;
		FILETIME ftFiletime;
		ULONGLONG dwFilesize;
		int nType;
	};
	
	struct AFITEM_Image : public AFITEM
	{
		typedef AFITEM base_class;
		AFITEM_Image();
		~AFITEM_Image();
		
		void LoadFromMemory(unsigned char* pBuf, long size);
		virtual CBitmap* GetBitmap(bool bIsAlphaBlend);
	private:
		AFileImageWrapper pThumbnail;
	};

	struct AFITEM_Directory : public AFITEM
	{
		typedef AFITEM base_class;
		AFITEM_Directory(CBitmap* pDB)
			: pDicBitmap(pDB)
		{
			SetType(AFT_DIR);
		}
		virtual CBitmap* GetBitmap(bool) { return pDicBitmap; }
	private:
		CBitmap* pDicBitmap;
	};

	struct AFITEM_Sound : public AFITEM
	{
		typedef AFITEM base_class;
		AFITEM_Sound(CBitmap* pB)
			: pBitmap(pB)
		{
			SetType(AFT_SOUND);
		}
		virtual CBitmap* GetBitmap(bool) { return pBitmap; }
	private:
		CBitmap* pBitmap;
	};

	struct AFITEM_Custom : public AFITEM_Image
	{
		typedef AFITEM_Image base_class;
		AFITEM_Custom(AFileDialog* pFD, bool bIsImage)
			: m_pFileDialog(pFD)
			, m_bIsImage(bIsImage)
		{
			SetType(AFT_CUSTOM);
		}
		virtual CBitmap* GetBitmap(bool);
	private:
		AFileDialog* m_pFileDialog;
		bool m_bIsImage;
	};
	
	struct AFITEM_Default : public AFITEM
	{
		typedef AFITEM base_class;
		AFITEM_Default(AFileDialog* pFD)
			: pFileDialog(pFD)
		{
			SetType(AFT_DEFAULT);
		}
		virtual CBitmap* GetBitmap(bool);
	private:
		AFileDialog* pFileDialog;
	};

	struct AFITEM_Other : public AFITEM
	{
		typedef AFITEM base_class;
		AFITEM_Other(AFileDialog* pFD)
			: pFileDialog(pFD)
		{
			SetType(AFT_OTHER);
		}
		virtual CBitmap* GetBitmap(bool);
	private:
		AFileDialog* pFileDialog;
	};
	
	friend struct AFileDialog::AFITEM_Default;
	friend CBitmap* getBitmap(AFileDialog* pFileDialog, const TCHAR* szFullName);

	typedef AFVector<AFITEM*> AFItemArray;
	typedef AFItemArray::iterator AFItemIterator;


protected:

	// Configurations serializable
	struct AFConfigures
	{
		// Constructors / destructor
		AFConfigures()
			: m_nLastSelectedFilter(AFF_TEXTUREFILE)
			, m_bLastCheckedAlphaSwitch(FALSE)
			, m_nCX(520)
			, m_nCY(450)
			, m_nListStyle(FLS_THUMBNAIL)
		{
			LoadSettingsFromReg();
		}
		~AFConfigures()
		{
			SaveSettingsToReg();
		}

		// Property Get / Set Methods
		void OnCurRelativePathChanged(const TCHAR* szCurFull);
		void OnCurSelectedFilterChanged(int nCurFilter) { m_nLastSelectedFilter = nCurFilter; }
		void OnCurAlphaBlendSwitchChanged(BOOL bIsAlphaBlend) { m_bLastCheckedAlphaSwitch = bIsAlphaBlend; }
		void OnSize(int cx, int cy) { m_nCX = cx, m_nCY = cy; }
		void OnChangeListStyle(int nListStyle) { m_nListStyle = nListStyle; }
		void OnChangeSortStyle(int nSortStyle) { m_nSortStyle = nSortStyle; }

		const TCHAR* GetLastRelativePath() const { return m_strLastOpenRelativePath; }
		int GetLastSelectedFilter() const { return m_nLastSelectedFilter; }
		BOOL GetLastCheckedAlphaBlendSwitch() const { return m_bLastCheckedAlphaSwitch; }
		int GetLastCX() const { return m_nCX; }
		int GetLastCY() const { return m_nCY; }
		int GetLastListStyle() const { return m_nListStyle; }

	private:		// Private Properties
		CString		m_strLastOpenRelativePath;		// The path we opened at the last time (relative path)
		int			m_nLastSelectedFilter;			// The filter type we last selected
		BOOL		m_bLastCheckedAlphaSwitch;		// Whether the alpha blend state is opened last time
		int			m_nCX, m_nCY;					// Remember the last dialog size set by user
		int			m_nListStyle;					// Remember the last liststyle selected by user
		int			m_nSortStyle;

	private:		// Private operations
		void LoadSettingsFromReg();
		void SaveSettingsToReg();
		const TCHAR* GetRegSection() const;

	private:		// Entry name for saving / loading
		static TCHAR const * s_szEntryLastOpenRelativePath;
		static TCHAR const * s_szEntryLastSelectedFilter;
		static TCHAR const * s_szEntryLastCheckedAlphaSwitch;
		static TCHAR const * s_szEntryLastWndWidth;
		static TCHAR const * s_szEntryLastWndHeight;
		static TCHAR const * s_szEntryLastListStyle;
		static TCHAR const * s_szEntryLastSortStyle;

	} m_afConfigs;


// Construction
public:
	AFileDialog(A3DDevice* pDevice
		, BOOL bIsOpenFile = TRUE
		, LPCTSTR szInitRelativeDir = _AL("")
		, LPCTSTR szCaption = _AL("")
		, LPCTSTR szExt = _AL("")
		, int iListStyle = FLS_THUMBNAIL
		, CWnd* pParent = NULL);   // standard constructor
	~AFileDialog();

private:
	AFileDialog(const AFileDialog&);
	AFileDialog& operator = (const AFileDialog&);

public:

// Dialog Data
	//{{AFX_DATA(AFileDialog)
	enum { IDD = IDD_FILE_DIALOG };
	CButton	m_btUp;
	CButton	m_btBack;
	BaseButton	m_btOK;
	BaseButton	m_btCancel;
	BaseButton  m_OpenFolder;
	BaseButton m_cxAlphaBlend;
	BaseButton m_cxPWndShow;
	BaseStatic	m_txtLookIn;
	BaseStatic	m_txtFileType;

	BaseStatic	m_txtFileName;
	BaseEdit	m_edFileName;
	CComboBoxEx	m_cbPathShortCut;
	AComboBox	m_cbFileFilter;
	AListCtrl*	m_pLstFiles;
	AFilePreviewWnd* m_pPreviewWnd;
	unsigned __int64 m_uPreviewWndProcThread;
	CPoint m_origwsize;
	CPoint m_lastwsize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AFileDialog)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AFileDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCbFiletypes();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	virtual void OnOK();
	afx_msg void OnSelchangeCbPathShortcut();
	afx_msg void OnBtBack();
	afx_msg void OnBtUp();
	afx_msg LRESULT OnFolderProcCompleted(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedIsAlphaBlend();
	afx_msg void OnBnClickedIsWindowShow();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	//	Get relative file path with out basedir and initdir.
	const TCHAR* GetRelativeFileName();
	//	Get full file path name
	const TCHAR* GetFullFileName() const { return m_strSelFileFullName; }
	const AFVector<CString> GetSelFiles() const { return m_aSelFiles; }

private:

	HINSTANCE m_hHostResourceHandle;		// Host resource handle, in order to load the correct resource
	const CString m_strInitRelativeDir;		// Init relative dir
	CString m_strFullBaseDir;				// Full base dir
	CString m_strCurDir;					// Current dir
	CString m_strCaption;					// Dialog caption
	CString m_strExt;						// File filter ext
	CString m_strSelFileFullName;			// Sel file full name
	CString m_strRelativePath;				// Sel file relative path name (relative to m_strFullBaseDir)
	AFVector<CString> m_aSelFiles;			// All selected files
	AFVector<CString> m_aCustomExtTypes;	// Custom extension types

	CStringArray m_aDirs;					// Split the path of Current Directory without Base Directory
	CStringArray m_aDirHistory;				// Store the history path
	BOOL m_bIsOpenFile;
	int m_nErrorCode;
	int m_nCurFilterType;					// Current items in list ctrl
	int m_nListStyle;						// List style shown when dialog is created
	int m_nSortStyle;

	// Image cache lib
	ICLThreadPool* m_pICLPool;
	ICLFile* m_pICLFile;
	ICLNotifier* m_pMsgNorifier;

	// AListCtrl msg listener
	AListCtrlListener* m_pLstCtrlListener;
	
	// fileItems
	AFItemArray	m_aItems;

	// Angelica device
	A3DDevice* m_pDevice;
	AFSoundPlayItem* m_pSound;
	AMSoundEngine* m_pSoundEngine;
	AMSoundBufferMan* m_pSoundBufferMan;


	// Constant Items
	CBitmap m_bmDirectory;
	CBitmap m_bmPicture;
	CBitmap m_bmText;
	CBitmap m_bmSound;
	CBitmap m_bmOther;

	// For small icon of directory
	CBitmap m_bmDirSmall;

	// For button icon
	CBitmap m_bmArrowUp;
	CBitmap m_bmArrowBack;

	// For shortcut path selection
	CBitmap m_bmSmallDirectory;
	CImageList m_lstImgShortCut;

	// Dyn-resize control support
	DynSizeCtrlArray m_dynSizeArray;
	
private:
	void adjustPreviewWndPos();
	bool isAFItemInCustom(const AFITEM& item);
	bool checkIsInExt(const TCHAR ch);
	void updateCustomExtFiles(const TCHAR* szCustomExts);
	int getFilterTypeBySel(const AComboBox& cbFilter);
	int getSelIdxByFilterType(int iFilter);
	bool formatInitDir(const TCHAR * szRelativeDir);
	void initFileFilter();
	void initWndSize(int cx, int cy);
	void updatePathShortCut();
	void updateOnCurDirChanged(const TCHAR * szCurDir, bool bLogHis = true);
	void onUpdateFileList(const TCHAR * szDir);
	void updateFileListByDefaultIcon(const TCHAR * szDir);
	void updateListItems(int nFilterType);
	void updateListItems_Async(int nFilterType);
	int getFilterByExt(const TCHAR * szExt);
	void loadResources();
	size_t getAFItemByName(const TCHAR * szName) const;
	bool isDir(const TCHAR * szName) const;
	void splitCurDir(const CString& strCurDir, const CString& strBaseDir, CStringArray& aDirs);
	void filterDir(AFVector<CString>& vec);
	AFVector<CString>::const_iterator isContainDir(const AFVector<CString>& vec) const;
	void stepDir(const TCHAR * szName) const;
	bool isAlphaBlendChecked() const;
	bool isPWndShow() const;

	int createPreviewWnd(A3DDevice* pA3DDevice);
	void destroyPreviewWnd();

	afx_msg void OnFileLstClick(int nItem);
	afx_msg void OnFileLstDblClick(int nItem);	
public:
	virtual BOOL DestroyWindow();

	
	afx_msg void OnMove(int x, int y);
	
	friend class UpdateImageTask;
};

class CFileFind;
FILETIME sysTime2FileTime(const SYSTEMTIME& sysTime);
FILETIME ctime2FileTile(const CTime& time);
bool is_skip_file(CFileFind& finder);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AFILEDIALOG_H__E37FAFE0_0835_420C_87BA_8A0291469A70__INCLUDED_)
