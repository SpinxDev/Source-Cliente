// ICLTaskProcFolder.h: interface for the ICLTaskProcFolder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICLTASKPROCFOLDER_H__107E2B5A_0A0A_49AD_B954_4347521D1FF2__INCLUDED_)
#define AFX_ICLTASKPROCFOLDER_H__107E2B5A_0A0A_49AD_B954_4347521D1FF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ICLThreadTask.h"
#include "AC.h"
#include "AF.h"
#include "AM.h"
#include "A3D.h"
#include <A3DAdditionalView.h>


class A3DEngine;
class A3DGfxEngine;
class A3DCamera;
class A3DViewport;
class A3DAdditionalView;
class A3DRenderTarget;

class A3DDevice;
class ICLNotifier;
class ICLFile;


class ICLTaskProcFolder : public ICLThreadTask  
{
public:
	struct FILEINFO {
		CTime fTime;
		DWORD dwSize;
	};	

public:

//zjy#ifdef _ANGELICA2
//zjy	ICLTaskProcFolder(A3DDevice* pDevice, const TCHAR * szFolder, ICLNotifier* pMsgReceiver);
//zjy#elif _ANGELICA31
	ICLTaskProcFolder(const TCHAR * szFolder, ICLNotifier* pMsgReceiver, A3DDevice* pDevice, A3DGfxEngine* pDGfxEngine, A3DCamera* pDCamera, A3DViewport* pDViewport, A3DRenderTarget* pRenderTarget/*, A3DAdditionalView* pDAdditionalView*/);
//zjy#endif

	virtual ~ICLTaskProcFolder();
protected:
	// Override functions
	virtual bool Do();

private:
	CString m_strTaskFolder;
	A3DDevice* m_pDevice;
	ICLNotifier* m_pMsgReceiver;

	A3DGfxEngine* m_pDGfxEngine;
	A3DCamera* m_pDCamera;
	A3DViewport* m_pDViewport;
	A3DAdditionalView* m_pDAdditionalView;
	A3DRenderTarget* m_pRenderTarget;

	HWND m_hWnd;

private:
	void procOneFile(ICLFile* pICLFile, const TCHAR * szFilename, const FILEINFO& fi);
};

#endif // !defined(AFX_ICLTASKPROCFOLDER_H__107E2B5A_0A0A_49AD_B954_4347521D1FF2__INCLUDED_)
