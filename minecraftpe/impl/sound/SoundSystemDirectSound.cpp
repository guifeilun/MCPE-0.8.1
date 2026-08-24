#ifdef __WIN32__
#include <sound/SoundSystemDirectSound.hpp>
#include <sound/SoundDesc.hpp>
#include <math.h>
#include <sounddata.hpp>
#include <winsock2.h>
#include <windows.h>
#include <SDL/SDL_syswm.h>
#include <unknwn.h>
#include <string.h>
#include <stdio.h>

SoundSystemDirectSound::SoundSystemDirectSound(void) {
	this->dsound = NULL;
	this->playedCnt = 0;
	for(int i = 0; i < MAX_PLAYED; ++i) {
		this->buffers[i] = NULL;
	}
}

SoundSystemDirectSound::~SoundSystemDirectSound() {
	this->destroy();
}

bool_t SoundSystemDirectSound::checkErr(uint32_t a2) {
	if(a2) {
		return 1;
	}
	return 0;
}

void SoundSystemDirectSound::destroy(void) {
	for(int i = 0; i < MAX_PLAYED; ++i) {
		if(this->buffers[i]) {
			this->buffers[i]->Release();
			this->buffers[i] = NULL;
		}
	}
	if(this->dsound) {
		this->dsound->Release();
		this->dsound = NULL;
	}
	this->playedCnt = 0;
	printf("DirectSound destroyed\n");
}

void SoundSystemDirectSound::init(void) {
	HRESULT hr = DirectSoundCreate8(NULL, &this->dsound, NULL);
	if(FAILED(hr)){
		printf("DirectSoundCreate8 failed: %x\n", hr);
		this->dsound = NULL;
		return;
	}
	printf("DirectSoundCreate8 succeeded\n");
	
	HWND wnd = NULL;
	
	SDL_SysWMinfo info;
	if (SDL_GetWMInfo(&info)) {
		wnd = info.window;
		printf("Got window handle via SDL_GetWMInfo: %p\n", wnd);
	}
	
	if (!wnd) {
		wnd = GetForegroundWindow();
		if (wnd) {
			printf("Got window handle via GetForegroundWindow: %p\n", wnd);
		}
	}
	
	if (!wnd) {
		wnd = GetDesktopWindow();
		printf("Falling back to desktop window: %p\n", wnd);
	}
	
	hr = this->dsound->SetCooperativeLevel(wnd, DSSCL_NORMAL);
	if(FAILED(hr)){
		printf("SetCooperativeLevel failed: %x\n", hr);
		this->dsound->Release();
		this->dsound = NULL;
		return;
	}
	
	printf("DirectSound initialized successfully with window: %p\n", wnd);
}

void SoundSystemDirectSound::removeStoppedSounds(void) {
	this->playedCnt = 0;
	for(int i = 0; i < MAX_PLAYED; ++i) {
		if(!this->buffers[i]) continue;
		DWORD status;
		HRESULT hr = this->buffers[i]->GetStatus(&status);
		if(FAILED(hr)) {
			this->buffers[i]->Release();
			this->buffers[i] = NULL;
			continue;
		}
		if(status & DSBSTATUS_PLAYING){
			++this->playedCnt;
		}else{
			this->buffers[i]->Release();
			this->buffers[i] = NULL;
		}
	}
}

void SoundSystemDirectSound::setListenerPos(float a, float b, float c) {
}

void SoundSystemDirectSound::setListenerAngle(float a) {
}

void SoundSystemDirectSound::load(const std::string&) {
}

void SoundSystemDirectSound::play(const std::string&) {
}

void SoundSystemDirectSound::pause(const std::string&) {
}

void SoundSystemDirectSound::stop(const std::string&) {
}

