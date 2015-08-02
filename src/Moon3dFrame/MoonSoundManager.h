#pragma once
#include "MoonUtility.h"

class MoonSoundManager
{
	IGraphBuilder* _pGraph;
	IMediaControl* _pMediaControl;
	IMediaPosition* _pMediaPosition;
	IBaseFilter* _pSourceCurrent;
	IBaseFilter* _pSourceNext;
	int _nCount;
	char **_pMusicList;
public:
	MoonSoundManager(void);
	~MoonSoundManager(void);
	bool Initialize(void);
	void Cleanup();
	//bool LoadMp3(const char* strSoundPath);
	bool IsPlayingMp3();
	void PlayMp3();
	void StopMp3();
	void SwapBackgroundMusic();
};
