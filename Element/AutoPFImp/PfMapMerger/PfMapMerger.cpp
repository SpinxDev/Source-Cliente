#include <stdio.h>
#include <AString.h>
#include <vector.h>
using namespace abase;
#include <A3DMacros.h>



#include "../include/bitimage.h"
//#include "../include/HeightImage.h"
#include "../include/blockimage.h"
using namespace AutoMove;
//data
AString          g_szWorkDir;
AString          g_szOutRMap;
AString          g_szOutDHMap;
bool			 g_Verbose = false;
int				 g_iWidth = 0;   //width
int              g_iLength = 0;

AString          g_szFileExt = "prmap";

vector<CBitImage *>  g_SubRMaps;
//vector<CHeightImage<float>* >  g_SubPHMaps;
vector<CBlockImage<FIX16>* > g_SubDHMaps;

//func
void PrintHelp();
bool RetrieveConfigInfo(const char * szFilePath);
void Release();
CBitImage * LoadRMap(const char * szFile);
bool LoadRMap();
//CHeightImage<float> * LoadDHMap(const char * szFile);
CBlockImage<FIX16> * LoadDHMap(const char * szFile);
bool LoadDHMap();
void Merge();

//////////////////////////////////////////////////////////////////////////

void PrintHelp()
{
	printf("merge rmap and dhmap.\n");
	printf("author: kuiwu.\n");
	printf("usage:  PfMapMerger config_file\n");
}

void Release()
{
	int i;
	for (i = 0; i < (int)g_SubRMaps.size(); ++i)
	{
		A3DRELEASE(g_SubRMaps[i]);
	}

	for (i = 0; i < (int)g_SubDHMaps.size(); ++i)
	{
		A3DRELEASE(g_SubDHMaps[i]);
	}
}

CBitImage * LoadRMap(const char * szFile)
{
	CBitImage * pRMap = new CBitImage;
	if (!pRMap->Load(szFile))
	{
		A3DRELEASE(pRMap);
		return NULL;
	}
	if (g_Verbose)
	{
		printf("load %s\n", szFile);
	}
	return pRMap;
}

bool LoadRMap()
{
	int i;
	int count = g_iWidth * g_iLength;
	for (i = 0; i < count; ++i)
	{
		AString path;
		path.Format("%s%d.%s", g_szWorkDir, i+1, g_szFileExt);
		CBitImage * pRMap = LoadRMap(path);
		if (!pRMap)
		{
			printf("load %s fail\n", path);
			return false;
		}
		else
		{
			g_SubRMaps.push_back(pRMap);
		}
	}

	return true;
}

//CHeightImage<float> * LoadDHMap(const char * szFile)
CBlockImage<FIX16> * LoadDHMap(const char * szFile)
{

	//CHeightImage<float> * pDHMap = new CHeightImage<float>;
	CBlockImage<FIX16> * pDHMap = new CBlockImage<FIX16>;
	if (!pDHMap->Load(szFile))
	{
		A3DRELEASE(pDHMap);
		return NULL;
	}
	if (g_Verbose)
	{
		printf("load %s\n", szFile);
	}

	return pDHMap;
}

bool   LoadDHMap()
{
	int i;
	int count = g_iWidth * g_iLength;
	AString dhext = "pdhmap";
	if (g_szFileExt.CompareNoCase("rmap") == 0)
	{
		dhext = "dhmap";
	}

	for (i = 0; i < count; ++i)
	{
		AString path;
		path.Format("%s%d.%s", g_szWorkDir, i+1, dhext);
		//CHeightImage<float> * pDHMap = LoadDHMap(path);
		CBlockImage<FIX16> * pDHMap = LoadDHMap(path);
		if (!pDHMap)
		{
			printf("load %s fail\n", path);
			return false;
		}
		else
		{
			g_SubDHMaps.push_back(pDHMap);
		}
	}

	return true;
}

