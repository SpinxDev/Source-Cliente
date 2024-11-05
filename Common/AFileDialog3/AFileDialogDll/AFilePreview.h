/*
* FILE: AFilePreview.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/05/07
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AFilePreview_H_
#define _AFilePreview_H_


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class ARenderItem;
class A3DDevice;
class A3DEngine;
class A3DGfxEngine;
class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AFilePreview
//	
///////////////////////////////////////////////////////////////////////////

class AFilePreview
{

public:		//	Types

public:		//	Constructor and Destructor

	AFilePreview(void);
	virtual ~AFilePreview(void);

public:		//	Attributes

public:		//	Operations

	bool Init(A3DDevice* pA3DDevice, A3DGfxEngine* pA3DGfxEngine);
	void Release();

	bool Play(const TCHAR* szFile);
	void Stop();

	bool IsPlaying() const { return m_bIsPlaying; }
	A3DAABB GetItemAABB() const;
	A3DVECTOR3 GetItemPos() const;

	void Tick(DWORD dwTickDelta);
	void Render(A3DViewport* pViewport);

protected:	//	Attributes

	A3DEngine* m_pA3DEngine;
	A3DDevice* m_pA3DDevice;
	A3DGfxEngine* m_pA3DGFXEngine;
	ARenderItem* m_pRenderItem;
	AString m_strCurPreviewFile;
	bool m_bIsPlaying;
	mutable CCriticalSection m_cs;


protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AFilePreview_H_


