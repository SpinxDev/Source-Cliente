#ifndef _AFD_REQUIRE_HEADERS_H_
#define _AFD_REQUIRE_HEADERS_H_

#pragma warning(disable : 4786)

// Windows Headers
#include <shlwapi.h>

// C++ Headers
#include <set>


// Angelica Headers
#include <AF.h>
#include <A3D.h>
#include <AM.h>
#include <a3dpi.h>

#ifdef _ANGELICA31
#include "A3DGfxHeaders.h"

#elif defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
#include "LuaState.h"
#include "LuaBind.h"
#endif

// 3rd library Headers
//#include "ximage.h"

// Depend-Project Headers
#include "ICLThreadPool.h"
#include "ICLNotifier.h"
#include "ICLFile.h"
#include "ICLImg.h"
#include "ICLTaskProcFolder.h"
#include "ICLCommon.h"
#include "ICLImgImpl.h"
#include "ICLImgImplDDS.h"
#include "ICLImgImplECM.h"
#include "ICLImgImplGfx.h"
#include "ICLImgImplSMD.h"
#include "ICLImgImplUMD.h"
#include "ICLImgImplUMDS.h"
#include "ICLUnicodeCommon.h"
#include "PlatformIndependFuncs.h"

// Self-Project Headers
#include "resource.h"
#include "AFileDialogDll.h"
#include "AFileDialogWrapper.h"
#include "AFileDialog.h"
#include "AListCtrl.h"
#include "DynSizeCtrl.h"
#include "AFTaskPlaySound.h"

#endif