void Merge()
{
	int count = g_iWidth * g_iLength;
	int sub_img_width, sub_img_length;
	int sub_img_i, sub_img_j;
	int sub_img_w, sub_img_h;

	float sub_pixel_size;
	int final_width, final_length;
	CBitImage * final_rmap = NULL;
	//CHeightImage<float> * final_dhmap = NULL;
	CBlockImage<FIX16> * final_dhmap = NULL;

	int i, j;

	g_SubRMaps[0]->GetImageSize(sub_img_width, sub_img_length);
	sub_pixel_size = g_SubRMaps[0]->GetPixelSize();
	final_width = sub_img_width * g_iWidth;
	final_length = sub_img_length * g_iLength;

	final_rmap = new CBitImage;
	final_rmap->InitZero(final_width, final_length, sub_pixel_size);
	//final_dhmap  = new CHeightImage<float>;
	//final_dhmap->Init(final_width, final_length);
	final_dhmap  = new CBlockImage<FIX16>;
	final_dhmap->InitDafault(final_width, final_length, sub_pixel_size);

	

	for (j = 0; j < final_length; ++j)
	{
		for (i = 0; i < final_width; ++i)
		{
			sub_img_w = i / sub_img_width;
			sub_img_i = i % sub_img_width;
			sub_img_h = g_iLength -1 -j / sub_img_length;
			sub_img_j = j % sub_img_length;

			CBitImage * sub_rmap = g_SubRMaps[sub_img_h * g_iWidth + sub_img_w];
			bool bSet = sub_rmap->GetPixel(sub_img_i, sub_img_j);
			final_rmap->SetPixel(i, j, bSet);
			//CHeightImage<float> * sub_dhmap = g_SubPHMaps[sub_img_h * g_iWidth + sub_img_w];
			CBlockImage<FIX16> * sub_dhmap = g_SubDHMaps[sub_img_h * g_iWidth + sub_img_w];
			FIX16  val = sub_dhmap->GetPixel(sub_img_i, sub_img_j);
			final_dhmap->SetPixel(i, j, val);
			//float  val = sub_dhmap->GetHeight(sub_img_i, sub_img_j);
			//final_dhmap->SetHeight(i, j, val);
		}
	}

	AString path;
	path.Format("%s%s",g_szWorkDir, g_szOutRMap);
	if (g_Verbose)
	{
		printf("out rmap to %s\n", path);
	}
	final_rmap->Save(path);
	path.Format("%s%s", g_szWorkDir, g_szOutDHMap);
	if (g_Verbose)
	{
		printf("out dhmap to %s\n", path);
	}

	final_dhmap->Save(path);



	A3DRELEASE(final_rmap);
	A3DRELEASE(final_dhmap);

}


bool RetrieveConfigInfo(const char * szFilePath)
{
	FILE * fp = fopen(szFilePath, "r");
	if (!fp)
	{
		printf("open %s fail\n", szFilePath);
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
		else if (strcmp(token, "rext") == 0)
		{
			token = strtok(NULL, seps);
			 g_szFileExt = token;
		}
		else if (strcmp(token, "out_rmap") == 0)
		{
			token = strtok(NULL, seps);
			 g_szOutRMap = token;
		}
		else if (strcmp(token, "out_dhmap") == 0)
		{
			token = strtok(NULL, seps);
			 g_szOutDHMap = token;
		}

		else if (strcmp(token, "verbose") == 0)
		{
			token = strtok(NULL, seps);
			g_Verbose = (atoi(token) == 1);
		}
		else if (strcmp(token, "width") == 0)
		{
			token = strtok(NULL, seps);
			 g_iWidth = atoi(token) ;
		}
		else if (strcmp(token, "length") == 0)
		{
			token = strtok(NULL, seps);
			g_iLength = atoi(token);
		}
	}

	fclose(fp);
	return ret;
}



int main(int argc, char* argv[])
{
	PrintHelp();

	if (argc < 2)
	{
		return 0;
	}

	if (!RetrieveConfigInfo(argv[1]))
	{
		return 0;
	}

	if (g_Verbose)
	{
		printf("width %d length %d\n", g_iWidth, g_iLength);
	}

	if (!LoadRMap() || !LoadDHMap())
	{
		Release();
		return 0;
	}
		
	Merge();


	Release();
	return 0;
}