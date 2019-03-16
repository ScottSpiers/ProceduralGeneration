
#include "Sound.h"

Sound::Sound()
{
	m_sound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer = 0;
}

Sound::Sound(const Sound&)
{
}

Sound::~Sound()
{
}

bool Sound::Initialise(HWND hwnd)
{
	if (!InitialiseDirectSound(hwnd))
		return false;

	if (!LoadWaveFile("../Engine/data/Vivaldi_FourSeasons_Winter.wav", &m_secondaryBuffer))
		return false;

	if (!PlayWaveFile())
		return false;

	return true;
}

void Sound::Shutdown()
{
	if (m_secondaryBuffer)
	{
		m_secondaryBuffer->Release();
		m_secondaryBuffer = 0;
	}

	if (m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	if (m_sound)
	{
		m_sound->Release();
		m_sound = 0;
	}
}

bool Sound::InitialiseDirectSound(HWND hwnd)
{
	HRESULT res;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	res = DirectSoundCreate8(NULL, &m_sound, NULL);
	if (FAILED(res))
		return false;

	res = m_sound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	res = m_sound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(res))
		return false;

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 48000;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	res = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(res))
		return false;

	return true;
}

bool Sound::LoadWaveFile(char* filename, IDirectSoundBuffer8** sndBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT res;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
		return false;

	// Read in the wave file header.
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
		return false;

	// Check that the chunk ID is the RIFF format.
	if ((waveFileHeader.chunkID[0] != 'R') || (waveFileHeader.chunkID[1] != 'I') ||
		(waveFileHeader.chunkID[2] != 'F') || (waveFileHeader.chunkID[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if ((waveFileHeader.subChunkID[0] != 'f') || (waveFileHeader.subChunkID[1] != 'm') ||
		(waveFileHeader.subChunkID[2] != 't') || (waveFileHeader.subChunkID[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in stereo format.
	if (waveFileHeader.numChannels != 2)
		return false;

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if (waveFileHeader.sampleRate != 48000)
		return false;

	// Ensure that the wave file was recorded in 16 bit format.
	if (waveFileHeader.bitsPerSample != 16)
		return false;

	// Check for the data chunk header.
	if ((waveFileHeader.dataChunkID[0] != 'd') || (waveFileHeader.dataChunkID[1] != 'a') ||
		(waveFileHeader.dataChunkID[2] != 't') || (waveFileHeader.dataChunkID[3] != 'a'))
	{
		return false;
	}

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 48000;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;


	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	res = m_sound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(res))
		return false;

	res = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*sndBuffer);
	if (FAILED(res))
		return false;

	tempBuffer->Release();
	tempBuffer = 0;

	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
		return false;

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
		return false;

	// Close the file once done reading.
	error = fclose(filePtr);
	if (error != 0)
		return false;

	// Lock the secondary buffer to write wave data into it.
	res = (*sndBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(res))
		return false;

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	res = (*sndBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(res))
		return false;

	// Release the wave data since it was copied into the secondary buffer.
	delete[] waveData;
	waveData = 0;

	return true;
}

bool Sound::PlayWaveFile()
{
	HRESULT res;

	res = m_secondaryBuffer->SetCurrentPosition(0);
	if (FAILED(res))
		return false;

	res = m_secondaryBuffer->SetVolume(DSBVOLUME_MIN);
	if (FAILED(res))
		return false;

	res = m_secondaryBuffer->Play(0, 0, 0);
	if (FAILED(res))
		return false;

	return true;
}