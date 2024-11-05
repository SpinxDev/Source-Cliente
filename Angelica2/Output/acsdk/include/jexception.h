#ifndef __JEXCEPTION_H__
#define __JEXCEPTION_H__
#pragma once

#include <windows.h>
#include <eh.h>
class seh_exception
{
public:
	seh_exception(UINT code,PEXCEPTION_POINTERS pep)
	{
		m_exceptionCode = code;
		m_exceptionRecord = *pep->ExceptionRecord;
		m_context = *pep->ContextRecord;
		m_file	= NULL;
		m_line	= 0;
	}

	seh_exception(UINT code,PEXCEPTION_POINTERS pep,const char * file,int line)
	{
		m_exceptionCode = code;
		m_exceptionRecord = *pep->ExceptionRecord;
		m_context	= *pep->ContextRecord;
		m_file		= file;
		m_line		= line;
	}

	UINT		m_exceptionCode;
	EXCEPTION_RECORD m_exceptionRecord;
	CONTEXT		m_context;
	int		m_line;
	const char*	m_file;

};

int	jexception_init();
int	jexception_finalize();
int	jexception_handler(FILE * logfile,const seh_exception &seh);

// 这两个应该在工程里定义
#define J_EXCEPTION_ENABLED
#define J_EXCEPTION_LINE_ENABLED

//定义 J_EXCEPTION_RECORD() 和 J_EXCEPTION_LINE()


#ifdef J_EXCEPTION_ENABLED
	#define J_EXCEPTION_RECORD()	J_EXCEPTION_RECORD_IMP()
#else
	#define J_EXCEPTION_RECORD()	NULL
#endif

#ifdef J_EXCEPTION_LINE_ENABLED
	#define J_EXCEPTION_LINE()	do{g_jexception_line = __LINE__;} while(0)
#else
	#define J_EXCEPTION_LINE()	NULL
#endif



/* -----------------------------------------------------------------------------------*/
//具体实现，使用时不用关心
class JException
{
public:
	JException():m_flag(0){}
	void Push(const char * filename, int linenumber, DWORD eip);
	~JException();
private:
	int m_flag;
};

#define J_EXCEPTION_LABLE(T)	TMP_EXCEPTION_LABEL##T
#define J_EXCEPTION_RECORD_IMP()\
		JException MyException; \
		do{\
			DWORD exception_tmp; \
			__declspec(thread) extern int glb_exception_flag;\
			__asm { mov EBX,8h}\
			/*call near offset 0 ;acquire EIP*/ \
			__asm _emit 0xE8 __asm _emit 0x00  __asm _emit 0x00 __asm _emit 0x00 __asm _emit 0x00 \
			__asm { pop EAX}\
			__asm { mov exception_tmp, eax } \
			if(!glb_exception_flag & 0x01) 	\
				MyException.Push(__FILE__, __LINE__, exception_tmp);\
		}while(0)
#endif
__declspec(thread) extern int g_jexception_line;

