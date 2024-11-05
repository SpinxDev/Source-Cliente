
/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

	Note:
		
		This is a utility class which helps to hack the PhysX registry
	and load the PhysXCore.dll and PhysXCooking.dll from the specified 
	directory.

		
----------------------------------------------------------------------*/

#ifndef _APHYSXREGISTRYHACK_H_
#define _APHYSXREGISTRYHACK_H_

#define MAX_BUF_SIZE 512

#include "APhysXPrerequisites.h"

class APHYSX_DLL_EXPORT APhysXRegistryHack
{

public:

	APhysXRegistryHack();
	~APhysXRegistryHack();

	bool HackPhysXPath(const char* szMyPhysXCorePath);
	void CleanupHack();

	// Note: get this value after calling CleanupHack()...
	bool IsPhysXInstalled() { return m_bPhysXInstalled; }

protected:

	TCHAR m_szOriginalPhyXPath[MAX_BUF_SIZE];
	bool m_bPhysXInstalled;
	bool m_bHacked;
	
};




#endif