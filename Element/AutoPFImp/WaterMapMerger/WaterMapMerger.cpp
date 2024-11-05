/***************************************************
	file   : WaterMapMerger.cpp
	created: 2008/11/3
	author : liudong
	purpose: 将多张水面地图融合成一张。
	         地图是按照下述方式组织的
             1  2
             3  4 
			 每一个小地图原来的(0,0)点是各自的中心点,
			 合并后大地图的(0,0)点在大地图中心.

 ****************************************************/

#include <stdio.h>
#include <AString.h>
#include <vector.h>
using namespace abase;
#include <A3DMacros.h>

#include "waterareamap.h"
  
int g_nMapWidth = 0;//水平方向地图数目
int g_nMapHeight = 0;//垂直方向地图数目
float g_fSubMapWidth = 0;//子地图水平宽度
float g_fSubMapHeight = 0;//子地图垂直高度
abase::vector<CWaterAreaMap*> g_vecSubWaterMaps;
AString                       g_szWorkDir;
AString                       g_szOutWMap;

bool RetrieveConfigInfo(const char * szFilePath)
{
	FILE * fp = fopen(szFilePath, "r");
	if (!fp)
	{
		char msg[1000];
		sprintf(msg,"Error: can not read config file:  \"%s\"\n",szFilePath);
		printf(msg);
		return false;
	}

	char line[1000];
	char * token;
	char seps[]   = " =\t\n";


	bool ret = true;

	while (fgets(line, 1000, fp))
	{
		token = strtok(line, seps);
		
		if (!token || token[0] == '#')
		{
			continue;
		}

		if (strcmp(token, "work_dir") == 0)
		{
			token = strtok(NULL, seps);
			g_szWorkDir= token;
			int len = strlen(token); 
			if (token[len-1] != '\\' && token[len-1] != '/')
			{
				g_szWorkDir += "\\";
			}
		}
		else if (strcmp(token, "out_wmap") == 0)
		{
			token = strtok(NULL, seps);
			 g_szOutWMap = token;
		}
		else if (strcmp(token, "width") == 0)
		{
			token = strtok(NULL, seps);
			 g_nMapWidth = atoi(token) ;
		}
		else if (strcmp(token, "height") == 0)
		{
			token = strtok(NULL, seps);
			g_nMapHeight = atoi(token);
		}else if (strcmp(token, "sub_width") == 0)
		{
			token = strtok(NULL, seps);
			 g_fSubMapWidth = atof(token) ;
		}
		else if (strcmp(token, "sub_height") == 0)
		{
			token = strtok(NULL, seps);
			g_fSubMapHeight = atof(token);
		}
	}

	fclose(fp);
	return ret;
}

void Release()
{
	for (int i=0; i<(int)g_vecSubWaterMaps.size(); ++i)
	{
		CWaterAreaMap* pMap = g_vecSubWaterMaps[i];
		if (pMap)
		{
			delete pMap;
		}
	}
	g_vecSubWaterMaps.clear();
}

//get sub water maps.
bool LoadSubWaterMaps()
{
	char pathName[1000];
	const int NUM = g_nMapWidth*g_nMapHeight;
	for (int i=0; i<NUM; ++i)
	{
		sprintf(pathName,"%d.wmap",i+1);
		CWaterAreaMap* pWaterMap = new CWaterAreaMap();
		bool bResult = pWaterMap->Load(pathName);
		if (!bResult)
		{
			char msg[1000];
			sprintf(msg,"Error: can not load water map:  \"%s\"\n",pathName);
			printf(msg);
			return false;
		}
		if (pWaterMap->GetMapWidth()!=g_fSubMapWidth || pWaterMap->GetMapLength() !=g_fSubMapHeight)
		{
			char msg[1000];
			sprintf(msg,"Error: size of water map \"%s\"<%f,%f> is not <%f,%f> \n",pathName,pWaterMap->GetMapWidth(),pWaterMap->GetMapLength(),g_fSubMapWidth,g_fSubMapHeight);
			printf(msg);
			return false;
		}
		g_vecSubWaterMaps.push_back(pWaterMap);
	}
	return true;
}

//merge water areas.
bool MergeWaterMaps()
{
	CWaterAreaMap finalWaterArea;
	finalWaterArea.SetSize(g_fSubMapWidth*g_nMapWidth, g_fSubMapHeight*g_nMapHeight);
	const int NUM = g_nMapWidth*g_nMapHeight;
	const float X_CENTER = g_fSubMapWidth*g_nMapWidth*0.5f;
	const float Z_CENTER = g_fSubMapHeight*g_nMapHeight*0.5f;
	const float X_CENTER_ORI = g_fSubMapWidth*0.5f;
	const float Z_CENTER_ORI = g_fSubMapHeight*0.5f;
		
	for (int i=0; i<NUM; ++i)
	{
		float xCenter = (i%g_nMapWidth)*g_fSubMapWidth+X_CENTER_ORI;
		float zCenter = (g_nMapHeight-1-i/g_nMapWidth)*g_fSubMapHeight+Z_CENTER_ORI;\
		float xOffset = xCenter-X_CENTER;
		float zOffset = zCenter-Z_CENTER;
		CWaterAreaMap* pWaterMap = g_vecSubWaterMaps[i];
		if (!pWaterMap)
		{
			char msg[1000];
			sprintf(msg,"Error: fail to get water map:  %d\n",i+1);
			printf(msg);
			return false;
		}
		abase::vector<CWaterArea*>* pSubWaterAreas = pWaterMap->GetWaterAreas();
		for (int j=0; j<(int)pSubWaterAreas->size(); ++j)
		{
			CWaterArea* pWater =  (*pSubWaterAreas)[j];
			if (!pWater)
			{
				printf("Error: fail to get sub water info\n");
				return false;
			}
			finalWaterArea.AddWaterArea(pWater->CenterX+xOffset, pWater->CenterZ+zOffset, pWater->HalfWidth, pWater->HalfLength, pWater->Height); 
		}
	}

	//save final water area.
	AString pathName = g_szWorkDir + g_szOutWMap;
	bool bResult = finalWaterArea.Save(pathName);
	if (bResult)
	{
		char msg[1000];
		sprintf(msg,"save final file to \"%s\" \n",pathName);
		printf(msg);
	}else
	{
		char msg[1000];
		sprintf(msg,"fail to save final file : \"%s\" \n",pathName);
		printf(msg);
	}
	return bResult;
}

int main(int argc, char* argv[])
{
	const char* pFileName = NULL;
	char str[1000];
	if (argc < 2)
	{
		printf("please input config file: \n");
		scanf("%s",str);
		pFileName = str;
	}else
	{
		pFileName = argv[1];
	}
	
	printf("start to merge water map .\n");

	//get config info.
	bool bResult = RetrieveConfigInfo(pFileName);
	if (!bResult)
	{
		Release();
		return 1;
	}

	if (g_nMapWidth<=0 ||
		g_nMapHeight<=0 ||
		g_fSubMapWidth<=0 ||
		g_fSubMapHeight<=0)
	{
		printf("Error: input incorrect map size.\n");
		Release();
		return 1;
	}
	if (g_szOutWMap.GetLength()==0)
	{
		g_szOutWMap = "a.wmap";
	}

	if(!LoadSubWaterMaps())
	{
		Release();
		return 1;
	}

	if (!MergeWaterMaps())
	{
		Release();
		return 1;
	}

	Release();
    return 0;
}