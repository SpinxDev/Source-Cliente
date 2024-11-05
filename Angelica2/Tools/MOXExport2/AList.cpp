#include <Windows.h>
#include "AList.h"

AList::AList()
{
	m_pHead = m_pTail = NULL;
	m_nSize = 0;
}

AList::~AList()
{
	
}

bool AList::Init()
{
	m_pHead = (ALISTELEMENT *) malloc(sizeof(ALISTELEMENT));
	if( NULL == m_pHead )
		return false;

	m_pTail = (ALISTELEMENT *) malloc(sizeof(ALISTELEMENT));
	if( NULL == m_pTail )
		return false;

	m_pHead->pData = m_pTail->pData = NULL;
	m_pHead->pLast = m_pTail->pNext = NULL;
	m_pHead->pNext = m_pTail;
	m_pTail->pLast = m_pHead;
	m_nSize = 0;
	return true;
}

bool AList::Release()
{
	ALISTELEMENT * pThisElement = m_pHead;
	ALISTELEMENT * pElementToDel;

	//We do not carry out the work to release Element.pData, 
	//That is the caller's task :(
	while( pThisElement )
	{
		pElementToDel = pThisElement;
		pThisElement = pThisElement->pNext;

		free(pElementToDel);
	}

	m_pHead = m_pTail = NULL;
	m_nSize = 0;
	return true;
}

bool AList::Append(LPVOID pDataToAppend)
{
	return Insert(pDataToAppend, m_pTail->pLast);
}
	
bool AList::Insert(LPVOID pDataToInsert, ALISTELEMENT * pElement)
{
	ALISTELEMENT * pNewElement;
	if( NULL == pElement )
		return false;

	pNewElement = (ALISTELEMENT *) malloc(sizeof(ALISTELEMENT));
	if( NULL == pNewElement )
		return false;

	pNewElement->pData = pDataToInsert;
	pNewElement->pLast = pElement;
	pNewElement->pNext = pElement->pNext;
	pElement->pNext = pNewElement;
	pNewElement->pNext->pLast = pNewElement;

	m_nSize ++;
	return true;
}
	
bool AList::Insert(LPVOID pDataToInsert, LPVOID pDataInsertAfter)
{
	if( NULL == pDataInsertAfter )
		return false;

	ALISTELEMENT * pElement = FindElementByData(pDataInsertAfter);
	if( NULL == pElement )
		return false;

	return Insert(pDataToInsert, pElement);
}
	
bool AList::Delete(ALISTELEMENT * pElement)
{
	if( NULL == pElement )
		return false;

	pElement->pLast->pNext = pElement->pNext;
	pElement->pNext->pLast = pElement->pLast;

	free(pElement);
	m_nSize --;
	return true;
}
	
bool AList::Delete(LPVOID pData)
{
	ALISTELEMENT * pElement;
	if( NULL == pData )
		return false;

	pElement = FindElementByData(pData);
	if( NULL == pElement )
		return false;

	return Delete(pElement);
}

ALISTELEMENT * AList::FindElementByData(LPVOID pData)
{
	ALISTELEMENT * pThisElement = m_pHead->pNext;

	while(pThisElement != m_pTail)
	{
		if( pThisElement->pData == pData )
			return pThisElement;
		else
			pThisElement = pThisElement->pNext;
	}

	//Nothing be found;
	return NULL;
}

bool AList::IsValid(ALISTELEMENT * pElement)
{
	ALISTELEMENT * pThisElement = m_pHead->pNext;

	while( pThisElement != m_pTail )
	{
		if( pThisElement == pElement )
			return true;
		else
			pThisElement = pThisElement->pNext;
	}

	return false;
}