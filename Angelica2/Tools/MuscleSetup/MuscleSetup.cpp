/**********************************************************************
 *<
	FILE: MuscleSetup.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "MuscleSetup.h"
#include "MuscleMod.h"

#define MUSCLESETUP_CLASS_ID	Class_ID(0x40bb834e, 0x6da119a0)

BOOL CALLBACK SetupMuscleDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ParamMuscleDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CurveWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TestMuscleDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK VertWeightDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// my sub animations
#define POINT1_REF	0
#define POINT2_REF	1

#define MUSCLE_REF	0
#define END_REF		1
#define WEIGHT_REF	2

#define SUBOBJ_NULL		0
#define SUBOBJ_MUSCLE	1
#define SUBOBJ_END		2
#define SUBOBJ_WEIGHT	3

static GenSubObjType SOT_AFR(0);
static GenSubObjType SOT_EndPoint(0);
static GenSubObjType SOT_Weight(0);

// panel index
#define SETUP_PANEL				0
#define TEST_PANEL				1
#define PARAM_PANEL				2
#define VERTWEIGHT_PANEL		3

class MuscleSetup : public Modifier 
{
	public:
		// Some ui windows for setup panel and test panel
		HWND				hSetupPanel;
		
		HWND				hTestPanel;
		HWND				hParamPanel;
		HWND				hVertWeightPanel;

		// sub object
		Control				*ctrlMuscle;
		Control				*ctrlEnd;
		BOOL				bMuscleSelected;
		BOOL				bEndSelected;
		BOOL				bWeightSelected;

		MoveModBoxCMode		*moveMode;
		RotateModBoxCMode	*rotateMode;
		BOOL				bOriginalMesh;  // TRUE to indicate do not modify the object
		BOOL				bSoloMuscle;	// TRUE to indicate we only do deform with current muscle
		BOOL				bShowWeight;	// TRUE to indicate we want to see the weight shown

		IObjParam			*ip;			//Access to the interface
		INode 				*pNode;			// node pointer to which this modifier apply
		
		BOOL				bInputChanged;	// input changed
		MuscleMod			theMuscleMod;	// muscle deformer

		// ui varaibles
		// in setup panel
		HWND				hMuscleListCtrl;
		int					nCurSetupMuscle;
		ICustButton *		pButtonOriginal;
		ICustButton *		pButtonSoloMuscle; // when pushed down, only use current muscle to do deforming
		ICustButton *		pButtonShowWeight; // when pushed down, only use current muscle to do deforming
		ICustButton *		pButtonAddMuscle;
		ICustButton *		pButtonCloneMuscle;
		ICustButton *		pButtonDelMuscle;
		ICustButton *		pButtonImport;
		ICustButton *		pButtonExport;

		// in param panel
		ICustEdit *			pEditName;
		ICustEdit *			pEditRadiusX;
		ICustEdit *			pEditRadiusY;
		ICustEdit *			pEditRadiusZ;
		ICustEdit *			pEditPitch;
		ICustEdit *			pEditLength;
		
		ISpinnerControl *	pSpinRadiusX;
		ISpinnerControl *	pSpinRadiusY;
		ISpinnerControl *	pSpinRadiusZ;
		ISpinnerControl *	pSpinPitch;
		ISpinnerControl *	pSpinLength;

		ICustButton *		pButtonX;
		ICustButton *		pButtonY;
		ICustButton *		pButtonZ;

		HWND				hCurveWnd;
		HWND				hComboWnd;
		
		// in test panel
		ICustEdit *			pEditTension;
		ISpinnerControl *	pSpinTension;

		// in vertex weight panel
		ICustButton *		pButtonAddVert;
		ICustButton *		pButtonRemoveVert;
		ICustEdit *			pEditWeight;
		ISpinnerControl *	pSpinWeight;

		// From Animatable
		TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }
		int SubNumToRefNum(int subNum);
		BOOL AssignController(Animatable *control,int subAnim);

		//From Modifier
		ChannelMask ChannelsUsed()  { return OBJ_CHANNELS; }
		//TODO: Add the channels that the modifier actually modifies
		ChannelMask ChannelsChanged() { return GEOM_CHANNEL | TOPO_CHANNEL | SELECT_CHANNEL | SUBSEL_TYPE_CHANNEL | VERTCOLOR_CHANNEL | DISP_ATTRIB_CHANNEL; }
		//TODO: Return the ClassID of the object that the modifier can modify
		Class_ID InputType() {return triObjectClassID;}

		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc);

		void NotifyPreCollapse(INode *node, IDerivedObject *derObj, int index);
		void NotifyPostCollapse(INode *node,Object *obj, IDerivedObject *derObj, int index);

		Interval LocalValidity(TimeValue t);

		// From BaseObject
		//TODO: Return true if the modifier changes topology
		BOOL ChangeTopology() {return FALSE;}		
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext *mc);
		void GetWorldBoundBox(TimeValue t,INode* inode, ViewExp *vpt, Box3& box, ModContext *mc);		
		void Move(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE);
		void Rotate(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin=FALSE);
		
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}

		// sub object manipulation
		void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		void ActivateSubobjSel(int level, XFormModes& modes);
		void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert=FALSE);
		void ClearSelection(int selLevel);
		void SelectAll(int selLevel);
		void InvertSelection(int selLevel);
		
		BOOL HasUVW();
		void SetGenUVW(BOOL sw);

		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

		Interval GetValidity(TimeValue t);

		// Automatic texture support
		
		// Loading/Saving
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		//From Animatable
		Class_ID ClassID() {return MUSCLESETUP_CLASS_ID;}		
		SClass_ID SuperClassID() { return OSM_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

		RefTargetHandle Clone( RemapDir &remap );
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
			PartID& partID,  RefMessage message);


		int NumSubs() { return 2; }
		TSTR SubAnimName(int i);
		Animatable* SubAnim(int i) {return GetReference(i);}

		// TODO: Maintain the number or references here
		int NumRefs() { return 2; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		int NumSubObjTypes();
		ISubObjType *GetSubObjType(int i);

		Matrix3 CompMatrix(TimeValue t,INode *inode,ModContext *mc);

		void DeleteThis() { delete this; }		
		//Constructor/Destructor

		MuscleSetup();
		~MuscleSetup();		

		// update data between ui and memory
		bool UpdateSetupData(bool bSave);
		bool UpdateParamData(bool bSave);
		bool UpdateTestData(bool bSave);
		bool UpdateVertWeightData(bool bSave);

		bool UpdateControlPoints(bool bSave);
		bool UpdateVertexWeights(bool bSave);

		// drawing methods.
		void ShowCurve(HDC hdc, Rect& rect);

		// set correct panels according to current sub-object level
		void AdjustPanels(int level);
		void ReLocateToPanel(HWND hPanel);

		void AddSetupPanel();
		void AddTestPanel();
		void AddParamPanel();
		void AddVertWeightPanel();

		void RemoveSetupPanel();
		void RemoveTestPanel();
		void RemoveParamPanel();
		void RemoveVertWeightPanel();

	public:
		// my ui processing functions
		bool AddMuscle(BOOL bCloneCurrent);
		bool DelMuscle(int nIndex);
		bool MirrorMuscle(int nAxis);

		bool Import(const char * szFile);
		bool Export(const char * szFile);

		bool ExportToEngine(const char * szFile);

		bool ReEvaluate();		// update muscles data and re-modify the object
		bool RefreshObject();	// re-modify the object

		bool GenRandMuscleTensions();

		int GetCurSetupMuscle() { return nCurSetupMuscle; }
		bool SetCurSetupMuscle(int nIndex);

		bool ToggleMuscleLink(int indexHost, int indexChild);

	protected:
		// create a list of envelope points
		// return actual number of points.
		int GetEnvelopePoints(int nNumSegments, Point3 * pts, int idPlane);
};

class MuscleSetupClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new MuscleSetup(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return MUSCLESETUP_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("MuscleSetup"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

static MuscleSetupClassDesc MuscleSetupDesc;
ClassDesc2* GetMuscleSetupDesc() { return &MuscleSetupDesc; }

//--- MuscleSetup -------------------------------------------------------
MuscleSetup::MuscleSetup()
: theMuscleMod()
{
	ip				= NULL;
	hSetupPanel		= NULL;
	hTestPanel		= NULL;
	hParamPanel		= NULL;
	hVertWeightPanel = NULL;
	bOriginalMesh	= FALSE;
	bSoloMuscle		= FALSE;
	bShowWeight		= FALSE;
	pNode			= NULL;
	moveMode		= NULL;
	rotateMode		= NULL;

	bInputChanged	= TRUE;
	nCurSetupMuscle = -1;

	ctrlMuscle = NULL;
	ctrlEnd = NULL;
	MakeRefByID(FOREVER, MUSCLE_REF, NewDefaultMatrix3Controller());
	MakeRefByID(FOREVER, END_REF, NewDefaultPoint3Controller());
}

MuscleSetup::~MuscleSetup()
{
}

Matrix3 MuscleSetup::CompMatrix(TimeValue t,INode *inode,ModContext *mc)
{
	Interval iv;
	Matrix3 tm(1);	
	if (mc && mc->tm) tm = Inverse(*(mc->tm));
	if (inode) tm = tm * inode->GetObjTMBeforeWSM(t,&iv);
	return tm;
}

// sub object manipulation
RefTargetHandle MuscleSetup::GetReference(int i)
{
	switch (i) 
	{
	case MUSCLE_REF:
		return ctrlMuscle;

	case END_REF:
		return ctrlEnd;

	default: return NULL;
	}
}

void MuscleSetup::SetReference(int i, RefTargetHandle rtarg)
{
	// this function is called during a call to MakeRefByID
	switch (i) 
	{
	case MUSCLE_REF:
		ctrlMuscle = (Control *)rtarg;
		break;

	case END_REF:
		ctrlEnd = (Control *)rtarg;
		break;
	}
}

TSTR MuscleSetup::SubAnimName(int i)
{
	switch (i) 
	{
	case MUSCLE_REF:
		return GetString(IDS_MUSCLE);
		break;

	case END_REF:
		return GetString(IDS_MUSCLEEND);
		break;

	default: return _T(""); break;
	}
}

BOOL MuscleSetup::AssignController(Animatable *control,int subAnim)
{
	ReplaceReference(subAnim,(Control*)control);
	NotifyDependents(FOREVER,PART_GEOM,REFMSG_CHANGE);
	NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	return TRUE;
}

int MuscleSetup::SubNumToRefNum(int subNum)
{
	switch (subNum) 
	{
	case MUSCLE_REF:
		return MUSCLE_REF;

	case END_REF:
		return END_REF;
	}

	return -1;
}

int MuscleSetup::NumSubObjTypes() 
{ 
	return 2;
}

ISubObjType *MuscleSetup::GetSubObjType(int i) 
{
	static bool initialized = false;
	if(!initialized)
	{
		initialized = true;
		SOT_AFR.SetName(GetString(IDS_MUSCLECTRLAFR));
		SOT_Weight.SetName(GetString(IDS_MUSCLECTRLWEIGHT));
		SOT_EndPoint.SetName(GetString(IDS_MUSCLECTRLEND));
	}

	switch(i)
	{
	case 0:
		return &SOT_AFR;
	case 1:
		return &SOT_EndPoint;
	case 2:
		return &SOT_Weight;
	
	}
	return NULL;
}

/*===========================================================================*\
 |	The validity of the parameters.  First a test for editing is performed
 |  then Start at FOREVER, and intersect with the validity of each item
\*===========================================================================*/
Interval MuscleSetup::LocalValidity(TimeValue t)
{
	Interval iv = FOREVER;
	Matrix3 tm;
	ctrlMuscle->GetValue(t, &tm, iv, CTRL_RELATIVE);
	Point3 pt;
	ctrlEnd->GetValue(t, &pt, iv, CTRL_ABSOLUTE);
	return iv;
}


/*************************************************************************************************
*
	Between NotifyPreCollapse and NotifyPostCollapse, Modify is
	called by the system.  NotifyPreCollapse can be used to save any plugin dependant data e.g.
	LocalModData
*
\*************************************************************************************************/

void MuscleSetup::NotifyPreCollapse(INode *node, IDerivedObject *derObj, int index)
{
	//TODO:  Perform any Pre Stack Collapse methods here
}



/*************************************************************************************************
*
	NotifyPostCollapse can be used to apply the modifier back onto to the stack, copying over the
	stored data from the temporary storage.  To reapply the modifier the following code can be 
	used

	Object *bo = node->GetObjectRef();
	IDerivedObject *derob = NULL;
	if(bo->SuperClassID() != GEN_DERIVOB_CLASS_ID)
	{
		derob = CreateDerivedObject(obj);
		node->SetObjectRef(derob);
	}
	else
		derob = (IDerivedObject*) bo;

	// Add ourselves to the top of the stack
	derob->AddModifier(this,NULL,derob->NumModifiers());

*
\*************************************************************************************************/

