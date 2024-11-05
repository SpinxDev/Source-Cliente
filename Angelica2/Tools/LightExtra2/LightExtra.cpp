/**********************************************************************
 *<
	FILE: LightExtra.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "LightExtra.h"
#include "notify.h"	 
#include "stdmat.h"
#include "evalcol.h"
#include "Engine3DWindow.h"
#include "LightExtra3DWindow.h"

#include <io.h>
						   
#define LIGHTEXTRA_CLASS_ID	Class_ID(0x29e5043c, 0x34447278)

typedef struct _LIGHTEXTRA_OPTION
{
	int			nPreset;		// preset index
	bool		bUseMaps;		// true for use texture maps
	bool		bUseShadow;		// true for not create ray traced shadows;
	bool		bDefaultLight;	// true for automatically add 2 default lights
	bool		bUseBlur;		// true when we want the texture to be blured to avoid aliasing
	bool		bCreateSky;		// true when sky should be display
	bool		bVertexColorShading; // true when using vertex color shading for preview
	bool		bSmoothGrid;	// true when we need to sample 9 point at each light grid point to make it more smooth
	int			nBlurTime;		// how many times blur iteration
	float		f00, f01, f02;	// Laplase coefficience
	float		f10, f11, f12;
	float		f20, f21, f22;
	float		vStep;			// step size of sample
	float		vGridSize;		// grid size of the light grid
	float		vGridDim;		// the dim factor for light grid sample point
	int			nLMWidth;		// width of lightmaps
	int			nLMHeight;		// height of lightmaps

	A3DIBLLIGHTRANGE lightRange; // light range flag indicate whether we should use MODULATE, MODULATE2X, MODULATE4X when rendering

} LIGHTEXTRA_OPTION;

class LightExtra : public UtilityObj {
	public:
		HWND			hPanel;
		IUtil			*iu;
		Interface		*ip;
		ICustEdit		* pEditBlurTime;
		ICustEdit		* pBlurF00;
		ICustEdit		* pBlurF01;
		ICustEdit		* pBlurF02;
		ICustEdit		* pBlurF10;
		ICustEdit		* pBlurF11;
		ICustEdit		* pBlurF12;
		ICustEdit		* pBlurF20;
		ICustEdit		* pBlurF21;
		ICustEdit		* pBlurF22;
		ICustEdit		* pStep;
		ICustEdit		* pGridSize;
		ICustEdit		* pGridDim;

		LIGHTEXTRA_OPTION option;
		
		void BeginEditParams(Interface *ip,IUtil *iu);
		void EndEditParams(Interface *ip,IUtil *iu);

		void Init(HWND hWnd);
		void Destroy(HWND hWnd);
		

		void DeleteThis() { }		
		//Constructor/Destructor

		LightExtra();
		~LightExtra();		

		bool MessageTick();

		int  m_nTotalFaceCount;
		float m_vTotalFaceArea;

		int		m_nLastX;
		int		m_nLastY;

		LPBYTE 	m_pCurLightImage;
		HRGN	m_hrgnCurLightImage;
		int		m_nImgSN;
		int		m_nImgWidth;
		int		m_nImgHeight;
		char	m_szBaseMapName[MAX_PATH];

		bool	m_bCopyMaps;
		char	m_szDestFolder[MAX_PATH];
		char	m_szDestFile[MAX_PATH];
		
		bool SortSelectedNodesByMaps(Tab<INode *>& tabNodes);

		void GetLightMapName(char * szLightMap, int nImgSN);
		bool PrepareCurLightImage();
		bool CloseCurLightImage();

		int  CalculateSelFaceCount();
		float CalculateFaceArea(Mesh * pMesh, Matrix3 tm, int nFaceIndex);
		float CalculateMeshArea(Mesh * pMesh, Matrix3 tm);

		bool DetermineFaceLayout(Mesh * pMesh, Matrix3 tm, int nFaceIndex, float vStep, int * pnWidth, int * pnHeight, int * pnX, int * pnY, int * pnVMap0, int * pnVMap1, int * pnvMap2);
		bool DetermineBestNF(Mesh * pMesh, Matrix3 tm, const BitArray& faceShaded, int nFaceIndex, int nVMap0, int nVMap1, int nVMap2, int * pnBestNF, int * pnVMap0, int * pnVMap1, int * pnVMap2);
		bool AddTriIBLMeshFace(Mesh * pMesh, Matrix3 tm, Matrix3 tmTexture, bool bHasUV, LPBYTE pVertexColors, bool bFlip1, bool bFlip2, int nFaceIndex, int nVMap[3], bool bBelowHalf, A3DIBLTriSubMesh * pNewTriSubIBLMesh, A3DIBLScene * pIBLScene, float u0, float u1, float v0, float v1, const A3DMESHPROP& MeshProps);
		bool AddRectIBLMeshFace(Mesh * pMesh, Matrix3 tm, Matrix3 tmTexture, bool bHasUV, LPBYTE pVertexColors, bool bFlip1, bool bFlip2, int nFaceIndex1, int nFaceIndex2, int nVMap1[3], int nVMap2[3], A3DIBLTriSubMesh * pNewTriSubIBLMesh, A3DIBLScene * pIBLScene, float u0, float u1, float v0, float v1, const A3DMESHPROP& MeshProps);
		
		bool ShadeIBLMeshByTriangle(NodeShador& nodeShador, A3DIBLMesh * pIBLMesh, A3DIBLScene * pIBLScene, int nCurFaceCount, float vCurFaceArea, float vNodeArea, Mesh * pMesh, Matrix3 tm, Matrix3 tmTexture, bool bHasUV, bool bFlip1, bool bFlip2, const A3DMESHPROP& MeshProps);
		bool ShadeIBLMeshByBoundingBox(NodeShador& nodeShador, A3DIBLMesh * pIBLMesh, A3DIBLScene * pIBLScene, int nCurFaceCount, float vCurFaceArea, float vNodeArea, Mesh * pMesh, Matrix3 tm, Matrix3 tmTexture, bool bHasUV, bool bFlip1, bool bFlip2);
		bool ShadeLMesh(NodeShador& nodeShador, A3DIBLMesh * pIBLMesh, A3DIBLScene * pIBLScene, int nCurFaceCount, float vCurFaceArea, float vNodeArea, Mesh * pMesh, Matrix3 tm, Matrix3 tmTexture, bool bHasUV, bool bFlip1, bool bFlip2);

		bool ShadeSceneLightGrid(A3DDevice * pA3DDevice, NodeShador& nodeShador, A3DIBLScene * pIBLScene);

		bool ConvertNodeToIBL(A3DDevice * pA3DDevice, INode * pNode, int nCurFaceCount, float vCurFaceArea, NodeShador& nodeShador, A3DIBLMesh ** ppNewIBLMesh, A3DIBLScene * pIBLScene, float * pvNodeArea);

		bool SetupPresetOptions(int nPreset);
		bool SetOptionsToUI(); // set options to UI;
		bool UpdateOptions();

		bool ExportSelectedIBL();
		bool ShowSelectedIBL();

		bool AddSelNodesToIBLScene(A3DDevice * pA3DDevice, A3DIBLScene * pIBLScene);
		bool ExportIBLScene();
		bool ShowIBLScene();
		
		bool ExportSelectedIBLBB();
		// Use a Laplase conersion calculating the bluring image
		bool Blur(LPBYTE pOrgImage, int nWidth, int nHeight, RECT * pSubRect, float f00, float f01, float f02,
			float f10, float f11, float f12, float f20, float f21, float f22, bool bKeepEdge=false);

		//	Get mesh property of node
		void GetMeshProps(INode* pNode, A3DMESHPROP& MeshProps);
};

static LightExtra theLightExtra;

class LightExtraClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return &theLightExtra; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return LIGHTEXTRA_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("LightExtra2"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



static LightExtraClassDesc LightExtraDesc;
ClassDesc2* GetLightExtraDesc() { return &LightExtraDesc; }


static BOOL CALLBACK LightExtraDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			theLightExtra.Init(hWnd);
			break;

		case WM_DESTROY:
			theLightExtra.Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_BUTTONEXPORT:
				theLightExtra.UpdateOptions();

				if( theLightExtra.ip->GetSelNodeCount() == 0 )
				{
					MessageBox(hWnd, "None node has been selected!", "ABORT", MB_ICONINFORMATION | MB_OK);
					return TRUE;
				}
				theLightExtra.ExportSelectedIBL();
				break;

			case IDC_BUTTONEXPORTBB:
				theLightExtra.UpdateOptions();

				if( theLightExtra.ip->GetSelNodeCount() == 0 )
				{
					MessageBox(hWnd, "None node has been selected!", "ABORT", MB_ICONINFORMATION | MB_OK);
					return TRUE;
				}
				theLightExtra.ExportSelectedIBLBB();
				break;

			case IDC_BUTTONSHOW:
				theLightExtra.UpdateOptions();

				if( theLightExtra.ip->GetSelNodeCount() == 0 )
				{
					MessageBox(hWnd, "None node has been selected!", "ABORT", MB_ICONINFORMATION | MB_OK);
					return TRUE;
				}
				theLightExtra.ShowSelectedIBL();
				break;

			case IDC_BUTTONEXPORTIBLSCENE:
				theLightExtra.UpdateOptions();

				if( theLightExtra.ip->GetSelNodeCount() == 0 )
				{
					MessageBox(hWnd, "None node has been selected!", "ABORT", MB_ICONINFORMATION | MB_OK);
					return TRUE;
				}
				theLightExtra.ExportIBLScene();
				break;
				
			case IDC_BUTTONIBLSCENE:
				theLightExtra.UpdateOptions();

				if( theLightExtra.ip->GetSelNodeCount() == 0 )
				{
					MessageBox(hWnd, "None node has been selected!", "ABORT", MB_ICONINFORMATION | MB_OK);
					return TRUE;
				}
				theLightExtra.ShowIBLScene();
				break;

			case IDC_COMBOPRESET:
				if( HIWORD(wParam) == CBN_SELENDOK )
				{
					theLightExtra.SetupPresetOptions(SendMessage(GetDlgItem(hWnd, IDC_COMBOPRESET), CB_GETCURSEL, 0, 0));
					theLightExtra.SetOptionsToUI();
				}
				break;
			case IDC_RADIO1X:
			case IDC_RADIO2X:
			case IDC_RADIO4X:
			case IDC_CHECKSKY:
			case IDC_CHECKVC:
			case IDC_COMBOSIZE:
			case IDC_EDITBLURTIME:
			case IDC_EDITF00:
			case IDC_EDITF01:
			case IDC_EDITF02:
			case IDC_EDITF10:
			case IDC_EDITF11:
			case IDC_EDITF12:
			case IDC_EDITF20:
			case IDC_EDITF21:
			case IDC_EDITF22:
			case IDC_EDITSTEP:
			case IDC_EDITGRIDSIZE:
			case IDC_EDITGRIDDIM:
			case IDC_CHECKGRIDSMOOTH:
			case IDC_CHECKUSEMAPS:
			case IDC_CHECKUSESHADOW:
			case IDC_CHECKDEFAULTLIGHT:
			case IDC_CHECKUSEBLUR:
				theLightExtra.UpdateOptions();
				break;
			}
			break;
		case WM_CUSTEDIT_ENTER:
		case WM_KEYDOWN:
			if( wParam == VK_RETURN )
				theLightExtra.UpdateOptions();
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			theLightExtra.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}

	return TRUE;
}



//--- LightExtra -------------------------------------------------------
LightExtra::LightExtra()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
	m_bCopyMaps = false;
	SetupPresetOptions(0);
}

LightExtra::~LightExtra()
{

}

void LightExtra::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		LightExtraDlgProc,
		GetString(IDS_PARAMS),
		0);

	pEditBlurTime = GetICustEdit(GetDlgItem(hPanel, IDC_EDITBLURTIME));
	pBlurF00 = GetICustEdit(GetDlgItem(hPanel, IDC_EDITF00));
	pBlurF01 = GetICustEdit(GetDlgItem(hPanel, IDC_EDITF01));
	pBlurF02 = GetICustEdit(GetDlgItem(hPanel, IDC_EDITF02));

	pBlurF10 = GetICustEdit(GetDlgItem(hPanel, IDC_EDITF10));
	pBlurF11 = GetICustEdit(GetDlgItem(hPanel, IDC_EDITF11));
	pBlurF12 = GetICustEdit(GetDlgItem(hPanel, IDC_EDITF12));

	pBlurF20 = GetICustEdit(GetDlgItem(hPanel, IDC_EDITF20));
	pBlurF21 = GetICustEdit(GetDlgItem(hPanel, IDC_EDITF21));
	pBlurF22 = GetICustEdit(GetDlgItem(hPanel, IDC_EDITF22));

	pStep    = GetICustEdit(GetDlgItem(hPanel, IDC_EDITSTEP));
	pGridSize= GetICustEdit(GetDlgItem(hPanel, IDC_EDITGRIDSIZE));
	pGridDim = GetICustEdit(GetDlgItem(hPanel, IDC_EDITGRIDDIM));
	
	SendMessage(GetDlgItem(hPanel, IDC_COMBOPRESET), CB_ADDSTRING, 0, (LPARAM)"Max Scanline");
	SendMessage(GetDlgItem(hPanel, IDC_COMBOPRESET), CB_ADDSTRING, 0, (LPARAM)"FinalRender");
	SendMessage(GetDlgItem(hPanel, IDC_COMBOPRESET), CB_ADDSTRING, 0, (LPARAM)"Lightscape");

	SendMessage(GetDlgItem(hPanel, IDC_COMBOSIZE), CB_ADDSTRING, 0, (LPARAM)"256 * 256");
	SendMessage(GetDlgItem(hPanel, IDC_COMBOSIZE), CB_ADDSTRING, 0, (LPARAM)"512 * 512");

	SetOptionsToUI();
}
	
void LightExtra::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	this->ip = NULL;

	ReleaseICustEdit(pEditBlurTime);
	ReleaseICustEdit(pBlurF00);
	ReleaseICustEdit(pBlurF01);
	ReleaseICustEdit(pBlurF02);
	ReleaseICustEdit(pBlurF10);
	ReleaseICustEdit(pBlurF11);
	ReleaseICustEdit(pBlurF12);
	ReleaseICustEdit(pBlurF20);
	ReleaseICustEdit(pBlurF21);
	ReleaseICustEdit(pBlurF22);
	ReleaseICustEdit(pStep);
	ReleaseICustEdit(pGridSize);
	ReleaseICustEdit(pGridDim);
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}

void LightExtra::Init(HWND hWnd)
{
}

void LightExtra::Destroy(HWND hWnd)
{

}

bool LightExtra::SetupPresetOptions(int nPreset)
{
	switch(nPreset)
	{
		case 0:
			// Normal Scanline
			option.nPreset = 0;
			option.bUseMaps = false;
			option.bUseShadow = true;
			option.bDefaultLight = false;
			option.bUseBlur = false;
			option.nBlurTime = 2;
			option.f00 = 0.0f; option.f01 = 1.0f; option.f02 = 0.0f;
			option.f10 = 1.0f; option.f11 = 2.0f; option.f12 = 1.0f;
			option.f20 = 0.0f; option.f21 = 1.0f; option.f22 = 0.0f;
			option.vStep = 0.5f;
			option.vGridSize = 2.5f;
			option.vGridDim = 1.0f;
			option.nLMWidth = option.nLMHeight = 256;
			option.lightRange = A3DIBLLR_1X;
			option.bCreateSky = false;
			option.bVertexColorShading = false;
			option.bSmoothGrid = false;
			break;
		case 1:
			// FinalRender
			option.nPreset = 1;
			option.bUseMaps = false;
			option.bUseShadow = true;
			option.bDefaultLight = false;
			option.bUseBlur = false;
			option.nBlurTime = 2;
			option.f00 = 0.0f; option.f01 = 1.0f; option.f02 = 0.0f;
			option.f10 = 1.0f; option.f11 = 4.0f; option.f12 = 1.0f;
			option.f20 = 0.0f; option.f21 = 1.0f; option.f22 = 0.0f;
			option.vStep = 0.5f;
			option.vGridSize = 2.5f;
			option.vGridDim = 1.0f;
			option.nLMWidth = option.nLMHeight = 256;
			option.lightRange = A3DIBLLR_1X;
			option.bCreateSky = false;
			option.bVertexColorShading = false;
			option.bSmoothGrid = false;
			break;
		case 2:
			// Lightscape
			option.nPreset = 2;
			option.bUseMaps = false;
			option.bUseShadow = false;
			option.bDefaultLight = false;
			option.bUseBlur = false;
			option.nBlurTime = 1;
			option.f00 = 0.0f; option.f01 = 1.0f; option.f02 = 0.0f;
			option.f10 = 1.0f; option.f11 = 4.0f; option.f12 = 1.0f;
			option.f20 = 0.0f; option.f21 = 1.0f; option.f22 = 0.0f;
			option.vStep = 0.5f;
			option.vGridSize = 2.5f;
			option.vGridDim = 1.0f;
			option.nLMWidth = option.nLMHeight = 256;
			option.lightRange = A3DIBLLR_1X;
			option.bCreateSky = false;
			option.bVertexColorShading = false;
			option.bSmoothGrid = false;
			break;
	}

	return true;
}

bool LightExtra::SetOptionsToUI()
{
	SendMessage(GetDlgItem(hPanel, IDC_COMBOPRESET), CB_SETCURSEL, option.nPreset, 0);

	if( option.bUseMaps	)
		SendMessage(GetDlgItem(hPanel, IDC_CHECKUSEMAPS), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(GetDlgItem(hPanel, IDC_CHECKUSEMAPS), BM_SETCHECK, BST_UNCHECKED, 0);

	if( option.bUseShadow )
		SendMessage(GetDlgItem(hPanel, IDC_CHECKUSESHADOW), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(GetDlgItem(hPanel, IDC_CHECKUSESHADOW), BM_SETCHECK, BST_UNCHECKED, 0);

	if( option.bDefaultLight )
		SendMessage(GetDlgItem(hPanel, IDC_CHECKDEFAULTLIGHT), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(GetDlgItem(hPanel, IDC_CHECKDEFAULTLIGHT), BM_SETCHECK, BST_UNCHECKED, 0);

	if( option.bUseBlur )
		SendMessage(GetDlgItem(hPanel, IDC_CHECKUSEBLUR), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(GetDlgItem(hPanel, IDC_CHECKUSEBLUR), BM_SETCHECK, BST_UNCHECKED, 0);

	if( option.bCreateSky )
		SendMessage(GetDlgItem(hPanel, IDC_CHECKSKY), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(GetDlgItem(hPanel, IDC_CHECKSKY), BM_SETCHECK, BST_UNCHECKED, 0);

	if( option.bVertexColorShading )
		SendMessage(GetDlgItem(hPanel, IDC_CHECKVC), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(GetDlgItem(hPanel, IDC_CHECKVC), BM_SETCHECK, BST_UNCHECKED, 0);

	if( option.bSmoothGrid )
		SendMessage(GetDlgItem(hPanel, IDC_CHECKGRIDSMOOTH), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(GetDlgItem(hPanel, IDC_CHECKGRIDSMOOTH), BM_SETCHECK, BST_UNCHECKED, 0);

	char szText[256];
	sprintf(szText, "%d", option.nBlurTime);
	pEditBlurTime->SetText(szText);
	
	sprintf(szText, "%3.1f", option.f00);
	pBlurF00->SetText(szText);
	sprintf(szText, "%3.1f", option.f01);
	pBlurF01->SetText(szText);
	sprintf(szText, "%3.1f", option.f02);
	pBlurF02->SetText(szText);
	sprintf(szText, "%3.1f", option.f10);
	pBlurF10->SetText(szText);
	sprintf(szText, "%3.1f", option.f11);
	pBlurF11->SetText(szText);
	sprintf(szText, "%3.1f", option.f12);
	pBlurF12->SetText(szText);
	sprintf(szText, "%3.1f", option.f20);
	pBlurF20->SetText(szText);
	sprintf(szText, "%3.1f", option.f21);
	pBlurF21->SetText(szText);
	sprintf(szText, "%3.1f", option.f22);
	pBlurF22->SetText(szText);

	sprintf(szText, "%3.1f", option.vStep);
	pStep->SetText(szText);

	sprintf(szText, "%3.1f", option.vGridSize);
	pGridSize->SetText(szText);

	sprintf(szText, "%3.1f", option.vGridDim);
	pGridDim->SetText(szText);

	if( option.nLMWidth == 256 && option.nLMHeight == 256 )
	{
		SendMessage(GetDlgItem(hPanel, IDC_COMBOSIZE), CB_SETCURSEL, 0, 0);
	}
	else // 512 * 512
	{
		SendMessage(GetDlgItem(hPanel, IDC_COMBOSIZE), CB_SETCURSEL, 1, 0);
	}

	if( option.lightRange == A3DIBLLR_1X )
		SendMessage(GetDlgItem(hPanel, IDC_RADIO1X), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(GetDlgItem(hPanel, IDC_RADIO1X), BM_SETCHECK, BST_UNCHECKED, 0);

	if( option.lightRange == A3DIBLLR_2X )
		SendMessage(GetDlgItem(hPanel, IDC_RADIO2X), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(GetDlgItem(hPanel, IDC_RADIO2X), BM_SETCHECK, BST_UNCHECKED, 0);

	if( option.lightRange == A3DIBLLR_4X )
		SendMessage(GetDlgItem(hPanel, IDC_RADIO4X), BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(GetDlgItem(hPanel, IDC_RADIO4X), BM_SETCHECK, BST_UNCHECKED, 0);

	return true;
}

bool LightExtra::UpdateOptions()
{
	option.bUseMaps			= SendMessage(GetDlgItem(hPanel, IDC_CHECKUSEMAPS), BM_GETCHECK, 0, 0) ? true : false;
	option.bUseShadow		= SendMessage(GetDlgItem(hPanel, IDC_CHECKUSESHADOW), BM_GETCHECK, 0, 0) ? true : false;
	option.bDefaultLight	= SendMessage(GetDlgItem(hPanel, IDC_CHECKDEFAULTLIGHT), BM_GETCHECK, 0, 0) ? true : false;
	option.bUseBlur			= SendMessage(GetDlgItem(hPanel, IDC_CHECKUSEBLUR), BM_GETCHECK, 0, 0) ? true : false;
	option.bCreateSky		= SendMessage(GetDlgItem(hPanel, IDC_CHECKSKY), BM_GETCHECK, 0, 0) ? true : false;
	option.bVertexColorShading = SendMessage(GetDlgItem(hPanel, IDC_CHECKVC), BM_GETCHECK, 0, 0) ? true : false;
	option.bSmoothGrid	    = SendMessage(GetDlgItem(hPanel, IDC_CHECKGRIDSMOOTH), BM_GETCHECK, 0, 0) ? true : false;
	
	option.nBlurTime = pEditBlurTime->GetInt();
	option.f00 = pBlurF00->GetFloat();
	option.f01 = pBlurF01->GetFloat();
	option.f02 = pBlurF02->GetFloat();
	option.f10 = pBlurF10->GetFloat();
	option.f11 = pBlurF11->GetFloat();
	option.f12 = pBlurF12->GetFloat();
	option.f20 = pBlurF20->GetFloat();
	option.f21 = pBlurF21->GetFloat();
	option.f22 = pBlurF22->GetFloat();
	option.vStep = pStep->GetFloat();
	option.vGridSize = pGridSize->GetFloat();
	option.vGridDim  = pGridDim->GetFloat();

	if( 0 == SendMessage(GetDlgItem(hPanel, IDC_COMBOSIZE), CB_GETCURSEL, 0, 0) )
	{
		option.nLMWidth = option.nLMHeight = 256;
	}
	else
	{
		option.nLMWidth = option.nLMHeight = 512;
	}

	if( SendMessage(GetDlgItem(hPanel, IDC_RADIO1X), BM_GETCHECK, 0, 0) )
		option.lightRange = A3DIBLLR_1X;
	else if( SendMessage(GetDlgItem(hPanel, IDC_RADIO2X), BM_GETCHECK, 0, 0) )
		option.lightRange = A3DIBLLR_2X;
	else if( SendMessage(GetDlgItem(hPanel, IDC_RADIO4X), BM_GETCHECK, 0, 0) )
		option.lightRange = A3DIBLLR_4X;

	return true;
}

bool WriteBitmap(char * szFilename, int nWidth, int nHeight, LPBYTE pImage)
{
	BITMAPINFOHEADER	bmih;
	BITMAPFILEHEADER	bmfh;

	int nByteAlign;
	DWORD dwZero = 0;

	nByteAlign = ((nWidth * 3 + 3) & (~3)) - nWidth * 3;
	ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biBitCount = 24;
	bmih.biWidth = nWidth;
	bmih.biHeight = nHeight;
	bmih.biPlanes = 1;
	bmih.biCompression = BI_RGB;

	FILE * file = fopen(szFilename, "wb");
	if( NULL == file )
		return false;

	ZeroMemory(&bmfh, sizeof(bmfh));  
	bmfh.bfType = 0x4d42;
	bmfh.bfOffBits = sizeof(bmfh) + sizeof(bmih);
	bmfh.bfSize = bmfh.bfOffBits + (nWidth + nByteAlign) * nHeight;

	fwrite(&bmfh, sizeof(bmfh), 1, file);
	fwrite(&bmih, sizeof(bmih), 1, file);
	for(int y=0; y<nHeight; y++)
	{
		fwrite(pImage + (nHeight - 1 - y) * nWidth * 3, nWidth * 3, 1, file);
		if( nByteAlign )
			fwrite(&dwZero, nByteAlign, 1, file);
	}

	fclose(file);
	return true;
}

DWORD WINAPI dummyProgressStart(LPVOID arg)
{
    return(0);
}

/*
	compare to map's name
	if( szBestMap > szThisMap ) return 1;
	else if( szBestMap < szThisMap ) return -1;
	else return0;
*/
int MyMapSortFunction(char * szBestMap, char * szThisMap)
{
	int nBestType; // 0 - normal; 1 - sdr; 2 - tga alpha
	int nThisType;

	if( strstr(szBestMap, ".tga") ) // We have make map filename to lower case
	{
		nBestType = 2;
	}
	else if( strstr(szBestMap, ".sdr") )
	{
		nBestType = 1;
	}
	else
		nBestType = 0;

	if( strstr(szThisMap, ".tga") ) // We have make map filename to lower case
	{
		nThisType = 2;
	}
	else if( strstr(szThisMap, ".sdr") )
	{
		nThisType = 1;
	}
	else
		nThisType = 0;

	if( nBestType == nThisType )
		return strcmp(szBestMap, szThisMap);
	else
		return nBestType - nThisType;

	return -1;
}

