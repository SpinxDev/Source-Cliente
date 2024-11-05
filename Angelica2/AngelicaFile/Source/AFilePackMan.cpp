 /*
 * FILE: AFilePackage.cpp
 *
 * DESCRIPTION: A File Package Class for Angelica Engine
 *
 * CREATED BY: Hedi, 2002/4/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AFilePackMan.h"
#include "AFilePackage.h"
#include "AFilePackGame.h"
#include "AFI.h"
#include "AFPI.h"

#define new A_DEBUG_NEW

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

int	AFPCK_GUARDBYTE0 = 0xfdfdfeee;
int	AFPCK_GUARDBYTE1 = 0xf00dbeef;
int AFPCK_MASKDWORD  = 0xa8937462;
int	AFPCK_CHECKMASK	 = 0x59374231;

AFilePackMan g_AFilePackMan;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	AFilePackMan
//
///////////////////////////////////////////////////////////////////////////

AFilePackMan::AFilePackMan() : m_FilePcks()
{
}

AFilePackMan::~AFilePackMan()
{
	CloseAllPackages();
}

bool AFilePackMan::CreateFilePackage(const char * szPckFile, const char* szFolder, bool bEncrypt/* false */)
{
	AFilePackage * pFilePackage;
	if (!(pFilePackage = new AFilePackage))
	{
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Not enough memory!"));
		return false;
	}

	if (!pFilePackage->Open(szPckFile, szFolder, AFilePackage::CREATENEW, bEncrypt))
	{
		delete pFilePackage;
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
		return false;
	}

	m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::OpenFilePackage(const char * szPckFile, bool bCreateNew, bool bEncrypt)
{
	AFilePackage * pFilePackage;
	if (!(pFilePackage = new AFilePackage))
	{
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Not enough memory!"));
		return false;
	}

	if (!pFilePackage->Open(szPckFile, bCreateNew ? AFilePackage::CREATENEW : AFilePackage::OPENEXIST, bEncrypt))
	{
		delete pFilePackage;
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
		return false;
	}

	m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::OpenFilePackage(const char * szPckFile, const char* szFolder, bool bEncrypt/* false */)
{
	AFilePackage * pFilePackage;
	if (!(pFilePackage = new AFilePackage))
	{
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Not enough memory!"));
		return false;
	}

	if (!pFilePackage->Open(szPckFile, szFolder, AFilePackage::OPENEXIST, bEncrypt))
	{
		delete pFilePackage;
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
		return false;
	}

	m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::OpenFilePackageInGame(const char* szPckFile, bool bEncrypt/* false */)
{
	AFilePackGame* pFilePackage;
	if (!(pFilePackage = new AFilePackGame))
	{
		AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Not enough memory!"));
		return false;
	}

	if (!pFilePackage->Open(szPckFile, AFilePackGame::OPENEXIST, bEncrypt))
	{
		delete pFilePackage;
		AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Can not open package [%s]", szPckFile));
		return false;
	}

	m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::OpenFilePackageInGame(const char* szPckFile, const char* szFolder, bool bEncrypt/* false */)
{
	AFilePackGame* pFilePackage;
	if (!(pFilePackage = new AFilePackGame))
	{
		AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Not enough memory!"));
		return false;
	}

	if (!pFilePackage->Open(szPckFile, szFolder, AFilePackGame::OPENEXIST, bEncrypt))
	{
		delete pFilePackage;
		AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Can not open package [%s]", szPckFile));
		return false;
	}

	m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::CloseFilePackage(AFilePackBase* pFilePck)
{
	int nCount = m_FilePcks.size();
	for(int i=0; i<nCount; i++)
	{
		if( m_FilePcks[i] == pFilePck )
		{
			m_FilePcks.erase(&m_FilePcks[i]);

			pFilePck->Close();
			delete pFilePck;
			return true;
		}
	}

	return true;
}

bool AFilePackMan::CloseAllPackages()
{
	while(m_FilePcks.size())
	{
		AFilePackBase* pFilePck = m_FilePcks[0];

		m_FilePcks.erase(&m_FilePcks[0]);

		pFilePck->Close();
		delete pFilePck;
	}

	m_FilePcks.clear();
	
	return true;
}

AFilePackBase* AFilePackMan::GetFilePck(const char * szPath)
{
	char szLowPath[MAX_PATH];
	strncpy(szLowPath, szPath, MAX_PATH);
	strlwr(szLowPath);
	AFilePackage::NormalizeFileName(szLowPath);

	for(size_t i=0; i<m_FilePcks.size(); i++)
	{
		AFilePackBase* pPack = m_FilePcks[i];
		
		if( strstr(szLowPath, pPack->GetFolder()) == szLowPath )
		{
			return pPack;
		}
	}

	return NULL;
}

bool AFilePackMan::SetAlgorithmID(int id)
{
	switch(id)
	{
	case 111:
		AFPCK_GUARDBYTE0 = 0xab12908f;
		AFPCK_GUARDBYTE1 = 0xb3231902;
		AFPCK_MASKDWORD  = 0x2a63810e;
		AFPCK_CHECKMASK	 = 0x18734563;
		break;

	default:
		AFPCK_GUARDBYTE0 = 0xfdfdfeee + id * 0x72341f2;
		AFPCK_GUARDBYTE1 = 0xf00dbeef + id * 0x1237a73;
		AFPCK_MASKDWORD  = 0xa8937462 + id * 0xab2321f;
		AFPCK_CHECKMASK	 = 0x59374231 + id * 0x987a223;
		break;
	}

	return true;
}

