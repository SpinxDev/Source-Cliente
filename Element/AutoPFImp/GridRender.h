// GridRender.h: interface for the CGridRender class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class A3DOrthoCamera;
class A2DSprite;

namespace AutoMove
{
  class	CLayerMap;
}


class CGridRender  
{
public:
	enum
	{
		OP_NONE,
		OP_SETSTART,
		OP_SETGOAL,
		OP_ISLAND_ADDBORDER,
		OP_ISLAND_DELBORDER,
		OP_ISLAND_SELECT,
		OP_ISLAND_SETPASSTHROUGH,
	};
	CGridRender();
	virtual ~CGridRender();

	void Render();
	
	void Tick(DWORD dwTickTime);

	bool Init(int mapW, int mapL);
	void Release();

	void Resize(int cx, int cy);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonUp(int x, int y, DWORD dwFlags);
	
	void SetOp(int op)
	{
		m_Op = op;
	}
	int GetOp()
	{
		return m_Op;
	}

	void SetLayerMap(AutoMove::CLayerMap * pLayerMap, int iLayer)
	{
		m_pLayerMap = pLayerMap;
		m_iLayer = iLayer;
	}
	void UpdateMap();
private:
	A3DOrthoCamera  * m_pA3DCamera;
	A2DSprite		* m_pSprite;
	BYTE            * m_pData;
	AutoMove::CLayerMap		* m_pLayerMap;
	int						  m_iLayer;
	bool              m_bRDrag;
	A3DPOINT2         m_ptLastMou;
	int               m_iHeight;
	int               m_iWidth;
	int               m_Op;
	
	bool              m_bEndAddWayEdge;
	A3DPOINT2         m_ptWayEdgeStart;  //scr
	A3DPOINT2         m_ptWayEdgeEnd;    



private:	
	void	_UpdateFromMap();
	bool    _FrameMove(DWORD dwDeltaTime);
	void	_MoveCameraRight(bool bRight, float fDeltaTime);
	void	_MoveCameraUp(bool bUp, float fDeltaTime);
	

};