void MuscleSetup::NotifyPostCollapse(INode *node,Object *obj, IDerivedObject *derObj, int index)
{
	//TODO: Perform any Post Stack collapse methods here.

}


/*************************************************************************************************
*
	ModifyObject will do all the work in a full modifier
    This includes casting objects to their correct form, doing modifications
	changing their parameters, etc
*
\************************************************************************************************/


void MuscleSetup::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
{
	Interval valid = FOREVER;
	assert(os->obj->IsSubClassOf(triObjectClassID));
	TriObject * triObj = (TriObject *) os->obj;

	// now take down my objectstate
	if( bInputChanged )
	{
		Matrix3 tmMC = *(mc.tm);
		theMuscleMod.SetupMesh(tmMC, triObj->mesh.getNumVerts(), triObj->mesh.verts);
		bInputChanged = FALSE;
	}
	
	/*
	// just a simple noise deformation for test
	triObj->mesh.selLevel = 0;
	triObj->mesh.dispFlags = 0;
	triObj->mesh.vertSel.ClearAll();
	triObj->mesh.faceSel.ClearAll();
	triObj->mesh.edgeSel.ClearAll();

	if( !bOriginalMesh )
	{
		// now deform the object
		for(int i=0; i<triObj->mesh.getNumVerts(); i++)
		{
			// do a noise bump
			const Point3& vert = triObj->mesh.getVert(i);

			triObj->mesh.setVert(i, vert * (1.0f + (rand() % 100) / 1000.0f));
		}
	}*/

	if( !bOriginalMesh )
	{
		assert( triObj->mesh.getNumVerts() == theMuscleMod.GetNumVerts() );
		theMuscleMod.DeformMesh(bSoloMuscle ? GetCurSetupMuscle() : -1);
		Point3 * pNewVerts = theMuscleMod.GetDeformedVerts();

		if( theMuscleMod.GetNumMuscles() )
		{
			Matrix3 tmMCInv = *mc.tm;
			tmMCInv.Invert();
			for(int i=0; i<triObj->mesh.getNumVerts(); i++)
			{
				triObj->mesh.setVert(i, pNewVerts[i] * tmMCInv);
			}
		}
	}
	
	triObj->UpdateValidity(GEOM_CHAN_NUM, valid);
	triObj->UpdateValidity(TOPO_CHAN_NUM, valid);
	triObj->UpdateValidity(SELECT_CHAN_NUM, valid);
	triObj->UpdateValidity(SUBSEL_TYPE_CHAN_NUM, FOREVER);
	triObj->UpdateValidity(DISP_ATTRIB_CHAN_NUM, FOREVER);

	// now try to make the mesh colorful
	if( ip )
	{
		ModContextList	list;
		INodeTab		NodeTab;

		ip->GetModContexts(list, NodeTab);

		int i;

		if( bShowWeight )
		{
			for(i = 0 ; i < NodeTab.Count() ; i++)
			{
				NodeTab[i]->SetShadeCVerts(TRUE);
				NodeTab[i]->SetCVertMode(TRUE);	
			}

			// now make the mesh colorful.
			triObj->mesh.setNumVCFaces(triObj->mesh.getNumFaces());
			triObj->mesh.setNumVertCol(triObj->mesh.getNumVerts());
			
			for(i=0; i<triObj->mesh.getNumVerts(); i++)
			{
				triObj->mesh.vertCol[i] = Point3(0.0f, 1.0f, 0.0f);
			}

			if( GetCurSetupMuscle() >= 0 )
			{
				int		nAFRVerts;
				int *	pAFRIndices;
				float *	pAFRWeights;
				theMuscleMod.GetMuscleAFRInfo(GetCurSetupMuscle(), nAFRVerts, pAFRIndices, pAFRWeights);
				for(i=0; i<nAFRVerts; i++)
				{
					float w = fabs(pAFRWeights[i]);
					triObj->mesh.vertCol[pAFRIndices[i]] = Point3(w, 0.0f, 1.0f - w);
				}
			}

			for(i=0; i<triObj->mesh.getNumFaces(); i++)
			{
				triObj->mesh.vcFace[i].t[0] = triObj->mesh.faces[i].v[0];
				triObj->mesh.vcFace[i].t[1] = triObj->mesh.faces[i].v[1];
				triObj->mesh.vcFace[i].t[2] = triObj->mesh.faces[i].v[2];
			}

			triObj->UpdateValidity(VERT_COLOR_CHAN_NUM, Interval(t,t));
		}
		else
		{
			int i;
			for(i=0; i<NodeTab.Count(); i++)
			{
				NodeTab[i]->SetShadeCVerts(FALSE);
				NodeTab[i]->SetCVertMode(FALSE);	
			}
		}
	}
}

void MuscleSetup::AddSetupPanel()
{
	// now let these windows show up in the command panel
	if (!hSetupPanel)
	{
		hSetupPanel = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_PANELSETUP),
				SetupMuscleDlgProc, 
				GetString(IDS_PANELSETUP), 
				(LPARAM)this);		
		ip->RegisterDlgWnd(hSetupPanel);

		hMuscleListCtrl = GetDlgItem(hSetupPanel, IDC_LISTCTRLMUSCLES);
		ListView_SetBkColor(hMuscleListCtrl, GetCustSysColor(COLOR_WINDOW));
		ListView_SetTextBkColor(hMuscleListCtrl, GetCustSysColor(COLOR_WINDOW));
		ListView_SetTextColor(hMuscleListCtrl, GetCustSysColor(COLOR_WINDOWTEXT));
		ListView_SetIconSpacing(hMuscleListCtrl, 20, 20);

		HBITMAP		hbm;
		HIMAGELIST hImgMuscles = ImageList_Create(16, 16, ILC_COLOR24, 3, 0);
		HIMAGELIST hImgLinks = ImageList_Create(16, 16, ILC_COLOR24, 2, 0);

		hbm = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAPDIRSINGLE));
		ImageList_Add(hImgMuscles, hbm, NULL);
		DeleteObject(hbm);
		hbm = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAPDIRDOUBLE));
		ImageList_Add(hImgMuscles, hbm, NULL);
		DeleteObject(hbm);
		hbm = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAPDIRCIRCLE));
		ImageList_Add(hImgMuscles, hbm, NULL);
		DeleteObject(hbm);
		
		hbm = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAPNOLINK));
		ImageList_Add(hImgLinks, hbm, NULL);
		DeleteObject(hbm);
		hbm = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAPLINK));
		ImageList_Add(hImgLinks, hbm, NULL);
		DeleteObject(hbm);
		hbm = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAPDISABLEDLINK));
		ImageList_Add(hImgLinks, hbm, NULL);
		DeleteObject(hbm);

		ListView_SetImageList(hMuscleListCtrl, hImgMuscles, LVSIL_SMALL);
		SendMessage(hMuscleListCtrl, LVM_SETIMAGELIST, LVSIL_STATE, (LPARAM)hImgLinks);

		// setup to the muscles list and select the last one.
		for(int i=0; i<theMuscleMod.GetNumMuscles(); i++)
		{
			MUSCLEDATA muscle = theMuscleMod.GetMuscleData(i);
			
			LVITEM item;
			memset(&item, 0, sizeof(item));
			item.mask = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
			item.iItem = i;
			item.state = INDEXTOSTATEIMAGEMASK(1);
			item.iImage = (int) muscle.type;
			item.stateMask = LVIS_STATEIMAGEMASK;
			item.pszText = muscle.szName;
			ListView_InsertItem(hMuscleListCtrl, &item);
		}

		// now set up the cust control objects
		pButtonOriginal = GetICustButton(GetDlgItem(hSetupPanel, IDC_BUTTONORIGINAL));
		pButtonOriginal->SetType(CBT_CHECK);
		pButtonOriginal->SetHighlightColor(GREEN_WASH);
		pButtonOriginal->SetCheck(bOriginalMesh);

		pButtonSoloMuscle = GetICustButton(GetDlgItem(hSetupPanel, IDC_BUTTONSOLOMUSCLE));
		pButtonSoloMuscle->SetType(CBT_CHECK);
		pButtonSoloMuscle->SetHighlightColor(GREEN_WASH);
		pButtonSoloMuscle->SetCheck(bSoloMuscle);

		pButtonShowWeight = GetICustButton(GetDlgItem(hSetupPanel, IDC_BUTTONSHOWWEIGHT));
		pButtonShowWeight->SetType(CBT_CHECK);
		pButtonShowWeight->SetHighlightColor(GREEN_WASH);
		pButtonShowWeight->SetCheck(bShowWeight);
		
		pButtonAddMuscle = GetICustButton(GetDlgItem(hSetupPanel, IDC_BUTTONADD));
		pButtonCloneMuscle = GetICustButton(GetDlgItem(hSetupPanel, IDC_BUTTONCLONE));
		pButtonDelMuscle = GetICustButton(GetDlgItem(hSetupPanel, IDC_BUTTONDEL));
		pButtonImport = GetICustButton(GetDlgItem(hSetupPanel, IDC_BUTTONIMPORT));
		pButtonExport = GetICustButton(GetDlgItem(hSetupPanel, IDC_BUTTONEXPORT));
	} 
	else
	{
		SetWindowLong(hSetupPanel,GWL_USERDATA,(LONG)this);
	}
}
						   
void MuscleSetup::RemoveSetupPanel()
{
	if( !hSetupPanel )
		return;		 

	ReleaseICustButton(pButtonAddMuscle);
	ReleaseICustButton(pButtonCloneMuscle);
	ReleaseICustButton(pButtonDelMuscle);
	ReleaseICustButton(pButtonImport);
	ReleaseICustButton(pButtonExport);
	ReleaseICustButton(pButtonOriginal);
	ReleaseICustButton(pButtonSoloMuscle);
	ReleaseICustButton(pButtonShowWeight);
	
	if( hSetupPanel )
	{
		ip->UnRegisterDlgWnd(hSetupPanel);
		ip->DeleteRollupPage(hSetupPanel);
		hSetupPanel = NULL;
	}

	hMuscleListCtrl = NULL;
}

void MuscleSetup::AddTestPanel()
{
	if (!hTestPanel)
	{
		hTestPanel = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_PANELTEST),
				TestMuscleDlgProc, 
				GetString(IDS_PANELTEST), 
				(LPARAM)this);		
		ip->RegisterDlgWnd(hTestPanel);
		pEditTension = GetICustEdit(GetDlgItem(hTestPanel, IDC_EDITTENSION));
		pSpinTension = GetISpinner(GetDlgItem(hTestPanel, IDC_SPINTENSION));
		pSpinTension->LinkToEdit(GetDlgItem(hTestPanel, IDC_EDITTENSION), EDITTYPE_FLOAT); 
		pSpinTension->SetLimits(-2.0f, 2.0f);
		pSpinTension->SetAutoScale();
	} 
	else
	{
		SetWindowLong(hTestPanel,GWL_USERDATA,(LONG)this);
	}
}

void MuscleSetup::RemoveTestPanel()
{
	if( !hTestPanel )
		return;

	ReleaseISpinner(pSpinTension);
	ReleaseICustEdit(pEditTension);

	if( hTestPanel )
	{
		ip->UnRegisterDlgWnd(hTestPanel);
		ip->DeleteRollupPage(hTestPanel);
		hTestPanel = NULL;
	}
}

