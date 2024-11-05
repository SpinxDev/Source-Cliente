
#include "APhysXStdAfx.h"

APHYSX_DEFINE_SINGLETON(APhysXStringManager)

const char* APhysXStringManager::GetString(const char* str)
{
	// exception handle first...
	if (str	== 0 || str[0] == 0)
	{
		return 0;
	}

	//int	l =	(int)strlen(str);
	//if (l == 0)
	//{
	//	return 0;
	//}

	// use construct-lock-destruct-unlock instead multi-branch unlock
	APhysXMutexLock mlStringList(m_MutexStringList);

	/*StringList* pCurStringElement = m_Root;

	// search the string list
	while(pCurStringElement)
	{
		if(strcmp(pCurStringElement->mString, str) == 0)
		{
			// m_MutexStringList.Unlock();
			return pCurStringElement->mString;
		}

		pCurStringElement = pCurStringElement->mNextString;
	}

	// can't find one, so we add a new string element
	int iLen = sizeof(StringList) + l + 1;
	char* buf = new char[iLen];

	StringList* pNewStringElement = (StringList*)buf;
	pNewStringElement++;
	buf = (char *)pNewStringElement;
	pNewStringElement--;

	pNewStringElement->mNextString = m_Root;
	pNewStringElement->mString = buf;

	m_Root = pNewStringElement;
	m_StringCount++;
	m_StringMem += iLen;

	strcpy(buf, str);

	return buf;
	*/

	AString strIn = str;
	StringMap::iterator it = m_StringMap.find(strIn);
	if (it != m_StringMap.end())
	{
		const char* pBuf = it->second;
		return pBuf;
	}

	char* pBuf = new char[strIn.GetLength() + 1];
	strcpy(pBuf, str);

	m_StringMap[strIn] = pBuf;

	return pBuf;
}

void APhysXStringManager::Release()
{
	m_MutexStringList.Lock();

	/*StringList* pCurStringElement = m_Root;

	while(pCurStringElement)
	{
		StringList *next = pCurStringElement->mNextString;
		char *c	=	(char*)pCurStringElement;
		delete c;
		pCurStringElement = next;
	}

	m_Root = NULL;
	m_StringCount = 0;
	m_StringMem = 0;
	*/

	for (StringMap::iterator it = m_StringMap.begin(); it != m_StringMap.end(); ++it)
		delete[] it->second;
	m_StringMap.clear();

	m_MutexStringList.Unlock();
}
