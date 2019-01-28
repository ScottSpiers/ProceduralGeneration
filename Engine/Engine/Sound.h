#pragma once


#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")


#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

class Sound
{
private:
	struct WaveHeaderType
	{
		char chunkID[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkID[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkID[4];
		unsigned long dataSize;
	};

public:
	Sound();
	Sound(const Sound&);
	~Sound();

	bool Initialise(HWND);
	void Shutdown();

private:
	bool InitialiseDirectSound(HWND);
	bool LoadWaveFile(char*, IDirectSoundBuffer8**);
	bool PlayWaveFile();

private:
	IDirectSound8* m_sound;
	IDirectSoundBuffer* m_primaryBuffer;
	IDirectSoundBuffer8* m_secondaryBuffer;
};