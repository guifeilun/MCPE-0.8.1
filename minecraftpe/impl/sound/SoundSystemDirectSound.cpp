#ifdef __WIN32__

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#pragma comment(lib, "winmm.lib")

#include <sound/SoundSystemDirectSound.hpp>
#include <sound/SoundDesc.hpp>
#include <sounddata.hpp>

#define MAX_WAVE_OUT 8

struct WaveOutInstance {
	HWAVEOUT hWaveOut;
	WAVEHDR waveHdr;
	LPBYTE pData;
	DWORD dataSize;
	BOOL playing;
	BOOL stopped;
};

static WaveOutInstance g_instances[MAX_WAVE_OUT];
static CRITICAL_SECTION g_cs;
static BOOL g_initialized = FALSE;

static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

SoundSystemDirectSound::SoundSystemDirectSound(void) {
	this->playedCnt = 0;
	
	if(!g_initialized) {
		for(int i = 0; i < MAX_WAVE_OUT; ++i) {
			ZeroMemory(&g_instances[i], sizeof(WaveOutInstance));
		}
		InitializeCriticalSection(&g_cs);
		g_initialized = TRUE;
	}
}

SoundSystemDirectSound::~SoundSystemDirectSound() {
	this->destroy();
}

bool_t SoundSystemDirectSound::checkErr(uint32_t a2) {
	return (a2 != 0) ? 1 : 0;
}

void SoundSystemDirectSound::destroy(void) {
	EnterCriticalSection(&g_cs);
	for(int i = 0; i < MAX_WAVE_OUT; ++i) {
		if(g_instances[i].hWaveOut) {
			waveOutReset(g_instances[i].hWaveOut);
			if(g_instances[i].waveHdr.dwFlags & WHDR_PREPARED) {
				waveOutUnprepareHeader(g_instances[i].hWaveOut, &g_instances[i].waveHdr, sizeof(WAVEHDR));
			}
			if(g_instances[i].pData) {
				free(g_instances[i].pData);
				g_instances[i].pData = NULL;
			}
			waveOutClose(g_instances[i].hWaveOut);
			g_instances[i].hWaveOut = NULL;
			g_instances[i].playing = FALSE;
			g_instances[i].stopped = FALSE;
		}
	}
	this->playedCnt = 0;
	LeaveCriticalSection(&g_cs);
	printf("WaveOut destroyed\n");
}

static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	if(uMsg == WOM_DONE) {
		for(int i = 0; i < MAX_WAVE_OUT; ++i) {
			if(g_instances[i].hWaveOut == hwo) {
				g_instances[i].playing = FALSE;
				g_instances[i].stopped = TRUE;
				break;
			}
		}
	}
}

void SoundSystemDirectSound::init(void) {
	int devCount = waveOutGetNumDevs();
	if(devCount == 0) {
		printf("No WaveOut devices available!\n");
		return;
	}
	printf("WaveOut initialized, %d device(s) available\n", devCount);
}

void SoundSystemDirectSound::removeStoppedSounds(void) {
	EnterCriticalSection(&g_cs);
	int count = 0;
	for(int i = 0; i < MAX_WAVE_OUT; ++i) {
		if(g_instances[i].hWaveOut && g_instances[i].stopped) {
			if(g_instances[i].waveHdr.dwFlags & WHDR_PREPARED) {
				waveOutUnprepareHeader(g_instances[i].hWaveOut, &g_instances[i].waveHdr, sizeof(WAVEHDR));
			}
			if(g_instances[i].pData) {
				free(g_instances[i].pData);
				g_instances[i].pData = NULL;
			}
			waveOutClose(g_instances[i].hWaveOut);
			g_instances[i].hWaveOut = NULL;
			g_instances[i].playing = FALSE;
			g_instances[i].stopped = FALSE;
		}
		if(g_instances[i].hWaveOut && g_instances[i].playing) {
			++count;
		}
	}
	this->playedCnt = count;
	LeaveCriticalSection(&g_cs);
}

void SoundSystemDirectSound::setListenerPos(float a, float b, float c) {}
void SoundSystemDirectSound::setListenerAngle(float a) {}
void SoundSystemDirectSound::load(const std::string&) {}
void SoundSystemDirectSound::play(const std::string&) {}
void SoundSystemDirectSound::pause(const std::string&) {}
void SoundSystemDirectSound::stop(const std::string&) {}

