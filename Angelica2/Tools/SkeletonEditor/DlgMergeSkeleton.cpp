// DlgMergeSkeleton.cpp : implementation file
//

#include "Global.h"
#include "SkeletonEditor.h"
#include "MainFrm.h"
#include "SkeletonEditorDoc.h"
#include "CommonFileName.h"
#include "DlgSelectFile.h"
#include "InterfaceWrapper.h"
#include "DlgMergeSkeleton.h"
#include "Render.h"
#include "Model.h"

#include "A3D.h"
#include "AF.h"

#define new A_DEBUG_NEW

//	Track segment
struct TRACKSEGMENT
{
	int		iStartTime;		//	Segment start time in ms
	int		iEndTime;		//	Segment end time in ms
	int		iStartKey;		//	Segment start key frame
	int		iEndKey;		//	Segment end key frame
};

typedef AArray<TRACKSEGMENT, TRACKSEGMENT&> ASegArray;
typedef AArray<A3DVECTOR3, A3DVECTOR3&> APosArray;
typedef AArray<A3DQUATERNION, A3DQUATERNION&> ARotArray;

//	Merge position tracks
bool _MergePosTracks(ASegArray* aSegs, APosArray* aPos, A3DSkeleton* pSkeleton, int& iTotalTime, int iTotalFrame, A3DAnimJoint* pJoint, int iFlag);
//	Merge rotation tracks
bool _MergeRotTracks(ASegArray* aSegs, ARotArray* aRot, A3DSkeleton* pSkeleton, int& iTotalTime, int iTotalFrame, A3DAnimJoint* pJoint, int iFlag);

/////////////////////////////////////////////////////////////////////////////
// CDlgMergeSkeleton dialog


CDlgMergeSkeleton::CDlgMergeSkeleton(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMergeSkeleton::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMergeSkeleton)
	m_strSMDFile = _T("");
	m_strTrackDir = _T("");
	//}}AFX_DATA_INIT
}