void MuscleSetup::AddParamPanel()
{
	if (!hParamPanel)
	{
		hParamPanel = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_PANELPARAM),
				ParamMuscleDlgProc, 
				GetString(IDS_PANELPARAM), 
				(LPARAM)this);		
		ip->RegisterDlgWnd(hParamPanel);

		hComboWnd = GetDlgItem(hParamPanel, IDC_COMBOTYPE);
		SendMessage(hComboWnd, CB_ADDSTRING, 0, (LPARAM)"单向");
		SendMessage(hComboWnd, CB_ADDSTRING, 0, (LPARAM)"双向");
		SendMessage(hComboWnd, CB_ADDSTRING, 0, (LPARAM)"环形");
		SendMessage(hComboWnd, CB_SETCURSEL, 0, 0);

		// now set up the cust control objects
		pEditName = GetICustEdit(GetDlgItem(hParamPanel, IDC_EDITNAME));

		pEditRadiusX = GetICustEdit(GetDlgItem(hParamPanel, IDC_EDITRADIUSX));
		pSpinRadiusX = GetISpinner(GetDlgItem(hParamPanel, IDC_SPINRADIUSX));
		pSpinRadiusX->SetLimits(0.0f, 100.0f);
		pSpinRadiusX->LinkToEdit(GetDlgItem(hParamPanel, IDC_EDITRADIUSX), EDITTYPE_FLOAT); 
		pSpinRadiusX->SetAutoScale();

		pEditRadiusY = GetICustEdit(GetDlgItem(hParamPanel, IDC_EDITRADIUSY));
		pSpinRadiusY = GetISpinner(GetDlgItem(hParamPanel, IDC_SPINRADIUSY));
		pSpinRadiusY->SetLimits(0.0f, 100.0f);
		pSpinRadiusY->LinkToEdit(GetDlgItem(hParamPanel, IDC_EDITRADIUSY), EDITTYPE_FLOAT); 
		pSpinRadiusY->SetAutoScale();

		pEditRadiusZ = GetICustEdit(GetDlgItem(hParamPanel, IDC_EDITRADIUSZ));
		pSpinRadiusZ = GetISpinner(GetDlgItem(hParamPanel, IDC_SPINRADIUSZ));
		pSpinRadiusZ->SetLimits(0.0f, 100.0f);
		pSpinRadiusZ->LinkToEdit(GetDlgItem(hParamPanel, IDC_EDITRADIUSZ), EDITTYPE_FLOAT); 
		pSpinRadiusZ->SetAutoScale();

		pEditPitch = GetICustEdit(GetDlgItem(hParamPanel, IDC_EDITPITCH));
		pSpinPitch = GetISpinner(GetDlgItem(hParamPanel, IDC_SPINPITCH));
		pSpinPitch->SetLimits(0.0f, 100.0f);
		pSpinPitch->LinkToEdit(GetDlgItem(hParamPanel, IDC_EDITPITCH), EDITTYPE_FLOAT); 
		pSpinPitch->SetAutoScale();

		pEditLength = GetICustEdit(GetDlgItem(hParamPanel, IDC_EDITLENGTH));
		pSpinLength = GetISpinner(GetDlgItem(hParamPanel, IDC_SPINLENGTH));
		pSpinLength->SetLimits(0.0f, 100.0f);
		pSpinLength->LinkToEdit(GetDlgItem(hParamPanel, IDC_EDITLENGTH), EDITTYPE_FLOAT); 
		pSpinLength->SetAutoScale();

		pButtonX = GetICustButton(GetDlgItem(hParamPanel, IDC_BUTTONX));
		pButtonY = GetICustButton(GetDlgItem(hParamPanel, IDC_BUTTONY));
		pButtonZ = GetICustButton(GetDlgItem(hParamPanel, IDC_BUTTONZ));

		hCurveWnd = GetDlgItem(hParamPanel, IDC_STATICCURVE);
		SetWindowLong(hCurveWnd, GWL_WNDPROC, (LONG)CurveWndProc);
		SetWindowLong(hCurveWnd, GWL_USERDATA, (LONG)this);
	}
	else
	{
		SetWindowLong(hParamPanel,GWL_USERDATA,(LONG)this);
	}
}

void MuscleSetup::RemoveParamPanel()
{
	if( !hParamPanel )
		return;

	ReleaseICustEdit(pEditRadiusX);
	ReleaseICustEdit(pEditRadiusY);
	ReleaseICustEdit(pEditRadiusZ);
	ReleaseICustEdit(pEditPitch);
	ReleaseICustEdit(pEditLength);
	ReleaseISpinner(pSpinRadiusX);
	ReleaseISpinner(pSpinRadiusY);
	ReleaseISpinner(pSpinRadiusZ);
	ReleaseISpinner(pSpinPitch);
	ReleaseISpinner(pSpinLength);
	ReleaseICustEdit(pEditName);
	ReleaseICustButton(pButtonX);
	ReleaseICustButton(pButtonY);
	ReleaseICustButton(pButtonZ);

	if( hParamPanel )
	{
		ip->UnRegisterDlgWnd(hParamPanel);
		ip->DeleteRollupPage(hParamPanel);
		hParamPanel = NULL;
	}

	if( hCurveWnd )
	{
		SetWindowLong(hCurveWnd, GWL_WNDPROC, (LONG) NULL);
		SetWindowLong(hCurveWnd, GWL_USERDATA, (LONG)NULL);
		hCurveWnd = NULL;
	}

	if( hComboWnd )
	{
		SendMessage(hComboWnd, CB_DELETESTRING, 2, 0);
		SendMessage(hComboWnd, CB_DELETESTRING, 1, 0);
		SendMessage(hComboWnd, CB_DELETESTRING, 0, 0);
		hComboWnd = NULL;
	}
}

void MuscleSetup::AddVertWeightPanel()
{
	if (!hVertWeightPanel)
	{
		hVertWeightPanel = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_PANELVERTWEIGHT),
				VertWeightDlgProc, 
				GetString(IDS_PANELVERTWEIGHT), 
				(LPARAM)this);		
		ip->RegisterDlgWnd(hVertWeightPanel);

		pButtonAddVert = GetICustButton(GetDlgItem(hVertWeightPanel, IDC_BUTTONADDVERT));
		pButtonAddVert->SetType(CBT_CHECK);
		pButtonAddVert->SetHighlightColor(GREEN_WASH);
		pButtonAddVert->SetCheck(TRUE);

		pButtonRemoveVert = GetICustButton(GetDlgItem(hVertWeightPanel, IDC_BUTTONREMOVEVERT));
		pButtonRemoveVert->SetType(CBT_CHECK);
		pButtonRemoveVert->SetHighlightColor(GREEN_WASH);
		pButtonRemoveVert->SetCheck(TRUE);

		pEditWeight = GetICustEdit(GetDlgItem(hVertWeightPanel, IDC_EDITWEIGHT));
		pSpinWeight = GetISpinner(GetDlgItem(hVertWeightPanel, IDC_SPINWEIGHT));
		pSpinWeight->SetLimits(0.0f, 1.0f);
		pSpinWeight->LinkToEdit(GetDlgItem(hVertWeightPanel, IDC_EDITWEIGHT), EDITTYPE_FLOAT); 
		pSpinWeight->SetAutoScale();
	} 
	else
	{
		SetWindowLong(hVertWeightPanel,GWL_USERDATA,(LONG)this);
	}
}

void MuscleSetup::RemoveVertWeightPanel()
{
	if( !hVertWeightPanel )
		return;

	ReleaseISpinner(pSpinWeight);
	ReleaseICustEdit(pEditWeight);
	ReleaseICustButton(pButtonAddVert);
	ReleaseICustButton(pButtonRemoveVert);

	if( hVertWeightPanel )
	{
		ip->UnRegisterDlgWnd(hVertWeightPanel);
		ip->DeleteRollupPage(hVertWeightPanel);
		hVertWeightPanel = NULL;
	}
}

void MuscleSetup::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	this->ip = ip;

	// Create sub object editing modes.
	moveMode    = new MoveModBoxCMode(this,ip);
	rotateMode	= new RotateModBoxCMode(this, ip);

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);

	AddSetupPanel();
	AddTestPanel();
	AddParamPanel();

	// fill up these panels.
	SetCurSetupMuscle(GetCurSetupMuscle());
}

void MuscleSetup::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	// NOTE: This flag must be cleared before sending the REFMSG_END_EDIT
	ClearAFlag(A_MOD_BEING_EDITED);

	ModContextList	list;
	INodeTab		NodeTab;
	ip->GetModContexts(list, NodeTab);
	for(int i = 0 ; i < NodeTab.Count() ; i++)
	{
		NodeTab[i]->SetShadeCVerts(FALSE);
		NodeTab[i]->SetCVertMode(FALSE);	
	}
	
	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);

	ip->DeleteMode(rotateMode);
	if( rotateMode ) delete rotateMode;
	rotateMode = NULL;

	ip->DeleteMode(moveMode);	
	if (moveMode) delete moveMode;
	moveMode = NULL;	

	RemoveSetupPanel();
	RemoveTestPanel();
	RemoveParamPanel();
	RemoveVertWeightPanel();

	this->ip = NULL;
}



Interval MuscleSetup::GetValidity(TimeValue t)
{
	Interval valid = FOREVER;
	//TODO: Return the validity interval of the modifier
	return valid;
}

RefTargetHandle MuscleSetup::Clone(RemapDir& remap)
{
	MuscleSetup* newmod = new MuscleSetup();	
	newmod->ReplaceReference(MUSCLE_REF, ctrlMuscle->Clone(remap));
	newmod->ReplaceReference(END_REF, ctrlEnd->Clone(remap));
	BaseClone(this, newmod, remap);
	return(newmod);
}


//From ReferenceMaker 
RefResult MuscleSetup::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
{
	if( message == REFMSG_CHANGE )
	{
		if( hTarget == (RefTargetHandle) ctrlMuscle )
		{
			UpdateControlPoints(true);
		}
		else if( hTarget == (RefTargetHandle) ctrlEnd )
		{
			UpdateControlPoints(true);
			UpdateParamData(false);
		}
	}
	
	return REF_SUCCEED;
}

/****************************************************************************************
*
 	NotifyInputChanged is called each time the input object is changed in some way
 	We can find out how it was changed by checking partID and message
*
\****************************************************************************************/

void MuscleSetup::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
{
	bInputChanged = TRUE;
}

//From Object
BOOL MuscleSetup::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}

void MuscleSetup::SetGenUVW(BOOL sw) 
{  
	if (sw==HasUVW()) return;
	//TODO: Set the plugin internal value to sw				
}

#define MUSCLEDATACHUNK_ID 0x1000

bool MuscleSetup::Import(const char * szFile)
{
	ULONG handle;
	NotifyDependents(FOREVER, (PartID)&handle, REFMSG_GET_NODE_HANDLE);
	INode *node;
	node = ip->GetINodeByHandle(handle);

	Interval iv;
	Matrix3 objTM = node->GetObjTMBeforeWSM(ip->GetTime(), &iv);
	Matrix3 tmToMod = Inverse(objTM) * theMuscleMod.GetModTM();

	FILE * pFile = fopen(szFile, "rb");
	if( NULL == pFile )
		return false;

	// first remove old muscles.
	theMuscleMod.RemoveAllMuscles();
	
	// now load new muscles.
	int		i;
	int		nIndex;

	DWORD dwVersion;
	fread(&dwVersion, sizeof(dwVersion), 1, pFile);
	if( dwVersion == 0x10003005 )
	{
		int nNumMuscles = 0;
		fread(&nNumMuscles, sizeof(nNumMuscles), 1, pFile);
		int nIDSeed = 0;
		fread(&nIDSeed, sizeof(nIDSeed), 1, pFile);
		theMuscleMod.SetIDSeed(nIDSeed);

		Point3	vecCenter;
		Point3	vecMuscleDir;
		Point3	vecEnd;

		Point3	vecXAxis;
		Point3	vecYAxis;
		Point3	vecZAxis;
		Point3	vecRadius;
		for(i=0; i<nNumMuscles; i++)
		{
			MUSCLEDATA muscle;

			fread(muscle.szName, 32, 1, pFile);

			fread(&vecCenter, sizeof(vecCenter), 1, pFile);
			fread(&vecMuscleDir, sizeof(vecMuscleDir), 1, pFile);
			fread(&vecEnd, sizeof(vecEnd), 1, pFile);
			
			fread(&vecXAxis, sizeof(vecXAxis), 1, pFile);
			fread(&vecYAxis, sizeof(vecYAxis), 1, pFile);
			fread(&vecZAxis, sizeof(vecZAxis), 1, pFile);
			fread(&vecRadius, sizeof(vecRadius), 1, pFile);

			fread(&muscle.vPitch, sizeof(muscle.vPitch), 1, pFile);
			fread(&muscle.type, sizeof(muscle.type), 1, pFile);

			fread(&muscle.vTension, sizeof(muscle.vTension), 1, pFile);
			int id;
			fread(&id, sizeof(id), 1, pFile);

			muscle.vecCenter = tmToMod.PointTransform(vecCenter);
			muscle.vecMuscleDir = Normalize(tmToMod.VectorTransform(vecMuscleDir));

			vecEnd = tmToMod.PointTransform(vecEnd);
			muscle.vLength = (vecEnd - muscle.vecCenter).Length();

			muscle.vecXAxis = tmToMod.VectorTransform(vecXAxis);
			muscle.vecYAxis = tmToMod.VectorTransform(vecYAxis);
			muscle.vecZAxis = tmToMod.VectorTransform(vecZAxis);
			muscle.vecRadius.x = vecRadius.x * muscle.vecXAxis.Length();
			muscle.vecRadius.y = vecRadius.y * muscle.vecYAxis.Length();
			muscle.vecRadius.z = vecRadius.z * muscle.vecZAxis.Length();
			muscle.vecXAxis = Normalize(muscle.vecXAxis);
			muscle.vecYAxis = Normalize(muscle.vecYAxis);
			muscle.vecZAxis = Normalize(muscle.vecZAxis);

			theMuscleMod.AddLoadedMuscle(muscle, id, nIndex);
		}

		// now rebuild the link architecture
		for(i=0; i<nNumMuscles; i++)
		{
			int nLink;
			fread(&nLink, sizeof(nLink), 1, pFile);
			for(int j=0; j<nLink; j++)
			{
				int muscleID;
				fread(&muscleID, sizeof(muscleID), 1, pFile);

				int nChildIndex = theMuscleMod.FindMuscleByID(muscleID);
				if( nChildIndex != -1 )
					theMuscleMod.AddMuscleLink(i, nChildIndex);
			}
		}
	}

	fclose(pFile);

	ListView_DeleteAllItems(hMuscleListCtrl);
	for(i=0; i<theMuscleMod.GetNumMuscles(); i++)
	{
		MUSCLEDATA muscle = theMuscleMod.GetMuscleData(i);
		
		LVITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
		item.iItem = i;
		item.state = INDEXTOSTATEIMAGEMASK(1);
		item.iImage = (int) muscle.type;
		item.stateMask = LVIS_STATEIMAGEMASK;
		item.pszText = muscle.szName;
		ListView_InsertItem(hMuscleListCtrl, &item);
	}
	SetCurSetupMuscle(theMuscleMod.GetNumMuscles() - 1);
	return true;
}