bool LightExtra::SortSelectedNodesByMaps(Tab<INode *> &tabNodes)
{
	int			i;
	char		* szTextureMap;
	Tab<char *> tabNodesMaps;

	// We not only sort the maps accordint to its map name, but 
	// we will consider the file extension first, we will put all .tga file at the
	// latter part of the list;
	for(i=0; i<ip->GetSelNodeCount(); i++)
	{
		INode * pNode = ip->GetSelNode(i);
		tabNodes.Append(1, &pNode, 0);

		szTextureMap = new char[MAX_PATH];
		szTextureMap[0] = '\0';
		tabNodesMaps.Append(1, &szTextureMap, 0);

		ObjectState os = pNode->EvalWorldState(ip->GetTime());
		if( !os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
			continue;
		
		TriObject	*objTri;
		objTri = (TriObject *)os.obj->ConvertToType(ip->GetTime(), Class_ID(TRIOBJ_CLASS_ID, 0));
		if( !objTri )
			continue;

		// Get the texture name;
		Mtl * pMtl;
		pMtl = pNode->GetMtl();
		if( pMtl )
		{
			Mtl * pRealMtl = pMtl;
			Class_ID lid = pMtl->ClassID();

			if( pMtl->ClassID() == Class_ID(0x459a16ee, 0x27977f9c) ) // Lightscape mtl
			{
				// we just get the lightscape mtl's baseobject mtl
				pRealMtl = pMtl->GetSubMtl(0);
			}

			if( pRealMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0) )
			{
				StdMat * pStdMtl = (StdMat *) pRealMtl;
				TimeValue t = ip->GetTime();
				
				if( pStdMtl->SubTexmapOn(ID_DI) )
				{
					Texmap * pTexmap = pStdMtl->GetSubTexmap(ID_DI);
					if( pTexmap )
					{
						if( 0 == objTri->mesh.getNumTVerts() ) //If not uvw maping
						{
						}
						else
						{
							if( pTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
							{
								BitmapTex * pBitmapTex = (BitmapTex *)pTexmap;
								sprintf(szTextureMap, pBitmapTex->GetMapName());
								strlwr(szTextureMap);
								// Now we have to replace all slash '/' in filename to back slash '\'
								int nLen = strlen(szTextureMap);
								for(int i=0; i<nLen; i++)
								{
									if( szTextureMap[i] == '/' )
										szTextureMap[i] = '\\';
								}
							}
						}
					}
				}
			}
		}

		if( objTri != os.obj )
			objTri->DeleteMe();
	}

	// Now we can sort all selected nodes by its map name;
	for(i=0; i<tabNodes.Count() - 1; i++)
	{
		int		nBest = i;
		char *	szBestMaps = tabNodesMaps[i];
		for(int k=i+1; k<tabNodes.Count(); k++)
		{
			if( MyMapSortFunction(szBestMaps, tabNodesMaps[k]) > 0 )
			{
				nBest = k;
				szBestMaps = tabNodesMaps[k];
			}
		}
		if( nBest != i )
		{
			INode * pNodeTemp;
			pNodeTemp = tabNodes[i];
			tabNodes[i] = tabNodes[nBest];
			tabNodes[nBest] = pNodeTemp;

			char * szTemp;
			szTemp = tabNodesMaps[i];
			tabNodesMaps[i] = tabNodesMaps[nBest];
			tabNodesMaps[nBest] = szTemp;
		}
	}

	// Now release the texture maps tab
	for(i=0; i<tabNodesMaps.Count(); i++)
	{
		char * szTextureMap = tabNodesMaps[i];
		delete [] szTextureMap;
	}

	tabNodesMaps.ZeroCount();
	tabNodesMaps.Shrink();
	return true;
}

bool LightExtra::ExportSelectedIBL()
{
	NodeShadorOptions shadeOptions;
	ZeroMemory(&shadeOptions, sizeof(shadeOptions));
	shadeOptions.bUseMaps = option.bUseMaps;
	shadeOptions.bCastShadows = option.bUseShadow;
	shadeOptions.bDefaultLights = option.bDefaultLight;
	shadeOptions.bSmoothGrid = option.bSmoothGrid;
	shadeOptions.vScale = 1.0f;

	m_bCopyMaps = false;
	NodeShador nodeShador(shadeOptions);

	int nMapCount = 0;
	ip->ProgressStart(GetString(IDS_LIGHTEXTRA_PROGRESS), TRUE, dummyProgressStart, NULL);
	for(int i=0; i<ip->GetSelNodeCount(); i++)
	{
		// Update progress
		ip->ProgressUpdate(int(i * 100.0f / ip->GetSelNodeCount()));

		INode * pNode = ip->GetSelNode(i);

		if( !nodeShador.BeginShade(pNode, ip->GetTime()) )
			return false;

		for(int id=0; id<nodeShador.GetFaceCount(); id++)
		{
			LPBYTE pImage;
			LPBYTE pImageMono = NULL;
			int    nWidth;
			int	   nHeight;
			
			int nVMap[3];
			nodeShador.ShadeTriangle(id, &pImage, 1.0f, &nWidth, &nHeight, &nVMap[0], &nVMap[1], &nVMap[2]);

			// Do some blur operating;
			for(int l=0; l<option.nBlurTime; l++)
			{
				Blur(pImage, nWidth, nHeight, NULL,
					option.f00, option.f01, option.f02,
					option.f10, option.f11, option.f12,
					option.f20, option.f21, option.f22);
			}

			char szFilename[MAX_PATH];
			sprintf(szFilename, "c:\\temp\\face%05d.bmp", nMapCount);

			WriteBitmap(szFilename, nWidth, nHeight, pImage);

			a_free(pImage);

			nMapCount ++;
		}

		nodeShador.AfterShade();
		ip->ProgressUpdate(int((i + 1) * 100.0f / ip->GetSelNodeCount()));

		if( ip->GetCancel() )
			break;
	}

	ip->ProgressEnd();
	return true;
}

/*
	Create an IBL scene and save it to disk
*/
bool LightExtra::ExportIBLScene()
{
	if( ip->GetSelNodeCount() == 0 )
		return true;

	// First browse for dest files;
	char szFilename[MAX_PATH];
	char szPath[MAX_PATH];
	szFilename[0] = '\0';
	szPath[0] = '\0';

	OPENFILENAME	ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hPanel;
	ofn.hInstance = hInstance;
	ofn.lpstrFilter = "IBL Scene File(*.ils)\0*.ils\0";
	ofn.lpstrFile = szFilename;
	ofn.lpstrInitialDir = szPath;
	ofn.lpstrDefExt = ".ils";
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select the filename to be saved";
	ofn.Flags = OFN_OVERWRITEPROMPT;
	if( !GetSaveFileName(&ofn) )
	{
		return true;
	}

	af_GetFilePath(szFilename, szPath, MAX_PATH);
	strcpy(m_szDestFolder, szPath);
	if( m_szDestFolder[0] && m_szDestFolder[strlen(m_szDestFolder) - 1] == '\\' )
		m_szDestFolder[strlen(m_szDestFolder) - 1] = '\0';
	strcpy(m_szDestFile, szFilename);

	AFile fileToSave;
	EnableWindow(GetDlgItem(hPanel, IDC_BUTTONEXPORTIBLSCENE), FALSE);

	// Create 3D Engine;
	A3DConfig::SetRunEnv(A3DRUNENV_PURESERVER);
	A3DEngine * pA3DEngine = new A3DEngine();

	A3DDEVFMT devFmt;
	ZeroMemory(&devFmt, sizeof(devFmt));
	devFmt.bWindowed = true;
	devFmt.nWidth = 640;
	devFmt.nHeight = 480;

	if( !pA3DEngine->Init(hInstance, hPanel, &devFmt, A3DDEVTYPE_HAL) )
		goto Failure;
	
	A3DIBLScene * pIBLScene;
	pIBLScene = new A3DIBLScene();
	if( !pIBLScene->Init(pA3DEngine, false))
		goto Failure;

	pIBLScene->SetLightRange(option.lightRange);

	m_bCopyMaps = true;
	if( !AddSelNodesToIBLScene(pA3DEngine->GetA3DDevice(), pIBLScene) )
		goto Failure;

	if( !fileToSave.Open(m_szDestFile, AFILE_CREATENEW | AFILE_BINARY) )
		goto Failure;

	pIBLScene->Save(&fileToSave);
	
	fileToSave.Close();

	pIBLScene->Release();
	delete pIBLScene;
	
	EnableWindow(GetDlgItem(hPanel, IDC_BUTTONEXPORTIBLSCENE), TRUE);
	A3DConfig::SetRunEnv(A3DRUNENV_GAME);

	pA3DEngine->Release();
	delete pA3DEngine;

	return true;

Failure:

	pA3DEngine->Release();
	delete pA3DEngine;

	MessageBox(hPanel, "Can not create export file!", "FAIL", MB_ICONSTOP | MB_OK);
	EnableWindow(GetDlgItem(hPanel, IDC_BUTTONEXPORTIBLSCENE), TRUE);
	A3DConfig::SetRunEnv(A3DRUNENV_GAME);
	return false;
}

/*
	Create an IBL Scene from current selected nodes
*/
bool LightExtra::AddSelNodesToIBLScene(A3DDevice * pA3DDevice, A3DIBLScene * pIBLScene)
{
	// First calculate the face count;
	int nTotalFaces = CalculateSelFaceCount();
	if( nTotalFaces == 0 )
	{
		MessageBox(hPanel, "There are no faces can be seen!", "INFO", MB_ICONINFORMATION | MB_OK);
		return true;
	}

	// now fill in the data for that ibl mesh;
	NodeShadorOptions shadeOptions;
	ZeroMemory(&shadeOptions, sizeof(shadeOptions));
	shadeOptions.bUseMaps = option.bUseMaps;
	shadeOptions.bCastShadows = option.bUseShadow;
	shadeOptions.bDefaultLights = option.bDefaultLight;
	shadeOptions.bSmoothGrid = option.bSmoothGrid;
	switch(option.lightRange)
	{
	case A3DIBLLR_1X:
		shadeOptions.vScale = 1.0f;
		break;
	case A3DIBLLR_2X:
		shadeOptions.vScale = 0.5f;
		break;
	case A3DIBLLR_4X:
		shadeOptions.vScale = 0.25f;
		break;
	default:
		shadeOptions.vScale = 1.0f;
		break;
	}

	NodeShador nodeShador(shadeOptions);

	ip->ProgressStart(GetString(IDS_LIGHTEXTRA_PROGRESS), TRUE, dummyProgressStart, NULL);
	
	int nCurFaceCount = 0;
	float vCurFaceArea = 0.0f;

	m_nImgWidth = option.nLMWidth;
	m_nImgHeight = option.nLMHeight;
	m_nImgSN = 0;
	m_pCurLightImage = NULL;
	m_hrgnCurLightImage = NULL;
	m_nLastX = 0;
	m_nLastY = 0;

	// First get all nodes and sort them according to the diffuse maps;
	Tab<INode *>	tabNodes;
	SortSelectedNodesByMaps(tabNodes);
	
	for(int i=0; i<ip->GetSelNodeCount(); i++)
	{
		float			vNodeArea;
		INode			* pNode = tabNodes[i];

		if( !nodeShador.BeginShade(pNode, ip->GetTime()) )
			continue;

		if( !ConvertNodeToIBL(pA3DDevice, pNode, nCurFaceCount, vCurFaceArea, nodeShador, NULL, pIBLScene, &vNodeArea) )
			continue;

		char szMsg[100];
		sprintf(szMsg, "----- %d -----\n", i);
		OutputDebugString(szMsg);

		nCurFaceCount += nodeShador.GetFaceCount();
		vCurFaceArea  += vNodeArea;

		nodeShador.AfterShade();
	}

	tabNodes.ZeroCount();
	tabNodes.Shrink();
	// try to close last light image
	CloseCurLightImage();
	ip->ProgressUpdate(100);

	// Now shade scene light grid
	ip->ProgressStart(GetString(IDS_SHADELIGHTGRID), TRUE, dummyProgressStart, NULL);
	ip->ProgressUpdate(0);
	ShadeSceneLightGrid(pA3DDevice, nodeShador, pIBLScene);

	ip->ProgressEnd();
	return true;
}

/*
	convert the selected nodes to ibl scene and show it;
*/
bool LightExtra::ShowIBLScene()
{
	if( ip->GetSelNodeCount() == 0 )
		return true;

	EnableWindow(GetDlgItem(hPanel, IDC_BUTTONIBLSCENE), FALSE);

	// Create 3D window;
	LightExtra3DWindow	the3DWindow;
	HWND				h3DWnd;

	if( !the3DWindow.Init(hInstance, hPanel, &h3DWnd, true, option.bCreateSky) )
	{
		EnableWindow(GetDlgItem(hPanel, IDC_BUTTONIBLSCENE), TRUE);
		return false;
	}
			
	if( option.bVertexColorShading )
		g_pA3DConfig->SetFlagLightMapScene(false);
	else
		g_pA3DConfig->SetFlagLightMapScene(true);

	A3DIBLScene * pIBLScene;
	pIBLScene = new A3DIBLScene();
	if( !pIBLScene->Init(the3DWindow.GetA3DEngine(), false) )
	{
		EnableWindow(GetDlgItem(hPanel, IDC_BUTTONIBLSCENE), TRUE);
		return false;
	}
	pIBLScene->SetLightRange(option.lightRange);
	the3DWindow.SetIBLScene(pIBLScene);

	m_bCopyMaps = false;
	if( !AddSelNodesToIBLScene(the3DWindow.GetA3DDevice(), pIBLScene) )
	{
		EnableWindow(GetDlgItem(hPanel, IDC_BUTTONIBLSCENE), TRUE);
		return false;
	}

	the3DWindow.LoadAllLightMaps();
	
	the3DWindow.Run();
	
	pIBLScene->Release();
	delete pIBLScene;
	
	the3DWindow.Release();
	EnableWindow(GetDlgItem(hPanel, IDC_BUTTONIBLSCENE), TRUE);
	return true;
}

bool LightExtra::ShowSelectedIBL()
{
	if( ip->GetSelNodeCount() == 0 )
		return true;

	EnableWindow(GetDlgItem(hPanel, IDC_BUTTONSHOW), FALSE);

	// First calculate the face count;
	int nTotalFaces = CalculateSelFaceCount();
	if( nTotalFaces == 0 )
	{
		MessageBox(hPanel, "There are no faces can be seen!", "INFO", MB_ICONINFORMATION | MB_OK);
		EnableWindow(GetDlgItem(hPanel, IDC_BUTTONSHOW), FALSE);
		return true;
	}

	LightExtra3DWindow	the3DWindow;
	HWND				h3DWnd;

	if( !the3DWindow.Init(hInstance, hPanel, &h3DWnd, true, option.bCreateSky) )
	{
		EnableWindow(GetDlgItem(hPanel, IDC_BUTTONSHOW), TRUE);
		return false;
	}

	// now fill in the data for that ibl mesh;
	NodeShadorOptions shadeOptions;
	ZeroMemory(&shadeOptions, sizeof(shadeOptions));
	shadeOptions.bUseMaps = option.bUseMaps;
	shadeOptions.bCastShadows = option.bUseShadow;
	shadeOptions.bDefaultLights = option.bDefaultLight;
	shadeOptions.bSmoothGrid = option.bSmoothGrid;
	switch(option.lightRange)
	{
	case A3DIBLLR_1X:
		shadeOptions.vScale = 1.0f;
		break;
	case A3DIBLLR_2X:
		shadeOptions.vScale = 0.5f;
		break;
	case A3DIBLLR_4X:
		shadeOptions.vScale = 0.25f;
		break;
	default:
		shadeOptions.vScale = 1.0f;
		break;
	}

	m_bCopyMaps = false;
	NodeShador nodeShador(shadeOptions);

	ip->ProgressStart(GetString(IDS_LIGHTEXTRA_PROGRESS), TRUE, dummyProgressStart, NULL);
	
	int nCurFaceCount = 0;
	float vCurFaceArea = 0.0f;

	m_nImgWidth = option.nLMWidth;
	m_nImgHeight = option.nLMHeight;
	m_nImgSN = 0;
	m_pCurLightImage = NULL;
	m_hrgnCurLightImage = NULL;
	m_nLastX = 0;
	m_nLastY = 0;

	// First get all nodes and sort them according to the diffuse maps;
	Tab<INode *>	tabNodes;
	SortSelectedNodesByMaps(tabNodes);
	
	for(int i=0; i<ip->GetSelNodeCount(); i++)
	{
		A3DIBLMesh		* pNewIBLMesh;
		float			vNodeArea;
		INode			* pNode = ip->GetSelNode(i);

		pNode = tabNodes[i];

		if( !nodeShador.BeginShade(pNode, ip->GetTime()) )
			continue;

		if( ConvertNodeToIBL(the3DWindow.GetA3DDevice(), pNode, nCurFaceCount, vCurFaceArea, nodeShador, &pNewIBLMesh, NULL, &vNodeArea) )
			the3DWindow.AddIBLMesh(pNewIBLMesh);

		nCurFaceCount += nodeShador.GetFaceCount();
		vCurFaceArea  += vNodeArea;

		nodeShador.AfterShade();
	}

	tabNodes.ZeroCount();
	tabNodes.Shrink();
	// try to close last light image
	CloseCurLightImage();

	ip->ProgressEnd();

	the3DWindow.LoadAllLightMaps();

	the3DWindow.Run();

	the3DWindow.Release();
	EnableWindow(GetDlgItem(hPanel, IDC_BUTTONSHOW), TRUE);
	return true;
}

//	Get mesh property of node
void LightExtra::GetMeshProps(INode* pNode, A3DMESHPROP& MeshProps)
{
	MeshProps.dwProps	= 0;
	MeshProps.MtlType	= A3DMTL_CONCRETE;

	int	nval;
	BOOL bval;

	if (pNode->GetUserPropBool("双面", bval) && bval)
		MeshProps.Set2SidesFlag(true);
	if (pNode->GetUserPropBool("可射穿", bval) && bval)
		MeshProps.SetShootThroughFlag(true);
	if (pNode->GetUserPropBool("不留痕", bval) && bval)
		MeshProps.SetNoMarkFlag(true);
	if (pNode->GetUserPropBool("碰不到", bval) && bval)
		MeshProps.SetPickThroughFlag(true);
	if (pNode->GetUserPropBool("地表面", bval) && bval)
		MeshProps.SetOnGroundFlag(true);
	if (pNode->GetUserPropBool("分割面", bval) && bval)
		MeshProps.SetBSPPVSFlag(true);
	if (pNode->GetUserPropBool("不渲染", bval) && bval)
		MeshProps.SetNotRenderFlag(true);
	if (pNode->GetUserPropBool("忽略行走", bval) && bval)
		MeshProps.SetWalkIgnoreFlag(true);
	if (pNode->GetUserPropBool("行走", bval) && bval)
		MeshProps.SetOnlyForWalkFlag(true);

	if (pNode->GetUserPropInt("材料", nval))
		MeshProps.MtlType = (A3DMATERIALTYPE)nval;
}

bool LightExtra::ConvertNodeToIBL(A3DDevice * pA3DDevice, INode * pNode, int nCurFaceCount, 
								  float vCurFaceArea, NodeShador& nodeShador, A3DIBLMesh ** ppNewIBLMesh, 
								  A3DIBLScene * pIBLScene, float * pvNodeArea)
{
	Matrix3		tm;
	char		szTextureName[MAX_PATH];
	Matrix3		tmTexture;
	bool		bHasUV = false;

	// Now we call UpdateOptions every time when a node begin to be shaded
	UpdateOptions();

	float val;
	if( pNode->GetUserPropFloat("采样步长", val) )
		option.vStep = val;
	BOOL bval;
	if( pNode->GetUserPropBool("柔化", bval) )
		option.bUseBlur = bval ? true : false;

	//	Get node's mesh properties
	A3DMESHPROP MeshProps;
	GetMeshProps(pNode, MeshProps);

	if( ppNewIBLMesh && pIBLScene )
	{
		MessageBox(hPanel, "You can not create IBLMesh at the same time with IBLScene!", "ERROR", MB_ICONSTOP | MB_OK);
		return false;
	}

	ObjectState os = pNode->EvalWorldState(ip->GetTime());
	if( !os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
		return false;
	
	TriObject	*objTri;
	objTri = (TriObject *)os.obj->ConvertToType(ip->GetTime(), Class_ID(TRIOBJ_CLASS_ID, 0));
	if( !objTri )
		return false;

	objTri->mesh.buildNormals();

	// Get the texture name;
	szTextureName[0] = '\0';
	Mtl * pMtl;

	pMtl = pNode->GetMtl();
	if( pMtl )
	{
		Mtl * pRealMtl = pMtl;
		Class_ID lid = pMtl->ClassID();

		if( pMtl->ClassID() == Class_ID(0x459a16ee, 0x27977f9c) ) // Lightscape mtl
		{
			// we just get the lightscape mtl's baseobject mtl
			pRealMtl = pMtl->GetSubMtl(0);
		}

		if( pRealMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0) )
		{
			StdMat * pStdMtl = (StdMat *) pRealMtl;
			TimeValue t = ip->GetTime();
			
			if (pStdMtl->GetTwoSided())
				MeshProps.Set2SidesFlag(true);

			if (pStdMtl->SubTexmapOn(ID_DI))
			{
				Texmap * pTexmap = pStdMtl->GetSubTexmap(ID_DI);
				if( pTexmap )
				{
					if( 0 == objTri->mesh.getNumTVerts() ) //If not uvw maping
						bHasUV = false;
					else
					{
						if( pTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
						{
							BitmapTex * pBitmapTex = (BitmapTex *)pTexmap;
							sprintf(szTextureName, pBitmapTex->GetMapName());
							// Now we have to replace all slash '/' in filename to back slash '\'
							int nLen = strlen(szTextureName);
							for(int i=0; i<nLen; i++)
							{
								if( szTextureName[i] == '/' )
									szTextureName[i] = '\\';
							}
							bHasUV = true;

							BitmapInfo bmi;
							if( BMMRES_SUCCESS != TheManager->GetImageInfo(&bmi, szTextureName) )
							{
								char szInfo[1024];
								sprintf(szInfo, "找不到物体[%s]的贴图文件[%s]！该物体的贴图信息将无法导出！", pNode->GetName(), szTextureName);
								MessageBox(hPanel, szInfo, "WARNING", MB_ICONINFORMATION | MB_OK);
								szTextureName[0] = '\0';
								bHasUV = false;
							}
							
							if( bHasUV )
							{
								Bitmap * pTexBitmap = pBitmapTex->GetBitmap(ip->GetTime());
							}
						}
						pTexmap->GetUVTransform(tmTexture);
					}
				}
			}
		}
	}
	
	A3DIBLMesh * pIBLMesh = NULL;

	// If we have copied file, we can only use relative file names
	if( !m_bCopyMaps )
		strcpy(m_szBaseMapName, szTextureName);
	else
	{
		af_GetFileTitle(szTextureName, m_szBaseMapName, MAX_PATH);

		// If need copy maps, we just copy it;
		char szTextureFolder[MAX_PATH];
		sprintf(szTextureFolder, "%s\\%s", m_szDestFolder, "Textures");

		if( -1 == _access(szTextureFolder, 0) )
			CreateDirectory(szTextureFolder, NULL);

		// If has a map we just copy it
		if( szTextureName[0] )
		{
			char szFileName[MAX_PATH];
			char szDestName[MAX_PATH];

			af_GetFileTitle(szTextureName, szFileName, MAX_PATH);
			sprintf(szDestName, "%s\\%s", szTextureFolder, szFileName);

			CopyFile(szTextureName, szDestName, FALSE);
		}
	}

	if( ppNewIBLMesh )
	{
		// Create the ibl mesh;
		pIBLMesh = new A3DIBLMesh();

		bool bval;
		if( bHasUV )
		{
			bval = pIBLMesh->Init(pA3DDevice, m_szBaseMapName, option.lightRange, !option.bVertexColorShading);
		}
		else
		{
			bval = pIBLMesh->Init(pA3DDevice, NULL, option.lightRange, !option.bVertexColorShading);
		}

		if( !bval )
		{
			return false;
		}
	}
	
	bool bFlip1, bFlip2;
	tm = pNode->GetNodeTM(ip->GetTime());
	if( DotProd(CrossProd(tm.GetRow(0), tm.GetRow(1)), tm.GetRow(2)) > 0 )
		bFlip1 = false;
	else
		bFlip1 = true;
	tm = pNode->GetObjTMAfterWSM(ip->GetTime());
	if( DotProd(CrossProd(tm.GetRow(0), tm.GetRow(1)), tm.GetRow(2)) > 0 )
		bFlip2 = false;
	else
		bFlip2 = true;
	
	Box3 bb = objTri->mesh.getBoundingBox();
	float l1, l2, l3;
	l1 = tm.VectorTransform(bb[0] - bb[1]).Length();
	l2 = tm.VectorTransform(bb[0] - bb[2]).Length();
	l3 = tm.VectorTransform(bb[0] - bb[4]).Length();
	
	float vNodeArea = CalculateMeshArea(&objTri->mesh, tm);

	if( pIBLScene )
	{
		if (!ShadeIBLMeshByTriangle(nodeShador, NULL, pIBLScene, nCurFaceCount, 
				vCurFaceArea, vNodeArea, &objTri->mesh, tm, tmTexture, bHasUV, 
				bFlip1, bFlip2, MeshProps))
		{
			return false;
		}
	}
	else
	{
		// Maybe we should do some attach work here, to attach all same texture's mesh together!
		// ...
		// ...
		if( l1 > 2.0f || l2 > 2.0f || l3 > 2.0f )
		{
			if (!ShadeIBLMeshByTriangle(nodeShador, pIBLMesh, NULL, nCurFaceCount, 
					vCurFaceArea, vNodeArea, &objTri->mesh, tm, tmTexture, bHasUV, 
					bFlip1, bFlip2, MeshProps))
			{
				return false;
			}
		}
		/*else if( l1 > 10.0f || l2 > 10.0f || l3 > 10.0f )
		{
			if( !ShadeIBLMeshByBoundingBox(nodeShador, pIBLMesh, NULL, nCurFaceCount, vCurFaceArea, vNodeArea, &objTri->mesh, tm, tmTexture, bHasUV, bFlip1, bFlip2) )
				return false;
		}*/
		else
		{
			if( !ShadeLMesh(nodeShador, pIBLMesh, NULL, nCurFaceCount, vCurFaceArea, vNodeArea, &objTri->mesh, tm, tmTexture, bHasUV, bFlip1, bFlip2) )
				return false;
		}
	}

	if( objTri != os.obj )
		objTri->DeleteMe();

	if( ppNewIBLMesh )
		*ppNewIBLMesh = pIBLMesh;

	*pvNodeArea   = vNodeArea;
	return true;
}

float LightExtra::CalculateFaceArea(Mesh * pMesh, Matrix3 tm, int nFaceIndex)
{
	Point3 v0 = tm.PointTransform(pMesh->verts[pMesh->faces[nFaceIndex].v[0]]);
	Point3 v1 = tm.PointTransform(pMesh->verts[pMesh->faces[nFaceIndex].v[1]]);
	Point3 v2 = tm.PointTransform(pMesh->verts[pMesh->faces[nFaceIndex].v[2]]);

	Point3 e10, e21;
	e10 = v1 - v0;
	e21 = v2 - v1;

	return Length(CrossProd(e10, e21)) / 2.0f;
}

float LightExtra::CalculateMeshArea(Mesh * pMesh, Matrix3 tm)
{
	float vArea = 0.0f;

	for(int i=0; i<pMesh->numFaces; i++)
	{
		vArea += CalculateFaceArea(pMesh, tm, i);
	}

	return vArea;
}

int LightExtra::CalculateSelFaceCount()
{
	if( ip->GetSelNodeCount() == 0 )
		return 0;

	m_nTotalFaceCount = 0;
	m_vTotalFaceArea  = 0.0f;

	ip->ProgressStart(GetString(IDS_CALCULATEFACE), TRUE, dummyProgressStart, NULL);

	for(int i=0; i<ip->GetSelNodeCount(); i++)
	{
		MessageTick();
		ip->ProgressUpdate(100.0f * i / ip->GetSelNodeCount());

		INode * pNode = ip->GetSelNode(i);
		ObjectState os = pNode->EvalWorldState(ip->GetTime());
		if( !os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
			continue;
	
		TriObject	*objTri;
		objTri = (TriObject *)os.obj->ConvertToType(ip->GetTime(), Class_ID(TRIOBJ_CLASS_ID, 0));
		if( !objTri )
			continue;

		Matrix3 tm = pNode->GetObjTMAfterWSM(ip->GetTime());
		m_nTotalFaceCount += objTri->mesh.numFaces;
		m_vTotalFaceArea  += CalculateMeshArea(&objTri->mesh, tm);

		if( objTri != os.obj )
			objTri->DeleteMe();
	}

	ip->ProgressUpdate(100);

	return m_nTotalFaceCount;
}

bool LightExtra::MessageTick()
{
	MSG msg;

	while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
	{
		GetMessage(&msg, NULL, 0, 0);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

bool LightExtra::ExportSelectedIBLBB()
{
	NodeShadorOptions shadeOptions;
	ZeroMemory(&shadeOptions, sizeof(shadeOptions));
	shadeOptions.bUseMaps = option.bUseMaps;
	shadeOptions.bCastShadows = option.bUseShadow;
	shadeOptions.bDefaultLights = option.bDefaultLight;
	shadeOptions.bSmoothGrid = option.bSmoothGrid;
	shadeOptions.vScale = 1.0f;

	NodeShador nodeShador(shadeOptions);

	int nMapCount = 0;
	ip->ProgressStart(GetString(IDS_LIGHTEXTRA_PROGRESS), TRUE, dummyProgressStart, NULL);
	for(int i=0; i<ip->GetSelNodeCount(); i++)
	{
		// Update progress
		ip->ProgressUpdate(int(i * 100.0f / ip->GetSelNodeCount()));

		INode * pNode = ip->GetSelNode(i);

		if( !nodeShador.BeginShade(pNode, ip->GetTime()) )
			continue;

		LPBYTE pImage;
		int    nWidth;
		int	   nHeight;

		NodeBoxShadeInfo boxInfo;
		nodeShador.ShadeBoundingBox(&pImage, 1.0f, &nWidth, &nHeight, &boxInfo);

		// Do not do bluring work;
		/*
		for(int l=0; l<option.nBlurTime; l++)
		{
			Blur(pImage, nWidth, nHeight, 
				option.f00, option.f01, option.f02,
				option.f10, option.f11, option.f12,
				option.f20, option.f21, option.f22);
		}*/

		char szFilename[MAX_PATH];
		sprintf(szFilename, "c:\\temp\\face%05d.bmp", nMapCount);

		WriteBitmap(szFilename, nWidth, nHeight, pImage);

		a_free(pImage);

		nMapCount ++;

		nodeShador.AfterShade();
		ip->ProgressUpdate(int((i + 1) * 100.0f / ip->GetSelNodeCount()));

		if( ip->GetCancel() )
			break;
	}

	ip->ProgressEnd();
	return true;
}

// This function will determine where to put the new face's lightmap
// if the current lightmap can not hold it, it just return false to indicate another lightmap should be supplied
// An empty lightmap must can hold any face of the mesh;
bool LightExtra::DetermineFaceLayout(Mesh * pMesh, Matrix3 tm, int nFaceIndex, float vStep, int * pnWidth, int * pnHeight, int * pnX, int * pnY, int * pnVMap0, int * pnVMap1, int * pnVMap2)
{
	int nWidth, nHeight;
	int f = nFaceIndex;

	// Get face's 3 vertex position;
	Point3 vt[3], e10, e20, e21;
	float  l10, l20, l21;
	vt[0] = tm.PointTransform(pMesh->verts[pMesh->faces[f].v[0]]);
	vt[1] = tm.PointTransform(pMesh->verts[pMesh->faces[f].v[1]]);
	vt[2] = tm.PointTransform(pMesh->verts[pMesh->faces[f].v[2]]);
	e10 = vt[1] - vt[0];
	e20 = vt[2] - vt[0];
	e21 = vt[2] - vt[1];
	l10 = e10.Length();
	l20 = e20.Length();
	l21 = e21.Length();

	int nVMap[3];
	float lh, lv;
	Point3 edgeH, edgeV;

	// 0*--------1
	//  |      /
	//  |    /
	//  |  /
	//  |/
	// 2
	if( l10 > l20 && l10 > l21 )
	{
		// the 3-rd vertex is the perpendicular
		if( l20 > l21 )
		{
			nVMap[0] = 2;
			nVMap[1] = 0;
			nVMap[2] = 1;
			lh = l20;
			lv = l21;
			edgeH = e20;
			edgeV = e21;
		}
		else
		{
			nVMap[0] = 2;
			nVMap[1] = 1;
			nVMap[2] = 0;
			lh = l21;
			lv = l20;
			edgeH = e21;
			edgeV = e20;
		}
	}
	else if( l20 > l10 && l20 > l21 )
	{
		// the 2-nd vertex is the perpendicular
		if( l21 > l10 )
		{
			nVMap[0] = 1;
			nVMap[1] = 2;
			nVMap[2] = 0;
			lh = l21;
			lv = l10;
			edgeH = e21;
			edgeV = e10;
		}
		else
		{
			nVMap[0] = 1;
			nVMap[1] = 0;
			nVMap[2] = 2;
			lh = l10;
			lv = l21;
			edgeH = e10;
			edgeV = e21;
		}
	}
	else
	{
		// the first vertex is the perpendicular
		if( l10 > l20 )
		{
			nVMap[0] = 0;
			nVMap[1] = 1;
			nVMap[2] = 2;
			lh = l10;
			lv = l20;
			edgeH = e10;
			edgeV = e20;
		}
		else
		{
			nVMap[0] = 0;
			nVMap[1] = 2;
			nVMap[2] = 1;
			lh = l20;
			lv = l10;
			edgeH = e20;
			edgeV = e10;
		}
	}

	nWidth = lh / vStep;
	nHeight = lv / vStep;

	// We shall not create too small lightmap section, for that will cause a lot of artifacts
	// 5 is the experimental result of minimum size
	if( nWidth < 5 )
		nWidth = 5;
	
	if( nHeight < 5 )
		nHeight = 5;

	if( nWidth > m_nImgWidth ) nWidth = m_nImgWidth;
	if( nHeight > m_nImgHeight ) nHeight = m_nImgHeight;

	// Now try to find out where to put this rectangle;
	int nX, nY;
	
	HRGN	hRgnThis = CreateRectRgn(0, 0, 1, 1);
	HRGN	hRgnCombined = CreateRectRgn(0, 0, 1, 1);

	bool bSettled = false;

//PLACEAGAIN:
	bool bFirstAttempt = false;
	nX = m_nLastX;
	nY = m_nLastY;
	while(nY + nHeight <= m_nImgHeight)
	{
		SetRectRgn(hRgnThis, nX, nY, nX + nWidth, nY + nHeight);
		CombineRgn(hRgnCombined, m_hrgnCurLightImage, hRgnThis, RGN_AND);

		if( EqualRgn(hRgnCombined, hRgnThis) )
		{
			bSettled = true;
			break;
		}

		nX ++;

		// See if we should do linefeed
		if( nX + nWidth > m_nImgWidth )
		{
			// If the left space is still large we should use it;
			if( m_nImgWidth - nX > m_nImgWidth / 8 && (m_nImgWidth - nX) * 2 > nWidth )
			{
				SetRectRgn(hRgnThis, nX, nY, m_nImgWidth, nY + nHeight);	
				CombineRgn(hRgnCombined, m_hrgnCurLightImage, hRgnThis, RGN_AND);
				if( EqualRgn(hRgnCombined, hRgnThis) )
				{
					nWidth = m_nImgWidth - nX;
					bSettled = true;
					break;
				}
			}

			// Now we have to discard the free space and move to next line;
			nX = 0;

			if( bFirstAttempt )
			{
				nY = 0;
				bFirstAttempt = false;
			}
			else
				nY ++;
		}

		if( nY + nHeight > m_nImgHeight )
		{
			// Last try, to see if bottom part can be used;
			while( nY < m_nImgHeight - nHeight / 2 )
			{
				if( m_nImgHeight - nY > m_nImgHeight / 8 && (m_nImgHeight - nY) * 2 > nHeight )
				{
					SetRectRgn(hRgnThis, nX, nY, nX + nWidth, m_nImgHeight);	
					CombineRgn(hRgnCombined, m_hrgnCurLightImage, hRgnThis, RGN_AND);
					if( EqualRgn(hRgnCombined, hRgnThis) )
					{
						nHeight = m_nImgHeight - nY;
						bSettled = true;
						break;
					}
				}
				nY ++;
			}
		}
	}

	if( !bSettled )
	{
		goto UNSETTLED;
		/*
		if( nWidth > nHeight )
			nWidth /= 2;
		else
			nHeight /= 2;
		goto PLACEAGAIN;*/
	}

	m_nLastX = nX + nWidth;
	m_nLastY = nY;
	
	// now fill the results
	*pnWidth = nWidth;
	*pnHeight = nHeight;
	*pnX = nX;
	*pnY = nY;
	*pnVMap0 = nVMap[0];
	*pnVMap1 = nVMap[1];
	*pnVMap2 = nVMap[2];

	DeleteObject(hRgnCombined);
	DeleteObject(hRgnThis);
	return true;

UNSETTLED:
	DeleteObject(hRgnCombined);
	DeleteObject(hRgnThis);
	return false;
}

bool LightExtra::DetermineBestNF(Mesh * pMesh, Matrix3 tm, const BitArray& faceShaded, int nFaceIndex, int nVMap0, int nVMap1, int nVMap2, int * pnBestNF, int * pnVMap0, int * pnVMap1, int * pnVMap2)
{
	int f = nFaceIndex;

	// Now we need to shade another half this rect;
	// First find the best neighbour;
	int edgeV1, edgeV2;
	edgeV1 = pMesh->faces[f].v[nVMap1];
	edgeV2 = pMesh->faces[f].v[nVMap2];

	*pnBestNF = -1;
	float vBestValue = 0.707f; // 45 degree

	// First, if this face is isolated, do not try any more
	if( pMesh->faces[f].v[0] == pMesh->faces[f].v[1] ||
		pMesh->faces[f].v[1] == pMesh->faces[f].v[2] ||
		pMesh->faces[f].v[0] == pMesh->faces[f].v[2] )
		return true;

	for(int nf=0; nf<pMesh->numFaces; nf++)
	{
		if( nf == f || faceShaded[nf] )
			continue;

		// If the face is isolated, we just skip it;
		if( pMesh->faces[nf].v[0] == pMesh->faces[nf].v[1] ||
			pMesh->faces[nf].v[1] == pMesh->faces[nf].v[2] ||
			pMesh->faces[nf].v[0] == pMesh->faces[nf].v[2] )
			continue;

		// Find if it use this edge
		bool bHasV1 = false, bHasV2 = false;
		int vMap0, vMap1, vMap2;
		for(int v=0; v<3; v++)
		{
			if( pMesh->faces[nf].v[v] == edgeV1 )
			{
				// We switch the 1 and 2 vertex maping for other half
				vMap2 = v;
				bHasV2 = true;
			}
			else if( pMesh->faces[nf].v[v] == edgeV2 )
			{
				vMap1 = v;
				bHasV1 = true;
			}  
			else
				vMap0 = v;
		}

		if( bHasV1 && bHasV2 )
		{
			// Now see if it is good enough;
			// use normal difference to determine
			float vValue = DotProd(pMesh->getFaceNormal(f), pMesh->getFaceNormal(nf));
			if( vValue > vBestValue )
			{
				// we should check if the share edge is long enough for other face;
				float l01, l02, l21;
				Point3 v0, v1, v2;
				v0 = tm.PointTransform(pMesh->verts[pMesh->faces[nf].v[vMap0]]);
				v1 = tm.PointTransform(pMesh->verts[pMesh->faces[nf].v[vMap1]]);
				v2 = tm.PointTransform(pMesh->verts[pMesh->faces[nf].v[vMap2]]);
				l01 = Length(v0 - v1);
				l02 = Length(v0 - v2);
				l21 = Length(v1 - v2);

				// We must select a neighbour that seems like this triangle, or 
				// to much error will be produced.
				if( l21 > l01 * 0.8f &&	l21 > l02 * 0.8f && l01 > l02 * 0.8f )
				{
					vBestValue = vValue;
					*pnBestNF = nf;
					*pnVMap0 = vMap0; *pnVMap1 = vMap1; *pnVMap2 = vMap2;
				}
			}
		}
	}

	return true;
}

bool LightExtra::AddTriIBLMeshFace(Mesh * pMesh, Matrix3 tm, Matrix3 tmTexture, bool bHasUV, 
								   LPBYTE pVertexColors, bool bFlip1, bool bFlip2, 
								   int nFaceIndex, int nVMap[3], bool bBelowHalf, 
								   A3DIBLTriSubMesh * pNewTriSubIBLMesh, A3DIBLScene * pIBLScene,
								   float u0, float u1, float v0, float v1, const A3DMESHPROP& MeshProps)
{
	int				f = nFaceIndex;
	A3DIBLVERTEX	v[3];
	int				n0, n1, n2;

	Point3 vert;
	for(int k=0; k<3; k++)
	{
		vert = tm.PointTransform(pMesh->verts[pMesh->faces[f].v[nVMap[k]]]);
		v[k].x = vert.x;
		v[k].y = vert.z;
		v[k].z = vert.y;
		// Now get vertex's color
		int vid;
		if( bHasUV )
			vid = pMesh->tvFace[f].t[nVMap[k]];
		else
			vid = pMesh->faces[f].v[nVMap[k]];
		v[k].diffuse  = A3DCOLORRGB(pVertexColors[vid * 3 + 2], pVertexColors[vid * 3 + 1], pVertexColors[vid * 3 + 0]); 
		v[k].specular = 0xff000000;
		if( bHasUV )
		{
			UVVert uv = pMesh->tVerts[pMesh->tvFace[f].t[nVMap[k]]];
			uv = uv * tmTexture;
			v[k].tu1 = uv.x;
			v[k].tv1 = 1.0f - uv.y;
		}
		else
			v[k].tu1 = v[k].tv1 = 0.0f;

		v[k].tu2 = v[k].tv2 = 0.0f;
	}

	// set the texture coordinates for the light map channel
	if( !bBelowHalf )
	{
		v[0].tu2 = u0;			v[0].tv2 = v0;
		v[1].tu2 = u1;			v[1].tv2 = v0;
		v[2].tu2 = u0;			v[2].tv2 = v1;		
	}
	else
	{
		v[0].tu2 = u1;			v[0].tv2 = v1;
		v[1].tu2 = u0;			v[1].tv2 = v1;
		v[2].tu2 = u1;			v[2].tv2 = v0;		
	}

	bool bFlip3 = true;
	// see if the neighbour face has been flip vertex index order
	for(int vi=0; vi<3; vi++)
	{
		int m[3];
		m[0] = nVMap[vi];
		m[1] = nVMap[(vi + 1) % 3];
		m[2] = nVMap[(vi + 2) % 3];

		if( m[0] == 0 && m[1] == 1 && m[2] == 2 )
			bFlip3 = false;
	}

	if( !bFlip1 && !bFlip2 )
	{
		if( bFlip3 )
		{
			n0 = 0; n1 = 1; n2 = 2;
		}
		else
		{
			n0 = 1; n1 = 0; n2 = 2;
		}
	}
	else
	{
		if( bFlip3 )
		{
			n0 = 1; n1 = 0; n2 = 2;
		}
		else
		{
			n0 = 0; n1 = 1; n2 = 2;
		}
	}
	
	if (pNewTriSubIBLMesh)
	{
		if (!pNewTriSubIBLMesh->AddIBLFace(v[0], v[1], v[2], n0, n1, n2))
			return false;
	}
	else
	{
		// Add it into an IBLScene
		char szLightMapName[MAX_PATH];
		char szLightMapFile[MAX_PATH];

		GetLightMapName(szLightMapName, m_nImgSN);
		if (m_bCopyMaps)
		{
			af_GetFileTitle(szLightMapName, szLightMapFile, MAX_PATH);
		}
		else
			strcpy(szLightMapFile, szLightMapName);

		if (!pIBLScene->AddTriFace(m_szBaseMapName, szLightMapFile, &v[n0], &v[n1], &v[n2], MeshProps))
			return false; 
	}

	return true;
}

bool LightExtra::AddRectIBLMeshFace(Mesh * pMesh, Matrix3 tm, Matrix3 tmTexture, bool bHasUV, 
									LPBYTE pVertexColors, bool bFlip1, bool bFlip2, 
									int nFaceIndex1, int nFaceIndex2, int nVMap1[3], 
									int nVMap2[3], A3DIBLTriSubMesh * pNewTriSubIBLMesh, 
									A3DIBLScene * pIBLScene, float u0, float u1, float v0,
									float v1, const A3DMESHPROP& MeshProps)
{
	A3DIBLVERTEX	v[4];
	int				n0, n1, n2, n3, n4, n5;

	Point3 vert;
	for(int k=0; k<4; k++)
	{
		int id;
		int f;
		
		if( k == 3 )
		{
			// Use second face's first vertex
			id = nVMap2[0];
			f = nFaceIndex2;
		}
		else
		{
			id = nVMap1[k];
			f = nFaceIndex1;
		}

		vert = tm.PointTransform(pMesh->verts[pMesh->faces[f].v[id]]);
		v[k].x = vert.x;
		v[k].y = vert.z;
		v[k].z = vert.y;
		// Now get vertex's color
		int vid;
		if( bHasUV )
			vid = pMesh->tvFace[f].t[id];
		else
			vid = pMesh->faces[f].v[id];
		v[k].diffuse  = A3DCOLORRGB(pVertexColors[vid * 3 + 2], pVertexColors[vid * 3 + 1], pVertexColors[vid * 3 + 0]); 
		v[k].specular = 0xff000000;
		if( bHasUV )
		{
			UVVert uv = pMesh->tVerts[pMesh->tvFace[f].t[id]];
			uv = uv * tmTexture;
			v[k].tu1 = uv.x;
			v[k].tv1 = 1.0f - uv.y;
		}
		else
			v[k].tu1 = v[k].tv1 = 0.0f;

		v[k].tu2 = v[k].tv2 = 0.0f;
	}

	// set the texture coordinates for the light map channel
	v[0].tu2 = u0;			v[0].tv2 = v0;
	v[1].tu2 = u1;			v[1].tv2 = v0;
	v[2].tu2 = u0;			v[2].tv2 = v1;		
	v[3].tu2 = u1;			v[3].tv2 = v1;

	bool bFlip3;
	int  vi;

	bFlip3 = true;
	// see if the neighbour face has been flip vertex index order
	for(vi=0; vi<3; vi++)
	{
		int m[3];
		m[0] = nVMap1[vi];
		m[1] = nVMap1[(vi + 1) % 3];
		m[2] = nVMap1[(vi + 2) % 3];

		if( m[0] == 0 && m[1] == 1 && m[2] == 2 )
			bFlip3 = false;
	}

	if( !bFlip1 && !bFlip2 )
	{
		if( bFlip3 )
		{
			n0 = 0; n1 = 1; n2 = 2;
		}
		else
		{
			n0 = 1; n1 = 0; n2 = 2;
		}
	}
	else
	{
		if( bFlip3 )
		{
			n0 = 1; n1 = 0; n2 = 2;
		}
		else
		{
			n0 = 0; n1 = 1; n2 = 2;
		}
	}
	
	bFlip3 = true;
	// see if the neighbour face has been flip vertex index order
	for(vi=0; vi<3; vi++)
	{
		int m[3];
		m[0] = nVMap2[vi];
		m[1] = nVMap2[(vi + 1) % 3];
		m[2] = nVMap2[(vi + 2) % 3];

		if( m[0] == 0 && m[1] == 1 && m[2] == 2 )
			bFlip3 = false;
	}

	if( !bFlip1 && !bFlip2 )
	{
		if( bFlip3 )
		{
			n3 = 3; n4 = 2; n5 = 1;
		}
		else
		{
			n3 = 2; n4 = 3; n5 = 1;
		}
	}
	else
	{
		if( bFlip3 )
		{
			n3 = 2; n4 = 3; n5 = 1;
		}
		else
		{
			n3 = 3; n4 = 2; n5 = 1;
		}
	}

	if( pNewTriSubIBLMesh )
	{
		if( !pNewTriSubIBLMesh->AddRectFace(v[0], v[1], v[2], v[3], n0, n1, n2, n3, n4, n5) )
			return false;
	}
	else
	{
		// Add into an IBL scene;
		char szLightMapName[MAX_PATH];
		char szLightMapFile[MAX_PATH];

		GetLightMapName(szLightMapName, m_nImgSN);
		if( m_bCopyMaps )
		{
			af_GetFileTitle(szLightMapName, szLightMapFile, MAX_PATH);
		}
		else
			strcpy(szLightMapFile, szLightMapName);

		if (!pIBLScene->AddTriFace(m_szBaseMapName, szLightMapFile, &v[n0], &v[n1], &v[n2], MeshProps))
			return false;

		if (!pIBLScene->AddTriFace(m_szBaseMapName, szLightMapFile, &v[n3], &v[n4], &v[n5], MeshProps))
			return false;
	}

	return true;
}

bool LightExtra::ShadeSceneLightGrid(A3DDevice * pA3DDevice, NodeShador& nodeShador, A3DIBLScene * pIBLScene)
{
	// First enumerate all selected nodes to get scene bounding box
	INode * pAnyNode = NULL;
	Box3	bbScene;
	
	for(int i=0; i<ip->GetSelNodeCount(); i++)
	{
		INode * pNode = ip->GetSelNode(i);
		ObjectState os = pNode->EvalWorldState(ip->GetTime());
		if( !os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
			continue;
	
		TriObject	*objTri;
		objTri = (TriObject *)os.obj->ConvertToType(ip->GetTime(), Class_ID(TRIOBJ_CLASS_ID, 0));
		if( !objTri )
			continue;

		pAnyNode = pNode;
		Matrix3 tm = pNode->GetObjTMAfterWSM(ip->GetTime());
		Box3 bb = objTri->mesh.getBoundingBox(&tm);
		bbScene += bb;

		if( objTri != os.obj )
			objTri->DeleteMe();
	}
	nodeShador.BeginShade(pAnyNode, ip->GetTime());

	A3DIBLLightGrid * pIBLLightGrid = new A3DIBLLightGrid();
	pIBLLightGrid->Init(pA3DDevice);
	
	nodeShador.ShadeLightGrid(bbScene.Min(), bbScene.Max(), option.vGridSize, option.vGridDim, pIBLLightGrid);
	
	nodeShador.AfterShade();

	pIBLScene->SetLightGrid(pIBLLightGrid);
	return true;
}

bool LightExtra::ShadeIBLMeshByTriangle(NodeShador& nodeShador, A3DIBLMesh * pIBLMesh, 
										A3DIBLScene * pIBLScene, int nCurFaceCount, 
										float vCurFaceArea, float vNodeArea, Mesh * pMesh,
										Matrix3 tm, Matrix3 tmTexture, bool bHasUV, 
										bool bFlip1, bool bFlip2, const A3DMESHPROP& MeshProps)
{
	int id;
	float vStep = option.vStep;

	char szLightMap[MAX_PATH];
	bool bHasUsed = false;

	GetLightMapName(szLightMap, m_nImgSN);
	A3DIBLTriSubMesh * pNewTriSubIBLMesh = NULL;
	if( pIBLMesh )
	{
		if( !pIBLMesh->AddIBLTriSubMesh(&pNewTriSubIBLMesh) )
			return false;
		pNewTriSubIBLMesh->SetLightMap(szLightMap);
	}

	BitArray faceShaded;
	faceShaded.SetSize(pMesh->numFaces, 0);

	// First we should retrieve vertex shading color for usage when user choose
	// not to use lightmaps.
	LPBYTE			pVertexColors;
	int				nVertCount;

	nodeShador.ShadeVertexColors(bHasUV, &pVertexColors, &nVertCount);

	for(id=0; id<nodeShador.GetFaceCount(); id++)
	{
		if( faceShaded[id] )
			continue;

		// if this triangle is isolated, just skip it
		if( pMesh->faces[id].v[0] == pMesh->faces[id].v[1] ||
			pMesh->faces[id].v[1] == pMesh->faces[id].v[2] ||
			pMesh->faces[id].v[0] == pMesh->faces[id].v[2] )
		{
			vCurFaceArea += CalculateFaceArea(pMesh, tm, id);
			ip->ProgressUpdate(100.0f * vCurFaceArea / m_vTotalFaceArea);
			faceShaded.Set(id);
			continue;
		}

		PrepareCurLightImage();

		vCurFaceArea += CalculateFaceArea(pMesh, tm, id);
		ip->ProgressUpdate(100.0f * vCurFaceArea / m_vTotalFaceArea);
		LPBYTE pImage;
		int    nWidth;
		int	   nHeight;
		int	   nX, nY;
		int	   nVMap[3];
		
		if( !DetermineFaceLayout(pMesh, tm, id, vStep, &nWidth, &nHeight, &nX, &nY, &nVMap[0], &nVMap[1], &nVMap[2]) )
		{
			// Close this light mesh;
			CloseCurLightImage();
			
			// Now Create another light map sub mesh;
			PrepareCurLightImage();
			GetLightMapName(szLightMap, m_nImgSN);

			if( pIBLMesh )
			{
				if( bHasUsed )
				{
					if( !pIBLMesh->AddIBLTriSubMesh(&pNewTriSubIBLMesh) )
						return false;
				}
				
				pNewTriSubIBLMesh->SetLightMap(szLightMap);
			}

			if( !DetermineFaceLayout(pMesh, tm, id, vStep, &nWidth, &nHeight, &nX, &nY, &nVMap[0], &nVMap[1], &nVMap[2]) )
				return false;

			bHasUsed = false;
		}

		// Then cut off this rect from the map region;
		HRGN   hNewRect = CreateRectRgn(nX, nY, nX + nWidth, nY + nHeight);
		CombineRgn(m_hrgnCurLightImage, m_hrgnCurLightImage, hNewRect, RGN_DIFF);
		DeleteObject(hNewRect);

		// Calculate this face's size;
		// nodeShador always create a top-triangle image
		// and n0 is the index of top-left corner vertex
		// n1 is the top-right corner vertex
		// n2 is the bottom-left corner vertex
		nodeShador.ShadeIntoTriangle(id, &pImage, nWidth, nHeight, nVMap[0], nVMap[1], nVMap[2]);
		faceShaded.Set(id);

		int nBestNF = -1;
		int nVMap1[3];
		DetermineBestNF(pMesh, tm, faceShaded, id, nVMap[0], nVMap[1], nVMap[2], &nBestNF, &nVMap1[0], &nVMap1[1], &nVMap1[2]);

		bool bShadeOtherHalf = false;
		if( nBestNF != -1 )
		{
			LPBYTE pImageOtherHalf;

			nodeShador.ShadeIntoTriangle(nBestNF, &pImageOtherHalf, nWidth, nHeight, nVMap1[0], nVMap1[1], nVMap1[2]);

			// Now put it into the rect image
			for(int x=0; x<nWidth; x++)
			{
				for(int y=0; y<nHeight; y++)
				{
					if( 1.0f * x / nWidth + 1.0f * y / nHeight > 1.0f )
						continue;

					int nDestIndex = (nHeight - 1 - y) * nWidth * 3 + (nWidth - 1 - x) * 3;
					int nSrcIndex = y * nWidth * 3 + x * 3;

					memcpy(pImage + nDestIndex, pImageOtherHalf + nSrcIndex, 3);
				}
			}

			a_free(pImageOtherHalf);
			bShadeOtherHalf = true;

			faceShaded.Set(nBestNF);
			vCurFaceArea += CalculateFaceArea(pMesh, tm, nBestNF);
		}

		for(int l=0; l<option.nBlurTime; l++)
		{
			Blur(pImage, nWidth, nHeight, NULL,
				option.f00, option.f01, option.f02,
				option.f10, option.f11, option.f12,
				option.f20, option.f21, option.f22);
		}

		// Then put it into the lightmap image;
		for(int x=0; x<nWidth; x++)
		{
			for(int y=0; y<nHeight; y++)
			{
				int nDestIndex = (nY + y) * m_nImgWidth * 3 + (nX + x) * 3;
				int nSrcIndex = y * nWidth * 3 + x * 3;

				memcpy(m_pCurLightImage + nDestIndex, pImage + nSrcIndex, 3);
			}
		}
		
		// Now choose add method
		bool bRectFace = false;

		if( bShadeOtherHalf )
		{
			if( bHasUV )
			{
				if( pMesh->tvFace[id].t[nVMap[1]] == pMesh->tvFace[nBestNF].t[nVMap1[2]] &&
					pMesh->tvFace[id].t[nVMap[2]] == pMesh->tvFace[nBestNF].t[nVMap1[1]] )
					bRectFace = true;
			}
			else
				bRectFace = true;
		}

		// Finally, we add them into the IBLSubMesh
		float u0, u1, v0, v1;
		u0 = (nX + 0.5f) / m_nImgWidth;
		u1 = (nX + nWidth - 0.5f) / m_nImgWidth;
		v0 = (nY + 0.5f) / m_nImgHeight;
		v1 = (nY + nHeight - 0.5f) / m_nImgHeight;
		if( bRectFace )
		{
			// Add a rectangle face here;
			AddRectIBLMeshFace(pMesh, tm, tmTexture, bHasUV, pVertexColors, bFlip1, 
				bFlip2, id, nBestNF, nVMap, nVMap1, pNewTriSubIBLMesh, pIBLScene, 
				u0, u1, v0, v1, MeshProps);
		}
		else
		{
			// Add one or two triangle here;
			AddTriIBLMeshFace(pMesh, tm, tmTexture, bHasUV, pVertexColors, bFlip1, bFlip2,
				id, nVMap, false, pNewTriSubIBLMesh, pIBLScene, u0, u1, v0, v1, MeshProps);

			if (bShadeOtherHalf)
			{
				AddTriIBLMeshFace(pMesh, tm, tmTexture, bHasUV, pVertexColors, bFlip1, 
					bFlip2, nBestNF, nVMap1, true, pNewTriSubIBLMesh, pIBLScene, u0, u1, 
					v0, v1, MeshProps);
			}
		}

		a_free(pImage);
		
		bHasUsed = true;
	}

	a_free(pVertexColors);
	return true;
}

bool LightExtra::ShadeIBLMeshByBoundingBox(NodeShador& nodeShador, A3DIBLMesh * pIBLMesh, A3DIBLScene * pIBLScene, int nCurFaceCount, float vCurFaceArea, float vNodeArea, Mesh * pMesh, Matrix3 tm, Matrix3 tmTexture, bool bHasUV, bool bFlip1, bool bFlip2)
{
	// first shade the bounding box it;
	LPBYTE pImage;
	int    nWidth;
	int	   nHeight;

	NodeBoxShadeInfo boxInfo;
	nodeShador.ShadeBoundingBox(&pImage, option.vStep, &nWidth, &nHeight, &boxInfo);	

	// if need blur, we just blur it here;
	for(int k=0; k<6; k++)
	{
		RECT subRect;
		subRect.left = boxInfo.x[k];
		subRect.top  = boxInfo.y[k];
		subRect.right = boxInfo.x[k] + boxInfo.width[k];
		subRect.bottom = boxInfo.y[k] + boxInfo.height[k];

		for(int l=0; l<option.nBlurTime; l++)
		{
			Blur(pImage, nWidth, nHeight, &subRect,
				option.f00, option.f01, option.f02,
				option.f10, option.f11, option.f12,
				option.f20, option.f21, option.f22);
		}
	}

	A3DIBLBBSubMesh * pNewBBSubIBLMesh;

	if( !pIBLMesh->AddIBLBBSubMesh(&pNewBBSubIBLMesh) )
		return false;

	if( !pNewBBSubIBLMesh->SetLightMap(nWidth, nHeight, pImage) )
		return false;

	char szFilename[MAX_PATH];
	sprintf(szFilename, "c:\\temp\\iblbbface%05d.bmp", nCurFaceCount);
	WriteBitmap(szFilename, nWidth, nHeight, pImage);

	a_free(pImage);

	int id;
	for(id=0; id<nodeShador.GetFaceCount(); id++)
	{
		MessageTick();
		
		A3DIBLVERTEX v[3];
		Point3 vLocal[3];
		int n0, n1, n2;

		Point3 vert[3];
		for(int k=0; k<3; k++)
		{
			vert[k] = tm.PointTransform(pMesh->verts[pMesh->faces[id].v[k]]);
			v[k].x = vert[k].x;
			v[k].y = vert[k].z;
			v[k].z = vert[k].y;
			v[k].diffuse  = 0xffffffff;
			v[k].specular = 0xff000000;
			if( bHasUV )
			{
				UVVert uv = pMesh->tVerts[pMesh->tvFace[id].t[k]];
				uv = uv * tmTexture;
				v[k].tu1 = uv.x;
				v[k].tv1 = 1.0f - uv.y;
			}
			else
				v[k].tu1 = v[k].tv1 = 0.0f;

			v[k].tu2 = v[k].tv2 = 0.0f;
		}

		// Now calculate the face normal
		Point3 edge10 = vert[1] - vert[0];
		Point3 edge21 = vert[2] - vert[1];
		Point3 faceNormal = CrossProd(edge10, edge21).Normalize();

		if( !bFlip1 && !bFlip2 )
		{
			n0 = 1; n1 = 0; n2 = 2;
		}
		else
		{
			n0 = 0; n1 = 1; n2 = 2;
			faceNormal *= -1.0f;
		}
		
		// Now calculate the vertex's light map texture coordinates;
		// first determine which axis plane it is facing;
		float vMaxProj = -99999.9f;
		int   np = -1;
		for(k=0; k<6; k++)
		{
			float vProj = DotProd(boxInfo.normal[k], faceNormal);
			if( vMaxProj < vProj )
			{
				np = k;
				vMaxProj = vProj;
			}
		}

		// then project the plane onto the triangle
		float sh, sv;
		sh = 1.0f - 1.0f / boxInfo.width[np];
		sv = 1.0f - 1.0f / boxInfo.height[np];

		float s;
		for(k=0; k<3; k++)
		{
			// shrink for half pixel to avoid edge interpolation
			s = DotProd(vert[k] - boxInfo.lt[np], boxInfo.r[np]) / boxInfo.r[np].Length() / boxInfo.r[np].Length();
			v[k].tu2 = (boxInfo.x[np] + 0.5f + boxInfo.width[np] * s * sh) / boxInfo.nTexWidth;

			s = DotProd(vert[k] - boxInfo.lt[np], boxInfo.d[np]) / boxInfo.d[np].Length() / boxInfo.d[np].Length();
			v[k].tv2 = (boxInfo.y[np] + 0.5f + boxInfo.height[np] * s * sv) / boxInfo.nTexHeight;
		}

		if( !pNewBBSubIBLMesh->AddIBLFace(v[0], v[1], v[2], n0, n1, n2) )
			return false;
	}

	ip->ProgressUpdate(100.0f * (vCurFaceArea + vNodeArea) / m_vTotalFaceArea);
	return true;
}

bool LightExtra::ShadeLMesh(NodeShador& nodeShador, A3DIBLMesh * pIBLMesh, A3DIBLScene * pIBLScene, int nCurFaceCount, float vCurFaceArea, float vNodeArea, Mesh * pMesh, Matrix3 tm, Matrix3 tmTexture, bool bHasUV, bool bFlip1, bool bFlip2)
{
	// first shade the bounding box it;
	LPBYTE pVertexColors;
	int    nVertCount;
	int	   nFaceCount;
	
	nodeShador.ShadeVertexColors(bHasUV, &pVertexColors, &nVertCount);	

	A3DLSubMesh * pNewLSubMesh;

	if( !pIBLMesh->AddLSubMesh(&pNewLSubMesh) )
		return false;

	char szFilename[MAX_PATH];
	sprintf(szFilename, "c:\\temp\\iblvc%05d.bmp", nCurFaceCount);
	WriteBitmap(szFilename, nVertCount, 1, pVertexColors);

	A3DLVERTEX	* pVerts;
	WORD * pIndices;

	if( bHasUV )
		nVertCount = pMesh->numTVerts;
	else
		nVertCount = pMesh->numVerts;
	nFaceCount = pMesh->numFaces;

	pVerts = (A3DLVERTEX*)a_malloc(sizeof(A3DLVERTEX) * nVertCount);
	if( NULL == pVerts ) 
		return false;

	pIndices = (WORD*)a_malloc(sizeof(WORD) * 3 * nFaceCount);
	if( NULL == pIndices )
		return false;

	for(int f=0; f<nFaceCount; f++)
	{
		for(int k=0; k<3; k++)
		{
			int vid;
			A3DLVERTEX v;
			if( bHasUV )
				vid = pMesh->tvFace[f].t[k];
			else
				vid = pMesh->faces[f].v[k];

			Point3 vert = tm.PointTransform(pMesh->verts[pMesh->faces[f].v[k]]);
			v.x = vert.x;
			v.y = vert.z;
			v.z = vert.y;
			v.diffuse  = A3DCOLORRGB(pVertexColors[vid * 3 + 2], pVertexColors[vid * 3 + 1], pVertexColors[vid * 3 + 0]); 
			v.specular = 0xff000000;
			if( bHasUV )
			{
				UVVert uv = pMesh->tVerts[pMesh->tvFace[f].t[k]];
				uv = uv * tmTexture;
				v.tu = uv.x;
				v.tv = 1.0f - uv.y;
			}
			else
				v.tu = v.tv = 0.0f;

			pVerts[vid] = v;
		}

		int n0, n1, n2;
		if( !bFlip1 && !bFlip2 )
		{
			n0 = 1; n1 = 0; n2 = 2;
		}
		else
		{
			n0 = 0; n1 = 1; n2 = 2;
		}

		if( bHasUV )
		{
			pIndices[f * 3 + 0] = pMesh->tvFace[f].t[n0];
			pIndices[f * 3 + 1] = pMesh->tvFace[f].t[n1];
			pIndices[f * 3 + 2] = pMesh->tvFace[f].t[n2];
		}
		else
		{
			pIndices[f * 3 + 0] = pMesh->faces[f].v[n0];
			pIndices[f * 3 + 1] = pMesh->faces[f].v[n1];
			pIndices[f * 3 + 2] = pMesh->faces[f].v[n2];
		}
	}
	
	a_free(pVertexColors);

	if( !pNewLSubMesh->AddVerts(pVerts, nVertCount, A3DVT_LVERTEX) )
		return false;
	if( !pNewLSubMesh->AddFaces(pIndices, nFaceCount, A3DFMT_INDEX16) )
		return false;
	
	a_free(pVerts);
	a_free(pIndices);

	ip->ProgressUpdate(100.0f * (vCurFaceArea + vNodeArea) / m_vTotalFaceArea);
	return true;
}

void LightExtra::GetLightMapName(char * szLightMap, int nImgSN)
{
	if( m_bCopyMaps )
		sprintf(szLightMap, "%s\\%s\\ils_map%05d.bmp", m_szDestFolder, "Textures", m_nImgSN);
	else
		sprintf(szLightMap, "c:\\temp\\lightmap\\map%05d.bmp", m_nImgSN);
}

bool LightExtra::PrepareCurLightImage()
{
	if( m_pCurLightImage )
		return true;

	m_pCurLightImage = (LPBYTE)a_malloc(sizeof(BYTE) * m_nImgWidth * m_nImgHeight * 3);
	if( NULL == m_pCurLightImage )
		return false;

	ZeroMemory(m_pCurLightImage, m_nImgWidth * m_nImgHeight * sizeof(BYTE) * 3);

	m_hrgnCurLightImage = CreateRectRgn(0, 0, m_nImgWidth, m_nImgHeight);
	m_nLastX = 0;
	m_nLastY = 0;

	return true;
}

bool LightExtra::CloseCurLightImage()
{
	// See if already closed!
	if( !m_pCurLightImage )
		return true;

	char szFilename[MAX_PATH];
	GetLightMapName(szFilename, m_nImgSN);

	WriteBitmap(szFilename, m_nImgWidth, m_nImgHeight, m_pCurLightImage);
	a_free(m_pCurLightImage);
	m_pCurLightImage = NULL;

	DeleteObject(m_hrgnCurLightImage);
	m_hrgnCurLightImage = NULL;

	m_nImgSN ++;
	return true;
}

/*
	Function: blur
		use a laplase algorithm to blur the image
		IMPORTANT: for we use the blured image to form a whole image
			so, the edge of each blur should not be affected
		MODIFIED:
			the edge need not to be check within the blur range, for we have just
			shrink the blur range
*/

#define _ENTIRE_RECT
bool LightExtra::Blur(LPBYTE pOrgImage, int nWidth, int nHeight, RECT * pSubRect, float f00, float f01, float f02,
					  float f10, float f11, float f12, float f20, float f21, float f22, bool bKeepEdge)
{
	if( !option.bUseBlur )
		return true;

	LPBYTE pImageMono = NULL;
	
	pImageMono = (LPBYTE)a_malloc(sizeof(BYTE) * nWidth * nHeight);
	if( NULL == pImageMono )
		return false;

	int nIndexOrg;
	int nIndex;

	int nXStart, nXEnd, nYStart, nYEnd;

	if( pSubRect )
	{
		nXStart = pSubRect->left;
		nYStart = pSubRect->top;
		nXEnd   = pSubRect->right;
		nYEnd	= pSubRect->bottom;
	}
	else
	{
		nXStart = 0;
		nYStart = 0;
		nXEnd	= nWidth;
		nYEnd	= nHeight;
	}

	if( bKeepEdge )
	{
		nXStart ++;
		nYStart ++;
		nXEnd   --;
		nYEnd	--;
	}

	for(int c=0; c<3; c++)
	{
		int x, y;
		for(y=nYStart; y<nYEnd; y++)
		{
			nIndex = y * nWidth + nXStart;
			nIndexOrg = nIndex * 3 + c;
			for(x=nXStart; x<nXEnd; x++)
			{
				float vSum = 0.0f;
				float v = 0.0f;

				if( !bKeepEdge )
				{
					// First left-up corner;
					if( x > nXStart && y > nYStart )
					{
						vSum += f00;
						v += pOrgImage[nIndexOrg - (nWidth + 1) * 3] * f00;
					}
					if( y > nYStart )
					{
						vSum += f01;
						v += pOrgImage[nIndexOrg - nWidth * 3] * f01;
					}
					if( x < nXEnd - 1 && y > nYStart )
					{
						vSum += f02;
						v += pOrgImage[nIndexOrg - (nWidth - 1) * 3] * f02;
					}
					if( x > nXStart )
					{
						vSum += f10;
						v += pOrgImage[nIndexOrg - 3] * f10;
					}
					vSum += f11;
					v += pOrgImage[nIndexOrg] * f11;
					if( x < nXEnd - 1 )
					{
						vSum += f12;
						v += pOrgImage[nIndexOrg + 3] * f12;
					}
					if( x > nXStart && y < nYEnd - 1 )
					{
						vSum += f20;
						v += pOrgImage[nIndexOrg + (nWidth - 1) * 3] * f20;
					}
					if( y < nYEnd - 1 )
					{
						vSum += f21;
						v += pOrgImage[nIndexOrg + nWidth * 3] * f21;
					}
					if( x < nXEnd - 1 && y < nYEnd - 1 )
					{
						vSum += f22;
						v += pOrgImage[nIndexOrg + (nWidth + 1) * 3] * f22;
					}
				}
				else
				{
					vSum += f00;
					v += pOrgImage[nIndexOrg - (nWidth + 1) * 3] * f00;
					vSum += f01;
					v += pOrgImage[nIndexOrg - nWidth * 3] * f01;
					vSum += f02;
					v += pOrgImage[nIndexOrg - (nWidth - 1) * 3] * f02;
					vSum += f10;
					v += pOrgImage[nIndexOrg - 3] * f10;
					vSum += f11;
					v += pOrgImage[nIndexOrg] * f11;
					vSum += f12;
					v += pOrgImage[nIndexOrg + 3] * f12;
					vSum += f20;
					v += pOrgImage[nIndexOrg + (nWidth - 1) * 3] * f20;
					vSum += f21;
					v += pOrgImage[nIndexOrg + nWidth * 3] * f21;
					vSum += f22;
					v += pOrgImage[nIndexOrg + (nWidth + 1) * 3] * f22;
				}

				v /= vSum;

				int nValue = int(v);
				pImageMono[nIndex] = min(nValue, 255);

				nIndex ++;
				nIndexOrg += 3;
			}
		}

		for(y=nYStart; y<nYEnd; y++)
		{
			nIndex = y * nWidth + nXStart;
			nIndexOrg = nIndex * 3 + c;
			for(x=nXStart; x<nXEnd; x++)
			{
				pOrgImage[nIndexOrg] = pImageMono[nIndex];

				nIndex ++;
				nIndexOrg += 3;
			}
		}
	}
	
	a_free(pImageMono);
	return true;
}
