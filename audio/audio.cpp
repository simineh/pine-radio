// sound_route.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cstdio>
#include <boost\thread.hpp>
#include <boost\date_time\posix_time\posix_time_types.hpp>

class MyAudioSink
{
private:
	int counter;
public:
	MyAudioSink()
	{
		counter = 0;
	}

	HRESULT SetFormat(WAVEFORMATEX *pwfx)
	{
		return 0;
	}

	HRESULT CopyData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone)
	{
		printf("%i\n", numFramesAvailable);
		counter++;
		if (counter > 200) {
			return -1;
		}
		else
		{
			return 0;
		}
	}
};

class MyAudioSource
{
private:
	int counter;
public:
	MyAudioSource()
	{
		counter = 0;
	}

	HRESULT SetFormat(WAVEFORMATEX *pwfx)
	{
		return 0;
	}
	HRESULT LoadData(UINT32 bufferFrameCount, BYTE *pData, DWORD *flags)
	{
		FLOAT *intBuffer = (FLOAT *)pData;
		bool highLevel = false;
		for (int i = 0; i < bufferFrameCount; ++i)
		{
			int left = i * 2;
			int right = left + 1;

			if (highLevel)
			{
				intBuffer[left] = 0.5;
				intBuffer[right] = 0;
			}
			else
			{
				intBuffer[left] = 0;
				intBuffer[right] = 0.5;
			}
			//printf("%i\n", intBuffer[left]);
			if (i % 20 == 0)
			{
				highLevel = !highLevel;
			}
			
		}
		//bool highLevel = false;
		//int bufferPos = 0;
		//for (int i = 0; i < bufferFrameCount; ++i)
		//{
		//	for (int j = 0; j < 8; ++j)
		//	{
		//		if (highLevel)
		//		{
		//			pData[bufferPos] = 0xff;
		//		}
		//		else
		//		{
		//			pData[bufferPos] = 0x00;
		//		}
		//		printf("%i\n", (int)pData[bufferPos]);
		//		bufferPos++;
		//	}
		//	if (i % 10 == 0)
		//	{
		//		highLevel = !highLevel;
		//	}
		//}
		//printf("%i\n", bufferFrameCount);
		counter++;
		if (counter > 100) {
			return -1;
		}
		else
		{
			return 0;
		}
	}
};

//-----------------------------------------------------------
// Record an audio stream from the default audio capture
// device. The RecordAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data from the
// capture device. The main loop runs every 1/2 second.
//-----------------------------------------------------------

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

HRESULT GetDevice(LPWSTR deviceId, IMMDeviceEnumerator *pEnumerator, IMMDevice **ppDevice)
{
	HRESULT hr;
	IMMDeviceCollection *pDeviceCollection = NULL;
	UINT cDevices = 0;
	IMMDevice *pMyDevice = NULL;

	hr = pEnumerator->GetDevice(deviceId, ppDevice);
	EXIT_ON_ERROR(hr)

	//hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pDeviceCollection);
	//EXIT_ON_ERROR(hr)

	//hr = pDeviceCollection->GetCount(&cDevices);
	//EXIT_ON_ERROR(hr);

	//hr = pDeviceCollection->Item(0, ppDevice);
	//EXIT_ON_ERROR(hr)

	//for (int i = 0; i < cDevices; ++i)
	//{
	//	hr = pDeviceCollection->Item(i, &pMyDevice);
	//	EXIT_ON_ERROR(hr)

	//	//pMyDevice->

	//	SAFE_RELEASE(pMyDevice)
	//}

	//hr =  pEnumerator->GetDefaultAudioEndpoint(
 //                       eCapture, eConsole, ppDevice);
Exit:
	//SAFE_RELEASE(pDeviceCollection)
	//SAFE_RELEASE(pMyDevice)
	return hr;
}

struct InputStruct
{
	IMMDeviceEnumerator *pEnumerator;
    IMMDevice *pDevice;
    IAudioClient *pAudioClient;
    IAudioCaptureClient *pCaptureClient;
    WAVEFORMATEX *pwfx;
	UINT32 bufferFrameCount;

	InputStruct()
	{
		pEnumerator = NULL;
		pDevice = NULL;
		pAudioClient = NULL;
		pCaptureClient = NULL;
		pwfx = NULL;
	}

	~InputStruct()
	{
		CoTaskMemFree(pwfx);
		SAFE_RELEASE(pEnumerator)
		SAFE_RELEASE(pDevice)
		SAFE_RELEASE(pAudioClient)
		SAFE_RELEASE(pCaptureClient)
	}
};

