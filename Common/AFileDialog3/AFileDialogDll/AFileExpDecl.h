#ifndef _AFILEEXP_H_
#define _AFILEEXP_H_

#ifdef _USRDLL

#define _AFILE_EXP_ __declspec(dllexport)

#else

#define _AFILE_EXP_ __declspec(dllimport)

#endif

#endif