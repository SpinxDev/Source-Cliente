#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_


#if defined(_USE_STD)

#define AFVector std::vector

#else

#define AFVector abase::vector

#endif


enum AFDErrorCodes
{
	AFDE_OK				= 1,		// OK
	AFDE_PATHNOTEXIST,				// Path Not Exist
	AFDE_DLLNOTINITIALIZED,			// Dll not initialized (call AFD_DllInitialize() first please)
};


#endif