struct OutputStruct
{
	IMMDeviceEnumerator *pEnumerator;
    IMMDevice *pDevice;
    IAudioClient *pAudioClient;
    IAudioRenderClient *pRenderClient;
    WAVEFORMATEX *pwfx;

	OutputStruct()
	{
		pEnumerator = NULL;
		pDevice = NULL;
		pAudioClient = NULL;
		pRenderClient = NULL;
		//pwfx = NULL;
	}

	~OutputStruct()
	{
		CoTaskMemFree(pwfx);
		SAFE_RELEASE(pEnumerator)
		SAFE_RELEASE(pDevice)
		SAFE_RELEASE(pAudioClient)
		SAFE_RELEASE(pRenderClient)
	}
};


InputStruct *OpenInputDevice(LPWSTR deviceId)
{
	InputStruct *inputStruct = new InputStruct();

	HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 numFramesAvailable;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData;
    DWORD flags;

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
		   (void**)&inputStruct->pEnumerator);

	hr = GetDevice(deviceId, inputStruct->pEnumerator, &inputStruct->pDevice);

    hr = inputStruct->pDevice->Activate(
                    IID_IAudioClient, CLSCTX_ALL,
                    NULL, (void**)&inputStruct->pAudioClient);

    hr = inputStruct->pAudioClient->GetMixFormat(&inputStruct->pwfx);

    hr = inputStruct->pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0,
                         hnsRequestedDuration,
                         0,
                         inputStruct->pwfx,
                         NULL);

    // Get the size of the allocated buffer.
	hr = inputStruct->pAudioClient->GetBufferSize(&inputStruct->bufferFrameCount);

    hr = inputStruct->pAudioClient->GetService(
                         IID_IAudioCaptureClient,
                         (void**)&inputStruct->pCaptureClient);
	return inputStruct;
}

OutputStruct *OpenOutputDevice(LPWSTR deviceId)
{
	OutputStruct *outputStruct = new OutputStruct();
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE *pData;
    DWORD flags = 0;

	hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&outputStruct->pEnumerator);

    //hr = outputStruct->pEnumerator->GetDefaultAudioEndpoint(
    //                    eRender, eConsole, &outputStruct->pDevice);
	hr = outputStruct->pEnumerator->GetDevice(deviceId, &outputStruct->pDevice);

    hr = outputStruct->pDevice->Activate(
                    IID_IAudioClient, CLSCTX_ALL,
                    NULL, (void**)&outputStruct->pAudioClient);

    hr = outputStruct->pAudioClient->GetMixFormat(&outputStruct->pwfx);

    hr = outputStruct->pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0,
                         hnsRequestedDuration,
                         0,
                         outputStruct->pwfx,
                         NULL);

    hr = outputStruct->pAudioClient->GetService(
                         IID_IAudioRenderClient,
                         (void**)&outputStruct->pRenderClient);

	return outputStruct;
}

/*HRESULT RecordAudioStream(MyAudioSink *pMySink)
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData;
    DWORD flags;

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

	hr = GetDevice(pEnumerator, &pDevice);
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate(
                    IID_IAudioClient, CLSCTX_ALL,
                    NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL);
    EXIT_ON_ERROR(hr)

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(
                         IID_IAudioCaptureClient,
                         (void**)&pCaptureClient);
    EXIT_ON_ERROR(hr)

    // Notify the audio sink which format to use.
    hr = pMySink->SetFormat(pwfx);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                     bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (bDone == FALSE)
    {
        // Sleep for half the buffer duration.
        Sleep(hnsActualDuration/REFTIMES_PER_MILLISEC/2);

        hr = pCaptureClient->GetNextPacketSize(&packetLength);
        EXIT_ON_ERROR(hr)

        while (packetLength != 0)
        {
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer(
                                   &pData,
                                   &numFramesAvailable,
                                   &flags, NULL, NULL);
            EXIT_ON_ERROR(hr)

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
            {
                pData = NULL;  // Tell CopyData to write silence.
            }

            // Copy the available capture data to the audio sink.
            hr = pMySink->CopyData(
                              pData, numFramesAvailable, &bDone);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            EXIT_ON_ERROR(hr)
        }
    }

    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr)

Exit:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pCaptureClient)

    return hr;
}

HRESULT PlayAudioStream(MyAudioSource *pMySource)
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioRenderClient *pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE *pData;
    DWORD flags = 0;

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->GetDefaultAudioEndpoint(
                        eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate(
                    IID_IAudioClient, CLSCTX_ALL,
                    NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL);
    EXIT_ON_ERROR(hr)

    // Tell the audio source which format to use.
    hr = pMySource->SetFormat(pwfx);
    EXIT_ON_ERROR(hr)

    // Get the actual size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(
                         IID_IAudioRenderClient,
                         (void**)&pRenderClient);
    EXIT_ON_ERROR(hr)

    // Grab the entire buffer for the initial fill operation.
    hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
    EXIT_ON_ERROR(hr)

    // Load the initial data into the shared buffer.
    hr = pMySource->LoadData(bufferFrameCount, pData, &flags);
    EXIT_ON_ERROR(hr)

    hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                        bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start playing.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        // Sleep for half the buffer duration.
        Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

        // See how much buffer space is available.
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        EXIT_ON_ERROR(hr)

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
        EXIT_ON_ERROR(hr)

        // Get next 1/2-second of data from the audio source.
        hr = pMySource->LoadData(numFramesAvailable, pData, &flags);
        EXIT_ON_ERROR(hr)

        hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
        EXIT_ON_ERROR(hr)
    }

    // Wait for last data in buffer to play before stopping.
    Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

    hr = pAudioClient->Stop();  // Stop playing.
    EXIT_ON_ERROR(hr)

Exit:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pRenderClient)

    return hr;
}*/