bool MuscleSetup::Export(const char * szFile)
{
	// First get node pointer;
	ULONG handle;
	NotifyDependents(FOREVER, (PartID)&handle, REFMSG_GET_NODE_HANDLE);
	INode *node;
	node = ip->GetINodeByHandle(handle);

	Interval iv;
	Matrix3 objTM = node->GetObjTMBeforeWSM(ip->GetTime(), &iv);
	Matrix3 tmToWorld = Inverse(theMuscleMod.GetModTM()) * objTM;
	
	FILE * pFile = fopen(szFile, "wb");
	if( NULL == pFile )
		return false;

	int		i;
	// first save my version
	DWORD	dwVersion = MUSCLESYSTEM_VERSION;
	fwrite(&dwVersion, sizeof(dwVersion), 1, pFile);
	
	int nNumMuscles = theMuscleMod.GetNumMuscles();
	fwrite(&nNumMuscles, sizeof(nNumMuscles), 1, pFile);
	int nIDSeed = theMuscleMod.GetIDSeed();
	fwrite(&nIDSeed, sizeof(nIDSeed), 1, pFile);

	Point3	vecCenter;
	Point3	vecMuscleDir;
	Point3	vecEnd;

	Point3	vecXAxis;
	Point3	vecYAxis;
	Point3	vecZAxis;
	Point3	vecRadius;
	for(i=0; i<nNumMuscles; i++)
	{
		MUSCLEDATA muscle = theMuscleMod.GetMuscleData(i);

		vecCenter		= tmToWorld.PointTransform(muscle.vecCenter);
		vecMuscleDir	= Normalize(tmToWorld.VectorTransform(muscle.vecMuscleDir));
		vecEnd 			= tmToWorld.PointTransform(muscle.vecCenter - muscle.vLength * muscle.vecMuscleDir);
		vecXAxis		= tmToWorld.VectorTransform(muscle.vecXAxis);
		vecYAxis		= tmToWorld.VectorTransform(muscle.vecYAxis);
		vecZAxis		= tmToWorld.VectorTransform(muscle.vecZAxis);
		vecRadius.x		= muscle.vecRadius.x * vecXAxis.Length();
		vecRadius.y		= muscle.vecRadius.y * vecYAxis.Length();
		vecRadius.z		= muscle.vecRadius.z * vecZAxis.Length();
		vecXAxis		= Normalize(vecXAxis);
		vecYAxis		= Normalize(vecYAxis);
		vecZAxis		= Normalize(vecZAxis);
		
		fwrite(muscle.szName, 32, 1, pFile);

		fwrite(&vecCenter, sizeof(vecCenter), 1, pFile);
		fwrite(&vecMuscleDir, sizeof(vecMuscleDir), 1, pFile);
		fwrite(&vecEnd, sizeof(vecEnd), 1, pFile);

		fwrite(&vecXAxis, sizeof(vecXAxis), 1, pFile);
		fwrite(&vecYAxis, sizeof(vecYAxis), 1, pFile);
		fwrite(&vecZAxis, sizeof(vecZAxis), 1, pFile);
		fwrite(&vecRadius, sizeof(vecRadius), 1, pFile);

		fwrite(&muscle.vPitch, sizeof(muscle.vPitch), 1, pFile);
		fwrite(&muscle.type, sizeof(muscle.type), 1, pFile);

		fwrite(&muscle.vTension, sizeof(muscle.vTension), 1, pFile);

		int id = theMuscleMod.GetMuscleID(i);
		fwrite(&id, sizeof(id), 1, pFile);
	}

	// now save these muscle's link information
	for(i=0; i<nNumMuscles; i++)
	{
		int nLink = theMuscleMod.GetNumLinkedMuscles(i);
		fwrite(&nLink, sizeof(nLink), 1, pFile);

		for(int j=0; j<nLink; j++)
		{
			int muscleID = theMuscleMod.GetLinkedMuscleID(i, j);
			fwrite(&muscleID, sizeof(muscleID), 1, pFile);
		}
	}

	fclose(pFile);
	return true;
}

bool MuscleSetup::ExportToEngine(const char * szFile)
{
	// First get node pointer;
	ULONG handle;
	NotifyDependents(FOREVER, (PartID)&handle, REFMSG_GET_NODE_HANDLE);
	INode *node;
	node = ip->GetINodeByHandle(handle);

	Interval iv;
	Matrix3 objTM = node->GetObjTMBeforeWSM(ip->GetTime(), &iv);
	Matrix3 tmToWorld = Inverse(theMuscleMod.GetModTM()) * objTM;
	
	FILE * pFile = fopen(szFile, "wb");
	if( NULL == pFile )
		return false;

	int		i;
	// first save mesh's name
	char * szName = node->GetName();
	int nLen = strlen(szName);
	fwrite(&nLen, sizeof(int), 1, pFile);
	fwrite(szName, nLen, 1, pFile);

	// then output these muscles
	int nNumMuscles = theMuscleMod.GetNumMuscles();
	fwrite(&nNumMuscles, sizeof(nNumMuscles), 1, pFile);
	
	Point3	vecCenter;
	Point3	vecMuscleDir;
	Point3	vecEnd;

	Point3	vecXAxis;
	Point3	vecYAxis;
	Point3	vecZAxis;
	Point3	vecRadius;
	for(i=0; i<nNumMuscles; i++)
	{
		MUSCLEDATA muscle = theMuscleMod.GetMuscleData(i);

		vecCenter		= tmToWorld.PointTransform(muscle.vecCenter);
		vecMuscleDir	= Normalize(tmToWorld.VectorTransform(muscle.vecMuscleDir));
		vecEnd 			= tmToWorld.PointTransform(muscle.vecCenter - muscle.vLength * muscle.vecMuscleDir);
		vecXAxis		= tmToWorld.VectorTransform(muscle.vecXAxis);
		vecYAxis		= tmToWorld.VectorTransform(muscle.vecYAxis);
		vecZAxis		= tmToWorld.VectorTransform(muscle.vecZAxis);
		vecRadius.x		= muscle.vecRadius.x * vecXAxis.Length();
		vecRadius.y		= muscle.vecRadius.y * vecYAxis.Length();
		vecRadius.z		= muscle.vecRadius.z * vecZAxis.Length();
		vecXAxis		= Normalize(vecXAxis);
		vecYAxis		= Normalize(vecYAxis);
		vecZAxis		= Normalize(vecZAxis);
		
		// now convert from max coordinates to Anglecia coordinates
		float vTemp;
		vTemp = vecCenter.y;
		vecCenter.y = vecCenter.z;
		vecCenter.z = vTemp;
		vTemp = vecMuscleDir.y;
		vecMuscleDir.y = vecMuscleDir.z;
		vecMuscleDir.z = vTemp;
		vTemp = vecEnd.y;
		vecEnd.y = vecEnd.z;
		vecEnd.z = vTemp;
		vTemp = vecXAxis.y;
		vecXAxis.y = vecXAxis.z;
		vecXAxis.z = vTemp;
		vTemp = vecYAxis.y;
		vecYAxis.y = vecYAxis.z;
		vecYAxis.z = vTemp;
		vTemp = vecZAxis.y;
		vecZAxis.y = vecZAxis.z;
		vecZAxis.z = vTemp;
		vTemp = vecRadius.y;
		vecRadius.y = vecRadius.z;
		vecRadius.z = vTemp;

		fwrite(muscle.szName, 32, 1, pFile);

		fwrite(&vecCenter, sizeof(vecCenter), 1, pFile);
		fwrite(&vecMuscleDir, sizeof(vecMuscleDir), 1, pFile);
		fwrite(&vecEnd, sizeof(vecEnd), 1, pFile);

		fwrite(&vecXAxis, sizeof(vecXAxis), 1, pFile);
		fwrite(&vecYAxis, sizeof(vecYAxis), 1, pFile);
		fwrite(&vecZAxis, sizeof(vecZAxis), 1, pFile);
		fwrite(&vecRadius, sizeof(vecRadius), 1, pFile);

		fwrite(&muscle.vPitch, sizeof(muscle.vPitch), 1, pFile);
		fwrite(&muscle.type, sizeof(muscle.type), 1, pFile);

		fwrite(&muscle.vTension, sizeof(muscle.vTension), 1, pFile);
	}

	// now save these muscle's link information
	for(i=0; i<nNumMuscles; i++)
	{
		int nLink = theMuscleMod.GetNumLinkedMuscles(i);
		fwrite(&nLink, sizeof(nLink), 1, pFile);

		for(int j=0; j<nLink; j++)
		{
			int muscleID = theMuscleMod.GetLinkedMuscleID(i, j);
			int muscleIndex = theMuscleMod.FindMuscleByID(muscleID);
			fwrite(&muscleIndex, sizeof(muscleIndex), 1, pFile);
		}
	}

	fclose(pFile);
	return true;
}

IOResult MuscleSetup::Load(ILoad *iload)
{
	// First load the modifier name by
	// calling the base class version.
	Modifier::Load(iload);

	IOResult result = iload->OpenChunk();
	if( IO_OK  !=  result || iload->CurChunkID() != MUSCLEDATACHUNK_ID )
	{
		// just ignore the errors.
		return IO_OK;
	}

	int		i;
	ULONG	in;
	int		nIndex;

	DWORD dwVersion;
	iload->Read(&dwVersion, sizeof(dwVersion), &in);

	if( dwVersion == 0x10003005 )
	{
		int nNumMuscles = 0;
		iload->Read(&nNumMuscles, sizeof(nNumMuscles), &in);
		int nIDSeed = 0;
		iload->Read(&nIDSeed, sizeof(nIDSeed), &in);
		theMuscleMod.SetIDSeed(nIDSeed);
		for(i=0; i<nNumMuscles; i++)
		{
			MUSCLEDATA muscle;

			iload->Read(muscle.szName, 32, &in);

			iload->Read(&muscle.vecCenter, sizeof(muscle.vecCenter), &in);
			iload->Read(&muscle.vecMuscleDir, sizeof(muscle.vecMuscleDir), &in);
			iload->Read(&muscle.vLength, sizeof(muscle.vLength), &in);

			iload->Read(&muscle.vecXAxis, sizeof(muscle.vecXAxis), &in);
			iload->Read(&muscle.vecYAxis, sizeof(muscle.vecYAxis), &in);
			iload->Read(&muscle.vecZAxis, sizeof(muscle.vecZAxis), &in);
			iload->Read(&muscle.vecRadius, sizeof(muscle.vecRadius), &in);
			iload->Read(&muscle.vPitch, sizeof(muscle.vPitch), &in);
			iload->Read(&muscle.type, sizeof(muscle.type), &in);

			iload->Read(&muscle.vTension, sizeof(muscle.vTension), &in);
			int id;
			iload->Read(&id, sizeof(id), &in);

			theMuscleMod.AddLoadedMuscle(muscle, id, nIndex);
		}

		// now rebuild the link architecture
		for(i=0; i<nNumMuscles; i++)
		{
			int nLink;
			iload->Read(&nLink, sizeof(nLink), &in);
			for(int j=0; j<nLink; j++)
			{
				int muscleID;
				iload->Read(&muscleID, sizeof(muscleID), &in);

				int nChildIndex = theMuscleMod.FindMuscleByID(muscleID);
				if( nChildIndex != -1 )
					theMuscleMod.AddMuscleLink(i, nChildIndex);
			}
		}
	}
	else if( dwVersion == 0x10003000 )
	{
		int nNumMuscles = 0;
		iload->Read(&nNumMuscles, sizeof(nNumMuscles), &in);
		int nIDSeed = 0;
		iload->Read(&nIDSeed, sizeof(nIDSeed), &in);
		theMuscleMod.SetIDSeed(nIDSeed);
		for(i=0; i<nNumMuscles; i++)
		{
			MUSCLEDATA muscle;

			iload->Read(muscle.szName, 32, &in);

			iload->Read(&muscle.vecCenter, sizeof(muscle.vecCenter), &in);
			iload->Read(&muscle.vLength, sizeof(muscle.vLength), &in);

			iload->Read(&muscle.vecXAxis, sizeof(muscle.vecXAxis), &in);
			iload->Read(&muscle.vecYAxis, sizeof(muscle.vecYAxis), &in);
			iload->Read(&muscle.vecZAxis, sizeof(muscle.vecZAxis), &in);
			iload->Read(&muscle.vecRadius, sizeof(muscle.vecRadius), &in);
			iload->Read(&muscle.vPitch, sizeof(muscle.vPitch), &in);
			iload->Read(&muscle.type, sizeof(muscle.type), &in);

			muscle.vecMuscleDir = muscle.vecXAxis;

			iload->Read(&muscle.vTension, sizeof(muscle.vTension), &in);
			int id;
			iload->Read(&id, sizeof(id), &in);

			theMuscleMod.AddLoadedMuscle(muscle, id, nIndex);
		}

		// now rebuild the link architecture
		for(i=0; i<nNumMuscles; i++)
		{
			int nLink;
			iload->Read(&nLink, sizeof(nLink), &in);
			for(int j=0; j<nLink; j++)
			{
				int muscleID;
				iload->Read(&muscleID, sizeof(muscleID), &in);

				int nChildIndex = theMuscleMod.FindMuscleByID(muscleID);
				if( nChildIndex != -1 )
					theMuscleMod.AddMuscleLink(i, nChildIndex);
			}
		}
	}
	else
		return IO_ERROR;
	
	iload->CloseChunk();
		
	nCurSetupMuscle = theMuscleMod.GetNumMuscles() - 1;
	return IO_OK;
}

