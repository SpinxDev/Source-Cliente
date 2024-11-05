/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXOUTPUTSTREAM_H_
#define _APHYSXOUTPUTSTREAM_H_


class APHYSX_DLL_EXPORT APhysXOutputStream : public NxUserOutputStream
{

public:
	APhysXOutputStream()
	{
		m_pLog = NULL;
	}


	void reportError(NxErrorCode e, const char* message, const char* file, int line)
	{
		if(!m_pLog) return;

		// make sure that szBuf is large enough....
		char szBuf[1024];
		char* szErrorInfo = szBuf;
		int iCur = 0;

		iCur = sprintf(szErrorInfo, "NxErrorStream => %s (%d) : ", file, line);
		szErrorInfo += iCur;

		switch (e)
		{
		case NXE_INVALID_PARAMETER:
			iCur = sprintf(szErrorInfo, "invalid parameter");
			break;
		case NXE_INVALID_OPERATION:
			iCur = sprintf(szErrorInfo, "invalid operation");
			break;
		case NXE_OUT_OF_MEMORY:
			iCur = sprintf(szErrorInfo, "out of memory");
			break;
		case NXE_DB_INFO:
			iCur = sprintf(szErrorInfo, "info");
			break;
		case NXE_DB_WARNING:
			iCur = sprintf(szErrorInfo, "warning");
			break;
		default:
			iCur = sprintf(szErrorInfo, "unknown error");
		}
		szErrorInfo += iCur;

		iCur = sprintf(szErrorInfo, " : %s", message);

		m_pLog->LogString(szBuf);
	}

	NxAssertResponse reportAssertViolation(const char* message, const char* file, int line)
	{
		if(m_pLog)
		{
			m_pLog->Log("NxErrorStream => access violation : %s (%s line %d)", message, file, line);
		}

#ifdef APHYSX_DEBUG

		switch (::MessageBoxA(0, message, "NxErrorStream => AssertViolation, see APhysXEngine.log for details.", MB_ABORTRETRYIGNORE))
		{
		case IDRETRY:
			return NX_AR_CONTINUE;
		case IDIGNORE:
			return NX_AR_IGNORE;
		case IDABORT:
		default:
			return NX_AR_BREAKPOINT;
		}

#else

		// in release mode, we just try to continue the execution...
		return NX_AR_CONTINUE;

#endif

	}

	void print(const char* message)
	{
		if(!m_pLog) return;

		// make sure that szBuf is large enough....
		char szBuf[1024];
		
		sprintf(szBuf, "NxErrorStream => %s", message);
		m_pLog->LogString(szBuf);
	}

	void InitLog(ALog* pLog)
	{
		m_pLog = pLog;
	}

protected:

	ALog* m_pLog;

};

#endif