// PageSuppleTest.cpp : implementation file
//

#include "Global.h"
#include "Model.h"
#include "PageSuppleTest.h"
#include "A3D.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CPageSuppleTest dialog


CPageSuppleTest::CPageSuppleTest(CWnd* pParent /*=NULL*/)
	: CDialog(CPageSuppleTest::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPageSuppleTest)
	m_strWarning = _T("");
	m_strCoefficient = _T("0.0");
	m_strAirResist = _T("0.0");
	m_strGravity = _T("0.0");
	m_strVertWeight = _T("0.0");
	m_strLength = _T("0.0");
	m_strRadius = _T("0.0");
	m_strX = _T("0.0");
	m_strY = _T("0.0");
	m_strZ = _T("0.0");
	//}}AFX_DATA_INIT

	m_pModel = NULL;
}


void CPageSuppleTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageSuppleTest)
	DDX_Control(pDX, IDC_LIST_PHYSHAPE, m_PhyShapeList);
	DDX_Control(pDX, IDC_SPIN_Z, m_ZSpin);
	DDX_Control(pDX, IDC_SPIN_Y, m_YSpin);
	DDX_Control(pDX, IDC_SPIN_X, m_XSpin);
	DDX_Control(pDX, IDC_SPIN_RADIUS, m_RadiusSpin);
	DDX_Control(pDX, IDC_LIST_SUPPLEMESH, m_SuppleMeshList);
	DDX_Control(pDX, IDC_SPIN_LENGTH, m_LengthSpin);
	DDX_Text(pDX, IDC_STATIC_WARNING, m_strWarning);
	DDX_Text(pDX, IDC_EDIT_COEFFICIENT, m_strCoefficient);
	DDX_Text(pDX, IDC_EDIT_AIRRESIST, m_strAirResist);
	DDX_Text(pDX, IDC_EDIT_GRAVITY, m_strGravity);
	DDX_Text(pDX, IDC_EDIT_VERTWEIGHT, m_strVertWeight);
	DDX_Text(pDX, IDC_EDIT_LENGTH, m_strLength);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_strRadius);
	DDX_Text(pDX, IDC_EDIT_X, m_strX);
	DDX_Text(pDX, IDC_EDIT_Y, m_strY);
	DDX_Text(pDX, IDC_EDIT_Z, m_strZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageSuppleTest, CDialog)
	//{{AFX_MSG_MAP(CPageSuppleTest)
	ON_LBN_SELCHANGE(IDC_LIST_PHYSHAPE, OnSelchangeListPhyshape)
	ON_LBN_SELCHANGE(IDC_LIST_SUPPLEMESH, OnSelchangeListSupplemesh)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_X, OnDeltaposSpinX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Y, OnDeltaposSpinY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Z, OnDeltaposSpinZ)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH, OnDeltaposSpinLength)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RADIUS, OnDeltaposSpinRadius)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_ACTIVATEPAGE, OnActivatePage)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageSuppleTest message handlers

BOOL CPageSuppleTest::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_strWarning = "这里提供修改柔体数据的功能只是为了方便观看柔体的效果，修改后"
		"的数据不会被存到模型文件中。正确的做法是在这里调整到满意的效果后，再在"
		"3DMAX中进行相应的参数设置。";

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	Release resources
void CPageSuppleTest::Release()
{
}

//	Bind model object with this dialog
bool CPageSuppleTest::BindModel(CModel* pModel)
{
	//	Clear old information
	if (m_pModel)
		ClearAllInfo();

	if (!pModel)
		return true;

	m_pModel = pModel;

	m_XSpin.SetBuddy(GetDlgItem(IDC_EDIT_X));
	m_YSpin.SetBuddy(GetDlgItem(IDC_EDIT_Y));
	m_ZSpin.SetBuddy(GetDlgItem(IDC_EDIT_Z));
	m_LengthSpin.SetBuddy(GetDlgItem(IDC_EDIT_LENGTH));
	m_RadiusSpin.SetBuddy(GetDlgItem(IDC_EDIT_RADIUS));

	//	Update supple mesh list
	UpdateSuppleMeshList();

	//	Update physique shape list
	UpdatePhysiqueShapeList();

	return true;
}