IOResult MuscleSetup::Save(ISave *isave)
{
	// First save the modifier name by
	// calling the base class version.
	Modifier::Save(isave);

	isave->BeginChunk(MUSCLEDATACHUNK_ID);

	int		i;
	ULONG	out;

	// first save my version
	DWORD	dwVersion = MUSCLESYSTEM_VERSION;
	isave->Write(&dwVersion, sizeof(dwVersion), &out);
	
	int nNumMuscles = theMuscleMod.GetNumMuscles();
	isave->Write(&nNumMuscles, sizeof(nNumMuscles), &out);
	int nIDSeed = theMuscleMod.GetIDSeed();
	isave->Write(&nIDSeed, sizeof(nIDSeed), &out);

	for(i=0; i<nNumMuscles; i++)
	{
		MUSCLEDATA muscle = theMuscleMod.GetMuscleData(i);

		isave->Write(muscle.szName, 32, &out);

		isave->Write(&muscle.vecCenter, sizeof(muscle.vecCenter), &out);
		isave->Write(&muscle.vecMuscleDir, sizeof(muscle.vecMuscleDir), &out);
		isave->Write(&muscle.vLength, sizeof(muscle.vLength), &out);

		isave->Write(&muscle.vecXAxis, sizeof(muscle.vecXAxis), &out);
		isave->Write(&muscle.vecYAxis, sizeof(muscle.vecYAxis), &out);
		isave->Write(&muscle.vecZAxis, sizeof(muscle.vecZAxis), &out);
		isave->Write(&muscle.vecRadius, sizeof(muscle.vecRadius), &out);
		isave->Write(&muscle.vPitch, sizeof(muscle.vPitch), &out);
		isave->Write(&muscle.type, sizeof(muscle.type), &out);

		isave->Write(&muscle.vTension, sizeof(muscle.vTension), &out);

		int id = theMuscleMod.GetMuscleID(i);
		isave->Write(&id, sizeof(id), &out);
	}

	// now save these muscle's link information
	for(i=0; i<nNumMuscles; i++)
	{
		int nLink = theMuscleMod.GetNumLinkedMuscles(i);
		isave->Write(&nLink, sizeof(nLink), &out);

		for(int j=0; j<nLink; j++)
		{
			int muscleID = theMuscleMod.GetLinkedMuscleID(i, j);
			isave->Write(&muscleID, sizeof(muscleID), &out);
		}
	}

	isave->EndChunk();
	return IO_OK;
}

// window message processing procs.
BOOL CALLBACK SetupMuscleDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// we use window user data to store current modifier's pointer
	MuscleSetup *ins = (MuscleSetup*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!ins && msg!=WM_INITDIALOG) return FALSE;

	MUSCLEDATA muscle;
	switch (msg) 
	{
		case WM_INITDIALOG:
			SetWindowLong(hWnd, GWL_USERDATA, lParam);
			break;

		case WM_DESTROY:
			break;

		case WM_NOTIFY:
			{
			NMHDR * pHDR = (NMHDR *) lParam;
			if( pHDR->hwndFrom == ins->hMuscleListCtrl )
			{
				if( pHDR->code == NM_CLICK )
				{
					NMITEMACTIVATE * pMsg = (NMITEMACTIVATE *) lParam;

					LVHITTESTINFO info;
					memset(&info, 0, sizeof(info));

					int nCount = ListView_GetItemCount(ins->hMuscleListCtrl);
					info.pt = pMsg->ptAction;

					ListView_HitTest(ins->hMuscleListCtrl, &info);
					if( info.flags & LVHT_ONITEMSTATEICON )
					{
						if( info.iItem != ins->GetCurSetupMuscle() )
						{
							ins->ToggleMuscleLink(ins->GetCurSetupMuscle(), info.iItem);
							ins->UpdateSetupData(false);
						}
						break;
					}
					else if( info.flags & LVHT_ONITEMLABEL || info.flags & LVHT_ONITEMICON )
					{
						if( pMsg->uKeyFlags & LVKF_CONTROL )
							ins->ReLocateToPanel(ins->hParamPanel);
						else if( pMsg->uKeyFlags & LVKF_SHIFT )
							ins->ReLocateToPanel(ins->hTestPanel);
						else if( pMsg->uKeyFlags & LVKF_ALT )
							ins->ReLocateToPanel(ins->hVertWeightPanel);

						break;
					}
				}
				else if( pHDR->code == LVN_ITEMCHANGED )
				{	
					NMLISTVIEW * pnmv = (NMLISTVIEW *) lParam;

					if( pnmv->uChanged & LVIF_STATE )
					{
						if( (pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED) )
							ins->SetCurSetupMuscle(-1);
						else if( !(pnmv->uOldState & LVIS_SELECTED) && (pnmv->uNewState & LVIS_SELECTED) )
							ins->SetCurSetupMuscle(pnmv->iItem);
					}

					break;
				}
				/*
				else if( pHDR->code == LVN_BEGINLABELEDIT )
				{
					return FALSE;
				}*/
			}
			}
			break;
		
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_BUTTONADD:
				ins->AddMuscle(FALSE);
				break;

			case IDC_BUTTONCLONE:
				ins->AddMuscle(TRUE);
				break;

			case IDC_BUTTONDEL:
				ins->DelMuscle(ins->GetCurSetupMuscle());
				break;

			case IDC_BUTTONORIGINAL:
				ins->bOriginalMesh = ins->pButtonOriginal->IsChecked();
				ins->RefreshObject();
				break;

			case IDC_BUTTONSOLOMUSCLE:
				ins->bSoloMuscle = ins->pButtonSoloMuscle->IsChecked();
				ins->RefreshObject();
				break;

			case IDC_BUTTONSHOWWEIGHT:
				ins->bShowWeight = ins->pButtonShowWeight->IsChecked();
				ins->RefreshObject();
				break;

			case IDC_BUTTONIMPORT:
				{
				char		szFilename[MAX_PATH];
				char		szPath[MAX_PATH];

				szPath[0] = '\0';
				szFilename[0] = '\0';

				OPENFILENAME	ofn;
				memset(&ofn, 0, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.hInstance = hInstance;
				ofn.lpstrFilter = "Muscle Data File(*.msd)\0*.msd\0";
				ofn.lpstrFile = szFilename;
				ofn.lpstrInitialDir = szPath;
				ofn.lpstrDefExt = ".msd";
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrTitle = "Select the muscle data file to open";
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
				if( !GetOpenFileName(&ofn) )
					break;
				
				ins->Import(ofn.lpstrFile);
				}
				break;

			case IDC_BUTTONEXPORT:	
				{
				char		szFilename[MAX_PATH];
				char		szPath[MAX_PATH];

				szPath[0] = '\0';
				szFilename[0] = '\0';

				OPENFILENAME	ofn;
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.hInstance = hInstance;
				ofn.lpstrFilter = "Muscle Data File(*.msd)\0*.msd\0";
				ofn.lpstrFile = szFilename;
				ofn.lpstrInitialDir = szPath;
				ofn.lpstrDefExt = ".msd";
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrTitle = "Select the muscle data file to be saved";
				ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
				if( !GetSaveFileName(&ofn) )
					break;
				
				ins->Export(ofn.lpstrFile);
				}
				break;

			case IDC_BUTTONEXPORT2ENGINE:	
				{
				char		szFilename[MAX_PATH];
				char		szPath[MAX_PATH];

				szPath[0] = '\0';
				szFilename[0] = '\0';

				OPENFILENAME	ofn;
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.hInstance = hInstance;
				ofn.lpstrFilter = "Muscle Data File(*.md)\0*.md\0";
				ofn.lpstrFile = szFilename;
				ofn.lpstrInitialDir = szPath;
				ofn.lpstrDefExt = ".md";
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrTitle = "Select the muscle data file to be saved";
				ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
				if( !GetSaveFileName(&ofn) )
					break;
				
				ins->ExportToEngine(ofn.lpstrFile);
				}
				break;
			}
			
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ins->ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK CurveWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MuscleSetup *ins = (MuscleSetup*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!ins )
		return FALSE;

	switch (msg)    
	{
		case WM_COMMAND: 	
			break;
		case WM_MOUSEMOVE: 	
			break;
		case WM_LBUTTONUP: 
			break;
		case WM_PAINT: 	
			{
			PAINTSTRUCT ps;
			Rect rect;
			HDC hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rect);
			ins->ShowCurve(hdc, rect);
			EndPaint(hWnd, &ps);
			}													
			break;
		case WM_CREATE:
		case WM_DESTROY: 
			break;
    }

	return DefWindowProc(hWnd,msg,wParam,lParam);
}

