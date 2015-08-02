#include "MoonSoundManager.h"
#include <string.h>

MoonSoundManager::MoonSoundManager(void)
{
}

MoonSoundManager::~MoonSoundManager(void)
{
	Cleanup();
}

bool MoonSoundManager::Initialize()
{
	srand(timeGetTime());
	CoInitialize(NULL);
	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
									IID_IGraphBuilder, (void**)&_pGraph);
	_pGraph->QueryInterface(IID_IMediaControl, (void**)&_pMediaControl);
	_pGraph->QueryInterface(IID_IMediaPosition, (void**)&_pMediaPosition);
	_pSourceCurrent = NULL;
	_pSourceNext = NULL;
	
	char ch[MAX_PATH];
	GetPrivateProfileString("backgroundmusic","count","0",ch,MAX_PATH,"../mp3/music.cfg");
	_nCount = CharToInt(ch);
	_pMusicList = new char*[_nCount];
	char cNum[5] = {'\0','\0','\0','\0','\0'};
	string strNum;
	for(int i = 0; i<_nCount; i++)
	{
		sprintf_s(cNum,"%d",rand()%_nCount);
		strNum = cNum;
		GetPrivateProfileString("backgroundmusic",strNum.c_str(),NULL,ch,MAX_PATH,"../mp3/music.cfg");
		_pMusicList[i] = new char[MAX_PATH];
		strcpy(_pMusicList[i],ch);
	}

	return true;
}

void MoonSoundManager::Cleanup()
{
	StopMp3();
	SAFE_RELEASE(_pSourceCurrent);
	SAFE_RELEASE(_pSourceNext);
	SAFE_RELEASE(_pMediaControl);
	SAFE_RELEASE(_pMediaPosition);
	SAFE_RELEASE(_pGraph);
	CoUninitialize();
}

//bool MoonSoundManager::LoadMp3(const char* strSoundPath)
//{
//	WCHAR wstrSoundPath[MAX_PATH];
//	MultiByteToWideChar(CP_ACP,0,strSoundPath,-1,wstrSoundPath,MAX_PATH);
//	_pGraph->AddSourceFilter(wstrSoundPath,wstrSoundPath,&_pSourceCurrent);
//	return true;
//}

bool MoonSoundManager::IsPlayingMp3()
{
	REFTIME refPosition;
	REFTIME refDuration;
	_pMediaPosition->get_CurrentPosition(&refPosition);
	_pMediaPosition->get_Duration(&refDuration);
	if(refPosition < refDuration)
		return true;
	else
		return false;
}

void MoonSoundManager::PlayMp3()
{
	_pMediaPosition->put_CurrentPosition(0);
	_pMediaControl->Run();
}

void MoonSoundManager::StopMp3()
{
	_pMediaControl->Stop();
}

void MoonSoundManager::SwapBackgroundMusic()
{
	if(!IsPlayingMp3())
	{
		//char ch[MAX_PATH];
		string str;

		str = "../mp3/";
		str += _pMusicList[rand()%_nCount];
		WCHAR wstrSoundPath[MAX_PATH];
		MultiByteToWideChar(CP_ACP,0,str.c_str(),-1,wstrSoundPath,MAX_PATH);

		HRESULT hr;
		IPin *pPin = NULL;

		// OPTIMIZATION OPPORTUNITY
		// This will open the file, which is expensive. To optimize, this
		// should be done earlier, ideally as soon as we knew this was the
		// next file to ensure that the file load doesn't add to the
		// filter swapping time & cause a hiccup.
		//
		// Add the new source filter to the graph. (Graph can still be running)
		hr = _pGraph->AddSourceFilter(wstrSoundPath, wstrSoundPath, &_pSourceNext);

		// Get the first output pin of the new source filter. Audio sources 
		// typically have only one output pin, so for most audio cases finding 
		// any output pin is sufficient.
		if(SUCCEEDED(hr))
		{
			hr = _pSourceNext->FindPin(L"Output", &pPin);  
		}

		// Stop the graph
		if(SUCCEEDED(hr))
		{
			hr = _pMediaControl->Stop();
		}

		// Break all connections on the filters. You can do this by adding 
		// and removing each filter in the graph
		if(SUCCEEDED(hr))
		{
			IEnumFilters *pFilterEnum = NULL;

			if(SUCCEEDED(hr = _pGraph->EnumFilters(&pFilterEnum)))
			{
				int iFiltCount = 0;
				int iPos = 0;

				// Need to know how many filters. If we add/remove filters during the
				// enumeration we'll invalidate the enumerator
				while(S_OK == pFilterEnum->Skip(1))
				{
					iFiltCount++;
				}

				// Allocate space, then pull out all of the 
				IBaseFilter **ppFilters = reinterpret_cast<IBaseFilter **>
										  (_alloca(sizeof(IBaseFilter *) * iFiltCount));
				pFilterEnum->Reset();

				while(S_OK == pFilterEnum->Next(1, &(ppFilters[iPos++]), NULL));

				SAFE_RELEASE(pFilterEnum);

				for(iPos = 0; iPos < iFiltCount; iPos++)
				{
					_pGraph->RemoveFilter(ppFilters[iPos]);

					// Put the filter back, unless it is the old source
					if(ppFilters[iPos] != _pSourceCurrent)
					{
						_pGraph->AddFilter(ppFilters[iPos], NULL);
					}
					SAFE_RELEASE(ppFilters[iPos]);
				}
			}
		}

		// We have the new output pin. Render it
		if(SUCCEEDED(hr))
		{
			// Release the old source filter, if it exists
			SAFE_RELEASE(_pSourceCurrent)

			hr = _pGraph->Render(pPin);
			_pSourceCurrent = _pSourceNext;
			_pSourceNext = NULL;
		}

		SAFE_RELEASE(pPin);
		//SAFE_RELEASE(g_pSourceNext); // In case of errors

		PlayMp3();
	}
}
