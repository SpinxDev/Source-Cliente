#if !defined(AFX_PROPERTYFORM_H__9E684573_7B02_4527_9645_A2A5A009CDBD__INCLUDED_)
#define AFX_PROPERTYFORM_H__9E684573_7B02_4527_9645_A2A5A009CDBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyForm.h : header file
//

#include <A3DShader.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Index in image list
#define	IMG_GENERAL			0
#define IMG_SELECTED		1
#define IMG_UNSELECTED		2
#define IMG_TREELINE		3
#define IMG_TCMODS			4

//	Node ID
#define ND_GENERALPROPS		0x00000000		//	General properties
#define ND_STAGE0PROPS		0x80000000		//	Stage 0 properties
#define ND_TCMODS			0xC0000000		//	TC modifies

#define MAKESTAGENODEID(iStage)		(ND_STAGE0PROPS + ((iStage) << 16))
#define GETSTAGEFROMNODEID(id)		(((id) & ~ND_STAGE0PROPS) >> 16)

//	General property nodes
enum
{
	GND_ZFUNC = ND_GENERALPROPS | 1,	//	Z compare function
	GND_ZENABLE,		//	Z compare enable
	GND_ZWENABLE,		//	Z write enable
	GND_ALPHACOMP,		//	alpha compare function
	GND_CULLMODE,		//	cull mode
	GND_BLEND,			//	blend operation
	GND_COLORGEN,		//	color generate
	GND_ALPHAGEN,		//	alpha generate
	GND_PIXELSHADER,	//	pixel shader
	GND_TFACTOR,		//	texture factor
	GND_ALPHATEX,		//	alpha texture flag
};

//	Stage property nodes
enum
{
	SND_FILTER = ND_STAGE0PROPS | 1,	//	magnification and minification filter 
	SND_ADDRESS,		//	U address method
	SND_COLOROP,		//	color operation
	SND_ALPHAOP,		//	alpha operation
	SND_COLORGEN,		//	color generate
	SND_ALPHAGEN,		//	alpha generate
	SND_ALPHACOMP,		//	alpha compare function
	SND_BUMPMAT,		//	bump matrix
	SND_BUMPSCALE,		//	bump scale factor
	SND_BUMPOFFSET,		//	bump offset factor
	SND_TEXTURE,		//	texture
	SND_COLORARG1,		//	color argument1
	SND_COLORARG2,		//	color argument2
	SND_ALPHAARG1,		//	alpha argument1
	SND_ALPHAARG2,		//	alpha argument2
	SND_RESULTARG,		//	Result argument
	SND_TEXCOORD,		//	Texture coordinate index
	SND_TEXTRANS,		//	Texture transform
};

//	TC modifies
enum
{
	TCND_SCALE = ND_TCMODS | 1,		//	TC scale
	TCND_SCROLL,		//	TC scroll
	TCND_ROTATE,		//	TC rotate
	TCND_STRETCH,		//	TC stretch
	TCND_TURB,			//	TC turbulence
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	CPropertyForm form view
//
///////////////////////////////////////////////////////////////////////////

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CPropertyForm : public CFormView
{
public:
	CPropertyForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPropertyForm)

// Form Data
public:
	//{{AFX_DATA(CPropertyForm)
	enum { IDD = IDD_PROPERTYFORM };
	CTreeCtrl	m_PropTree;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

	bool	ChangeShader(SHADERINFO* pShaderInfo);	//	Change current shader
	bool	Initialize();			//	Initialize object
	void	Release();				//	Release all resources

	bool	InsertStage(HTREEITEM hPreNode);	//	Insert a stage

