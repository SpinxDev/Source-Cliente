/*
* FILE: CHullArchive.cpp
*
* DESCRIPTION: Load & Save Convex hull data
*
* CREATED BY: 
*
* HISTORY:
*
* 
*/
#include "ConvexHullArchive.h"

#include "ConvexHullData.h"

//old .ecp file header
#define ECMODEL_PROJ_MAGIC (unsigned int) (('e'<<24) | ('c' << 16) | ('b' << 8) | ('p'))
#define ECMODEL_PROJ_VERSION (unsigned int) (0x00000001)

namespace CHBasedCD
{

//工程文件头
struct ECModelBrushProjHead_t
{
	unsigned int  nMagic;
	unsigned int  nVersion;
	char szECModelFile[MAX_PATH];
	int nFrame;
	unsigned int  nNumHull;
};

static const int dwCHFileId = 0xffffffff;
static const int dwVersion = 2;//current version 2
#define ASSERTREADSUCESS(len) { if (len == 0) return false; } 

ConvexHullArchive::ConvexHullArchive()
{
	m_szModelFile[0] = '\0';
	m_nNumHull = 0;
	m_genType = NONE;
	m_bOldEcp = false;	
}
ConvexHullArchive::~ConvexHullArchive()
{

}

const char* ConvexHullArchive::GetModelFile() const
{
	if (m_genType == NONE)
		return NULL;
	return m_szModelFile;
}

int ConvexHullArchive::GetFrame() const
{
	if (m_genType == NONE || m_genType == MOX)
		return -1;
	return m_nFrame;
}
//get hull number
int ConvexHullArchive::GetNumHull() const
{
	if (m_genType == NONE)
		return -1;
	return m_nNumHull;
}

void ConvexHullArchive::SetModelFile(const char* szModelFile)
{
	if (szModelFile)
		strcpy(m_szModelFile, szModelFile); 
	else
		m_szModelFile[0] = 0;
}

bool ConvexHullArchive::LoadHeader(AFile& af)
{
	DWORD dwRead = 0, dwLen = 0;
	int dwDWSize = sizeof(int);
	af.Read(&dwRead, dwDWSize, &dwLen);
	
	m_bOldEcp = false;
	ASSERTREADSUCESS(dwLen);
	if (dwRead == dwCHFileId)
	{
		af.Read(&dwRead, dwDWSize, &dwLen);
		ASSERTREADSUCESS(dwLen);
		if (dwRead != dwVersion)
			return false;

		af.Read(&dwRead, dwDWSize, &dwLen);
		ASSERTREADSUCESS(dwLen);
		m_genType = GENTYPE(dwRead);

		af.Read(m_szModelFile, sizeof(m_szModelFile), &dwRead);
		if (m_genType == ECM)
		{			
			af.Read(&m_nFrame, sizeof(int), &dwRead);
		}
		af.Read(&m_nNumHull, sizeof(int), &dwRead);
		ASSERTREADSUCESS(dwLen);
	}
	else if (dwRead == ECMODEL_PROJ_MAGIC)//old version .ecp
	{
		af.Seek(-(int)(dwDWSize), AFILE_SEEK_CUR);
		
		ECModelBrushProjHead_t header;
		af.Read(&header, sizeof(header), &dwLen);
		ASSERTREADSUCESS(dwLen);
		if (header.nVersion != ECMODEL_PROJ_VERSION)
			return false;

		m_genType = ECM;
		strcpy(m_szModelFile, header.szECModelFile);
		m_nFrame = header.nFrame;
		m_nNumHull = header.nNumHull;
		m_bOldEcp = true;
	}
	else //old version .chf
	{
		af.Seek(-(int)(dwDWSize), AFILE_SEEK_CUR);
		m_genType = MOX;
		m_szModelFile[0] = 0;
		af.Read(&m_nNumHull, sizeof(int), &dwLen);
		ASSERTREADSUCESS(dwLen);

		//skip mesh data
		int nMeshID, IntSize = sizeof(int);
		for (int i = 0; i < m_nNumHull; i++)
		{
			af.Read(&nMeshID, IntSize, &dwLen);
			af.Seek(nMeshID * IntSize, AFILE_SEEK_CUR);
		}
	}

	return true;
}

bool ConvexHullArchive::SaveHeader(AFile& af)
{
	DWORD dwLen = 0;
	int dwWrite, dwDWSize = sizeof(int);
	af.Write((void*)&dwCHFileId, dwDWSize, &dwLen);
	af.Write((void*)&dwVersion, dwDWSize, &dwLen);
	dwWrite = (int)m_genType;
	af.Write((void*)&dwWrite, dwDWSize, &dwLen);
	af.Write((void*)m_szModelFile, sizeof(m_szModelFile), &dwLen);
	if (m_genType == ECM)
	{		
		af.Write((void*)&m_nFrame, sizeof(int), &dwLen);
	}
	af.Write((void*)&m_nNumHull, sizeof(int), &dwLen);
	return true;
}

}