BOOL CALLBACK ParamMuscleDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// we use window user data to store current modifier's pointer
	MuscleSetup *ins = (MuscleSetup*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!ins && msg!=WM_INITDIALOG) return FALSE;

	MUSCLEDATA muscle;
	switch (msg) 
	{
		case WM_INITDIALOG:
			SetWindowLong(hWnd, GWL_USERDATA, lParam);
			break;

		case WM_DESTROY:
			break;
		
		// cust control messages
		case WM_CUSTEDIT_ENTER:
			ins->UpdateParamData(true);
			// update setup panel displays
			ins->UpdateSetupData(false);
			ins->UpdateControlPoints(false);
			break;

		case WM_COMMAND:
			if( HIWORD(wParam) == CBN_SELCHANGE )
			{
				ins->UpdateParamData(true);
				ins->UpdateSetupData(false);
			}
			else 
			{
				switch(LOWORD(wParam))
				{
				case IDC_BUTTONX:
					ins->MirrorMuscle(0);
					break;
				case IDC_BUTTONY:
					ins->MirrorMuscle(1);
					break;
				case IDC_BUTTONZ:
					ins->MirrorMuscle(2);
					break;
				}
			}
			break;

		case CC_SPINNER_CHANGE:
			ins->UpdateParamData(true);
			ins->UpdateControlPoints(false);
			InvalidateRect(ins->hCurveWnd, NULL, FALSE);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ins->ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK TestMuscleDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// we use window user data to store current modifier's pointer
	MuscleSetup *ins = (MuscleSetup*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!ins && msg!=WM_INITDIALOG) return FALSE;

	switch (msg) 
	{
		case WM_INITDIALOG:
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			break;

		case WM_DESTROY:
			break;

		// cust control messages
		case WM_CUSTEDIT_ENTER:
			ins->UpdateTestData(true);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_BUTTONRANDALL:
				ins->GenRandMuscleTensions();
				break;
			}
			break;

		case CC_SPINNER_CHANGE:
			ins->UpdateTestData(true);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ins->ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK VertWeightDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// we use window user data to store current modifier's pointer
	MuscleSetup *ins = (MuscleSetup*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!ins && msg!=WM_INITDIALOG) return FALSE;

	switch (msg) 
	{
		case WM_INITDIALOG:
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			break;

		case WM_DESTROY:
			break;

		// cust control messages
		case WM_CUSTEDIT_ENTER:
			ins->UpdateVertWeightData(true);
			break;

		case WM_COMMAND:
			break;

		case CC_SPINNER_CHANGE:
			ins->UpdateVertWeightData(true);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ins->ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

bool MuscleSetup::AddMuscle(BOOL bCloneCurrent)
{
	MUSCLEDATA theMuscle;
	memset(&theMuscle, 0, sizeof(theMuscle));

	if( GetCurSetupMuscle() >= 0 && bCloneCurrent )
	{
		// we just create a new muscle the same as current selected one.
		MUSCLEDATA muscleCur = theMuscleMod.GetMuscleData(GetCurSetupMuscle());

		sprintf(theMuscle.szName, "复制%s", muscleCur.szName);

		theMuscle.vecCenter		= muscleCur.vecCenter;
		theMuscle.vecMuscleDir	= muscleCur.vecMuscleDir;
		theMuscle.vLength		= muscleCur.vLength;

		theMuscle.vecXAxis		= muscleCur.vecXAxis;
		theMuscle.vecYAxis		= muscleCur.vecYAxis;
		theMuscle.vecZAxis		= muscleCur.vecZAxis;
		theMuscle.vecRadius		= muscleCur.vecRadius;
		theMuscle.vPitch		= muscleCur.vPitch;
		theMuscle.type			= muscleCur.type;

		theMuscle.vTension		= muscleCur.vTension;
	}
	else
	{
		sprintf(theMuscle.szName, "Muscle%02d", theMuscleMod.GetNumMuscles() + 1);

		// set default values for this new muscle
		theMuscle.vecCenter = Point3(0.0f, 0.0f, 0.0f);
		theMuscle.vecMuscleDir = Point3(1.0f, 0.0f, 0.0f);
		theMuscle.vLength	= 10.0f;
		
		theMuscle.vecXAxis	= Point3(1.0f, 0.0f, 0.0f);
		theMuscle.vecYAxis	= Point3(0.0f, 1.0f, 0.0f);
		theMuscle.vecZAxis	= Point3(0.0f, 0.0f, 1.0f);
		theMuscle.vecRadius = Point3(1.0f, 1.0f, 1.0f);
		theMuscle.vPitch	= 1.0f;
		theMuscle.type		= MUSCLE_TYPE_SINGLEDIR;
		
		theMuscle.vTension = 0.0f;
	}

	int nIndex;
	theMuscleMod.AddMuscle(theMuscle, nIndex);

	RefreshObject();//ReEvaluate();

	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
	item.iItem = nIndex;
	item.state = INDEXTOSTATEIMAGEMASK(1);
	item.iImage = (int) theMuscle.type;
	item.stateMask = LVIS_STATEIMAGEMASK;
	item.pszText = theMuscle.szName;
	ListView_InsertItem(hMuscleListCtrl, &item);

	return SetCurSetupMuscle(nIndex);
}

bool MuscleSetup::DelMuscle(int nIndex)
{
	if( IDYES != MessageBox(hSetupPanel, "确定要删除吗？", "请确认", MB_ICONQUESTION | MB_YESNO) )
		return true;

	if( nIndex >= 0 )
	{
		int		i;

		// first breaks these links
		for(i=0; i<theMuscleMod.GetNumMuscles(); i++)
		{
			if( i != nIndex && theMuscleMod.IsMuscleLinked(i, nIndex) )
			{
				theMuscleMod.RemoveMuscleLink(i, nIndex);
			}
		}

		theMuscleMod.DelMuscle(nIndex);

		ListView_DeleteAllItems(hMuscleListCtrl);
		for(i=0; i<theMuscleMod.GetNumMuscles(); i++)
		{
			MUSCLEDATA muscle = theMuscleMod.GetMuscleData(i);
			
			LVITEM item;
			memset(&item, 0, sizeof(item));
			item.mask = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
			item.iItem = i;
			item.state = INDEXTOSTATEIMAGEMASK(1);
			item.iImage = (int) muscle.type;
			item.stateMask = LVIS_STATEIMAGEMASK;
			item.pszText = muscle.szName;
			ListView_InsertItem(hMuscleListCtrl, &item);
		}

		if( nIndex >= theMuscleMod.GetNumMuscles() )
			nIndex = theMuscleMod.GetNumMuscles() - 1;
		SetCurSetupMuscle(nIndex);

		UpdateSetupData(false);
	}

	ReEvaluate();
	return true;
}

bool MuscleSetup::RefreshObject()
{
	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t, t), PART_ALL, REFMSG_CHANGE);
	ip->RedrawViews(t);
	return true;
}

bool MuscleSetup::ReEvaluate()
{	
	// first update current muscles data
	theMuscleMod.UpdateAllMuscleAFR();
	
	// then force update the modifier's state, this will result the ModifyObject to be called.
	RefreshObject();	
	return true;
}

bool MuscleSetup::SetCurSetupMuscle(int nIndex)
{
	nCurSetupMuscle = nIndex;

	if( nIndex < 0 || nIndex >= theMuscleMod.GetNumMuscles() )
	{
		// set correct state for controls in setup panel
		if( hSetupPanel )
		{
			pButtonCloneMuscle->Disable();
			pButtonDelMuscle->Disable();
			
			if( theMuscleMod.GetNumMuscles() == 0 )
				pButtonExport->Disable();

			for(int i=0; i<theMuscleMod.GetNumMuscles(); i++)
			{
				if( ListView_GetItemState(hMuscleListCtrl, i, LVIS_SELECTED) )
					ListView_SetItemState(hMuscleListCtrl, i, 0, LVIS_SELECTED);
			}
		}

		// set correct state for controls in param panel
		if( hParamPanel )
		{
			pEditName->Disable();
			pEditRadiusX->Disable();
			pEditRadiusY->Disable();
			pEditRadiusZ->Disable();
			pEditPitch->Disable();
			pEditLength->Disable();
			
			pSpinRadiusX->Disable();
			pSpinRadiusY->Disable();
			pSpinRadiusZ->Disable();
			pSpinPitch->Disable();
			pSpinLength->Disable();

			pButtonX->Disable();
			pButtonY->Disable();
			pButtonZ->Disable();

			EnableWindow(hComboWnd, FALSE);
		}

		// now for controls in test panel
		if( hTestPanel )
		{
			pEditTension->Disable();
			pSpinTension->Disable();
		}

		// now for vertex weight panel
		if( hVertWeightPanel )
		{
			pEditWeight->Disable();
			pSpinWeight->Disable();
		}

		NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
		ip->RedrawViews(ip->GetTime());
		return true;
	}
	else
	{
		// set correct state for controls in setup panel
		if( hSetupPanel )
		{
			pButtonCloneMuscle->Enable();
			pButtonDelMuscle->Enable();
			pButtonExport->Enable();
			if( !ListView_GetItemState(hMuscleListCtrl, nIndex, LVIS_SELECTED) )
				ListView_SetItemState(hMuscleListCtrl, nIndex, LVIS_SELECTED, LVIS_SELECTED);
			ListView_EnsureVisible(hMuscleListCtrl, nIndex, FALSE);
		}

		// set correct state for controls in param panel
		if( hParamPanel )
		{
			pEditName->Enable();
			pEditRadiusX->Enable();
			pEditRadiusY->Enable();
			pEditRadiusZ->Enable();
			pEditPitch->Enable();
			pEditLength->Enable();
			
			pSpinRadiusX->Enable();
			pSpinRadiusY->Enable();
			pSpinRadiusZ->Enable();
			pSpinPitch->Enable();
			pSpinLength->Enable();

			pButtonX->Enable();
			pButtonY->Enable();
			pButtonZ->Enable();

			EnableWindow(hComboWnd, TRUE);
		}

		// now for controls in test panel
		if( hTestPanel )
		{
			pEditTension->Enable();
			pSpinTension->Enable();
		}

		// now for vertex weight panel
		if( hVertWeightPanel )
		{
			pEditWeight->Enable();
			pSpinWeight->Enable();
		}

		UpdateSetupData(false);
		UpdateParamData(false);
		UpdateTestData(false);
		UpdateVertWeightData(false);

		UpdateControlPoints(false);
		UpdateVertexWeights(false);
	}

	return true;
}

bool MuscleSetup::UpdateControlPoints(bool bSave)
{
	if( GetCurSetupMuscle() < 0 )
		return true;

	MUSCLEDATA muscle;
	if( bSave )
	{
		muscle = theMuscleMod.GetMuscleData(GetCurSetupMuscle());
		
		if( bMuscleSelected )
		{
			Matrix3 tm(TRUE);
			ctrlMuscle->GetValue(ip->GetTime(), &tm, FOREVER, CTRL_RELATIVE);

			muscle.vecCenter = tm.GetRow(3);
			muscle.vecXAxis = Normalize(tm.GetRow(0));
			muscle.vecYAxis = Normalize(tm.GetRow(1));
			muscle.vecZAxis = Normalize(tm.GetRow(2));
		}

		if( bEndSelected )
		{
			Point3 pt;
			ctrlEnd->GetValue(ip->GetTime(), &pt, FOREVER, CTRL_ABSOLUTE);

			Point3 vecMuscle = muscle.vecCenter - pt;
			muscle.vLength = Length(vecMuscle);
			muscle.vecMuscleDir = Normalize(vecMuscle);
		}

		theMuscleMod.SetMuscleData(GetCurSetupMuscle(), muscle);
	}
	else
	{
		muscle = theMuscleMod.GetMuscleData(GetCurSetupMuscle());

		Matrix3 tm(muscle.vecXAxis, muscle.vecYAxis, muscle.vecZAxis, muscle.vecCenter);
		SetXFormPacket xForm(tm);
		ctrlMuscle->SetValue(ip->GetTime(), &xForm, TRUE, CTRL_RELATIVE);

		Point3 pt = muscle.vecCenter - muscle.vecMuscleDir * muscle.vLength;
		ctrlEnd->SetValue(ip->GetTime(), pt, TRUE, CTRL_ABSOLUTE);

		ip->FlushUndoBuffer();
		ip->RedrawViews(ip->GetTime());
	}

	return true;
}

bool MuscleSetup::UpdateVertexWeights(bool bSave)
{
	if( GetCurSetupMuscle() < 0 )
		return true;

	if( bSave )
	{
	}
	else
	{

	}

	return true;
}

bool MuscleSetup::UpdateSetupData(bool bSave)
{
	if( !hSetupPanel || GetCurSetupMuscle() < 0 )
		return true;

	MUSCLEDATA theMuscle;

	if( bSave ) // we store current data on the interface to memory
	{
	}
	else // we modify current interface data from memory
	{
		theMuscle = theMuscleMod.GetMuscleData(nCurSetupMuscle);

		// Update list box info
		LVITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = LVIF_IMAGE | LVIF_TEXT;
		item.iItem = GetCurSetupMuscle();
		item.iSubItem = 0;
		item.iImage = theMuscle.type;
		item.pszText = theMuscle.szName;
		ListView_SetItem(hMuscleListCtrl, &item);

		// now set the state images of each muscle
		item.mask = LVIF_STATE;
		item.stateMask = LVIS_STATEIMAGEMASK;

		// now update linked state image
		for(int i=0; i<theMuscleMod.GetNumMuscles(); i++)
		{
			item.iItem = i;
			if( i == GetCurSetupMuscle() )
				item.state = INDEXTOSTATEIMAGEMASK(3);
			else if( theMuscleMod.IsMuscleLinked(GetCurSetupMuscle(), i) )
				item.state = INDEXTOSTATEIMAGEMASK(2);
			else
				item.state = INDEXTOSTATEIMAGEMASK(1);
			ListView_SetItem(hMuscleListCtrl, &item);
		}
	}

	return true;
}

bool MuscleSetup::UpdateParamData(bool bSave)
{
	if( !hParamPanel || GetCurSetupMuscle() < 0 )
		return true;

	MUSCLEDATA theMuscle;

	if( bSave ) // we store current data on the interface to memory
	{
		BOOL	bValid;
		
		theMuscle = theMuscleMod.GetMuscleData(GetCurSetupMuscle());

		pEditName->GetText(theMuscle.szName, 32);

		float x, y, z, l, p;
		MUSCLE_TYPE type;
		x = pEditRadiusX->GetFloat(&bValid);
		y = pEditRadiusY->GetFloat(&bValid);
		z = pEditRadiusZ->GetFloat(&bValid);
		p = pEditPitch->GetFloat(&bValid);
		l = pEditLength->GetFloat(&bValid);
		type = (MUSCLE_TYPE) SendMessage(hComboWnd, CB_GETCURSEL, 0, 0);
		
		theMuscle.vecRadius = Point3(x, y, z);
		theMuscle.vPitch	= p;
		theMuscle.vLength	= l;
		theMuscle.type		= type;
		theMuscleMod.SetMuscleData(GetCurSetupMuscle(), theMuscle);

		RefreshObject();
	}
	else // we modify current interface data from memory
	{
		theMuscle = theMuscleMod.GetMuscleData(nCurSetupMuscle);

		pEditName->SetText(theMuscle.szName);
		pEditRadiusX->SetText(theMuscle.vecRadius.x);
		pEditRadiusY->SetText(theMuscle.vecRadius.y);
		pEditRadiusZ->SetText(theMuscle.vecRadius.z);
		pEditPitch->SetText(theMuscle.vPitch);
		pEditLength->SetText(theMuscle.vLength);
		pSpinRadiusX->SetValue(theMuscle.vecRadius.x, false);
		pSpinRadiusY->SetValue(theMuscle.vecRadius.y, false);
		pSpinRadiusZ->SetValue(theMuscle.vecRadius.z, false);
		pSpinPitch->SetValue(theMuscle.vPitch, false);
		pSpinLength->SetValue(theMuscle.vLength, false);
		SendMessage(hComboWnd, CB_SETCURSEL, theMuscle.type, 0);

		InvalidateRect(hCurveWnd, NULL, FALSE);
	}

	return true;
}

