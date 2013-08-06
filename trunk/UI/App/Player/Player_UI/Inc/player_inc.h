#pragma once
#include "App/Player/Player_Sound/Player_Sound.h"

interface  IPlayerListItemInfo
{
    virtual ~IPlayerListItemInfo() = 0 {};
	virtual const TCHAR*  GetDisplayText() = 0;
	virtual const TCHAR*  GetFilePath() = 0;
	virtual const TCHAR*  GetFileName() = 0;
	virtual const TCHAR*  GetFileTime() = 0;

    virtual const TCHAR*  GetArtist() = 0;
    virtual const TCHAR*  GetTitle() = 0;
    virtual const TCHAR*  GetAlbum() = 0;
    virtual int  GetBitrate() = 0;
    virtual int  GetSample() = 0; 
    virtual int  GetDuration() = 0;
    virtual mp3_channel_mode  GetChannelMode() = 0;
};

interface IBuffer
{
    virtual void   Alloc(DWORD dwSize) = 0;
    virtual void   Free() = 0;
    virtual void   Release() = 0;
    virtual byte*  Get() = 0;
    virtual void   SetSize(int) = 0;
    virtual DWORD  GetSize() = 0;
    virtual DWORD  GetCapacity() = 0;
    virtual void   ReallocMore(DWORD newSize) = 0;
};
HRESULT  CreateBuffer(IBuffer** pp);