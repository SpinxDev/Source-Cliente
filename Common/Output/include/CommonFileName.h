#include <A3D.h>
#include <af.h>
#include <am.h>


class CCommonFileDlg;

#ifdef _WINDLL
#define DLLFLAG __declspec(dllexport)
#else
#define DLLFLAG __declspec(dllimport)
#endif

#define AFILE_ALLOWMULFILES		1
#define AFILE_NOALLOWMULFILES	0


class DLLFLAG CCommonFileName
{
public:
	CCommonFileName(A3DEngine* pA3DEngine, DWORD dwmfileFlag, const char* sRelPath = "", const char* szExt = "", CWnd* pParent = NULL);
	~CCommonFileName();
	int DoModal();

	// single selection
	const char* GetFullFileName();
	const char* GetRelativeFileName();

	// multiple selection
	const char* GetFirstSelectedFullName();
	const char* GetFirstSelectedRelName();

	int GetSelectedNum();
	const char* GetNextSelectedFullName(int nIndex);
	const char* GetNextSelectedRelName(int nIndex);

	void SetFileExtension(const char* szExt);
	
private:
	CCommonFileDlg* m_pOfn;
	char m_aStr[MAX_PATH];
};