void playSound(InputStruct *input, OutputStruct *output)
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 numFramesAvailable;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData;
	BYTE *pOutputData;
    DWORD flags = 0;

	// Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                     input->bufferFrameCount / input->pwfx->nSamplesPerSec / 100;

    hr = input->pAudioClient->Start();  // Start recording.
	hr = output->pAudioClient->Start();

	MyAudioSource source;

    // Each loop fills about half of the shared buffer.
    while (bDone == FALSE)
    {
        // Sleep for half the buffer duration.
		boost::this_thread::sleep(boost::posix_time::millisec(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

        hr = input->pCaptureClient->GetNextPacketSize(&packetLength);

        while (packetLength != 0)
        {
			// Sleep for half the buffer duration.
			Sleep(hnsActualDuration/REFTIMES_PER_MILLISEC/2);

            // Get the available data in the shared buffer.
            hr = input->pCaptureClient->GetBuffer(
                                   &pData,
                                   &numFramesAvailable,
                                   &flags, NULL, NULL);

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
            {
                pData = NULL;  // Tell CopyData to write silence.
            }

			hr = output->pRenderClient->GetBuffer(numFramesAvailable, &pOutputData);

			//memcpy(pOutputData, pData, numFramesAvailable * input->pwfx->nChannels * input->pwfx->wBitsPerSample);
			FLOAT *inBuf = (FLOAT *)pData;
			FLOAT *outBuf = (FLOAT *)pOutputData;

			for (int i = 0; i < numFramesAvailable; ++i)
			{
				outBuf[i * 2] = inBuf[i * 2]*1;
				outBuf[i * 2 + 1] = inBuf[i * 2 + 1]*1;
			}

			//source.LoadData(numFramesAvailable, pOutputData, &flags);

			hr = output->pRenderClient->ReleaseBuffer(numFramesAvailable, 0);

            //// Copy the available capture data to the audio sink.
            //hr = pMySink->CopyData(
            //                  pData, numFramesAvailable, &bDone);

            hr = input->pCaptureClient->ReleaseBuffer(numFramesAvailable);

            hr = input->pCaptureClient->GetNextPacketSize(&packetLength);
        }
    }

    hr = input->pAudioClient->Stop();  // Stop recording.
}

struct AudioDevice
{
	LPWSTR id;
	LPWSTR name;

	AudioDevice(LPWSTR deviceId, LPWSTR deviceName)
	{
		id = copyToNew(deviceId);
		name = copyToNew(deviceName);
	}

	LPWSTR copyToNew(LPWSTR str)
	{
		size_t strLength = wcslen(str) + 1;
		LPWSTR copyOfStr = new WCHAR[strLength];
		wcscpy_s(copyOfStr, strLength, str);
		return copyOfStr;
	}

	~AudioDevice()
	{
		delete id;
		delete name;
	}
};

//struct AudioDevices
//{
//	int devicesCount;
//	AudioDevice **audioDevices;
//
//	AudioDevices(UINT count)
//	{
//		devicesCount = count;
//		audioDevices = new AudioDevice *[count];
//	}
//
//	~AudioDevices()
//	{
//		delete[] audioDevices;
//	}
//};

extern "C" __declspec(dllexport) void GetAudioDevices(EDataFlow dataFlow, AudioDevice **&audioDevices, int &audioDevicesCount)
{
    HRESULT hr = S_OK;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDeviceCollection *pCollection = NULL;
    IMMDevice *pEndpoint = NULL;
    IPropertyStore *pProps = NULL;
    LPWSTR pwszID = NULL;

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->EnumAudioEndpoints(
                        dataFlow, DEVICE_STATE_ACTIVE,
                        &pCollection);
    EXIT_ON_ERROR(hr)

    UINT  count;
    hr = pCollection->GetCount(&count);
    EXIT_ON_ERROR(hr)

	//AudioDevices *audioDevices = new AudioDevices(count);
	audioDevicesCount = count;
	audioDevices = new AudioDevice *[count];

    // Each loop prints the name of an endpoint device.
    for (ULONG i = 0; i < count; i++)
    {
        // Get pointer to endpoint number i.
        hr = pCollection->Item(i, &pEndpoint);
        EXIT_ON_ERROR(hr)

        // Get the endpoint ID string.
        hr = pEndpoint->GetId(&pwszID);
        EXIT_ON_ERROR(hr)
        
        hr = pEndpoint->OpenPropertyStore(
                          STGM_READ, &pProps);
        EXIT_ON_ERROR(hr)

        PROPVARIANT varName;
        // Initialize container for property value.
        PropVariantInit(&varName);

        // Get the endpoint's friendly-name property.
        hr = pProps->GetValue(
                       PKEY_Device_FriendlyName, &varName);
        EXIT_ON_ERROR(hr)

		audioDevices[i] = new AudioDevice(pwszID, varName.pwszVal);

        CoTaskMemFree(pwszID);
        pwszID = NULL;
        PropVariantClear(&varName);
        SAFE_RELEASE(pProps)
        SAFE_RELEASE(pEndpoint)
    }
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pCollection)

Exit:
    CoTaskMemFree(pwszID);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pCollection)
    SAFE_RELEASE(pEndpoint)
    SAFE_RELEASE(pProps)
}