void SoundSystemDirectSound::playAt(const struct SoundDesc& a2, float a3, float a4, float a5, float a6, float a7) {
	this->removeStoppedSounds();
	
	if(this->playedCnt >= MAX_PLAYED) {
		return;
	}
	
	for(int i = 0; i < MAX_PLAYED; ++i) {
		if(this->buffers[i]) continue; 
		
		WAVEFORMATEX wf;
		wf.wFormatTag = WAVE_FORMAT_PCM;
		wf.nSamplesPerSec = a2.sampleRate;
		wf.wBitsPerSample = 8 * a2.bytesPerSample;
		wf.nChannels = a2.channels;
		wf.nBlockAlign = a2.channels * a2.bytesPerSample;
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
		wf.cbSize = 0;
		
		DSBUFFERDESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
		desc.dwBufferBytes = a2.field_4;
		desc.dwReserved = 0;
		desc.lpwfxFormat = &wf;
		desc.guid3DAlgorithm = GUID_NULL;
		
		LPDIRECTSOUNDBUFFER tmp = NULL;
		HRESULT hr = this->dsound->CreateSoundBuffer(&desc, &tmp, NULL);
		if(FAILED(hr)){
			printf("CreateSoundBuffer failed: %x (channels=%d, sampleRate=%d, bytes=%d)\n", 
			       hr, a2.channels, a2.sampleRate, a2.field_4);
			break;
		}
		
		hr = tmp->QueryInterface(IID_IDirectSoundBuffer8, (void**)&this->buffers[i]);
		if(FAILED(hr)){
			printf("QueryInterface for IDirectSoundBuffer8 failed: %x\n", hr);
			tmp->Release();
			break;
		}
		tmp->Release();
		
		void* bf = NULL;
		DWORD bsize = 0;
		HRESULT lockResult = this->buffers[i]->Lock(0, a2.field_4, &bf, &bsize, NULL, 0, 0);

		if(lockResult == DSERR_BUFFERLOST) {
			hr = this->buffers[i]->Restore();
			if(SUCCEEDED(hr)) {
				lockResult = this->buffers[i]->Lock(0, a2.field_4, &bf, &bsize, NULL, 0, 0);
			} else {
				printf("Restore failed: %x\n", hr);
			}
		}
		
		if(FAILED(lockResult) || !bf){
			printf("Lock failed: %x\n", lockResult);
			this->buffers[i]->Release();
			this->buffers[i] = NULL;
			break;
		}
		
		memcpy(bf, a2.field_0, a2.field_4);
		
		hr = this->buffers[i]->Unlock(bf, bsize, NULL, 0);
		if(FAILED(hr)){
			printf("Unlock failed: %x\n", hr);
			this->buffers[i]->Release();
			this->buffers[i] = NULL;
			break;
		}
		
		LONG volume;
		if(a6 <= 0) {
			volume = DSBVOLUME_MIN;
		} else {
			float vol = a6 > 1.0f ? 1.0f : a6;
			volume = (LONG)(2000.0f * log10f(vol) + 0.5f);
			if(volume < DSBVOLUME_MIN) volume = DSBVOLUME_MIN;
			if(volume > DSBVOLUME_MAX) volume = DSBVOLUME_MAX;
		}
		this->buffers[i]->SetVolume(volume);
		
		hr = this->buffers[i]->Play(0, 0, 0);
		if(hr == DSERR_BUFFERLOST) {
			this->buffers[i]->Restore();
			lockResult = this->buffers[i]->Lock(0, a2.field_4, &bf, &bsize, NULL, 0, 0);
			if(SUCCEEDED(lockResult) && bf) {
				memcpy(bf, a2.field_0, a2.field_4);
				this->buffers[i]->Unlock(bf, bsize, NULL, 0);
			}
			hr = this->buffers[i]->Play(0, 0, 0);
		}
		
		if(FAILED(hr)){
			printf("Play failed: %x\n", hr);
			this->buffers[i]->Release();
			this->buffers[i] = NULL;
			break;
		}
		
		++this->playedCnt;
		break; 
	}
}
#endif
