#ifdef __WIN32__

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <queue>

#pragma comment(lib, "winmm.lib")

#include <sound/SoundSystemDirectSound.hpp>
#include <sound/SoundDesc.hpp>
#include <sounddata.hpp>

#define MAX_WAVE_OUT 4

struct WaveOutInstance {
	HWAVEOUT hWaveOut;
	WAVEHDR waveHdr;
	LPBYTE pData;
};

struct SoundRequest {
	LPBYTE pData;
	DWORD dataSize;
	WAVEFORMATEX wf;
	float volume;
};

static WaveOutInstance g_instances[MAX_WAVE_OUT];
static std::queue<SoundRequest> g_requestQueue;
static HANDLE g_hThread = NULL;
static HANDLE g_hEvent = NULL;
static volatile BOOL g_running = FALSE;
static CRITICAL_SECTION g_cs;

static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	if(uMsg == WOM_DONE) {
		for(int i = 0; i < MAX_WAVE_OUT; ++i) {
			if(g_instances[i].hWaveOut == hwo) {
				g_instances[i].hWaveOut = NULL;
				break;
			}
		}
	}
}

static DWORD WINAPI SoundThreadProc(LPVOID lpParam) {
	while(g_running) {
		WaitForSingleObject(g_hEvent, INFINITE);
		
		while(!g_requestQueue.empty()) {
			EnterCriticalSection(&g_cs);
			SoundRequest req = g_requestQueue.front();
			g_requestQueue.pop();
			LeaveCriticalSection(&g_cs);
			
			int slot = -1;
			for(int i = 0; i < MAX_WAVE_OUT; ++i) {
				if(g_instances[i].hWaveOut == NULL) {
					slot = i;
					break;
				}
			}
			if(slot == -1) {
				free(req.pData);
				continue;
			}
			
			MMRESULT result = waveOutOpen(&g_instances[slot].hWaveOut, WAVE_MAPPER, &req.wf, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);
			if(result != MMSYSERR_NOERROR) {
				free(req.pData);
				g_instances[slot].hWaveOut = NULL;
				continue;
			}
			
			g_instances[slot].pData = req.pData;
			ZeroMemory(&g_instances[slot].waveHdr, sizeof(WAVEHDR));
			g_instances[slot].waveHdr.lpData = (LPSTR)req.pData;
			g_instances[slot].waveHdr.dwBufferLength = req.dataSize;
			
			if(waveOutPrepareHeader(g_instances[slot].hWaveOut, &g_instances[slot].waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
				free(req.pData);
				g_instances[slot].pData = NULL;
				waveOutClose(g_instances[slot].hWaveOut);
				g_instances[slot].hWaveOut = NULL;
				continue;
			}
			
			if(req.volume > 0) {
				float vol = req.volume > 1.0f ? 1.0f : req.volume;
				waveOutSetVolume(g_instances[slot].hWaveOut, MAKELONG((DWORD)(vol * 0xFFFF), (DWORD)(vol * 0xFFFF)));
			}
			
			if(waveOutWrite(g_instances[slot].hWaveOut, &g_instances[slot].waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
				waveOutUnprepareHeader(g_instances[slot].hWaveOut, &g_instances[slot].waveHdr, sizeof(WAVEHDR));
				free(req.pData);
				g_instances[slot].pData = NULL;
				waveOutClose(g_instances[slot].hWaveOut);
				g_instances[slot].hWaveOut = NULL;
			}
		}
	}
	return 0;
}

SoundSystemDirectSound::SoundSystemDirectSound(void) {
	ZeroMemory(g_instances, sizeof(g_instances));
	InitializeCriticalSection(&g_cs);
	
	g_running = TRUE;
	g_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_hThread = CreateThread(NULL, 0, SoundThreadProc, NULL, 0, NULL);
}

SoundSystemDirectSound::~SoundSystemDirectSound() {
	g_running = FALSE;
	SetEvent(g_hEvent);
	WaitForSingleObject(g_hThread, 1000);
	CloseHandle(g_hThread);
	CloseHandle(g_hEvent);
	DeleteCriticalSection(&g_cs);
	
	for(int i = 0; i < MAX_WAVE_OUT; ++i) {
		if(g_instances[i].hWaveOut) {
			waveOutReset(g_instances[i].hWaveOut);
			if(g_instances[i].waveHdr.dwFlags & WHDR_PREPARED) {
				waveOutUnprepareHeader(g_instances[i].hWaveOut, &g_instances[i].waveHdr, sizeof(WAVEHDR));
			}
			if(g_instances[i].pData) free(g_instances[i].pData);
			waveOutClose(g_instances[i].hWaveOut);
		}
	}
}

bool_t SoundSystemDirectSound::checkErr(uint32_t a2) { return a2 ? 1 : 0; }
void SoundSystemDirectSound::destroy(void) {}
void SoundSystemDirectSound::init(void) {}
void SoundSystemDirectSound::removeStoppedSounds(void) {}
void SoundSystemDirectSound::setListenerPos(float a, float b, float c) {}
void SoundSystemDirectSound::setListenerAngle(float a) {}
void SoundSystemDirectSound::load(const std::string&) {}
void SoundSystemDirectSound::play(const std::string&) {}
void SoundSystemDirectSound::pause(const std::string&) {}
void SoundSystemDirectSound::stop(const std::string&) {}

void SoundSystemDirectSound::playAt(const struct SoundDesc& a2, float a3, float a4, float a5, float a6, float a7) {
	EnterCriticalSection(&g_cs);
	if(g_requestQueue.size() >= 4) {
		LeaveCriticalSection(&g_cs);
		return;
	}
	LeaveCriticalSection(&g_cs);
	
	DWORD dataSize = a2.field_4;
	LPBYTE pData = (LPBYTE)malloc(dataSize);
	if(!pData) return;
	memcpy(pData, a2.field_0, dataSize);
	
	SoundRequest req;
	req.pData = pData;
	req.dataSize = dataSize;
	req.volume = a6;
	
	ZeroMemory(&req.wf, sizeof(WAVEFORMATEX));
	req.wf.wFormatTag = WAVE_FORMAT_PCM;
	req.wf.nSamplesPerSec = a2.sampleRate;
	req.wf.wBitsPerSample = 8 * a2.bytesPerSample;
	req.wf.nChannels = a2.channels;
	req.wf.nBlockAlign = req.wf.nChannels * (req.wf.wBitsPerSample / 8);
	req.wf.nAvgBytesPerSec = req.wf.nSamplesPerSec * req.wf.nBlockAlign;
	
	EnterCriticalSection(&g_cs);
	g_requestQueue.push(req);
	LeaveCriticalSection(&g_cs);
	
	SetEvent(g_hEvent);
}
#endif