extern "C" __declspec(dllexport) void DisposeAudioDevices(AudioDevice **audioDevices, int audioDevicesCount)
{
	if (audioDevices == NULL)
	{
		return;
	}

	for (int i = 0; i < audioDevicesCount; ++i)
	{
		delete audioDevices[i];
	}
	delete audioDevices;
}

struct callable
{
	InputStruct *input;
	OutputStruct *output;

    void operator()()
	{
		while (true)
		{
			playSound(input, output);
		}
	}

	callable(InputStruct *input, OutputStruct *output) : input(input), output(output)
	{
	}
};

struct SoundSession
{
	boost::thread *soundThread;
	InputStruct *input;
	OutputStruct *output;

	SoundSession(LPWSTR inputId, LPWSTR outputId)
	{
		input = OpenInputDevice(inputId);
		output = OpenOutputDevice(outputId);

		callable soundProc(input, output);
		soundThread = new boost::thread(soundProc);
	}

	~SoundSession()
	{
		soundThread->interrupt();
		soundThread->join();
		delete soundThread;

		delete input;
		delete output;
	}
};

extern "C" __declspec(dllexport) void *StartSound(LPWSTR inputId, LPWSTR outputId)
{
	return new SoundSession(inputId, outputId);
}

extern "C" __declspec(dllexport) void StopSound(void *startedSound)
{
	delete ((SoundSession *) startedSound);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//CoInitialize(NULL);

	//AudioDevices *inputAudioDevices = GetAudioDevices(eCapture);
	//AudioDevices *outputAudioDevices = GetAudioDevices(eRender);

	//LPWSTR inputId = inputAudioDevices->audioDevices[2]->id;
	//LPWSTR outputId = outputAudioDevices->audioDevices[0]->id;

	//void *soundSession = StartSound(inputId, outputId);
	//for (int i = 10; i >= 0; --i)
	//{
	//	printf("%i\n", i);
	//	Sleep(1000);
	//}
	//StopSound(soundSession);

	//DisposeAudioDevices(inputAudioDevices);
	//DisposeAudioDevices(outputAudioDevices);

	//CoUninitialize();
	return 0;
}

struct test_struct
{
	LPWSTR id;
	LPWSTR description;
};

//extern "C" __declspec(dllexport) void GetAudioDevices(EDataFlow dataFlow, AudioDevice **&ts, int &size)
//{
//	AudioDevices *inputAudioDevices = GetAudioDevices(dataFlow);
//	size = inputAudioDevices->devicesCount;
//	ts = inputAudioDevices->audioDevices;
//}
