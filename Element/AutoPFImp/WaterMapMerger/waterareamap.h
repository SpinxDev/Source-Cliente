 /*
 * FILE: WaterAreaMap.h
 *
 * DESCRIPTION:   set of classes to describe all the water areas in one block of the Map!
 *
 * CREATED BY: He wenfeng, 2005/5/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _WATERAREAMAP_H_
#define _WATERAREAMAP_H_

#include <stdio.h>

#include <vector.h>
#include "a3dvector.h"

#define NO_WATER 0.0f		// height of no-water area

#define WATER_AREA_MAP_VER 0xcc000001		// file version

// public member for direct access
struct CWaterArea
{

// member

	// center pos
	float CenterX;		
	float CenterZ;
	
	// area size
	float HalfWidth;		// corresponding to X
	float HalfLength;		// corresponding to Z

	// water height
	float Height;

// interface
	bool Inside(float x, float z)
	{
		return ( x <= CenterX + HalfWidth && x >= CenterX - HalfWidth
			   && z <= CenterZ + HalfLength && z >= CenterZ - HalfLength);
	}
};

class CWaterAreaMap
{
public:
	CWaterAreaMap();
	~CWaterAreaMap();

	void Release()
	{
		for(size_t i=0; i<m_WaterAreas.size(); i++)
			delete m_WaterAreas[i];

		m_WaterAreas.clear();
	}

	float GetWaterHeight(const A3DVECTOR3& vPos)
	{
		return GetWaterHeight(vPos.x, vPos.z);
	}

	float GetWaterHeight(float x, float z)
	{
		for(size_t i=0; i<m_WaterAreas.size(); i++)
		{
			if(m_WaterAreas[i]->Inside(x, z))
			{
				return m_WaterAreas[i]->Height;
			}
		}
		return NO_WATER;
	}

	bool IsUnderWater(const A3DVECTOR3& vPos)
	{
		return vPos.y < GetWaterHeight(vPos);
	}
	
	void AddWaterArea(float fcx, float fcz, float fHalfWidth, float fHalfLength, float fHeight)
	{
		CWaterArea *pWaterArea = new CWaterArea;
		pWaterArea->CenterX = fcx;
		pWaterArea->CenterZ = fcz;
		pWaterArea->HalfWidth = fHalfWidth;
		pWaterArea->HalfLength = fHalfLength;
		pWaterArea->Height = fHeight;
		m_WaterAreas.push_back(pWaterArea);
	}

	void SetSize(float fWidth, float fLength)
	{
		m_fWidth = fWidth;
		m_fLength = fLength;
	}
	
	float GetMapWidth() { return m_fWidth; }
	float GetMapLength() { return m_fLength; }
		
	// Load and save interface
	bool Load(const char* szFile);
	bool Load(FILE* FileToLoad);

	bool Save(const char* szFile);
	bool Save(FILE* FileToSave);

#ifdef _WATER_MAP_MERGE_EDITOR
    abase::vector<CWaterArea*>* GetWaterAreas()
	{
		return &m_WaterAreas;
	}
#endif
	
private:

	abase::vector<CWaterArea *> m_WaterAreas;

	// Size of one block water area map
	float m_fWidth;
	float m_fLength;
};

#endif