//	Clear all information in dialog
void CPageSuppleTest::ClearAllInfo()
{
	m_PhyShapeList.ResetContent();
	m_SuppleMeshList.ResetContent();

	g_Configs.iEditPhyShape = -1;

	UpdateData(FALSE);
}

//	On activate / disactivate this page
void CPageSuppleTest::OnActivatePage(WPARAM wParam, LPARAM lParam)
{
	if (wParam)	//	Activate this page
	{
		int iSel = m_PhyShapeList.GetCurSel();
		if (iSel != LB_ERR)
			g_Configs.iEditPhyShape = iSel;
	}
	else	//	Disactivate this page
	{
		g_Configs.iEditPhyShape = -1;
	}
}

//	Update supple mesh list
void CPageSuppleTest::UpdateSuppleMeshList()
{
	if (!m_pModel)
		return;

	A3DSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();

	int i, j, iNumSkin = pSkinModel->GetSkinNum();

	for (i=0; i < iNumSkin; i++)
	{
		A3DSkin* pSkin = pSkinModel->GetA3DSkin(i);
		if (!pSkin)
			continue;

		int iNumSuppleMesh = pSkin->GetSuppleMeshNum();
		for (j=0; j < iNumSuppleMesh; j++)
		{
			A3DSuppleMesh* pSuppleMesh = pSkin->GetSuppleMesh(j);
			int iItem = m_SuppleMeshList.InsertString(-1, pSuppleMesh->GetName());
			m_SuppleMeshList.SetItemData(iItem, (DWORD)pSuppleMesh);
		}
	}
}

//	Update physique shape list
void CPageSuppleTest::UpdatePhysiqueShapeList()
{
	if (!m_pModel)
		return;

	A3DSkinModel* pSkinModel = m_pModel->GetA3DSkinModel();

	int i, iNumPhyShape = pSkinModel->GetSkinPhyShapeNum();

	for (i=0; i < iNumPhyShape; i++)
	{
		A3DSkinPhyShape* pPhyShape = pSkinModel->GetSkinPhyShape(i);
		int iItem = m_PhyShapeList.InsertString(-1, pPhyShape->GetName());
		m_PhyShapeList.SetItemData(iItem, (DWORD)pPhyShape);
	}
}

void CPageSuppleTest::OnSelchangeListSupplemesh() 
{
	int iSel = m_SuppleMeshList.GetCurSel();
	if (iSel != LB_ERR)
		UpdateSuppleData(iSel, false);
}

void CPageSuppleTest::OnSelchangeListPhyshape() 
{
	int iSel = m_PhyShapeList.GetCurSel();
	if (iSel != LB_ERR)
	{
		g_Configs.iEditPhyShape = iSel;
		UpdatePhysiqueShapeData(iSel, false);
	}
	else
		g_Configs.iEditPhyShape = -1;
}

//	Update supple data
void CPageSuppleTest::UpdateSuppleData(int iItem, bool bSave)
{
	A3DSuppleMesh* pSuppleMesh = (A3DSuppleMesh*)m_SuppleMeshList.GetItemData(iItem);

	if (bSave)
	{
		UpdateData(TRUE);

		pSuppleMesh->SetGravityFactor(atof(m_strGravity));
		pSuppleMesh->SetAirResistance(atof(m_strAirResist));
		pSuppleMesh->SetVertWeight(atof(m_strVertWeight));

		int i, iNumSpring = pSuppleMesh->GetSpringNum();
		A3DSuppleMesh::SPRING* aSprings = pSuppleMesh->GetSpringBuf();
		float fCoefficient = (float)atof(m_strCoefficient);

		for (i=0; i < iNumSpring; i++)
			aSprings[i].fCoefficient = fCoefficient;
	}
	else
	{
		m_strCoefficient.Format("%.4f", pSuppleMesh->GetSpringBuf()->fCoefficient);
		m_strGravity.Format("%.4f", pSuppleMesh->GetGravityFactor());
		m_strAirResist.Format("%.4f", pSuppleMesh->GetAirResistance());
		m_strVertWeight.Format("%.4f", pSuppleMesh->GetVertWeight());

		UpdateData(FALSE);
	}
}

