#ifdef __WIN32__

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <windows.h>
#include <mmsystem.h>
#include <queue>
#include <map>

#pragma comment(lib, "winmm.lib")

#include <sound/SoundSystemDirectSound.hpp>
#include <sound/SoundDesc.hpp>

#define MAX_WAVE_OUT 8

struct WaveOutInstance {
	HWAVEOUT hWaveOut;
	WAVEHDR waveHdr;
	LPBYTE pData;
};

struct SoundKey {
	DWORD size, rate, channels, bits;
	bool operator<(const SoundKey& o) const {
		if(size != o.size) return size < o.size;
		if(rate != o.rate) return rate < o.rate;
		if(channels != o.channels) return channels < o.channels;
		return bits < o.bits;
	}
};

static WaveOutInstance inst[MAX_WAVE_OUT];
static std::queue<std::pair<SoundKey, LPBYTE>> q;
static std::map<SoundKey, int> cnt;
static HANDLE thread = NULL, event = NULL;
static volatile BOOL running = FALSE;
static CRITICAL_SECTION cs;

static void CALLBACK done(HWAVEOUT hwo, UINT msg, DWORD_PTR a, DWORD_PTR b, DWORD_PTR c) {
	if(msg == WOM_DONE) {
		for(int i = 0; i < MAX_WAVE_OUT; ++i) {
			if(inst[i].hWaveOut == hwo) {
				if(inst[i].pData) {
					auto it = cnt.find(inst[i].key);
					if(it != cnt.end() && --it->second <= 0) cnt.erase(it);
				}
				inst[i].hWaveOut = NULL;
				break;
			}
		}
	}
}

static DWORD WINAPI threadProc(LPVOID) {
	while(running) {
		WaitForSingleObject(event, INFINITE);
		while(!q.empty()) {
			EnterCriticalSection(&cs);
			auto req = q.front(); q.pop();
			LeaveCriticalSection(&cs);
			
			int slot = -1;
			for(int i = 0; i < MAX_WAVE_OUT; ++i) {
				if(inst[i].hWaveOut == NULL) { slot = i; break; }
			}
			if(slot == -1) { free(req.second); continue; }
			
			WAVEFORMATEX wf = {0};
			wf.wFormatTag = WAVE_FORMAT_PCM;
			wf.nSamplesPerSec = 44100;
			wf.wBitsPerSample = 16;
			wf.nChannels = 2;
			wf.nBlockAlign = 4;
			wf.nAvgBytesPerSec = 176400;
			
			if(waveOutOpen(&inst[slot].hWaveOut, WAVE_MAPPER, &wf, (DWORD_PTR)done, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
				free(req.second);
				inst[slot].hWaveOut = NULL;
				continue;
			}
			
			inst[slot].pData = req.second;
			inst[slot].key = req.first;
			ZeroMemory(&inst[slot].waveHdr, sizeof(WAVEHDR));
			inst[slot].waveHdr.lpData = (LPSTR)req.second;
			inst[slot].waveHdr.dwBufferLength = 4096;
			
			if(waveOutPrepareHeader(inst[slot].hWaveOut, &inst[slot].waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
				free(req.second);
				inst[slot].pData = NULL;
				waveOutClose(inst[slot].hWaveOut);
				inst[slot].hWaveOut = NULL;
				continue;
			}
			
			waveOutSetVolume(inst[slot].hWaveOut, 0xFFFF);
			
			if(waveOutWrite(inst[slot].hWaveOut, &inst[slot].waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
				waveOutUnprepareHeader(inst[slot].hWaveOut, &inst[slot].waveHdr, sizeof(WAVEHDR));
				free(req.second);
				inst[slot].pData = NULL;
				waveOutClose(inst[slot].hWaveOut);
				inst[slot].hWaveOut = NULL;
			}
		}
	}
	return 0;
}

SoundSystemDirectSound::SoundSystemDirectSound() {
	ZeroMemory(inst, sizeof(inst));
	InitializeCriticalSection(&cs);
	running = TRUE;
	event = CreateEvent(NULL, FALSE, FALSE, NULL);
	thread = CreateThread(NULL, 0, threadProc, NULL, 0, NULL);
}

SoundSystemDirectSound::~SoundSystemDirectSound() {
	running = FALSE;
	SetEvent(event);
	WaitForSingleObject(thread, 1000);
	CloseHandle(thread); CloseHandle(event);
	DeleteCriticalSection(&cs);
	for(int i = 0; i < MAX_WAVE_OUT; ++i) {
		if(inst[i].hWaveOut) {
			waveOutReset(inst[i].hWaveOut);
			if(inst[i].waveHdr.dwFlags & WHDR_PREPARED)
				waveOutUnprepareHeader(inst[i].hWaveOut, &inst[i].waveHdr, sizeof(WAVEHDR));
			if(inst[i].pData) free(inst[i].pData);
			waveOutClose(inst[i].hWaveOut);
		}
	}
	cnt.clear();
}

bool_t SoundSystemDirectSound::checkErr(uint32_t) { return 0; }
void SoundSystemDirectSound::destroy() {}
void SoundSystemDirectSound::init() {}
void SoundSystemDirectSound::removeStoppedSounds() {}
void SoundSystemDirectSound::setListenerPos(float,float,float) {}
void SoundSystemDirectSound::setListenerAngle(float) {}
void SoundSystemDirectSound::load(const std::string&) {}
void SoundSystemDirectSound::play(const std::string&) {}
void SoundSystemDirectSound::pause(const std::string&) {}
void SoundSystemDirectSound::stop(const std::string&) {}

static SoundKey makeKey(const SoundDesc& a) {
	return {a.field_4, a.sampleRate, a.channels, a.bytesPerSample};
}

void SoundSystemDirectSound::playAt(const SoundDesc& a, float, float, float, float vol, float) {
	SoundKey key = makeKey(a);
	
	EnterCriticalSection(&cs);
	if(cnt[key] >= 2) { LeaveCriticalSection(&cs); return; }
	
	LPBYTE data = (LPBYTE)malloc(a.field_4);
	if(!data) { LeaveCriticalSection(&cs); return; }
	memcpy(data, a.field_0, a.field_4);
	
	cnt[key]++;
	q.push({key, data});
	LeaveCriticalSection(&cs);
	SetEvent(event);
}
#endif