bool MuscleSetup::UpdateTestData(bool bSave)
{
	if( !hTestPanel || GetCurSetupMuscle() < 0 )
		return true;

	MUSCLEDATA theMuscle;

	if( bSave )
	{
		BOOL	bValid;

		theMuscle = theMuscleMod.GetMuscleData(GetCurSetupMuscle());

		theMuscle.vTension = pEditTension->GetFloat(&bValid);
		if( !bValid )
		{
			pEditTension->GiveFocus();
			MessageBeep(MB_OK);
			return false;
		}

		theMuscleMod.SetMuscleData(GetCurSetupMuscle(), theMuscle);

		for(int i=0; i<theMuscleMod.GetNumMuscles(); i++)
		{
			if( theMuscleMod.IsMuscleLinked(GetCurSetupMuscle(), i) )
			{
				MUSCLEDATA linkMuscle = theMuscleMod.GetMuscleData(i);
				linkMuscle.vTension = theMuscle.vTension;
				theMuscleMod.SetMuscleData(i, linkMuscle);
			}
		}

		RefreshObject();
	}
	else
	{
		theMuscle = theMuscleMod.GetMuscleData(nCurSetupMuscle);

		pEditTension->SetText(theMuscle.vTension);
		pSpinTension->SetValue(theMuscle.vTension, false);
	}
	return true;
}

bool MuscleSetup::UpdateVertWeightData(bool bSave)
{
	if( !hVertWeightPanel )
		return true;

	MUSCLEDATA theMuscle;

	if( bSave )
	{
		/*
		BOOL	bValid;

		theMuscle = theMuscleMod.GetMuscleData(GetCurSetupMuscle());

		theMuscle.vTension = pEditTension->GetFloat(&bValid);
		if( !bValid )
		{
			pEditTension->GiveFocus();
			MessageBeep(MB_OK);
			return false;
		}

		theMuscleMod.SetMuscleData(GetCurSetupMuscle(), theMuscle);
		*/
		RefreshObject();
	}
	else
	{
		/*
		theMuscle = theMuscleMod.GetMuscleData(nCurSetupMuscle);

		pEditTension->SetText(theMuscle.vTension);
		pSpinTension->SetValue(theMuscle.vTension, false);
		*/
	}
	return true;
}

// sub object manipulation
void MuscleSetup::GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t, INode *node,ModContext *mc)
{
	Matrix3 tm = CompMatrix(t,node,mc);

	if( bMuscleSelected )
	{
		Matrix3 tmLocal(TRUE);
		ctrlMuscle->GetValue(t, &tmLocal, FOREVER, CTRL_RELATIVE);
		tm = tmLocal * tm;
	}
	else if( bEndSelected )
	{
		Point3 c;
		ctrlEnd->GetValue(t, &c, FOREVER, CTRL_ABSOLUTE);
		tm.PreTranslate(c);
	}
	else if( bWeightSelected )
	{
		Matrix3 tmLocal(TRUE);
		ctrlMuscle->GetValue(t, &tmLocal, FOREVER, CTRL_RELATIVE);
		tm = tmLocal * tm;
	}

	cb->Center(tm.GetTrans(),0);
}

void MuscleSetup::GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t, INode *node,ModContext *mc)
{
	Matrix3 tm = CompMatrix(t,node,mc);
		
	if( bMuscleSelected )
	{
		Matrix3 tmLocal(TRUE);
		ctrlMuscle->GetValue(t, &tmLocal, FOREVER, CTRL_RELATIVE);
		tm = tmLocal * tm;
	}
	else if( bEndSelected )
	{
		Point3 c;
		ctrlEnd->GetValue(t, &c, FOREVER, CTRL_ABSOLUTE);
		tm.PreTranslate(c);
	}
	else if( bWeightSelected )
	{
		Matrix3 tmLocal(TRUE);
		ctrlMuscle->GetValue(t, &tmLocal, FOREVER, CTRL_RELATIVE);
		tm = tmLocal * tm;
	}
	
	cb->TM(tm,0);
}

void MuscleSetup::SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert)
{
	//if (theHold.Holding()) theHold.Put(new SelRestore(this));
	/*
	while (hitRec) {
		assert(hitRec->hitInfo<=1);
		BOOL state = selected;
		if (invert) state = !sel[hitRec->hitInfo];
		if (state) sel[hitRec->hitInfo] = 1;
		else       sel[hitRec->hitInfo] = 0;
		if (!all) break;
		hitRec = hitRec->Next();
		}	
		*/
	//NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
}

void MuscleSetup::ClearSelection(int selLevel)
{
	//if (theHold.Holding()) theHold.Put(new SelRestore(this));
	//sel[0] = sel[1] = 0;
	//NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
}

void MuscleSetup::SelectAll(int selLevel)
{
	//if (theHold.Holding()) theHold.Put(new SelRestore(this));
	//sel[0] = sel[1] = 1;
	//NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
}

void MuscleSetup::InvertSelection(int selLevel)
{
	//if (theHold.Holding()) theHold.Put(new SelRestore(this));
	//sel[0] = !sel[0];
	//sel[1] = !sel[1];
	//NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
}

// set correct panels according to current sub-object level
void MuscleSetup::AdjustPanels(int level)
{
	// first manage the dynamic panels
	switch(level)
	{
	case 0:
		// show setup panel and test panel
		AddTestPanel();
		AddParamPanel();
		RemoveVertWeightPanel();
		break;

	case 1:
		// show setup panel and param panel
		AddParamPanel();
		AddTestPanel();
		RemoveVertWeightPanel();
		break;

	case 2:
		// show setup panel and vertex weight panel
		AddVertWeightPanel();
		break;

	default:
		break;
	}

	// update informations displayed in the panels.
	SetCurSetupMuscle(GetCurSetupMuscle());
}

void MuscleSetup::ActivateSubobjSel(int level, XFormModes& modes)
{
	AdjustPanels(level);

	bMuscleSelected = FALSE;
	bEndSelected = FALSE;
	bWeightSelected = FALSE;

	switch (level) 
	{
		case SUBOBJ_MUSCLE: // muscle envelop controller
			modes = XFormModes(moveMode, rotateMode, NULL, NULL, NULL, NULL);
			bMuscleSelected = TRUE;
			break;		

		case SUBOBJ_END: // end point controller
			modes = XFormModes(moveMode, NULL, NULL, NULL, NULL, NULL);
			bEndSelected = TRUE;
			break;

		case SUBOBJ_WEIGHT: // Vertex weight controller
			modes = XFormModes(moveMode, rotateMode, NULL, NULL, NULL, NULL);
			bWeightSelected = TRUE;
			break;
	}
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
}

void MuscleSetup::GetWorldBoundBox(
		TimeValue t,INode* inode, ViewExp *vpt, 
		Box3& box, ModContext *mc)
{
	Matrix3 tm = CompMatrix(t,inode,mc);
	Point3 pt1, pt2, pt3, pt4, pt5;

	box.Init();
	if( GetCurSetupMuscle() >= 0 )
	{
		MUSCLEDATA muscle = theMuscleMod.GetMuscleData(GetCurSetupMuscle());
		box += tm * muscle.vecCenter;
		box += tm * (muscle.vecCenter - muscle.vecMuscleDir * muscle.vLength);
		box += tm * (muscle.vecCenter + muscle.vecXAxis * muscle.vecRadius.x);
		box += tm * (muscle.vecCenter - muscle.vecXAxis * muscle.vecRadius.x);
		box += tm * (muscle.vecCenter + muscle.vecYAxis * muscle.vecRadius.y);
		box += tm * (muscle.vecCenter - muscle.vecYAxis * muscle.vecRadius.y);
		box += tm * (muscle.vecCenter + muscle.vecZAxis * muscle.vecRadius.z);
		box += tm * (muscle.vecCenter - muscle.vecZAxis * muscle.vecRadius.z);
	}
	else
	{
		pt1 = Point3(0.0f, 0.0f, 0.0f);
		pt2 = Point3(0.0f, 0.0f, -1.0f);
		box += pt1 * tm;
		box += pt2 * tm;
	}
}

int MuscleSetup::GetEnvelopePoints(int nNumSegments, Point3 * pts, int idPlane)
{
	if( GetCurSetupMuscle() >= 0 )
	{
		MUSCLEDATA muscle = theMuscleMod.GetMuscleData(GetCurSetupMuscle());
		Matrix3 tmMuscle(muscle.vecXAxis, muscle.vecYAxis, muscle.vecZAxis, muscle.vecCenter);
		float	rx, ry, rz;
		Point3	pt;

		rx = muscle.vecRadius.x;
		ry = muscle.vecRadius.y;
		rz = muscle.vecRadius.z;

		int i;
		switch(idPlane)
		{
		case 1: // xz plane
			for(i=0; i<nNumSegments; i++)
			{
				float rad = TWOPI * i / nNumSegments;
				pt.x = cos(rad) * rx;
				pt.y = 0.0f;
				pt.z = sin(rad) * rz;
				pts[i] = pt * tmMuscle;
			}
			break;

		case 0: // yz plane
			for(i=0; i<nNumSegments; i++)
			{
				float rad = TWOPI * i / nNumSegments;
				pt.x = 0.0f;
				pt.y = cos(rad) * ry;
				pt.z = sin(rad) * rz;
				pts[i] = pt * tmMuscle;
			}
			break;

		case 2: // xy plane
			for(i=0; i<nNumSegments; i++)
			{
				float rad = TWOPI * i / nNumSegments;
				pt.x = cos(rad) * rx;
				pt.y = sin(rad) * ry;
				pt.z = 0.0f;
				pts[i] = pt * tmMuscle;
			}
			break;
		}

		return nNumSegments;
	}
	else
		return 0;
}