//	Update physique shape data
void CPageSuppleTest::UpdatePhysiqueShapeData(int iItem, bool bSave)
{
	A3DSkinPhyShape* pPhyShape = (A3DSkinPhyShape*)m_PhyShapeList.GetItemData(iItem);

	if (bSave)
	{
		UpdateData(TRUE);

		A3DCAPSULE cc = pPhyShape->GetCapsule();
		cc.vCenter.x = (float)atof(m_strX);
		cc.vCenter.y = (float)atof(m_strY);
		cc.vCenter.z = (float)atof(m_strZ);
		cc.fHalfLen = (float)atof(m_strLength) * 0.5f;
		cc.fRadius	= (float)atof(m_strRadius);
		pPhyShape->SetCapsule(cc);
	}	
	else
	{
		const A3DCAPSULE& cc = pPhyShape->GetCapsule();
		m_strX.Format("%.4f", cc.vCenter.x);
		m_strY.Format("%.4f", cc.vCenter.y);
		m_strZ.Format("%.4f", cc.vCenter.z);
		m_strLength.Format("%.4f", cc.fHalfLen * 2.0f);
		m_strRadius.Format("%.4f", cc.fRadius);

		UpdateData(FALSE);
	}
}

BOOL CPageSuppleTest::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			OnReturnDown(pMsg->hwnd);
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

//	Handle key RETURN down
void CPageSuppleTest::OnReturnDown(HWND hWnd)
{
	bool bMeshData = false;

	if (hWnd == GetDlgItem(IDC_EDIT_COEFFICIENT)->m_hWnd)
	{
		//	Spring coefficient factor
		AcceptFloatValue(IDC_EDIT_COEFFICIENT, m_strCoefficient, 0.0, 1e7f);
		bMeshData = true;
	}
	else if (hWnd == GetDlgItem(IDC_EDIT_GRAVITY)->m_hWnd)
	{
		//	Gravity factor
		AcceptFloatValue(IDC_EDIT_GRAVITY, m_strGravity, 0.0, 1e7f);
		bMeshData = true;
	}
	else if (hWnd == GetDlgItem(IDC_EDIT_AIRRESIST)->m_hWnd)
	{
		//	Air resistance factor
		AcceptFloatValue(IDC_EDIT_AIRRESIST, m_strAirResist, 0.0, 1e7f);
		bMeshData = true;
	}
	else if (hWnd == GetDlgItem(IDC_EDIT_VERTWEIGHT)->m_hWnd)
	{
		//	Vertex weight
		AcceptFloatValue(IDC_EDIT_VERTWEIGHT, m_strVertWeight, 0.0, 1e7f);
		bMeshData = true;
	}

	if (bMeshData)
	{
		int iSel = m_SuppleMeshList.GetCurSel();
		if (iSel != LB_ERR)
			UpdateSuppleData(iSel, true);
	}

	bool bPhyShapeData = false;

	if (hWnd == GetDlgItem(IDC_EDIT_X)->m_hWnd)
	{
		AcceptFloatValue(IDC_EDIT_X, m_strX, -1e7f, 1e7f);
		bPhyShapeData = true;
	}
	else if (hWnd == GetDlgItem(IDC_EDIT_Y)->m_hWnd)
	{
		AcceptFloatValue(IDC_EDIT_Y, m_strY, -1e7f, 1e7f);
		bPhyShapeData = true;
	}
	else if (hWnd == GetDlgItem(IDC_EDIT_Z)->m_hWnd)
	{
		AcceptFloatValue(IDC_EDIT_Z, m_strZ, -1e7f, 1e7f);
		bPhyShapeData = true;
	}
	else if (hWnd == GetDlgItem(IDC_EDIT_LENGTH)->m_hWnd)
	{
		AcceptFloatValue(IDC_EDIT_LENGTH, m_strLength, 0.0f, 1e7f);
		bPhyShapeData = true;
	}
	else if (hWnd == GetDlgItem(IDC_EDIT_RADIUS)->m_hWnd)
	{
		AcceptFloatValue(IDC_EDIT_RADIUS, m_strRadius, 0.0f, 1e7f);
		bPhyShapeData = true;
	}

	if (bPhyShapeData)
	{
		int iSel = m_PhyShapeList.GetCurSel();
		if (iSel != LB_ERR)
			UpdatePhysiqueShapeData(iSel, true);
	}
}

