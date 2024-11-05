#include <stdio.h>
#define _WIN32_WINDOWS 0x0410 
#include <windows.h>
#include <winbase.h>
#include <eh.h>
#include <assert.h>
#include "jexception.h"

//强制在调试器状态下也使用这个异常机制
#define JEXCEPTION_FORCE_USE 0

/*数据类型定义*/
typedef struct __exception_stack_node_t
{
	const char *	filename;
	int		linenumber;
	DWORD		iaddr;
} exception_stack_node;

#define J_EXCEPTION_MAX_STACK_SIZE	4096
typedef struct __exception_stack_t
{
	exception_stack_node	stack[J_EXCEPTION_MAX_STACK_SIZE];
	int			sp;
}exception_stack_t;



/*变量定义，使用线程相关变量*/
__declspec(thread) static exception_stack_t *ex_stack = NULL;
__declspec(thread) static int	seh_flag		= 1;
__declspec(thread) static int	seh_pop_count		= 0;

__declspec(thread) int		glb_exception_flag	= 0;
__declspec(thread) int		g_jexception_line	= 0;


/*将SEH转化为C++异常的转化器*/
static void translateSEHtoCE(UINT code,PEXCEPTION_POINTERS pep)
{
	seh_flag = 0;
	seh_pop_count = 0;
	const char * file = NULL;
	int line = 0;
	if(ex_stack && ex_stack->sp > 0) {
		file = ex_stack->stack[ex_stack->sp - 1].filename;
		line = ex_stack->stack[ex_stack->sp - 1].linenumber;
	}
	throw seh_exception(code,pep,file,line);
}

//初始化函数
int jexception_init()
{
	assert(!ex_stack);
//如果在DEBUG状态中，那么本异常机制不会生效
#ifdef _DEBUG
	if(!IsDebuggerPresent() || JEXCEPTION_FORCE_USE) 
	{
		_set_se_translator(translateSEHtoCE);
	}
#else
	_set_se_translator(translateSEHtoCE);
#endif

	ex_stack = (exception_stack_t *) calloc(1,sizeof(exception_stack_t));
	if(!ex_stack) return -2;
	return 0;
}

//释放函数
int jexception_finalize()
{
	free(ex_stack);
	ex_stack = NULL;
//如果在DEBUG状态中，那么本异常机制不会生效
#ifdef _DEBUG
	if(!IsDebuggerPresent() || JEXCEPTION_FORCE_USE) _set_se_translator(NULL);
#else
	_set_se_translator(NULL);
#endif
	return 0;
}

//入栈
static void ex_push(exception_stack_t * __es,const char * filename, int linenumber, DWORD eip)
{

	if(__es->sp >= J_EXCEPTION_MAX_STACK_SIZE){
		// raise a Exception
		RaiseException(0xAABBCCDD,0,NULL,NULL);
	}
	else
	{
		int tmp = __es->sp;
		__es->stack[tmp].filename	= filename;
		__es->stack[tmp].iaddr		= eip;
		__es->stack[tmp].linenumber	= linenumber;
		__es->sp ++;
	}
}

//出栈
static void ex_pop(exception_stack_t * __es)
{
	if(__es->sp <= 0){
		// raise a Exception
		RaiseException(0xAABBCCDE,0,NULL,NULL);
	}
	else
	{
		__es->sp --;
	}
}

//JException记录调用栈
void JException::Push(const char * filename, int linenumber, DWORD eip)
{
	m_flag = 1;
	ex_push(ex_stack,filename,linenumber,eip);
}

//如果没有发现异常,那么更新调用栈
JException::~JException()
{
	if(m_flag )
	{
		if(seh_flag){
			ex_pop(ex_stack);
		}
		else
		{
			seh_pop_count++;
		}
	}	
}

//异常处理和日志，每次异常发生后必须调用
int jexception_handler(FILE * logfile,const seh_exception & seh)

