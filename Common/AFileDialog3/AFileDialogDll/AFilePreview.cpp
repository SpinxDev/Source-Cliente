/*
* FILE: AFilePreview.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/05/07
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "SyncWrapper.h"
#include "AFilePreview.h"
#include "GfxRenderItem.h"
#include "A3DGFXEx.h"
#include "a3dgfxexman.h"
#include "ECMRenderItem.h"

#ifdef _ANGELICA31
#include "SMDRenderItem.h"
#include "UMDRenderItem.h"
#include "UMDSRenderItem.h"

#elif defined _ANGELICA2
#include "ARenderItem.h"

#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static ARenderItem* _CreateRenderItem(const AString& strFilePath, A3DDevice* pA3DDevice, A3DGfxEngine* pA3DGFXEngine)
{
	ARenderItem* pRenderItem = NULL;

#ifdef _ANGELICA31
	if (af_CheckFileExt(strFilePath, ".gfx3"))
	{
		// set gfx preview item
		pRenderItem = new GfxRenderItem(strFilePath, pA3DGFXEngine);
	}
	else if (af_CheckFileExt(strFilePath, ".ecm3"))
	{
		// set ecm preview item
		pRenderItem = new ECMRenderItem(strFilePath, pA3DGFXEngine);
	}
	else if (af_CheckFileExt(strFilePath, ".SMD"))
	{
		// set smd preview item
		pRenderItem = new SMDRenderItem(strFilePath, pA3DDevice);
	}
	else if (af_CheckFileExt(strFilePath, ".umd"))
	{
		// set umd preview item
		pRenderItem = new UMDRenderItem(strFilePath, pA3DDevice);
	}
	else if (af_CheckFileExt(strFilePath, ".umds"))
	{
		// set umds preview item
		pRenderItem = new UMDSRenderItem(strFilePath, pA3DDevice);
	}
#elif defined _ANGELICA2
	if (af_CheckFileExt(strFilePath, ".gfx"))
	{
		// set gfx preview item
		pRenderItem = new GfxRenderItem(strFilePath, pA3DGFXEngine);
	}

	else if (af_CheckFileExt(strFilePath, ".ecm"))
	{
		// set ecm preview item
		pRenderItem = new ECMRenderItem(strFilePath, pA3DGFXEngine);
	}

#endif

	return pRenderItem;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AFilePreview
//	
///////////////////////////////////////////////////////////////////////////

AFilePreview::AFilePreview(void)
: m_bIsPlaying(false)
, m_pRenderItem(NULL)
, m_pA3DEngine(NULL)
, m_pA3DDevice(NULL)
, m_pA3DGFXEngine(NULL)
{
}

AFilePreview::~AFilePreview(void)
{
}

bool AFilePreview::Init(A3DDevice* pA3DDevice, A3DGfxEngine* pA3DGfxEngine)
{
#ifdef _ANGELICA31
	if (!pA3DDevice || !pA3DGfxEngine)
		return false;
#elif defined _ANGELICA2
	if (!pA3DDevice)
		return false;
#endif

	m_pA3DDevice = pA3DDevice;
	m_pA3DEngine = pA3DDevice->GetA3DEngine();
	m_pA3DGFXEngine = pA3DGfxEngine;
	return true;
}

void AFilePreview::Release()
{
	
	A3DRELEASE(m_pRenderItem);
	m_bIsPlaying = false;
}

bool AFilePreview::Play(const TCHAR* szFile)
{

	AString strFilePath;

	if (szFile)
		strFilePath = AC2AS(szFile);

	SyncWrapper<CCriticalSection> _AutoLock(m_cs);

	// 如果传入的是空文件名，则停止
	if (strFilePath.IsEmpty())
	{
		Stop();
		return true;
	}

	// 如果当前已经在播放该文件 则立即返回
	if (strFilePath.Compare(m_strCurPreviewFile) == 0)
	{
		ASSERT( m_bIsPlaying );
		return true;
	}

	// 如果当前未在播放该文件 则尝试开始播放该文件
	// 首先释放原先正在播放的
	A3DRELEASE(m_pRenderItem);
	m_pRenderItem = _CreateRenderItem(strFilePath, m_pA3DDevice, m_pA3DGFXEngine);

	// 如果创建RenderItem成功，并且加载文件也成功了
	if (m_pRenderItem && m_pRenderItem->Init())
	{
		m_strCurPreviewFile = strFilePath;
		m_bIsPlaying = true;
		m_pRenderItem->Play();
	}
	else
	{
		m_strCurPreviewFile.Empty();
		m_bIsPlaying = false;
	}


	return true;
}

void AFilePreview::Stop()
{
	SyncWrapper<CCriticalSection> _AutoLock(m_cs);

	m_strCurPreviewFile.Empty();
	A3DRELEASE(m_pRenderItem);
	m_bIsPlaying = false;
}

void AFilePreview::Tick(DWORD dwTickDelta)
{
	SyncWrapper<CCriticalSection> _AutoLock(m_cs);
	if (!m_pRenderItem)
		return;

	m_pRenderItem->Tick(dwTickDelta);
}

void AFilePreview::Render(A3DViewport* pViewport)
{
	SyncWrapper<CCriticalSection> _AutoLock(m_cs);
	if (!m_pRenderItem)
		return;


	m_pRenderItem->Render(m_pA3DEngine, pViewport);

	m_pA3DEngine->GetA3DLitModelRender()->Render(pViewport);

	m_pA3DEngine->GetA3DDevice()->SetAlphaBlendEnable(false);
#ifdef _ANGELICA31
	m_pA3DEngine->GetA3DSkinMan()->GetCurSkinRender()->Render(pViewport, A3DSkinRender::RD_ALL);
#elif defined _ANGELICA2
	m_pA3DEngine->GetA3DSkinMan()->GetCurSkinRender()->Render(pViewport, false);
#endif
	m_pA3DEngine->GetA3DDevice()->SetAlphaBlendEnable(true);
#ifdef _ANGELICA31
	m_pA3DGFXEngine->GetA3DGfxExMan()->RenderAllGfx(pViewport);
#elif defined _ANGELICA2
	AfxGetGFXExMan()->RenderAllGfx(pViewport);
#endif
}


A3DAABB AFilePreview::GetItemAABB() const
{
	SyncWrapper<CCriticalSection> _AutoLock(m_cs);
	A3DAABB aabb(0.f, 0.f);

	if (m_pRenderItem)
		m_pRenderItem->GetAABB(&aabb);
	
	return aabb;
}

A3DVECTOR3 AFilePreview::GetItemPos() const
{
	SyncWrapper<CCriticalSection> _AutoLock(m_cs);

	if (m_pRenderItem)
		return m_pRenderItem->GetPos();

	return A3DVECTOR3(0.f);
}