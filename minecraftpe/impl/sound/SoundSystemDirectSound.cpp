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

#define MAX_WAVE_OUT 16

struct WaveOutInstance {
	HWAVEOUT hWaveOut;
	WAVEHDR waveHdr;
	LPBYTE pData;
};

static WaveOutInstance g_instances[MAX_WAVE_OUT];
static BOOL g_initialized = FALSE;

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

SoundSystemDirectSound::SoundSystemDirectSound(void) {
	if(!g_initialized) {
		ZeroMemory(g_instances, sizeof(g_instances));
		g_initialized = TRUE;
	}
}

SoundSystemDirectSound::~SoundSystemDirectSound() {
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
	int slot = -1;
	for(int i = 0; i < MAX_WAVE_OUT; ++i) {
		if(g_instances[i].hWaveOut == NULL) {
			slot = i;
			break;
		}
	}
	if(slot == -1) return;
	
	WAVEFORMATEX wf = {0};
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nSamplesPerSec = a2.sampleRate;
	wf.wBitsPerSample = 8 * a2.bytesPerSample;
	wf.nChannels = a2.channels;
	wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	
	if(waveOutOpen(&g_instances[slot].hWaveOut, WAVE_MAPPER, &wf, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
		g_instances[slot].hWaveOut = NULL;
		return;
	}
	
	DWORD dataSize = a2.field_4;
	LPBYTE pData = (LPBYTE)malloc(dataSize);
	if(!pData) {
		waveOutClose(g_instances[slot].hWaveOut);
		g_instances[slot].hWaveOut = NULL;
		return;
	}
	memcpy(pData, a2.field_0, dataSize);
	g_instances[slot].pData = pData;
	
	ZeroMemory(&g_instances[slot].waveHdr, sizeof(WAVEHDR));
	g_instances[slot].waveHdr.lpData = (LPSTR)pData;
	g_instances[slot].waveHdr.dwBufferLength = dataSize;
	
	if(waveOutPrepareHeader(g_instances[slot].hWaveOut, &g_instances[slot].waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
		free(pData);
		g_instances[slot].pData = NULL;
		waveOutClose(g_instances[slot].hWaveOut);
		g_instances[slot].hWaveOut = NULL;
		return;
	}
	
	if(a6 > 0) {
		float vol = (a6 > 1.0f ? 1.0f : a6) * 0.35f;
		waveOutSetVolume(g_instances[slot].hWaveOut, MAKELONG((DWORD)(vol * 0xFFFF), (DWORD)(vol * 0xFFFF)));
	}
	
	if(waveOutWrite(g_instances[slot].hWaveOut, &g_instances[slot].waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
		waveOutUnprepareHeader(g_instances[slot].hWaveOut, &g_instances[slot].waveHdr, sizeof(WAVEHDR));
		free(pData);
		g_instances[slot].pData = NULL;
		waveOutClose(g_instances[slot].hWaveOut);
		g_instances[slot].hWaveOut = NULL;
	}
}
#endif