// display and hit test methods
int MuscleSetup::Display(TimeValue t, INode* inode, ViewExp *vpt, 
					int flagst, ModContext *mc)
{
	if( GetCurSetupMuscle() < 0 )
		return 0;

	GraphicsWindow *gw = vpt->getGW();
	Point3 pt[4];
	Matrix3 tm = CompMatrix(t, inode, mc);
	int savedLimits;

	gw->setRndLimits((savedLimits = gw->getRndLimits()) & ~GW_ILLUM);
	gw->setTransform(tm);
	
	float v = 0.7f;
	if (ip && ip->GetSubObjectLevel() != 0) 
		v = 1.0f;

	// Draw Both End point
	Point3 pt0 = Point3(0.0f, 0.0f, 0.0f);
	Point3 pt1 = Point3(0.0f, 0.0f, 1.0f);

	Matrix3 tmLocal(TRUE);
	ctrlMuscle->GetValue(t, &tmLocal, FOREVER, CTRL_RELATIVE);

	MUSCLEDATA muscle = theMuscleMod.GetMuscleData(GetCurSetupMuscle());
	pt0 = tmLocal.GetRow(3);
	pt1 = pt0 - muscle.vecMuscleDir * muscle.vLength;
	
	gw->setColor(LINE_COLOR, 0.0f, v, 0.0f);
	gw->marker(&pt1, DOT_MRKR);
	gw->setColor(LINE_COLOR, v, v, 0.0f);
	gw->marker(&pt0, SM_DOT_MRKR);
	
	pt[0] = pt0;
	pt[1] = pt1;
	gw->polyline(2, pt, NULL, NULL, 0, NULL);

	// now 3 axis direction
	pt[1] = pt0 + muscle.vecXAxis * muscle.vecRadius.x;
	gw->setColor(LINE_COLOR, v, 0.0f, 0.0f);
	gw->polyline(2, pt, NULL, NULL, 0, NULL);

	pt[1] = pt0 + muscle.vecYAxis * muscle.vecRadius.y;
	gw->setColor(LINE_COLOR, 0.0f, v, 0.0f);
	gw->polyline(2, pt, NULL, NULL, 0, NULL);

	pt[1] = pt0 + muscle.vecZAxis * muscle.vecRadius.z;
	gw->setColor(LINE_COLOR, 0.0f, 0.0f, v);
	gw->polyline(2, pt, NULL, NULL, 0, NULL);

#define NUM_SEGMENTS		16
	// now the affected region indicator
	Point3	ptEnvs[NUM_SEGMENTS];
	int		nPtEnvs;

	gw->setColor(LINE_COLOR, v, 0.0f, 0.0f);
	nPtEnvs = GetEnvelopePoints(NUM_SEGMENTS, ptEnvs, 0);
	if( nPtEnvs )	
		gw->polyline(nPtEnvs, ptEnvs, NULL, NULL, 1, NULL);

	nPtEnvs = GetEnvelopePoints(NUM_SEGMENTS, ptEnvs, 1);
	if( nPtEnvs )	
		gw->polyline(nPtEnvs, ptEnvs, NULL, NULL, 1, NULL);

	nPtEnvs = GetEnvelopePoints(NUM_SEGMENTS, ptEnvs, 2);
	if( nPtEnvs )	
		gw->polyline(nPtEnvs, ptEnvs, NULL, NULL, 1, NULL);
		
	// at last if we want to see these verts, we should display them
	if( bShowWeight && (gw->getRndMode() & GW_WIREFRAME) ) 
	{
		gw->startMarkers();

		float		w;
		// we needn't to transform the point by MC invert tm because we have 
		// set the world transform to include the MC invert. This is different 
		// with that in ModifyObject
		Point3 *	pVerts;
		if( bOriginalMesh )
			pVerts = theMuscleMod.GetOriginalVerts();
		else
			pVerts = theMuscleMod.GetDeformedVerts();

		Point3		vertInObj;
		int			nAFRVerts;
		int *		pAFRIndices;
		float *		pAFRWeights;

		theMuscleMod.GetMuscleAFRInfo(GetCurSetupMuscle(), nAFRVerts, pAFRIndices, pAFRWeights);
		for(int i=0; i<nAFRVerts; i++)
		{
			w = fabs(pAFRWeights[i]);
			gw->setColor(LINE_COLOR, w, 0.0f, 1.0f - w);

			vertInObj = pVerts[pAFRIndices[i]];
			gw->marker(&vertInObj, DOT_MRKR);
		}

		gw->endMarkers();
	}

	gw->setRndLimits(savedLimits);
	return 0;
}

int MuscleSetup::HitTest(
		TimeValue t, INode* inode, 
		int type, int crossing, int flags, 
		IPoint2 *p, ViewExp *vpt, ModContext* mc)
{
	GraphicsWindow *gw = vpt->getGW();
	Point3 pt;
	HitRegion hr;
	int savedLimits, res = 0;
	Matrix3 tm = CompMatrix(t,inode,mc);

	MakeHitRegion(hr,type, crossing,4,p);
	gw->setHitRegion(&hr);	
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
	gw->setTransform(tm);

	if( ip && ip->GetSubObjectLevel() == 1 )
	{
		/*
		// Hit test start point
		if ((flags&HIT_SELONLY   &&  sel[0]) ||
			(flags&HIT_UNSELONLY && !sel[0]) ||
			!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) {
		
			gw->clearHitCode();
			p1->GetValue(t,&pt,FOREVER,CTRL_ABSOLUTE);
			gw->marker(&pt,HOLLOW_BOX_MRKR);
			if (gw->checkHitCode()) {
				vpt->LogHit(inode, mc, gw->getHitDistance(), 0, NULL); 
				res = 1;
				}
			}

		// Hit test end point
		if ((flags&HIT_SELONLY   &&  sel[1]) ||
			(flags&HIT_UNSELONLY && !sel[1]) ||
			!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) {
		
			gw->clearHitCode();
			p2->GetValue(t,&pt,FOREVER,CTRL_ABSOLUTE);
			gw->marker(&pt,HOLLOW_BOX_MRKR);
			if (gw->checkHitCode()) {
				vpt->LogHit(inode, mc, gw->getHitDistance(), 1, NULL); 
				res = 1;
				}
			}
			*/
	}
	else if( ip && ip->GetSubObjectLevel() == 2 )
	{
		res = 0;
	}
	gw->setRndLimits(savedLimits);
	return res;
}

void MuscleSetup::Move(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin)
{
	if( GetCurSetupMuscle() < 0 )
		return; 

	if( bMuscleSelected )
	{
		SetXFormPacket xForm(val, partm, tmAxis);
		ctrlMuscle->SetValue(t, &xForm, TRUE, CTRL_RELATIVE);
	}
	else if( bEndSelected )
	{
		ctrlEnd->SetValue(
			t, VectorTransform(tmAxis*Inverse(partm),val),
			TRUE,CTRL_RELATIVE);
	}
	else if( bWeightSelected )
	{
		SetXFormPacket xForm(val, partm, tmAxis);
		ctrlMuscle->SetValue(t, &xForm, TRUE, CTRL_RELATIVE);
	}
}

void MuscleSetup::Rotate(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin)
{
	if( GetCurSetupMuscle() < 0 )
		return; 

	if( bMuscleSelected )
	{
		SetXFormPacket xForm(val, localOrigin, partm, tmAxis);
		ctrlMuscle->SetValue(t, &xForm, TRUE, CTRL_RELATIVE);
	}
	else if( bEndSelected )
	{
	}
	else if( bWeightSelected )
	{
		SetXFormPacket xForm(val, localOrigin, partm, tmAxis);
		ctrlMuscle->SetValue(t, &xForm, TRUE, CTRL_RELATIVE);
	}
}

bool MuscleSetup::GenRandMuscleTensions()
{
	for(int i=0; i<theMuscleMod.GetNumMuscles(); i++)
	{
		MUSCLEDATA muscle = theMuscleMod.GetMuscleData(i);
		muscle.vTension = -1.0f + (rand() % 200) / 100.0f;
		theMuscleMod.SetMuscleData(i, muscle);

		for(int j=0; j<theMuscleMod.GetNumMuscles(); j++)
		{
			if( theMuscleMod.IsMuscleLinked(i, j) )
			{
				MUSCLEDATA linkMuscle = theMuscleMod.GetMuscleData(j);
				linkMuscle.vTension = muscle.vTension;
				theMuscleMod.SetMuscleData(j, linkMuscle);
			}
		}
	}

	// update information on these panels.
	UpdateTestData(false);
	RefreshObject();
	return true;
}

void VertLine(HDC hdc,int x, int ystart, int yend) 
{
	MoveToEx(hdc, x, ystart, NULL); 
	if( ystart<=yend )
		LineTo(hdc, x, yend+1);
	else 
		LineTo(hdc, x, yend-1);
}

void MuscleSetup::ShowCurve(HDC hdc, Rect& rect)
{
	int w, h, npts, xcen, ybot, ytop, ylast, i, iy;
	HPEN linePen = (HPEN) GetStockObject(WHITE_PEN);
	HPEN fgPen = CreatePen(PS_SOLID, 0, GetCustSysColor(COLOR_BTNFACE));
	HPEN bgPen = CreatePen(PS_SOLID, 0, GetCustSysColor(COLOR_BTNSHADOW));

	w = rect.w();
	h = rect.h()-3;
	npts = (w-2)/2;
	xcen = rect.left+npts;
	ybot = rect.top+h;
	ytop = rect.top+2;
	ylast = -1;

	float vPitch;
	if( GetCurSetupMuscle() >= 0 )
		vPitch = theMuscleMod.GetMuscleData(GetCurSetupMuscle()).vPitch;
	else
		vPitch = 1.0f;

	for(i=0; i<npts; i++) 
	{
		float v = theMuscleMod.GetCurvedWeight(1.0f - i/((float)npts), vPitch);
		if( v>2.0f ) v = 2.0f; // keep iy from wrapping
		iy = ybot-(int)(v*((float)h-2.0f));

		if (iy<ytop) iy = ytop;

		SelectPen(hdc, fgPen);
		VertLine(hdc,xcen+i,ybot,iy);
		VertLine(hdc,xcen-i,ybot,iy);

		if (iy-1>ytop) {
			// Fill in above curve
			SelectPen(hdc,bgPen);
			VertLine(hdc,xcen+i, ytop, iy-1);
			VertLine(hdc,xcen-i, ytop, iy-1);
			}
		if (ylast>=0) {
			SelectPen(hdc,linePen);
			VertLine(hdc,xcen+i-1,iy-1,ylast);
			VertLine(hdc,xcen-i+1,iy-1,ylast);
			}

		ylast = iy;
		}
	SelectObject( hdc, linePen );
	DeleteObject(fgPen);
	DeleteObject(bgPen);
	WhiteRect3D(hdc, rect, 1);
	
	return;
}

void MuscleSetup::ReLocateToPanel(HWND hPanel)
{
	if( !hPanel )
		return;

	IRollupWindow * pCommandRollup = ip->GetCommandPanelRollup();

	if( !pCommandRollup->IsPanelOpen(pCommandRollup->GetPanelIndex(hPanel)) )
		pCommandRollup->SetPanelOpen(pCommandRollup->GetPanelIndex(hPanel), TRUE); 

	int nY = 0;
	for(int i=0; i<pCommandRollup->GetNumPanels(); i++)
	{
		HWND hDlg = pCommandRollup->GetPanelDlg(i);
		if( hDlg == hPanel )
			break;
		else
			nY += pCommandRollup->GetPanelHeight(i);
	}

	pCommandRollup->SetScrollPos(nY);
	return;
}

bool MuscleSetup::ToggleMuscleLink(int indexHost, int indexChild)
{
	if( theMuscleMod.IsMuscleLinked(indexHost, indexChild) )
		theMuscleMod.RemoveMuscleLink(indexHost, indexChild);
	else
		theMuscleMod.AddMuscleLink(indexHost, indexChild);
	return true;
}

Matrix3 MirrorMatrix(const Point3& p, // IN: point on the plane
					 const Point3& n) // IN: plane perpendicular direction
{
	Matrix3 ret;

	//V' = V - 2((V - P)*N)N)
	float dot = p.x*n.x + p.y*n.y + p.z*n.z;
	
	Point3 row0 = Point3(1-2*n.x*n.x, -2*n.y*n.x, -2*n.z*n.x);
	Point3 row1 = Point3(-2*n.x*n.y, 1-2*n.y*n.y, -2*n.z*n.y);
	Point3 row2 = Point3(-2*n.x*n.z, -2*n.y*n.z, 1-2*n.z*n.z);
	Point3 row3 = Point3(2*dot*n.x, 2*dot*n.y, 2*dot*n.z);

	/*
	ret.m[0][0] = 1 - 2*n.x*n.x;
	ret.m[1][0] = - 2*n.x*n.y;
	ret.m[2][0] = - 2*n.x*n.z;
	ret.m[3][0] = 2*dot*n.x;

	ret.m[0][1] = - 2*n.y*n.x;
	ret.m[1][1] = 1 - 2*n.y*n.y;
	ret.m[2][1] = - 2*n.y*n.z;
	ret.m[3][1] = 2*dot*n.y;

	ret.m[0][2] = - 2*n.z*n.x;
	ret.m[1][2] = - 2*n.z*n.y;
	ret.m[2][2] = 1 - 2*n.z*n.z;
	ret.m[3][2] = 2*dot*n.z;*/
	ret.SetRow(0, row0);
	ret.SetRow(1, row1);
	ret.SetRow(2, row2);
	ret.SetRow(3, row3);

	return ret;
}

bool MuscleSetup::MirrorMuscle(int nAxis)
{
	if( GetCurSetupMuscle() < 0 )
		return false;

	Matrix3 tmMirror(TRUE);

	switch(nAxis)
	{
	case 0:
		tmMirror = MirrorMatrix(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f));
		break;

	case 1:
		tmMirror = MirrorMatrix(Point3(0.0f, 0.0f, 0.0f), Point3(0.0f, 1.0f, 0.0f));
		break;

	case 2:
		tmMirror = MirrorMatrix(Point3(0.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 1.0f));
		break;

	default:
		break;
	}

	MUSCLEDATA muscle = theMuscleMod.GetMuscleData(GetCurSetupMuscle());
	muscle.vecCenter = muscle.vecCenter * tmMirror;
	muscle.vecXAxis = muscle.vecXAxis * tmMirror;
	muscle.vecYAxis = muscle.vecYAxis * tmMirror;
	muscle.vecZAxis = muscle.vecZAxis * tmMirror;
	muscle.vecMuscleDir = muscle.vecMuscleDir * tmMirror;

	theMuscleMod.SetMuscleData(GetCurSetupMuscle(), muscle);
	UpdateControlPoints(false);
	return true;
}
