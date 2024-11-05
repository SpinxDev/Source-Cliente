/*
 * FILE: A3DRenderTarget.h
 *
 * DESCRIPTION: class representing a render target;
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DRENDERTARGET_H_
#define _A3DRENDERTARGET_H_

#include "A3DTypes.h"
#include "A3DPlatform.h"
#include "AList.h"
#include "A3DDevObject.h"

class A3DStream;
class A3DViewport;
class A3DDevice;

class A3DRenderTarget : public A3DDevObject
{
private:
	A3DDevice				* m_pA3DDevice;

	IDirect3DTexture8		* m_pTargetTexture;
	IDirect3DSurface8		* m_pTargetSurface;
	IDirect3DTexture8		* m_pDepthTexture;
	IDirect3DSurface8		* m_pDepthSurface;

	IDirect3DSurface8		* m_pColorSurface;

	A3DStream				* m_pA3DStream;

	A3DDEVFMT				m_DevFormat;
	bool					m_bNewTarget;
	bool					m_bNewDepth;

	bool					m_bUsingTexture;
	bool					m_bHasFilled;

	AList					m_ListViewport;

protected:
	bool CreatePlains();
	bool ReleasePlains();

public:
	A3DRenderTarget();
	~A3DRenderTarget();

	bool AddViewport(A3DViewport * pViewport, ALISTELEMENT ** ppElement);
	bool RenderAllViewport();

	bool PresentToBack(int x, int y, FLOAT z=0.0f);

	bool Init(A3DDevice * pA3DDevice, A3DDEVFMT &devFmt, bool bNewTarget, bool bNewDepth);
	bool Release();

	bool ExportColorToFile(char * szFullpath);
	bool ExportDepthToFile(char * szFullpath);

	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();

	bool AppearAsTexture(int nLayer);
	bool DisappearAsTexture(int nLayer);

	inline IDirect3DTexture8 * GetTargetTexture() { return m_pTargetTexture; }
	inline IDirect3DTexture8 * GetDepthTexture() { return m_pDepthTexture; }
	inline IDirect3DSurface8 * GetTargetSurface() { return m_pColorSurface; }
	inline IDirect3DSurface8 * GetDepthSurface() { return m_pDepthSurface; }
	inline int GetViewNum() { return m_ListViewport.GetSize(); }
	inline bool HasFilled() { return m_bHasFilled; }
	inline void SetHasFilled() { m_bHasFilled = true; }
};

typedef A3DRenderTarget * PA3DRenderTarget;

#endif