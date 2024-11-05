
#include "APhysXRegistryHack.h"


#define ENABLE_PHYSX_HACKING	1

//----------------------------------------------------------
// two choose one...
// the following macro definitions are exclusive...
// and now we use the latter hack method...

//#define	HACKMODE_CHANGE_PHYSX_REGISTRY				// change and write the registry of PhysX directly...
#define	HACKMODE_HOOK_QUERY_REGISTRY_FUNC				// hook the opening and querying windows 


#ifdef HACKMODE_HOOK_QUERY_REGISTRY_FUNC
//----------------------------------------------------------
// Note:
//		added by Wenfeng, Nov. 24, 2008
// code for hooking windows registry opening and querying functions...

//----------------------------------------------------------

namespace APhysXHook
{

	#include "mhook.h"
	#include "mhook.cpp"
};

// we should keep record of MyPhysXCorePath globally...
static AString s_strMyPhysXCorePath;

#endif
//----------------------------------------------------------

#define PHYSX_KEY_NAME			TEXT("Software\\AGEIA Technologies\\")
#define PHYSX_CORE_PATH_NAME	TEXT("PhysXCore Path")

APhysXRegistryHack::APhysXRegistryHack()
{
	m_bPhysXInstalled = false;
	m_bHacked = false;
}

APhysXRegistryHack::~APhysXRegistryHack()
{
	CleanupHack();
}


bool APhysXRegistryHack::HackPhysXPath(const char* szMyPhysXCorePath)
{

#if ENABLE_PHYSX_HACKING

#ifdef HACKMODE_CHANGE_PHYSX_REGISTRY

	HKEY PhysXKey;
	LONG Result;

	DWORD dwDisposition;


	// TCHAR* tchMyPhysXCorePath[MAX_BUF_SIZE];
	const TCHAR* tchMyPhysXCorePath = NULL;

#ifdef UNICODE
	
	AWString wstrMyPhysXCorePath = AS2WC(szMyPhysXCorePath);;
	tchMyPhysXCorePath = wstrMyPhysXCorePath;

#else

	tchMyPhysXCorePath = szMyPhysXCorePath;

#endif


	// create the PhysX Key...
	Result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, PHYSX_KEY_NAME, 0, TEXT(""), 0, KEY_READ | KEY_WRITE, NULL, &PhysXKey, &dwDisposition);

	if(Result == ERROR_SUCCESS)
	{
		if(dwDisposition == REG_CREATED_NEW_KEY)
		{
			// create the value: PhysXCore Path
			m_bPhysXInstalled = false;

			Result = RegSetValueEx(PhysXKey, PHYSX_CORE_PATH_NAME, 0, REG_SZ, (LPBYTE)tchMyPhysXCorePath, (lstrlen(tchMyPhysXCorePath) + 1) * sizeof(TCHAR));

			if(Result == ERROR_SUCCESS)
			{
				m_bHacked = true;
			}


		}
		else if(dwDisposition == REG_OPENED_EXISTING_KEY)
		{

			// we've installed the PhysX
			m_bPhysXInstalled = true;

			DWORD dwBufLen = MAX_BUF_SIZE;

			Result = RegQueryValueEx(PhysXKey, PHYSX_CORE_PATH_NAME, NULL, NULL, (LPBYTE)m_szOriginalPhyXPath, &dwBufLen);
			
			if(Result == ERROR_SUCCESS)
			{
				Result = RegSetValueEx(PhysXKey, PHYSX_CORE_PATH_NAME, 0, REG_SZ, (LPBYTE)tchMyPhysXCorePath, (lstrlen(tchMyPhysXCorePath) + 1) * sizeof(TCHAR));

				if(Result == ERROR_SUCCESS)
				{
					m_bHacked = true;
				}

			}
		}

		RegCloseKey(PhysXKey);

	}

	return m_bHacked;

#endif	// end of #ifdef HACKMODE_CHANGE_PHYSX_REGISTRY

#ifdef HACKMODE_HOOK_QUERY_REGISTRY_FUNC

	s_strMyPhysXCorePath = szMyPhysXCorePath;

	APhysXHook::InstallHook(TRUE);

	APhysXHook::SetPathA(s_strMyPhysXCorePath);

	m_bHacked = true;

	return false;

#endif  // end of #ifdef HACKMODE_HOOK_QUERY_REGISTRY_FUNC

#else

	return false;
	
#endif  // end of #if ENABLE_PHYSX_HACKING

}

void APhysXRegistryHack::CleanupHack()
{

#if ENABLE_PHYSX_HACKING

#ifdef HACKMODE_CHANGE_PHYSX_REGISTRY
	
	if(m_bHacked)
	{

		if(m_bPhysXInstalled)
		{
			HKEY PhysXKey;
			LONG Result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, PHYSX_KEY_NAME, 0, KEY_WRITE | KEY_READ, &PhysXKey);
			
			if (Result == ERROR_SUCCESS)
			{
				Result = RegSetValueEx(PhysXKey, PHYSX_CORE_PATH_NAME, 0, REG_SZ, (LPBYTE)m_szOriginalPhyXPath, (lstrlen(m_szOriginalPhyXPath) + 1) * sizeof(TCHAR));

				// no need to check the Result...
				RegCloseKey(PhysXKey);
			}

		}
		else
		{
			// we should delete the key...
			LONG Result = RegDeleteKey(HKEY_LOCAL_MACHINE, PHYSX_KEY_NAME);

			// no need to check the Result...
		}

		m_bHacked = false;
	}

#endif	// end of #ifdef HACKMODE_CHANGE_PHYSX_REGISTRY

#ifdef HACKMODE_HOOK_QUERY_REGISTRY_FUNC

	APhysXHook::InstallHook(FALSE);

	m_bPhysXInstalled = (APhysXHook::GetInstallFlag() == TRUE);

	m_bHacked = false;
	

#endif  // end of #ifdef HACKMODE_HOOK_QUERY_REGISTRY_FUNC

#else

	// do nothing...

#endif  // end of #if ENABLE_PHYSX_HACKING

}
