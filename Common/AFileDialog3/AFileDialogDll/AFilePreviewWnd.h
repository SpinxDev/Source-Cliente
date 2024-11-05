#pragma once

#define PREVIEW_SIZE 256

class A3DAABB;
class A3DDevice;
class A3DCamera;
class A3DViewport;


class A3DRenderTarget;	

class AFilePreview;
class ARenderItem;
class A3DGfxEngine;

#ifdef _ANGELICA2

struct IDirect3DSurface8;

#elif defined _ANGELICA31

class A3DAdditionalView;
struct IDirect3DSurface9;

#endif
// AFilePreviewWnd

class AFilePreviewWnd : public CWnd
{
	DECLARE_DYNAMIC(AFilePreviewWnd)

public:

	class Listener
	{
	public:

#ifdef _ANGELICA2
		
		virtual void AfterRender(IDirect3DSurface8* pBackBuffer) = 0;
		
#elif defined _ANGELICA31
		
		virtual void AfterRender(IDirect3DSurface9* pBackBuffer) = 0;
		
#endif
		
	};

public:

	static DWORD s_dwPreviewWndSize;

private:

public:
	
	AFilePreviewWnd();
	virtual ~AFilePreviewWnd();

public:

	bool Init(A3DDevice* pA3DDevice, A3DGfxEngine* pA3DGfxEngine);
	void Release();

	//void SetRenderItem(RenderItem* pItem) { m_pRenderItem = pItem; }
	bool SetPreviewFile(const TCHAR* szFile, bool bIsAutoUpdateWindowShow = true);
	
	void AddListener(Listener* pListener);
	void RemoveListener(Listener* pListener);

	bool IsWShow() const { return m_bIsWShow; }
	void SetWShow(bool bIsWShow) { m_bIsWShow = bIsWShow; }
	void Play();
	void Stop();

	void Tick(DWORD dwTickDelta);
	void Render();

	A3DDevice* GetA3DDevice() {return m_pA3DDevice;}
	A3DCamera* GetA3DCamera() {return m_pA3DCamera;}
	A3DViewport* GetA3DViewPort() {return m_pA3DViewport;}
	//A3DAdditionalView* GetA3DAdditionalView() {/*return m_pAdditionalView;*/}
	A3DRenderTarget* GetA3DRenderTarget() {return m_pA3DRenderTarget;}

	A3DGfxEngine* GetA3DGfxEngine() {return m_pA3DGfxEngine;}


private:
	
	A3DDevice* m_pA3DDevice;
	A3DCamera* m_pA3DCamera;
	A3DViewport* m_pA3DViewport;
	//A3DAdditionalView* m_pAdditionalView;

	A3DRenderTarget* m_pA3DRenderTarget;

	A3DGfxEngine* m_pA3DGfxEngine;
	//ARenderItem* m_pRenderItem;
	AFilePreview* m_pFilePreviewObject;
	AString m_strLastPreviewFile;
	DWORD m_dwTickCount;
	CCriticalSection m_cs;
	APtrList<Listener*> m_pListeners;
	bool m_bIsWShow;

protected:

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
public:
	afx_msg void OnNcPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);

	afx_msg void OnMove(int x, int y);
};


