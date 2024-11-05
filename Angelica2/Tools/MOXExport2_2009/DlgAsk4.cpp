#include "resource.h"
#include "DlgAsk4.h"

DlgAsk4 * g_pDlgAsk4;

static BOOL CALLBACK Ask4DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
			SetWindowText(GetDlgItem(hWnd, IDC_STATICINFO), g_pDlgAsk4->GetInfo());
			SetWindowText(hWnd, g_pDlgAsk4->GetTitle());

			RECT rectWnd, rectClient;

			GetWindowRect(hWnd, &rectWnd);
			GetClientRect(hWnd, &rectClient);

			SetWindowPos(hWnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - rectWnd.right + rectWnd.left) / 2,
				(GetSystemMetrics(SM_CYSCREEN) - rectWnd.bottom + rectWnd.top) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			break;

		case WM_DESTROY:
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_BUTTONYES:
				EndDialog(hWnd, ASK4_YES);
				break;
			case IDC_BUTTONYESALL:
				EndDialog(hWnd, ASK4_YESALL);
				break;
			case IDC_BUTTONNO:
				EndDialog(hWnd, ASK4_NO);
				break;
			case IDC_BUTTONNOALL:
				EndDialog(hWnd, ASK4_NOALL);
				break;
			}
			break;
		
		default:
			return FALSE;
	}
	return TRUE;
}

bool DlgAsk4::Init(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hParentWnd = hWnd;
	
	return true;
}

bool DlgAsk4::SetTitleInfo(char * szTitle, char * szInfo)
{
	m_pszTitle = szTitle;
	m_pszInfo  = szInfo;

	return true;
}

int DlgAsk4::DoModal()
{
	g_pDlgAsk4 = this;

	return DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_DIALOGASK4), m_hParentWnd, Ask4DlgProc);
}
