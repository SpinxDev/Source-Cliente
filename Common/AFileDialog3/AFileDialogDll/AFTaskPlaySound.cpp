#include "StdAfx.h"
#include "AFTaskPlaySound.h"

int is_support_sound(const TCHAR* szFile)
{
	ACString tmpstr(szFile);
	AString str = AC2AS(tmpstr);
	str.MakeLower();
	if (af_CheckFileExt(str, ".wav"))
		return AF_SUPPORT_SOUND_WAV;
	if (af_CheckFileExt(str, ".mp3"))
		return AF_SUPPORT_SOUND_MP3;
	if (af_CheckFileExt(str, ".ogg"))
		return AF_SUPPORT_SOUND_OGG;
	return AF_SUPPORT_SOUND_UNKNOWN;
}

AMSoundEngine* get_AM_sound_engine(A3DDevice* device)
{
	if (!device) return NULL;
	A3DEngine* pEngine = device->GetA3DEngine();
	if (!pEngine) return NULL;
	return pEngine->GetAMSoundEngine();
}

AMSoundBufferMan* get_AM_sound_buffer_man(A3DDevice* device)
{
	AMSoundEngine* sound_engine = get_AM_sound_engine(device);
	if (!sound_engine) return NULL;
	return sound_engine->GetAMSoundBufferMan();
}


AFSoundPlayItem::AFSoundPlayItem()
: pBuffer(NULL)
, pStream(new AMSoundStream) 
, bStreamInited(false)
{

}

AFSoundPlayItem::~AFSoundPlayItem()
{
	delete pStream;
}

void AFSoundPlayItem::StopSound()
{
	if (pBuffer) pBuffer->Stop();
	if (pStream && pStream->IsPlaying()) pStream->Stop();
}

void AFSoundPlayItem::LoadWavSound(AMSoundBufferMan* pMan, const char* szFile)
{
	if (!pMan || !szFile) 
		return;

	Release(pMan);
	pBuffer = pMan->Load2DSound(szFile);
	if (!pBuffer)
		return;

	pBuffer->Play(false);
}

void AFSoundPlayItem::LoadStreamSound(AMSoundEngine* pAMEngine, const char* szFile)
{
	if (!pAMEngine || !szFile) return;
	Release(pAMEngine->GetAMSoundBufferMan());
	bStreamInited = pStream->Init(pAMEngine, szFile);
	if (bStreamInited)
		pStream->Play();
}

void AFSoundPlayItem::Release(AMSoundBufferMan* pMan)
{
	if (pBuffer)
	{
		pMan->Release2DSound(&pBuffer);
		pBuffer = NULL;
	}

	if (pStream && bStreamInited)
	{ 
		pStream->Release();
		bStreamInited = false;
	}
}

AFTaskPlaySound::AFTaskPlaySound(A3DDevice* pDevice, AFSoundPlayItem* pPlayItem, const TCHAR* szFile)
: m_pPlayItem(pPlayItem)
, m_strFile(szFile)
, m_pAMEngine(get_AM_sound_engine(pDevice))
{
	m_pBufferMan = m_pAMEngine ? m_pAMEngine->GetAMSoundBufferMan() : NULL;
}

bool AFTaskPlaySound::Do()
{
	if (!m_pBufferMan || m_strFile.IsEmpty())
		return true;

	m_pPlayItem->Release(m_pBufferMan);

	AString str = AC2AS(m_strFile);
	int iSoundType = is_support_sound(m_strFile);
	if (AF_SUPPORT_SOUND_UNKNOWN == iSoundType)
		return true;
	else if (AF_SUPPORT_SOUND_WAV == iSoundType)
	{
		m_pPlayItem->LoadWavSound(m_pBufferMan, str);
	}
	else if (AF_SUPPORT_SOUND_OGG == iSoundType
		|| AF_SUPPORT_SOUND_MP3 == iSoundType)
	{
		m_pPlayItem->LoadStreamSound(m_pAMEngine, str);
	}
	return true;	
}