//	Accept float value
float CPageSuppleTest::AcceptFloatValue(int iControlID, CString& strValue, float fFloor, float fRoof)
{
	CDataExchange dx1(this, TRUE);
	DDX_Text(&dx1, iControlID, strValue);

	float fVal = atof(strValue);

	if (fVal < fFloor)
		fVal = fFloor;
	else if (fVal > fRoof)
		fVal = fRoof;

	strValue.Format("%.4f", fVal);

	CDataExchange dx2(this, FALSE);
	DDX_Text(&dx2, iControlID, strValue);

	return fVal;
}

//	Spin float value
float CPageSuppleTest::SpinFloatValue(NM_UPDOWN* pNMUpDown, int iControlID, CString& strValue, 
									  float fStep, float fFloor, float fRoof)
{
	float fVal = (float)atof(strValue);
	fVal += -pNMUpDown->iDelta * fStep;

	if (fVal < fFloor)
		fVal = fFloor;
	else if (fVal > fRoof)
		fVal = fRoof;

	strValue.Format("%.4f", fVal);

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, iControlID, strValue);

	return fVal;
}

void CPageSuppleTest::OnDeltaposSpinX(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	SpinFloatValue(pNMUpDown, IDC_EDIT_X, m_strX, 0.01f, -1e7f, 1e7f);

	int iSel = m_PhyShapeList.GetCurSel();
	if (iSel != LB_ERR)
		UpdatePhysiqueShapeData(iSel, true);
}

void CPageSuppleTest::OnDeltaposSpinY(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	SpinFloatValue(pNMUpDown, IDC_EDIT_Y, m_strY, 0.01f, -1e7f, 1e7f);

	int iSel = m_PhyShapeList.GetCurSel();
	if (iSel != LB_ERR)
		UpdatePhysiqueShapeData(iSel, true);
}

void CPageSuppleTest::OnDeltaposSpinZ(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	SpinFloatValue(pNMUpDown, IDC_EDIT_Z, m_strZ, 0.01f, -1e7f, 1e7f);

	int iSel = m_PhyShapeList.GetCurSel();
	if (iSel != LB_ERR)
		UpdatePhysiqueShapeData(iSel, true);
}

void CPageSuppleTest::OnDeltaposSpinLength(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	SpinFloatValue(pNMUpDown, IDC_EDIT_LENGTH, m_strLength, 0.01f, 0.0f, 1e7f);

	int iSel = m_PhyShapeList.GetCurSel();
	if (iSel != LB_ERR)
		UpdatePhysiqueShapeData(iSel, true);
}

void CPageSuppleTest::OnDeltaposSpinRadius(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	SpinFloatValue(pNMUpDown, IDC_EDIT_RADIUS, m_strRadius, 0.01f, 0.0f, 1e7f);

	int iSel = m_PhyShapeList.GetCurSel();
	if (iSel != LB_ERR)
		UpdatePhysiqueShapeData(iSel, true);
}

//	Update skin information
void CPageSuppleTest::UpdateSkinInfo()
{
	m_SuppleMeshList.ResetContent();
	UpdateSuppleMeshList();
}

//	Update physique information
void CPageSuppleTest::UpdatePhysiqueInfo()
{
	m_PhyShapeList.ResetContent();
	UpdatePhysiqueShapeList();
}


