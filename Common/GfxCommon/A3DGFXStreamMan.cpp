#include "StdAfx.h"
#include "A3DGFXStreamMan.h"

#ifdef _ANGELICA21
	static const int _default_rect_count = 2560;
#else
	static const int _default_rect_count = 2560;
#endif
static const int _default_vert_buf = 32 * 4 * _default_rect_count;
static const int _default_index_count = 6 * _default_rect_count;

bool A3DSharedStream::Init(A3DDevice* pDevice)
{
	if (A3DStream::Init(
		pDevice,
		1,
		0,
		_default_vert_buf,
		_default_index_count,
		A3DSTRM_REFERENCEPTR,
		A3DSTRM_STATIC))
	{
		PresetIndexBuf();
		return true;
	}
	else
		return false;
}

void A3DSharedStream::Appear(int nVertType, int nVertSize, bool bUseIndex)
{
#if defined (_ANGELICA21) || defined(_ANGELICA22)

	m_pA3DDevice->GetD3DDevice()->SetStreamSource(0, m_pVertexBuffer, m_dwCurLockOffset, nVertSize);
	m_dwCurLockOffset += m_dwCurLockSize;

	if (bUseIndex)
		m_pA3DDevice->GetD3DDevice()->SetIndices(m_pIndexBuffer);

#if defined(_ANGELICA22)
	
	m_pA3DDevice->ClearVertexShader();
	m_pA3DDevice->SetFVF(nVertType);

#endif

#else

	m_pA3DDevice->GetD3DDevice()->SetStreamSource(0, m_pVertexBuffer, nVertSize);
	m_pA3DDevice->ClearVertexShader();
	m_pA3DDevice->SetDeviceVertexShader(nVertType);

	if (bUseIndex)
		m_pA3DDevice->GetD3DDevice()->SetIndices(m_pIndexBuffer, 0);

#endif
}

int A3DGFXStreamMan::GetMaxVertexBufferSize()
{
	return _default_vert_buf;
}

int A3DGFXStreamMan::GetMaxIndexCount()
{
	return _default_index_count;
}

bool A3DGFXStreamMan::Init(A3DDevice* pDevice)
{
	Lock();

	if (m_nRef == 0 && !m_SharedStream.Init(pDevice))
	{
		Unlock();
		return false;
	}

	m_nRef++;

	Unlock();
	return true;
}

void A3DGFXStreamMan::Release()
{
	if (m_nRef == 0) return;

	Lock();
	if (--m_nRef == 0) m_SharedStream.Release();
	Unlock();
}

bool A3DGFXStreamMan::LockVertexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags)
{
#if defined(_ANGELICA21) || defined(_ANGELICA22)

	bool bRet;
	DWORD dwLockFlag = 0;
	Lock();

	if (m_SharedStream.IsVertexBufValid())
	{
		if (SizeToLock + m_SharedStream.m_dwCurLockOffset > _default_vert_buf)
			m_SharedStream.m_dwCurLockOffset = 0;
		else
			dwLockFlag = D3DLOCK_NOOVERWRITE;

		bRet = m_SharedStream.LockVertexBuffer(m_SharedStream.m_dwCurLockOffset, SizeToLock, ppbData, dwLockFlag);
		m_SharedStream.m_dwCurLockSize = SizeToLock;
	}
	else
		bRet = false;

	if (!bRet)
		Unlock();

	return bRet;

#else
	Lock();
	bool bRet = (m_SharedStream.IsVertexBufValid() && m_SharedStream.LockVertexBuffer(OffsetToLock, SizeToLock, ppbData, dwFlags));
	if (!bRet) Unlock();
	return bRet;
#endif
}
