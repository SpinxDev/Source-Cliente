/*
* FILE: AFileDialog2.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/18
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AFileDialog2_H_
#define _AFileDialog2_H_

#include "IAFileDialog.h"
#include "AFileItemType.h"
#include "AListCtrl2.h"

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

class AListCtrl2;
class AListItem;
class AFileItem;
class AFileItemType;
class AFileDialog2MsgReciever;
class AListCtrl2Listener;
struct AFSoundPlayItem;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AFileDialog2
//	
///////////////////////////////////////////////////////////////////////////

class AFileDialog2 : public BaseDialog, public IAFileDialog
{

public:		//	Types

	enum { IDD = IDD_FILE_DIALOG };

	// Configurations serializable
	struct AFConfigures
	{
		// Constructors / destructor
		AFConfigures()
			: m_nLastSelectedFilter(0)
			, m_bLastCheckedAlphaSwitch(FALSE)
			, m_nCX(520)
			, m_nCY(450)
			, m_nListStyle(0)
			, m_nSortStyle(0)
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
		int GetLastSortStyle() const { return m_nSortStyle; }

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

public:		//	Constructor and Destructor

#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21

	AFileDialog2(A3DDevice* pDevice
				, BOOL bIsOpenFile
				, const TCHAR* szInitRelativeDir = _T("")
				, const TCHAR* szCaption = _T("")
				, const TCHAR* szCustomExts = _T("")
				, int iListStyle = AListCtrl2::LV_THUMBVIEW
				, CWnd* pParent = NULL);

#elif defined(_ANGELICA31)

	AFileDialog2(BOOL bIsOpenFile
				, const TCHAR* szInitRelativeDir = _T("")
				, const TCHAR* szCaption = _T("")
				, const TCHAR* szCustomExts = _T("")
				, int iListStyle = AListCtrl2::LV_THUMBVIEW
				, CWnd* pParent = NULL);

#endif
	virtual ~AFileDialog2(void);

public:		//	Attributes

public:		//	Operations
	//	Set init directory. relative to m_strRelativePath
	void SetInitSubDir(const TCHAR* szSubDir);
	//	Get current directory
	const TCHAR* GetCurDir() const { return m_strCurDir; }
	//	Get cached file item number
	int GetFileItemNum() const { return m_aFileItems.GetSize(); }
	//	Get cached file item by index
	AFileItem* GetFileItem(int iIdx) const { return m_aFileItems[iIdx]; }
	//	Find cached file item by file name
	AFileItem* FindFileItem(const TCHAR* szName, int* piIndex = NULL) const;
	//	Get cached file item image list
	CImageList* GetThumbNailImgList() { return &m_FileItemImages; }
	//	Get List ctrl 
	AListCtrl2*	GetListCtrl() const { return m_pLstCtrl; }

	CEvent& GetDestroyEvent() { return m_DestroyEvent; }

	//	Message handler
	void OnItemClicked(AListItem* pItem);
	void OnItemDbClicked(AListItem* pItem);

protected:	//	Attributes

	HINSTANCE m_hHostResourceHandle;		// Host resource handle, in order to load the correct resource
	const CString m_strInitRelativeDir;		// Init relative dir
	CString m_strFullBaseDir;				// Full base dir
	CString m_strCurDir;					// Current dir
	CString m_strCaption;					// Dialog caption
	CString m_strExt;						// File filter ext
	CString m_strSelFileFullName;			// Sel file full name
	CString m_strRelativePath;				// Sel file relative path name (relative to m_strFullBaseDir)
	CString m_strInitSubPath;				// relative to m_strRelativePath
	AFVector<CString> m_aSelFiles;			// All selected files
	AFVector<CString> m_aCustomExtTypes;	// Custom extension types
	BOOL m_bIsOpenFile;						// An open file dialog / or not
	DWORD m_nErrorCode;						// Error code
	CPoint m_origwsize;
	CPoint m_lastwsize;
	CStringArray m_aDirs;					// Split the path of Current Directory without Base Directory
	CStringArray m_aDirHistory;				// Store the history path
	int	m_nListStyle;						// Initialize list style
	int m_nSortStyle;

	// Image cache lib
	ICLThreadPool* m_pICLPool;
	AFileDialog2MsgReciever* m_pMsgNorifier;

	// Controls
	BaseButton	m_btUp;
	BaseButton	m_btBack;
	BaseButton	m_btOK;
	BaseButton	m_btCancel;
	BaseButton	m_cxAlphaBlend;
	BaseButton	m_cxPWndShow;
	BaseButton  m_OpenFolder;
	BaseButton	m_cxDisplayLod;
	BaseStatic	m_txtLookIn;
	BaseStatic	m_txtFileType;
	BaseStatic  m_txtFileInfo;
	BaseStatic	m_txtFileName;
	BaseEdit	m_edFileName;
	CComboBoxEx	m_cbPathShortCut;
	AComboBox	m_cbFileFilter;
	AListCtrl2*	m_pLstCtrl;					// ListCtrl
	AFilePreviewWnd* m_pPreviewWnd;			// PreviewWnd

	//	ListCtrl listener
	AListCtrl2Listener* m_pLstCtrlListener;

	// For button icon
	CBitmap m_bmArrowUp;
	CBitmap m_bmArrowBack;

	// Dyn-resize control support
	DynSizeCtrlArray m_dynSizeArray;

	// File item types
	AFileItemType* m_aSupportedTypes[AFileItemType::AFI_NUM];
	// File item caches in current directory
	APtrArray<AFileItem*> m_aFileItems;
	// File item image caches in current directory
	CImageList m_FileItemImages;

	CEvent m_DestroyEvent;

#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
	A3DDevice* m_pDevice;
#endif


protected:	//	Operations


	bool FormatInitDir(const TCHAR * szRelativeDir, const TCHAR * szInitSubDir);
	void InitFileFilters();
	void InitWndSize(int cx, int cy);
	void LoadResources();
	void CreateFileItemsForDir(const TCHAR * szDir);
	void ClearFileItems();
	void SetCurDir(const TCHAR* szDir, bool bLogHis = true);
	void UpdateOnCurDirChanged(const TCHAR * szCurDir);
	void UpdateListCtrlByFilter(const AFileItemType* pCurType);
	const AFileItemType* GetCurFilterType() const;
	int GetFilterCBIdxByType(int iType);
	void UpdatePathShortCut();
	void RecreateImageList();
	bool IgnoreItem(const TCHAR* szTitle);
	void ConstructInit(const TCHAR* szCustomExts);

	//	Preview Wnd related 
	bool IsPWndShow() const;
	bool IsAlphaBlendChecked() const;
	bool IsDisplayLod() const;
	int CreatePreviewWnd();
	void DestroyPreviewWnd();
	void AdjustPreviewWndPos();

	//	Play sound
	void TryPlaySound(const TCHAR* szFile);
	void StopSound();
	AFSoundPlayItem* m_pSound;

	//	Create a file dialog
	virtual INT_PTR DoModal();
	//	Get relative file path with out basedir and initdir.
	virtual const TCHAR* GetRelativeFileName();
	//	Get full file path name
	virtual const TCHAR* GetFullFileName() const;
	//	Get selected files
	virtual const AFVector<CString> GetSelFiles() const;
	//	On init dialog
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL DestroyWindow();
	

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT lpRect);
	afx_msg void OnSelchangeCbFiletypes();
	afx_msg void OnSelchangeCbPathShortcut();
	afx_msg LRESULT OnFolderProcCompleted(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateOneImage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateFirstFolder(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDbkSelectFile(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBtBack();
	afx_msg void OnBtUp();
	afx_msg void OnOpenFolder();
	afx_msg void OnBnClickedIsAlphaBlend();
	afx_msg void OnBnClickedIsWindowShow();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedDisplayLod();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AFileDialog2_H_


