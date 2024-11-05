#ifndef _AF_TASK_PLAY_SOUND_H_
#define _AF_TASK_PLAY_SOUND_H_

#include "ICLThreadTask.h"

class A3DDevice;
class AMSoundBuffer;
class AMSoundStream;
class AMSoundBufferMan;

struct AFSoundPlayItem
{
	AFSoundPlayItem();
	~AFSoundPlayItem();

	void LoadWavSound(AMSoundBufferMan* pMan, const char* szFile);
	void LoadStreamSound(AMSoundEngine* pAMEngine, const char* szFile);
	void StopSound();
	void Release(AMSoundBufferMan* pMan);
private:
	AMSoundBuffer* pBuffer;
	AMSoundStream* pStream;
	bool bStreamInited;
};

class AFTaskPlaySound : public ICLThreadTask
{
public:
	AFTaskPlaySound(A3DDevice* pDevice, AFSoundPlayItem* pPlayItem, const TCHAR* szFile);
	~AFTaskPlaySound() {}

public:
	bool Do();

private:
	AMSoundBufferMan* m_pBufferMan;
	AFSoundPlayItem* m_pPlayItem;
	ACString m_strFile;
	AMSoundEngine* m_pAMEngine;
};


enum {
	AF_SUPPORT_SOUND_UNKNOWN = -1,
	AF_SUPPORT_SOUND_WAV = 0,
	AF_SUPPORT_SOUND_MP3 = 1,
	AF_SUPPORT_SOUND_OGG = 2,
};
int is_support_sound(const TCHAR* szFile);
AMSoundEngine* get_AM_sound_engine(A3DDevice* device);
AMSoundBufferMan* get_AM_sound_buffer_man(A3DDevice* device);

#endif