	//	Update texture alpha and color operations
	void UpdateTextureOps(bool bAlpha, int iStage, SHADERSTAGE& Stage);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPropertyForm)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRclickProptree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGenZcompare();
	afx_msg void OnGenCullmode();
	afx_msg void OnGenZenable();
	afx_msg void OnGenZwrite();
	afx_msg void OnGenAlphacompare();
	afx_msg void OnGenBlend();
	afx_msg void OnGpropEdit();
	afx_msg void OnGpropRemove();
	afx_msg void OnStgHide();
	afx_msg void OnStgAddbefore();
	afx_msg void OnStgAddafter();
	afx_msg void OnStgMoveup();
	afx_msg void OnStgMovedown();
	afx_msg void OnStgTexture();
	afx_msg void OnStgBumpmat();
	afx_msg void OnStgBumpoffset();
	afx_msg void OnStgBumpscale();
	afx_msg void OnSpropEdit();
	afx_msg void OnStgFilter();
	afx_msg void OnStgAddress();
	afx_msg void OnStgAlphaop();
	afx_msg void OnStgColorop();
	afx_msg void OnSpropRemove();
	afx_msg void OnStgRemove();
	afx_msg void OnGenCopy();
	afx_msg void OnGenPaste();
	afx_msg void OnDblclkProptree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGenColorgen();
	afx_msg void OnGenAlphagen();
	afx_msg void OnStgTcscale();
	afx_msg void OnStgTcscroll();
	afx_msg void OnStgTcrotate();
	afx_msg void OnStgTcstretch();
	afx_msg void OnTcmpEdit();
	afx_msg void OnTcmpRemove();
	afx_msg void OnTcmpMoveup();
	afx_msg void OnTcmpMovedown();
	afx_msg void OnStgTcturb();
	afx_msg void OnGenPixelshader();
	afx_msg void OnStgCopy();
	afx_msg void OnStgCut();
	afx_msg void OnStgPaste();
	afx_msg void OnStgResultarg();
	afx_msg void OnGenTfactor();
	afx_msg void OnStgTexcoord();
	afx_msg void OnStgTextransform();
	afx_msg void OnGenAlphatex();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attributes
protected:

	SHADERINFO*	m_pCurShader;			//	Current shader's information
	CImageList	m_PropImageList;		//	Property image list

	HTREEITEM	m_hCurItem;				//	Currently selected tree item

	HMENU		m_hStageMenu;			//	IDR_STAGEMENU handle
	HMENU		m_hStagePropMenu;		//	IDR_STGPROPMENU handle
	HMENU		m_hGeneralMenu;			//	IDR_GENERALMENU handle
	HMENU		m_hGenPropMenu;			//	IDR_GENPROPMENU handle
	HMENU		m_hTCModMenu;			//	IDR_TCMODMENU handle
	HMENU		m_hTCMPropMenu;			//	IDR_TCMODPROPMENU handle

// Operations
protected:

	void		ClearWholeTree();			//	Clear whole tree
	DWORD		NodeIDToFlag(DWORD dwID);	//	Convert node ID to corresponding shader mask flag

	void		CreateGeneralProps(HTREEITEM hGeneral, SHADERINFO* pShaderInfo);			//	Create general property tree
	void		CreateOneGenProp(HTREEITEM hGeneral, DWORD dwID, HTREEITEM hNode, A3DShader* pShader);
	void		CreateStageProps(HTREEITEM hStage, SHADERINFO* pShaderInfo, int iStage);	//	Create one stage property tree
	void		CreateOneStgProp(HTREEITEM hStage, DWORD dwID, HTREEITEM hNode, SHADERSTAGE& Stage);
	HTREEITEM	CreatePropertyNode(DWORD dwID, char* szText, HTREEITEM hParent);	//	Create a property node
	void		CreateTCModifies(HTREEITEM hTCMod, SHADERSTAGE& Stage);		//	Create TC modifies nodes

	void		UpdateGeneralProp(DWORD dwID, SHADERGENERAL& General);			//	Update one general property
	void		UpdateStageProp(int iStage, DWORD dwID, SHADERSTAGE& Stage);	//	Update one property in specified stage

	void		DoStgTexture(int iStage);		//	Set stage's texture
	void		DoStgFilter(int iStage);		//	Set texture filter mode
	void		DoStgAddress(int iStage);		//	Set texture address mode
	void		DoStgBumpMat(int iStage);		//	Set bump-mapping matrix
	void		DoStgBumpOffset(int iStage);	//	Set bump-mapping offset value
	void		DoStgBumpScale(int iStage);		//	Set bump-mapping scale value
	void		DoStgColorOp(int iStage);		//	Set color operation
	void		DoStgAlphaOp(int iStage);		//	Set alpha operation
	void		DoStgResultArg(int iStage);		//	Set result argument
	void		DoStgTexcoord(int iStage);		//	Set texture coordinates index
	void		DoStgTextransform(int iStage);	//	Set texture transform matrix
	void		DoStgTCScale(int iStage);		//	Set tc scale
	void		DoStgTCScroll(int iStage);		//	Set tc scroll
	void		DoStgTCRotate(int iStage);		//	Set tc rotate
	void		DoStgTCStretch(int iStage);		//	Set stretch
	void		DoStgTCTurb(int iStage);		//	Set stretch
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYFORM_H__9E684573_7B02_4527_9645_A2A5A009CDBD__INCLUDED_)
