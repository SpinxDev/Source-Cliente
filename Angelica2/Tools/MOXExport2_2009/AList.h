#include <Windows.h>

#ifndef _ALIST_H_
#define _ALIST_H_

typedef struct _ALISTELEMENT
{
	LPVOID			pData;
	_ALISTELEMENT * pNext;
	_ALISTELEMENT * pLast;
} ALISTELEMENT, * PALISTELEMENT;

class AList
{
private:
	ALISTELEMENT *	m_pHead;
	ALISTELEMENT *	m_pTail;
	int				m_nSize;

public:
	AList();
	~AList();

	bool Init();
	bool Release();
	
	bool Append(LPVOID pDataToAppend);
	bool Insert(LPVOID pDataToInsert, ALISTELEMENT * pElement);
	bool Insert(LPVOID pDataToInsert, LPVOID pDataInsertAfter);
	bool Delete(ALISTELEMENT * pElement);
	bool Delete(LPVOID pData);

	bool IsValid(ALISTELEMENT * pElement);
	ALISTELEMENT * FindElementByData(LPVOID pData);

	inline int GetSize() { return m_nSize; }
	inline ALISTELEMENT * GetHead() { return m_pHead; }
	inline ALISTELEMENT * GetTail() { return m_pTail; }
};

typedef AList * PAList;

#endif