void CDlgMergeSkeleton::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMergeSkeleton)
	DDX_Control(pDX, IDC_LIST_SKELETON, m_SkeletonList);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strSMDFile);
	DDX_Text(pDX, IDC_EDIT_TRACKDIR, m_strTrackDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMergeSkeleton, CDialog)
	//{{AFX_MSG_MAP(CDlgMergeSkeleton)
	ON_BN_CLICKED(IDC_BTN_ADDSKELETON, OnBtnAddskeleton)
	ON_BN_CLICKED(IDC_BTN_DELSKELETON, OnBtnDelskeleton)
	ON_BN_CLICKED(IDC_BTN_DOWN, OnBtnDown)
	ON_BN_CLICKED(IDC_BTN_MERGE, OnBtnMerge)
	ON_BN_CLICKED(IDC_BTN_UP, OnBtnUp)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_TOP, OnBtnTop)
	ON_BN_CLICKED(IDC_BTN_BOTTOM, OnBtnBottom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMergeSkeleton message handlers

BOOL CDlgMergeSkeleton::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//	Initialize joint list
	m_SkeletonList.InsertColumn(0, "文件名", LVCFMT_LEFT, 280);
	m_SkeletonList.InsertColumn(1, "骨头数", LVCFMT_LEFT, 50);
	m_SkeletonList.InsertColumn(2, "帧数", LVCFMT_LEFT, 45);
	m_SkeletonList.InsertColumn(3, "FPS", LVCFMT_LEFT, 45);

	m_SkeletonList.SetExtendedStyle(m_SkeletonList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	Add skeleton to list
void CDlgMergeSkeleton::OnBtnAddskeleton() 
{
	CCommonFileName FileDlg(g_Render.GetA3DEngine(), AFILE_ALLOWMULFILES, "Models");
	FileDlg.SetFileExtension("bon");
	if (FileDlg.DoModal() != IDOK)
		return;

	CString strFile;
	char szRelative[MAX_PATH];

	int i, iNumFile = FileDlg.GetSelectedNum();
	for (i=0; i < iNumFile; i++)
	{
		strFile = FileDlg.GetNextSelectedFullName(i);
		af_GetRelativePath(strFile, szRelative);

		//	Check whether this skeleton has existed
		if (CheckFileCollision(szRelative))
		{
			AUX_MessageBox(MB_OK, "骨骼文件 %s 已经存在!", szRelative);
			continue;
		}

		//	Add skeleton
		if (!AddSkeleton(strFile))
			AUX_MessageBox(MB_OK, "添加文件 %s 失败!", szRelative);
	}
}

//	Remove skeleton from list
void CDlgMergeSkeleton::OnBtnDelskeleton() 
{
/*	int iSel;

	POSITION pos = m_SkeletonList.GetFirstSelectedItemPosition();
	if (pos)
		iSel = m_SkeletonList.GetNextSelectedItem(pos);
	else
		return;

	A3DSkeleton* pSkeleton = (A3DSkeleton*)m_SkeletonList.GetItemData(iSel);
	pSkeleton->Release();
	delete pSkeleton;

	m_SkeletonList.DeleteItem(iSel);
*/
}

//	Move skeleton item's position in skeleton list
void CDlgMergeSkeleton::MoveSkeletonItemPos(int iOldPos, int iNewPos, bool bSelNew)
{
	if (iOldPos == iNewPos)
		return;

	//	Remove this item at first
	CString str0, str1, str2, str3;
	str0 = m_SkeletonList.GetItemText(iOldPos, 0);
	str1 = m_SkeletonList.GetItemText(iOldPos, 1);
	str2 = m_SkeletonList.GetItemText(iOldPos, 2);
	str3 = m_SkeletonList.GetItemText(iOldPos, 3);
	DWORD dwItemData = m_SkeletonList.GetItemData(iOldPos);
	m_SkeletonList.DeleteItem(iOldPos);

	//	Insert it to new position
	iNewPos = m_SkeletonList.InsertItem(iNewPos, str0);
	m_SkeletonList.SetItemText(iNewPos, 1, str1);
	m_SkeletonList.SetItemText(iNewPos, 2, str2);
	m_SkeletonList.SetItemText(iNewPos, 3, str3);
	m_SkeletonList.SetItemData(iNewPos, dwItemData);

	if (bSelNew)
		m_SkeletonList.SetItemState(iNewPos, LVIS_SELECTED, LVIS_SELECTED);
}

void CDlgMergeSkeleton::OnBtnDown()
{
	int iSel;

	POSITION pos = m_SkeletonList.GetFirstSelectedItemPosition();
	if (pos)
		iSel = m_SkeletonList.GetNextSelectedItem(pos);
	else
		return;

	if (iSel == m_SkeletonList.GetItemCount()-1)
		return;

	MoveSkeletonItemPos(iSel, iSel+1, true);
}

void CDlgMergeSkeleton::OnBtnUp() 
{
	int iSel;

	POSITION pos = m_SkeletonList.GetFirstSelectedItemPosition();
	if (pos)
		iSel = m_SkeletonList.GetNextSelectedItem(pos);
	else
		return;

	if (!iSel)
		return;

	MoveSkeletonItemPos(iSel, iSel-1, true);
}

void CDlgMergeSkeleton::OnBtnTop() 
{
	int iSel;

	POSITION pos = m_SkeletonList.GetFirstSelectedItemPosition();
	if (pos)
		iSel = m_SkeletonList.GetNextSelectedItem(pos);
	else
		return;

	if (!iSel)
		return;
	
	MoveSkeletonItemPos(iSel, 0, true);
}

void CDlgMergeSkeleton::OnBtnBottom() 
{
	int iSel;

	POSITION pos = m_SkeletonList.GetFirstSelectedItemPosition();
	if (pos)
		iSel = m_SkeletonList.GetNextSelectedItem(pos);
	else
		return;

	if (iSel == m_SkeletonList.GetItemCount()-1)
		return;

	MoveSkeletonItemPos(iSel, m_SkeletonList.GetItemCount()-1, true);
}

void CDlgMergeSkeleton::OnBtnMerge() 
{
	UpdateData(TRUE);

	//	Check skinmodel file
	if (!m_strSMDFile.GetLength() || !AUX_FileExist(m_strSMDFile))
	{
		MessageBox("请先选择一个存在的 Skin Model 文件。");
		return;
	}

	const char szChars[] = {'\\', '/', ':', '*', '?', '"', '<', '>', '|', '\0'};
	if (m_strTrackDir.FindOneOf(szChars) >= 0)
	{
		MessageBox("动作数据相对路径中不能包含字符: \\ / : * ? \" < > |");
		return;
	}

	//	If select skin model is being currently edited, refuse it
	CModel* pModel = AUX_GetMainFrame()->GetDocument()->GetModel();
	if (pModel)
	{
		char szRelFile[MAX_PATH];
		af_GetRelativePath(m_strSMDFile, szRelFile);
		if (!stricmp(szRelFile, pModel->GetA3DSkinModel()->GetFileName()))
		{
			MessageBox("不能处理当前正在编辑的模型文件。");
			return;
		}
	}

	//	Temporary bone file
	sprintf(m_szTempBone, "%sTemp\\TempBone.bon", g_szWorkDir);
	sprintf(m_szTempModel, "%sTemp\\TempModel.smd", g_szWorkDir);
	//	SMD file directory
	af_GetFilePath(m_strSMDFile, m_szSMDDir, MAX_PATH);

	//	Merge bone and export new bone file and track files
	if (m_SkeletonList.GetItemCount())
	{
		if (!MergeSkeleton())
		{
			MessageBox("骨骼合并失败！");
			return;
		}
	}

	//	Update action info in skinmodel file
	if (!UpdateModelActions())
	{
		MessageBox("模型动作更新失败！");
		return;
	}

	//	Copy bone file and track files
	if (!FinishCopyFiles())
	{
		MessageBox("拷贝文件失败！");
		return;
	}

	char szMsg[1024];
	sprintf(szMsg, "操作完成。错误动作个数：%d", m_iBadActCnt);
	MessageBox(szMsg);
}

//	Merge skeletons
bool CDlgMergeSkeleton::MergeSkeleton()
{
	//	Convert all candidate skeleton to new version
	if (!ConvertSkeletonFiles())
	{
		g_Log.Log("CDlgMergeSkeleton::MergeSkeleton, Failed to call ConvertSkeletonFiles()");
		return false;
	}

	//	Load all candidate skeletons and tracks
	if (!LoadSkeletonAndTracks())
	{
		g_Log.Log("CDlgMergeSkeleton::MergeSkeleton, Failed to call LoadSkeletonAndTracks()");
		return false;
	}

	//	Merge skeletons and export the new skeleton
	CSkeletonWrapper Skeleton;

	if (!Skeleton.Init())
	{
		g_Log.Log("CDlgMergeSkeleton::MergeSkeleton, Failed to initialize skeleton");
		return false;
	}

	//	Emit bone data
	if (!EmitBones(&Skeleton))
	{
		g_Log.Log("CDlgMergeSkeleton::MergeSkeleton, Failed to emit bones");
		return false;
	}

	//	Emit joints data
	if (!EmitJoints(&Skeleton))
	{
		g_Log.Log("CDlgMergeSkeleton::MergeSkeleton, Failed to emit joints");
		return false;
	}

	//	Emit hooks data
	if (!EmitHooks(&Skeleton))
	{
		g_Log.Log("CDlgMergeSkeleton::MergeSkeleton, Failed to emit hooks");
		return false;
	}

	if (!Skeleton.Save(m_szTempBone))
	{
		g_Log.Log("CDlgMergeSkeleton::MergeSkeleton, Failed to save skeleton file %s", m_szTempBone);
		return false;
	}

	Skeleton.Release();

	return true;
}

//	Convert all candidate skeleton to new version
bool CDlgMergeSkeleton::ConvertSkeletonFiles()
{
	int i, iTotal = m_SkeletonList.GetItemCount();

	//	Convert all skeleton file to new version
	for (i=0; i < iTotal; i++)
	{
		CString strFile = m_SkeletonList.GetItemText(i, 0);
		if (!AUX_ConvertSkeletonFile(strFile))
		{
			g_Log.Log("CDlgMergeSkeleton::ConvertSkeletonFiles, Failed to convert skeleton file %s", strFile);
			return false;
		}
	}

	return true;
}

//	Load all candidate skeletons and tracks
bool CDlgMergeSkeleton::LoadSkeletonAndTracks()
{
	//	Clear skeletons and tracks which weren't cleaned
	ClearSkeletonsAndTracks();

	int i, iTotal = m_SkeletonList.GetItemCount();
	A3DSkinMan* pA3DSkinMan = g_Render.GetA3DEngine()->GetA3DSkinMan();

	//	Load files
	for (i=0; i < iTotal; i++)
	{
		//	Load skeleton file
		CString strFile = m_SkeletonList.GetItemText(i, 0);
		A3DSkeleton* pSkeleton = pA3DSkinMan->LoadSkeletonFile(strFile);
		if (!pSkeleton)
		{
			g_Log.Log("CDlgMergeSkeleton::LoadSkeletonAndTracks, Failed to load skeleton file %s", strFile);
			return false;
		}

		//	Check whether bone names of skeletons match
		if (i > 0)
		{
			if (!CheckBoneMatch(pSkeleton))
			{
				pA3DSkinMan->ReleaseSkeleton(&pSkeleton);
				g_Log.Log("CDlgMergeSkeleton::LoadSkeletonAndTracks, bones of skeleton (%s) don't match", strFile);
				return false;
			}
		}

		//	Load track file
		char szTckFile[MAX_PATH];
		strcpy(szTckFile, strFile);
		AUX_ChangeFileExt(szTckFile, MAX_PATH, ".stck");

		A3DSklTrackSet* pTrackSet = new A3DSklTrackSet(false);
		if (!pTrackSet->Load(szTckFile))
		{
			delete pTrackSet;
			pA3DSkinMan->ReleaseSkeleton(&pSkeleton);
			g_Log.Log("CDlgMergeSkeleton::LoadSkeletonAndTracks, Failed to load track file %s", szTckFile);
			return false;
		}

		m_aSkeletons.Add(pSkeleton);
		m_aTracks.Add(pTrackSet);
	}

	return true;
}

//	Check whether bones of skeletons match
bool CDlgMergeSkeleton::CheckBoneMatch(A3DSkeleton* pNewSkeleton)
{
	A3DSkeleton* pSkeleton = m_aSkeletons[0];
	ASSERT(pSkeleton);

	if (pSkeleton->GetBoneNum() != pNewSkeleton->GetBoneNum())
	{
		g_Log.Log("CDlgMergeSkeleton::CheckBoneMatch, Bone number don't match");
		return false;
	}

	if (pSkeleton->GetAnimFPS() != pNewSkeleton->GetAnimFPS())
	{
		g_Log.Log("CDlgMergeSkeleton::CheckBoneMatch, FPS don't match");
		return false;
	}

	for (int i=0; i < pSkeleton->GetBoneNum(); i++)
	{
		//	严格来说应该保证相同索引的 bone 具有相同的名字，但后来发现即使是
		//	相同骨骼的模型经过 3DMAX 编辑后导出时并不能保证 bone 的顺序，这很
		//	可能是美工一些无意的操作(比如合并组然后又拆分组等)造成的。新的导
		//	出插件会将 bone 按名字排序，但是为了将就老的 bone 文件，这里只要
		//	求名字匹配而不要求索引也匹配
	//	Below is correct operation
	//	A3DBone* pBone1 = pSkeleton->GetBone(i);
	//	A3DBone* pBone2 = pNewSkeleton->GetBone(i);

	//	if (stricmp(pBone1->GetName(), pBone2->GetName()))
	//	{
	//		g_Log.Log("CDlgMergeSkeleton::CheckBoneMatch, Bone(%d) name don't match, %s != %s", pBone1->GetName(), pBone2->GetName());
	//		return false;
	//	}
	
		//	Here is temporary operation
		A3DBone* pBone1 = pSkeleton->GetBone(i);
		A3DBone* pBone2 = pNewSkeleton->GetBone(pBone1->GetName(), NULL);
		if (!pBone2)
		{
			g_Log.Log("CDlgMergeSkeleton::CheckBoneMatch, Couldn't find bone(%s) in skeleton file %s",
					pBone1->GetName(), pNewSkeleton->GetFileName());
			return false;
		}

		//	Check bone's initial matrix
	//	A3DMATRIX4 mat1 = pBone1->GetBoneInitTM();
	//	A3DMATRIX4 mat2 = pBone2->GetBoneInitTM();

	//	for (int m=0; m < 4; m++)
	//	{
	//		for (int n=0; n < 4; n++)
	//		{
	//			if (fabs(mat1.m[m][n] - mat2.m[m][n]) > 0.000001f)
	//			{
	//				g_Log.Log("CDlgMergeSkeleton::CheckBoneMatch, Model to bone matrix don't match");
	//				return false;
	//			}
	//		}
	//	}
	}

	return true;
}

//	Emit bones' data
bool CDlgMergeSkeleton::EmitBones(CSkeletonWrapper* pNewSkeleton)
{
	int i, j;
	A3DSkeleton* pSrcSkeleton = m_aSkeletons[0];

	//	Set animation fps
	pNewSkeleton->SetAnimFPS(pSrcSkeleton->GetAnimFPS());

	//	Copy bones
	for (i=0; i < pSrcSkeleton->GetBoneNum(); i++)
	{
		A3DBone* pSrcBone = pSrcSkeleton->GetBone(i);

		A3DBone* pDestBone = new A3DBone;
		ASSERT(pDestBone);

		pDestBone->Init(pNewSkeleton);
		pDestBone->SetName(pSrcBone->GetName());
		pDestBone->SetParent(pSrcBone->GetParent());
		pDestBone->SetOriginalMatrix(pSrcBone->GetOriginalMatrix());
		pDestBone->SetBoneInitTM(pSrcBone->GetBoneInitTM());
		pDestBone->SetBoneFlags(pSrcBone->GetBoneFlags());

		//	Copy joints
		int iJoint = pSrcBone->GetFirstJoint();
		while (iJoint >= 0)
		{
			pDestBone->AddJoint(iJoint);
			A3DJoint* pJoint = pSrcSkeleton->GetJoint(iJoint);
			iJoint = pJoint->GetSiblingJoint();
		}

		//	Copy children
		for (j=0; j < pSrcBone->GetChildNum(); j++)
			pDestBone->AddChild(pSrcBone->GetChild(j));

		pNewSkeleton->AddBone(pDestBone);
	}

	return true;
}

//	Emit joints' data
bool CDlgMergeSkeleton::EmitJoints(CSkeletonWrapper* pNewSkeleton)
{
	A3DSkeleton* pFirstSkeleton = m_aSkeletons[0];
	int iNumBone = pFirstSkeleton->GetBoneNum();

	int i;
	
	for (i=0; i < iNumBone; i++)
	{
		A3DAnimJoint* pJoint = pFirstSkeleton->GetBone(i)->GetAnimJointPtr();

		A3DAnimJoint* pDest = new A3DAnimJoint;
		if (!pDest)
			return false;

		pDest->Init(pNewSkeleton);
		pDest->SetBones(pJoint->GetParentBone(), pJoint->GetChildBone());
		pDest->SetName(pJoint->GetName());

		pNewSkeleton->AddJoint(pDest);
	}

/*	A3DSkeleton* pFirstSkeleton = m_aSkeletons[0];
	int iNumBone = pFirstSkeleton->GetBoneNum();

	ASegArray aPosSegs, aRotSegs;
	APosArray aPos;
	ARotArray aRot;

	m_aAnimJoints.RemoveAll(false);

	int i, j;
	
	for (i=0; i < iNumBone; i++)
	{
		int iSkeletonCnt  = 0;
		int iPosTotalTime = 0;
		int iRotTotalTime = 0;

		int iNumFrame = 0;
		int iTotalFrame = 0;

		AString strBoneName = pFirstSkeleton->GetBone(i)->GetName();
			
		//	Merge position and rotation tracks
		for (j=0; j < m_SkeletonList.GetItemCount(); j++)
		{
			A3DSkeleton* pSkeleton = (A3DSkeleton*)m_SkeletonList.GetItemData(j);

			iNumFrame = pSkeleton->GetAnimEndFrame() - pSkeleton->GetAnimStartFrame();
			iTotalFrame += iNumFrame + 1;

			//	For the reason said in function AddSkeleton(), we have to use
			//	bone name instead of bone index.
			//	Below is operation using bone index
		//	A3DAnimJoint* pJoint = pSkeleton->GetBone(i)->GetAnimJointPtr();
			//	Below is operation using bone name
			A3DAnimJoint* pJoint = NULL;
			if (!j)
				pJoint = pSkeleton->GetBone(i)->GetAnimJointPtr();
			else
				pJoint = pSkeleton->GetBone(strBoneName, NULL)->GetAnimJointPtr();
		
			int iFlag = 0;
			if (!iSkeletonCnt)
				iFlag |= 0x01;
			if (iSkeletonCnt == m_SkeletonList.GetItemCount()-1)
				iFlag |= 0x02;

			//	Merge position tracks
			_MergePosTracks(&aPosSegs, &aPos, pSkeleton, iPosTotalTime, iTotalFrame, pJoint, iFlag);

			//	Merge rotation tracks
			_MergeRotTracks(&aRotSegs, &aRot, pSkeleton, iRotTotalTime, iTotalFrame, pJoint, iFlag);

			iSkeletonCnt++;
		}

		A3DAnimJoint* pJoint = pFirstSkeleton->GetBone(i)->GetAnimJointPtr();

		A3DAnimJoint* pDest = new A3DAnimJoint;
		if (!pDest)
			return false;

		pDest->Init(pNewSkeleton);
		pDest->SetBones(pJoint->GetParentBone(), pJoint->GetChildBone());
		pDest->SetName(pJoint->GetName());

		//	Create position and rotation tracks
		if (!pDest->CreatePosTrack(aPos.GetSize(), pFirstSkeleton->GetAnimFPS(), aPosSegs.GetSize()) ||
			!pDest->CreateRotTrack(aRot.GetSize(), pFirstSkeleton->GetAnimFPS(), aRotSegs.GetSize()))
		{
			g_Log.Log("CModel::EmitJoints, Failed to create tracks");
			return false;
		}

		//	Copy position track data
		A3DVECTOR3* aDestPos = pDest->GetPosTrack()->GetKeyFrames();
		for (j=0; j < aPos.GetSize(); j++)
			aDestPos[j] = aPos[j];

		//	Copy position track segments
		A3DVector3Track::SEGMENT* aDestPosSegs = pDest->GetPosTrack()->GetSegments();
		for (j=0; j < aPosSegs.GetSize(); j++)
		{
			aDestPosSegs[j].iStartTime	= aPosSegs[j].iStartTime;
			aDestPosSegs[j].iEndTime	= aPosSegs[j].iEndTime;
			aDestPosSegs[j].iStartKey	= aPosSegs[j].iStartKey;
			aDestPosSegs[j].iEndKey		= aPosSegs[j].iEndKey;
		}

		//	Copy rotation track data
		A3DQUATERNION* aDestRot = pDest->GetRotTrack()->GetKeyFrames();
		for (j=0; j < aRot.GetSize(); j++)
			aDestRot[j] = aRot[j];

		//	Copy rotation track segments
		A3DQuaternionTrack::SEGMENT* aDestRotSegs = pDest->GetRotTrack()->GetSegments();
		for (j=0; j < aRotSegs.GetSize(); j++)
		{
			aDestRotSegs[j].iStartTime	= aRotSegs[j].iStartTime;
			aDestRotSegs[j].iEndTime	= aRotSegs[j].iEndTime;
			aDestRotSegs[j].iStartKey	= aRotSegs[j].iStartKey;
			aDestRotSegs[j].iEndKey		= aRotSegs[j].iEndKey;
		}

		int iJointIdx = pNewSkeleton->AddJoint(pDest);
		m_aAnimJoints.Add(iJointIdx);

		aPosSegs.RemoveAll(false);
		aRotSegs.RemoveAll(false);
		aPos.RemoveAll(false);
		aRot.RemoveAll(false);
	}
*/
	return true;
}

//	Emit hooks' data
bool CDlgMergeSkeleton::EmitHooks(CSkeletonWrapper* pNewSkeleton)
{
	int i, j;
	A3DSkeleton* pFirstSkeleton = m_aSkeletons[0];

	for (i=0; i < m_SkeletonList.GetItemCount(); i++)
	{
		A3DSkeleton* pSkeleton = m_aSkeletons[i];
		int iNumHook = pSkeleton->GetHookNum();

		for (j=0; j < iNumHook; j++)
		{
			A3DSkeletonHook* pSrcHook = pSkeleton->GetHook(j);

			//	Check whether there is a hook with the same name
			if (pNewSkeleton->GetHook(pSrcHook->GetName(), NULL))
				continue;

			A3DSkeletonHook* pDestHook = new A3DSkeletonHook(pNewSkeleton);
			ASSERT(pDestHook);

			pDestHook->SetHookType(pSrcHook->GetHookType());
			pDestHook->SetName(pSrcHook->GetName());
			pDestHook->SetHookTM(pSrcHook->GetHookTM());

			//	For the reason said in function AddSkeleton(), we have to use
			//	bone name instead of bone index.
			//	Below is operation using bnoe index
		//	pDestHook->SetBone(pSrcHook->GetBone());
			//	Below is operation using bone name
			if (!i)	//	Is first skeleton ?
			{
				pDestHook->SetBone(pSrcHook->GetBone());
			}
			else
			{
				int iIndex;
				A3DBone* pBone = pSkeleton->GetBone(pSrcHook->GetBone());
				if (!pBone)	//	This hook isn't linked to a bone
					pDestHook->SetBone(-1);
				else if (pFirstSkeleton->GetBone(pBone->GetName(), &iIndex))
					pDestHook->SetBone(iIndex);
				else	//	Counldn't find specified bone in first skeleton
				{
					delete pDestHook;
					continue;
				}
			}

			pNewSkeleton->AddHook(pDestHook);
		}
	}
	
	return true;
}

//	Check skeleton file collision
bool CDlgMergeSkeleton::CheckFileCollision(const char* szFile)
{
	int iNumFile = m_SkeletonList.GetItemCount();
	if (!iNumFile)
		return false;

	CString strTemp;

	for (int i=0; i < iNumFile; i++)
	{
		strTemp = m_SkeletonList.GetItemText(i, 0);
		if (!stricmp(strTemp, szFile))
			return true;
	}

	return false;
}

//	Add skeleton candidate
bool CDlgMergeSkeleton::AddSkeleton(const char* szFile)
{
	SKELETONINFO info1, info2;

	//	Read skeleton file info
	if (!AUX_ReadSkeletonInfo(szFile, info1))
	{
		g_Log.Log("CDlgMergeSkeleton::AddSkeleton, failed to read skeleton info of file %s", szFile);
		return false;
	}

	if (info1.dwVersion >= 6)
	{
		//	Read track file info
		char szTckFile[MAX_PATH];
		strcpy(szTckFile, szFile);
		AUX_ChangeFileExt(szTckFile, MAX_PATH, ".stck");
		if (!AUX_ReadTrackInfo(szTckFile, info2))
		{
			g_Log.Log("CDlgMergeSkeleton::AddSkeleton, failed to read track info of file %s", szFile);
			return false;
		}

		if (info1.iAnimFPS != info2.iAnimFPS || info1.iNumBone != info2.iNumBone)
		{
			g_Log.Log("CDlgMergeSkeleton::AddSkeleton, bone (%s) and his track don't match", szFile);
			return false;
		}

		info1.iStartFrame	= info2.iStartFrame;
		info1.iEndFrame		= info2.iEndFrame;
	}

	//	Add skeleton to list
	AddSkeletonToList(szFile, info1);

	//	Release all resources
/*	A3DSkeleton* pNewSkeleton = new A3DSkeleton;
	ASSERT(pNewSkeleton);

	if (!pNewSkeleton->Init())
	{
		delete pNewSkeleton;
		g_Log.Log("CDlgMergeSkeleton::AddSkeleton, Failed to initialize skeleton object");
		return false;
	}

	if (!pNewSkeleton->Load(szFile))
	{
		pNewSkeleton->Release();
		delete pNewSkeleton;
		AUX_MessageBox(MB_OK, "Failed to load skeleton from file %s", szFile);
		return false;
	}

	if (!m_SkeletonList.GetItemCount())
	{
		//	Add skeleton to list
		AddSkeletonToList(pNewSkeleton);
		return true;
	}

	//	Get the first skeleton as sample
	A3DSkeleton* pSkeleton = (A3DSkeleton*)m_SkeletonList.GetItemData(0);

	if (pSkeleton->GetBoneNum() != pNewSkeleton->GetBoneNum())
	{
		pNewSkeleton->Release();
		delete pNewSkeleton;
		g_Log.Log("CDlgMergeSkeleton::AddSkeleton, Bone number don't match");
		return false;
	}

	if (pSkeleton->GetAnimFPS() != pNewSkeleton->GetAnimFPS())
	{
		pNewSkeleton->Release();
		delete pNewSkeleton;
		g_Log.Log("CDlgMergeSkeleton::AddSkeleton, FPS don't match");
		return false;
	}

	for (int i=0; i < pSkeleton->GetBoneNum(); i++)
	{
		//	严格来说应该保证相同索引的 bone 具有相同的名字，但后来发现即使是
		//	相同骨骼的模型经过 3DMAX 编辑后导出时并不能保证 bone 的顺序，这很
		//	可能是美工一些无意的操作(比如合并组然后又拆分组等)造成的。新的导
		//	出插件会将 bone 按名字排序，但是为了将就老的 bone 文件，这里只要
		//	求名字匹配而不要求索引也匹配
	//	Below is correct operation
	//	A3DBone* pBone1 = pSkeleton->GetBone(i);
	//	A3DBone* pBone2 = pNewSkeleton->GetBone(i);

	//	if (stricmp(pBone1->GetName(), pBone2->GetName()))
	//	{
	//		pNewSkeleton->Release();
	//		delete pNewSkeleton;
	//		g_Log.Log("CDlgMergeSkeleton::AddSkeleton, Bone(%d) name don't match, %s != %s", pBone1->GetName(), pBone2->GetName());
	//		return false;
	//	}
	
		//	Here is temporary operation
		A3DBone* pBone1 = pSkeleton->GetBone(i);
		A3DBone* pBone2 = pNewSkeleton->GetBone(pBone1->GetName(), NULL);
		if (!pBone2)
		{
			pNewSkeleton->Release();
			delete pNewSkeleton;
			g_Log.Log("CDlgMergeSkeleton::AddSkeleton, Couldn't find bone(%s) in skeleton file %s", pBone1->GetName(), szFile);
			return false;
		}

		//	Check bone's initial matrix
	//	A3DMATRIX4 mat1 = pBone1->GetBoneInitTM();
	//	A3DMATRIX4 mat2 = pBone2->GetBoneInitTM();

	//	for (int m=0; m < 4; m++)
	//	{
	//		for (int n=0; n < 4; n++)
	//		{
	//			if (fabs(mat1.m[m][n] - mat2.m[m][n]) > 0.000001f)
	//			{
	//				pNewSkeleton->Release();
	//				delete pNewSkeleton;
	//				g_Log.Log("CDlgMergeSkeleton::AddSkeleton, Model to bone matrix don't match");
	//				return false;
	//			}
	//		}
	//	}
	}

	//	Add skeleton to list
	AddSkeletonToList(pNewSkeleton);
*/
	return true;
}

//	Add skeleton to list
void CDlgMergeSkeleton::AddSkeletonToList(const char* szFile, const SKELETONINFO& info)
{
	char szRelFile[MAX_PATH];
	af_GetRelativePath(szFile, szRelFile);

	int iItem = m_SkeletonList.InsertItem(m_SkeletonList.GetItemCount(), szRelFile);

	char szText[50];
	itoa(info.iNumBone, szText, 10);
	m_SkeletonList.SetItemText(iItem, 1, szText);

	itoa(info.iEndFrame - info.iStartFrame, szText, 10);
	m_SkeletonList.SetItemText(iItem, 2, szText);

	itoa(info.iAnimFPS, szText, 10);
	m_SkeletonList.SetItemText(iItem, 3, szText);
}

//	Release all skeletions
void CDlgMergeSkeleton::ReleaseAllSkeletons()
{
/*	for (int i=0; i < m_SkeletonList.GetItemCount(); i++)
	{
		A3DSkeleton* pSkeleton = (A3DSkeleton*)m_SkeletonList.GetItemData(i);
		A3DRELEASE(pSkeleton);
	}

	m_SkeletonList.DeleteAllItems();
*/
}

//	Clear skeleton and track array
void CDlgMergeSkeleton::ClearSkeletonsAndTracks()
{
	int i;
	A3DSkinMan* pA3DSkinMan = g_Render.GetA3DEngine()->GetA3DSkinMan();

	for (i=0; i < m_aSkeletons.GetSize(); i++)
	{
		pA3DSkinMan->ReleaseSkeleton(&m_aSkeletons[i]);
	}

	for (i=0; i < m_aTracks.GetSize(); i++)
	{
		delete m_aTracks[i];
	}

	m_aSkeletons.RemoveAll();
	m_aTracks.RemoveAll();
}

void CDlgMergeSkeleton::OnDestroy() 
{
	CDialog::OnDestroy();
}

/*	Merge position tracks

	aSegs: segments array
	aPos: position array
	pSkeleton: skeleton object
	iTotalTime (in & out): total time
	iTotalFrame: total frame number after this track merge (including the 1 separate frame)
	pJoint: address of joint whose track will be merged
	iFlag: bits: 0x00, normal skeleton
				 0x01, this is the first skeleton in list
				 0x02, this is the last skeleton in list
*/
bool _MergePosTracks(ASegArray* aSegs, APosArray* aPos, A3DSkeleton* pSkeleton, int& iTotalTime, int iTotalFrame, A3DAnimJoint* pJoint, int iFlag)
{
/*	A3DVector3Track* pPosTrack = pJoint->GetPosTrack();
	A3DVector3Track::SEGMENT* aSrcSegs = pPosTrack->GetSegments();
	A3DVECTOR3* aSrcPos = pPosTrack->GetKeyFrames();
	
	int iStartFrame = pSkeleton->GetAnimStartFrame();
	int iEndFrame = pSkeleton->GetAnimEndFrame();
	int iStartTime = pSkeleton->FrameToTime(iStartFrame);
	int iEndTime = pSkeleton->FrameToTime(iEndFrame);

	if (iFlag & 0x01)
		iTotalTime = iStartTime;

	if (iStartTime != pPosTrack->GetTrackStartTime())
	{
		if (!(iFlag & 0x01))	//	Isn't the first skeleton
		{
			//	Add an extra segment and key frame
			TRACKSEGMENT Seg;
			Seg.iStartTime	= iTotalTime;
			Seg.iEndTime	= iTotalTime + pPosTrack->GetTrackStartTime();
			Seg.iStartKey	= aPos->GetSize();
			Seg.iEndKey		= aPos->GetSize();
			aSegs->Add(Seg);

			aPos->Add(aSrcPos[0]);
		}
	}

	int i;

	//	Copy segments in source joint
	for (i=0; i < pPosTrack->GetSegmentNum(); i++)
	{
		TRACKSEGMENT Seg;
		Seg.iStartTime	= iTotalTime + aSrcSegs[i].iStartTime;
		Seg.iEndTime	= iTotalTime + aSrcSegs[i].iEndTime;
		Seg.iStartKey	= aPos->GetSize() + aSrcSegs[i].iStartKey;
		Seg.iEndKey		= aPos->GetSize() + aSrcSegs[i].iEndKey;
		aSegs->Add(Seg);
	}

	//	Copy key frames
	for (i=0; i < pPosTrack->GetNumKeys(); i++)
		aPos->Add(aSrcPos[i]);

	if (!(iFlag & 0x02))	//	Isn't the last skeleton
	{
		int iBeginTime;

		//	Add an extra separate segment
		if (iEndTime != pPosTrack->GetTrackEndTime())
			iBeginTime = pPosTrack->GetTrackEndTime();
		else
			iBeginTime = iEndTime;

	//	iEndTime = pSkeleton->FrameToTime(iEndFrame+1);
		iEndTime = pSkeleton->FrameToTime(iTotalFrame) - iTotalTime;
		ASSERT(iEndTime >= 0 && iEndTime >= iBeginTime);

		//	Add an extra segment and key frame
		TRACKSEGMENT Seg;
		Seg.iStartTime	= iTotalTime + iBeginTime;
		Seg.iEndTime	= iTotalTime + iEndTime;
		Seg.iStartKey	= aPos->GetSize();
		Seg.iEndKey		= aPos->GetSize();
		aSegs->Add(Seg);

		aPos->Add(aSrcPos[pPosTrack->GetNumKeys()-1]);
	}

	iTotalTime += iEndTime - iStartTime;
*/
	return true;
}

//	Merge rotation tracks
bool _MergeRotTracks(ASegArray* aSegs, ARotArray* aRot, A3DSkeleton* pSkeleton, int& iTotalTime, int iTotalFrame, A3DAnimJoint* pJoint, int iFlag)
{
/*	A3DQuaternionTrack* pRotTrack = pJoint->GetRotTrack();
	A3DQuaternionTrack::SEGMENT* aSrcSegs = pRotTrack->GetSegments();
	A3DQUATERNION* aSrcRot = pRotTrack->GetKeyFrames();
	
	int iStartFrame = pSkeleton->GetAnimStartFrame();
	int iEndFrame = pSkeleton->GetAnimEndFrame();
	int iStartTime = pSkeleton->FrameToTime(iStartFrame);
	int iEndTime = pSkeleton->FrameToTime(iEndFrame);

	if (iFlag & 0x01)
		iTotalTime = iStartTime;

	if (iStartTime != pRotTrack->GetTrackStartTime())
	{
		if (!(iFlag & 0x01))	//	Isn't the first skeleton
		{
			//	Add an extra segment and key frame
			TRACKSEGMENT Seg;
			Seg.iStartTime	= iTotalTime;
			Seg.iEndTime	= iTotalTime + pRotTrack->GetTrackStartTime();
			Seg.iStartKey	= aRot->GetSize();
			Seg.iEndKey		= aRot->GetSize();
			aSegs->Add(Seg);

			aRot->Add(aSrcRot[0]);
		}
	}

	int i;

	//	Copy segments in source joint
	for (i=0; i < pRotTrack->GetSegmentNum(); i++)
	{
		TRACKSEGMENT Seg;
		Seg.iStartTime	= iTotalTime + aSrcSegs[i].iStartTime;
		Seg.iEndTime	= iTotalTime + aSrcSegs[i].iEndTime;
		Seg.iStartKey	= aRot->GetSize() + aSrcSegs[i].iStartKey;
		Seg.iEndKey		= aRot->GetSize() + aSrcSegs[i].iEndKey;
		aSegs->Add(Seg);
	}

	//	Copy key frames
	for (i=0; i < pRotTrack->GetNumKeys(); i++)
		aRot->Add(aSrcRot[i]);

	if (!(iFlag & 0x02))	//	Isn't the last skeleton
	{
		int iBeginTime;

		//	Add an extra separate segment
		if (iEndTime != pRotTrack->GetTrackEndTime())
			iBeginTime = pRotTrack->GetTrackEndTime();
		else
			iBeginTime = iEndTime;

	//	iEndTime = pSkeleton->FrameToTime(iEndFrame+1);
		iEndTime = pSkeleton->FrameToTime(iTotalFrame) - iTotalTime;
		ASSERT(iEndTime >= 0 && iEndTime >= iBeginTime);

		TRACKSEGMENT Seg;
		Seg.iStartTime	= iTotalTime + iBeginTime;
		Seg.iEndTime	= iTotalTime + iEndTime;
		Seg.iStartKey	= aRot->GetSize();
		Seg.iEndKey		= aRot->GetSize();
		aSegs->Add(Seg);

		aRot->Add(aSrcRot[pRotTrack->GetNumKeys()-1]);
	}

	iTotalTime += iEndTime - iStartTime;
*/
	return true;
}

void CDlgMergeSkeleton::OnCancel() 
{
	//	Clear skeletons and tracks which weren't cleaned
	ClearSkeletonsAndTracks();

	//	Release all skeletons
	ReleaseAllSkeletons();

	CDialog::OnCancel();
}

void CDlgMergeSkeleton::OnBtnBrowse() 
{
	CCommonFileName FileDlg(g_Render.GetA3DEngine(), 0);
	FileDlg.SetFileExtension("smd");
	if (FileDlg.DoModal() != IDOK)
		return;

	m_strSMDFile = FileDlg.GetFullFileName();

	CSkinModel* pSkinModel = new CSkinModel;
	if (!pSkinModel->Init(g_Render.GetA3DEngine()))
		return;

	if (!pSkinModel->Load(m_strSMDFile))
	{
		pSkinModel->Release();
		delete pSkinModel;
		MessageBox("加载模型文件失败!");
		return;
	}

	m_strTrackDir = pSkinModel->GetTrackDataDir();
	if (!m_strTrackDir.GetLength())
	{
		AString strTitle;
		af_GetFileTitle(m_strSMDFile, strTitle);
		af_ChangeFileExt(strTitle, "");
		m_strTrackDir.Format("tcks_%s", strTitle);
	}

	A3DRELEASE(pSkinModel);

	CDataExchange dx1(this, FALSE);
	DDX_Text(&dx1, IDC_EDIT_FILENAME, m_strSMDFile);

	CDataExchange dx2(this, FALSE);
	DDX_Text(&dx2, IDC_EDIT_TRACKDIR, m_strTrackDir);
}

//	Update action info in skinmodel file
bool CDlgMergeSkeleton::UpdateModelActions()
{
	//	Load skin model
	CSkinModel* pSkinModel = new CSkinModel;
	if (!pSkinModel->Init(g_Render.GetA3DEngine()))
		return false;

	if (!pSkinModel->Load(m_strSMDFile))
	{
		pSkinModel->Release();
		delete pSkinModel;
		g_Log.Log("CDlgMergeSkeleton::UpdateModelActions, Failed to load skin model %s", m_strSMDFile);
		return false;
	}

	//	Remove all old actions
	RemoveAllActions(pSkinModel);

	//	Change track data directory
	pSkinModel->SetTrackDataDir(m_strTrackDir);

	int i;
	A3DSkinModelMan* pSkinModelMan = g_Render.GetA3DEngine()->GetA3DSkinModelMan();

	m_iBadActCnt = 0;

	for (i=0; i < m_aTracks.GetSize(); i++)
	{
		A3DSklTrackSet* pTrackSet = m_aTracks[i];

		//	Get skeleton file title as action's name. Becuase we have check
		//	skeleton file name collision, so don't worry about same name 
		//	actions will be created.
		char szName[MAX_PATH];
		af_GetFileTitle(m_aSkeletons[i]->GetFileName(), szName, MAX_PATH);
		AUX_ChangeFileExt(szName, MAX_PATH, "");

		int iFrameRate = pTrackSet->GetAnimFPS();
		A3DSkinModelActionCore* pActionCore = new A3DSkinModelActionCore(iFrameRate);

		//	Set frame
		pActionCore->SetName(szName);
		float f1 = (float)pTrackSet->GetAnimStartFrame();
		float f2 = (float)pTrackSet->GetAnimEndFrame();
		pActionCore->SetStartAndEndFrame(f1, f2);

		//	Set track set file
		strcat(szName, ".stck");
		pActionCore->SetTrackSetFileName(szName, true);

		if (!pSkinModelMan->AddAction(pSkinModel, pActionCore))
		{
			delete pActionCore;
			m_iBadActCnt++;
			g_Log.Log("CPageEditAction::UpdateModelActions, Failed to add action %s", szName);
			continue;
		}
	}

	//	Force to change skin model version if necessary, otherwise Save function
	//	will fail.
	if (pSkinModel->GetVersion() < 7)
		pSkinModel->SetVersion(SKMDFILE_VERSION);

	//	Save model data to temporary file
	if (!pSkinModel->Save(m_szTempModel))
	{
		pSkinModel->Release();
		delete pSkinModel;
		g_Log.Log("CPageEditAction::UpdateModelActions, Failed to save model %s", m_strSMDFile);
		return false;
	}

	pSkinModel->Release();
	delete pSkinModel;

	return true;
}

//	Remove all actions in skin model
void CDlgMergeSkeleton::RemoveAllActions(A3DSkinModel* pSkinModel)
{
	APtrArray<CString*> aActNames;

	//	Get all action name
	A3DSkinModelActionCore* pAction = pSkinModel->GetFirstAction();
	while (pAction)
	{
		CString* pstr = new CString(pAction->GetName());
		aActNames.Add(pstr);

		pAction = pSkinModel->GetNextAction();
	}

	A3DSkinModelMan* pSkinModelMan = g_Render.GetA3DEngine()->GetA3DSkinModelMan();

	//	Remove all actions
	for (int i=0; i < aActNames.GetSize(); i++)
	{
		pSkinModelMan->RemoveActionByName(pSkinModel, *aActNames[i]);
		delete aActNames[i];
	}
}

//	Copy completed skeleton and track files
bool CDlgMergeSkeleton::FinishCopyFiles()
{
	//	Build new skeleton file name
	char szFile[MAX_PATH];
	sprintf(szFile, "%s%s", g_szWorkDir, m_strSMDFile);
	AUX_ChangeFileExt(szFile, MAX_PATH, ".bon");

	CopyFile(m_szTempBone, szFile, FALSE);

	//	Copy skin model file
	CopyFile(m_szTempModel, m_strSMDFile, FALSE);

	//	Create tracks directory
	char szTckDir[MAX_PATH], szPath[MAX_PATH];
	af_GetFilePath(m_strSMDFile, szPath, MAX_PATH);
	if (m_strTrackDir.GetLength())
		sprintf(szTckDir, "%s\\%s", szPath, m_strTrackDir);
	else
		strcpy(szTckDir, szPath);

	AUX_CreateDirectory(szTckDir);

	//	Clear all old track set files in this directory
	sprintf(szFile, "%s\\*.stck", szTckDir);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(szFile, &FindFileData);
	
	if (hFind != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(hFind, &FindFileData) != 0)
		{
			DeleteFile(FindFileData.cFileName);
		}

		FindClose(hFind);
	}

	//	Copy track set files
	char szFile2[MAX_PATH], szTitle[MAX_PATH];

	for (int i=0; i < m_aTracks.GetSize(); i++)
	{
		//	Source file name
		sprintf(szFile, "%s%s", g_szWorkDir, m_aSkeletons[i]->GetFileName());
		AUX_ChangeFileExt(szFile, MAX_PATH, ".stck");
		af_GetFileTitle(szFile, szTitle, MAX_PATH);

		//	Destination file name
		sprintf(szFile2, "%s\\%s", szTckDir, szTitle);

		//	Do copy
		CopyFile(szFile, szFile2, FALSE);
	}

	return true;
}