{
	int i;
	SYSTEMTIME st;
	FILE *file = NULL;
	char szTitle[MAX_PATH];
	exception_stack_t * es = ex_stack;
	_EXCEPTION_POINTERS	pex;
	LPEXCEPTION_POINTERS	lpex;

	pex.ContextRecord	= (struct _CONTEXT *)&seh.m_context;
	pex.ExceptionRecord	= (struct _EXCEPTION_RECORD *)&seh.m_exceptionRecord;
	es   = ex_stack;
	lpex = &pex;

	assert(!seh_flag);
	file = logfile;
	if( file )
	{
		GetLocalTime(&st);
		fprintf(file, "%04d.%02d.%02d %02d:%02d:%02d.%03d\n\n", st.wYear, st.wMonth,
			st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

		if(es->sp > 0 && es->sp <J_EXCEPTION_MAX_STACK_SIZE)
		{
			for( i = 0; i < es->sp; i++ )
			{
				strcpy(szTitle, es->stack[i].filename);
				fprintf(file, "\tFile: %s Line: %d EIP: 0x%08X\n", szTitle,
					es->stack[i].linenumber, es->stack[i].iaddr);
			}
			fprintf(file, "\n");
		}
		fprintf(file, "\tRealLine:%d\n", g_jexception_line);

		fprintf(file, "\tExceptionRecord->ExceptionCode = 0x%08X\n", lpex->ExceptionRecord->ExceptionCode);
		fprintf(file, "\tExceptionRecord->ExceptionFlags = 0x%08X\n", lpex->ExceptionRecord->ExceptionFlags);
		fprintf(file, "\tExceptionRecord->ExceptionAddress = 0x%08X\n", lpex->ExceptionRecord->ExceptionAddress);
		fprintf(file, "\tExceptionRecord->NumberParameters = %d\n\n", lpex->ExceptionRecord->NumberParameters);

		fprintf(file, "\tContextRecord->ContextFlags = 0x%08X\n", lpex->ContextRecord->ContextFlags);
		fprintf(file, "\tContextRecord->Dr0 = 0x%08X\n", lpex->ContextRecord->Dr0);
		fprintf(file, "\tContextRecord->Dr1 = 0x%08X\n", lpex->ContextRecord->Dr1);
		fprintf(file, "\tContextRecord->Dr2 = 0x%08X\n", lpex->ContextRecord->Dr2);
		fprintf(file, "\tContextRecord->Dr3 = 0x%08X\n", lpex->ContextRecord->Dr3);
		fprintf(file, "\tContextRecord->Dr6 = 0x%08X\n", lpex->ContextRecord->Dr6);
		fprintf(file, "\tContextRecord->Dr7 = 0x%08X\n", lpex->ContextRecord->Dr7);
		fprintf(file, "\tContextRecord->SegGs = 0x%08X\n", lpex->ContextRecord->SegGs);
		fprintf(file, "\tContextRecord->SegFs = 0x%08X\n", lpex->ContextRecord->SegFs);
		fprintf(file, "\tContextRecord->SegEs = 0x%08X\n", lpex->ContextRecord->SegEs);
		fprintf(file, "\tContextRecord->SegDs = 0x%08X\n", lpex->ContextRecord->SegDs);
		fprintf(file, "\tContextRecord->Edi = 0x%08X\n", lpex->ContextRecord->Edi);
		fprintf(file, "\tContextRecord->Esi = 0x%08X\n", lpex->ContextRecord->Esi);
		fprintf(file, "\tContextRecord->Ebx = 0x%08X\n", lpex->ContextRecord->Ebx);
		fprintf(file, "\tContextRecord->Edx = 0x%08X\n", lpex->ContextRecord->Edx);
		fprintf(file, "\tContextRecord->Ecx = 0x%08X\n", lpex->ContextRecord->Ecx);
		fprintf(file, "\tContextRecord->Eax = 0x%08X\n", lpex->ContextRecord->Eax);
		fprintf(file, "\tContextRecord->Ebp = 0x%08X\n", lpex->ContextRecord->Ebp);
		fprintf(file, "\tContextRecord->Eip = 0x%08X\n", lpex->ContextRecord->Eip);
		fprintf(file, "\tContextRecord->SegCs = 0x%08X\n", lpex->ContextRecord->SegCs);
		fprintf(file, "\tContextRecord->EFlags = 0x%08X\n", lpex->ContextRecord->EFlags);
		fprintf(file, "\tContextRecord->Esp = 0x%08X\n", lpex->ContextRecord->Esp);
		fprintf(file, "\tContextRecord->SegSs = 0x%08X\n\n", lpex->ContextRecord->SegSs);

		fflush(file);

		fprintf(file,"Stack:\n");
		unsigned char * sp ;
		for(i = 0; i < 256;i++)
		{
			int j ;
			sp = (unsigned char *)(lpex->ContextRecord->Esp + i * 16);
			fprintf(file,"%08X  ",sp);
			for(j = 0;j<16;j++)
			{
				fprintf(file,"%02X ",*sp);
				sp ++;
			}
			fprintf(file," ");

			sp = (unsigned char *)(lpex->ContextRecord->Esp + i * 16);
			for(j = 0;j<16;j++)
			{
				fprintf(file,"%c",(*sp<32 || *sp>127)?'.':*sp);
				sp ++;
			}
			fprintf(file,"\n");
		}
		
		fprintf(file, "Data near EIP:\n");
		unsigned char * eip ;
		for(i = 0; i < 64;i++)
		{
			int j ;
			eip = (unsigned char *)(lpex->ContextRecord->Eip+ i * 32 - 32 * 32);
			fprintf(file,"%08X  ",eip);
			for(j = 0;j<32;j++)
			{
				fprintf(file,"%02X ",*eip);
				eip ++;
			}
			fprintf(file,"\n");
		}
		fprintf(file,"\n\n");
		fflush(file);

		fprintf(file, "------------------------------------------------------------\n\n");
	}

	//reset
	for(i = 0; i< seh_pop_count;i++)
		ex_pop(ex_stack);
	seh_pop_count	= 0;
	seh_flag	= 1;
	return 0;
}


