#include <Windows.h>
#include <StdIO.h>
#include <StdLib.h>

#ifndef _DLGASK4_H_
#define _DLGASK4_H_

enum 
{
	ASK4_YES = 0,
	ASK4_YESALL,
	ASK4_NO,
	ASK4_NOALL
};

class DlgAsk4
{
private:
	HINSTANCE		m_hInstance;
	HWND			m_hParentWnd;

	char			* m_pszTitle;
	char			* m_pszInfo;

public:
	bool Init(HINSTANCE hInstance, HWND hWnd);
	bool SetTitleInfo(char * szTitle, char * szInfo);
	int DoModal();

	inline char * GetTitle() { return m_pszTitle; }
	inline char * GetInfo() { return m_pszInfo; }
};

#endif