void SoundSystemDirectSound::playAt(const struct SoundDesc& a2, float a3, float a4, float a5, float a6, float a7) {
	this->removeStoppedSounds();
	
	EnterCriticalSection(&g_cs);
	
	int slot = -1;
	for(int i = 0; i < MAX_WAVE_OUT; ++i) {
		if(g_instances[i].hWaveOut == NULL || g_instances[i].stopped) {
			slot = i;
			break;
		}
	}
	
	if(slot == -1) {
		LeaveCriticalSection(&g_cs);
		return;
	}
	
	if(g_instances[slot].hWaveOut) {
		waveOutReset(g_instances[slot].hWaveOut);
		if(g_instances[slot].waveHdr.dwFlags & WHDR_PREPARED) {
			waveOutUnprepareHeader(g_instances[slot].hWaveOut, &g_instances[slot].waveHdr, sizeof(WAVEHDR));
		}
		if(g_instances[slot].pData) {
			free(g_instances[slot].pData);
			g_instances[slot].pData = NULL;
		}
		waveOutClose(g_instances[slot].hWaveOut);
		g_instances[slot].hWaveOut = NULL;
	}
	
	WAVEFORMATEX wf;
	ZeroMemory(&wf, sizeof(WAVEFORMATEX));
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nSamplesPerSec = a2.sampleRate;
	wf.wBitsPerSample = 8 * a2.bytesPerSample;
	wf.nChannels = a2.channels;
	wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	wf.cbSize = 0;
	
	MMRESULT result = waveOutOpen(&g_instances[slot].hWaveOut, WAVE_MAPPER, &wf, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);
	if(result != MMSYSERR_NOERROR) {
		printf("waveOutOpen failed: %d\n", result);
		g_instances[slot].hWaveOut = NULL;
		LeaveCriticalSection(&g_cs);
		return;
	}
	
	DWORD dataSize = a2.field_4;
	LPBYTE pData = (LPBYTE)malloc(dataSize);
	if(!pData) {
		printf("malloc failed\n");
		waveOutClose(g_instances[slot].hWaveOut);
		g_instances[slot].hWaveOut = NULL;
		LeaveCriticalSection(&g_cs);
		return;
	}
	memcpy(pData, a2.field_0, dataSize);
	g_instances[slot].pData = pData;
	g_instances[slot].dataSize = dataSize;
	
	ZeroMemory(&g_instances[slot].waveHdr, sizeof(WAVEHDR));
	g_instances[slot].waveHdr.lpData = (LPSTR)pData;
	g_instances[slot].waveHdr.dwBufferLength = dataSize;
	g_instances[slot].waveHdr.dwFlags = 0;
	g_instances[slot].waveHdr.dwLoops = 0;
	
	result = waveOutPrepareHeader(g_instances[slot].hWaveOut, &g_instances[slot].waveHdr, sizeof(WAVEHDR));
	if(result != MMSYSERR_NOERROR) {
		printf("waveOutPrepareHeader failed: %d\n", result);
		free(pData);
		g_instances[slot].pData = NULL;
		waveOutClose(g_instances[slot].hWaveOut);
		g_instances[slot].hWaveOut = NULL;
		LeaveCriticalSection(&g_cs);
		return;
	}
	
	if(a6 > 0) {
		DWORD vol = (DWORD)((a6 > 1.0f ? 1.0f : a6) * 0xFFFF);
		waveOutSetVolume(g_instances[slot].hWaveOut, MAKELONG(vol, vol));
	}
	
	result = waveOutWrite(g_instances[slot].hWaveOut, &g_instances[slot].waveHdr, sizeof(WAVEHDR));
	if(result != MMSYSERR_NOERROR) {
		printf("waveOutWrite failed: %d\n", result);
		waveOutUnprepareHeader(g_instances[slot].hWaveOut, &g_instances[slot].waveHdr, sizeof(WAVEHDR));
		free(pData);
		g_instances[slot].pData = NULL;
		waveOutClose(g_instances[slot].hWaveOut);
		g_instances[slot].hWaveOut = NULL;
		LeaveCriticalSection(&g_cs);
		return;
	}
	
	g_instances[slot].playing = TRUE;
	g_instances[slot].stopped = FALSE;
	++this->playedCnt;
	
	LeaveCriticalSection(&g_cs);
}
#endif
