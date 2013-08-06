// player_uip.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "player_sound.h"
#include "MP3.h"

CMP3 g_mp3;

bool  mp3_init(HWND hMainWnd)
{
	return g_mp3.Init(hMainWnd);
}

bool  mp3_play()
{
	return g_mp3.Play();
}
bool  mp3_pause()
{
	return g_mp3.Pause();
}
bool  mp3_stop()
{
	return g_mp3.Stop();
}
bool  mp3_set_file(const TCHAR* szPath)
{
	return g_mp3.RenderFile(szPath);
}

bool  mp3_release()
{
	return g_mp3.Release();
}

bool  mp3_add_event_callback(IMp3EventCallback* p)
{
	return g_mp3.AddEventCallback(p);
}

bool  mp3_set_cur_pos( double percent )
{
	return g_mp3.SetCurPos( percent );
}
bool  mp3_get_cur_pos(double* pdSeconds, double* pdPercent)
{
    return g_mp3.GetCurPos(pdSeconds, pdPercent);
}

bool  mp3_set_volumn( long lPercent )
{
	return g_mp3.SetVolume(lPercent);
}

bool  mp3_mute( bool bMute )
{
	return g_mp3.Mute(bMute);
}

bool  mp3_set_pan(long lPan)
{
	return g_mp3.SetPan(lPan);
}

bool  mp3_set_visualization(VisualizationInfo* pInfo)
{
	return g_mp3.SetVisualization(pInfo);
}

bool  mp3_enable_eq(bool bEnable)
{
	return g_mp3.EnableEq(bEnable);
}
bool  mp3_set_eq(E_EQ_FREQ eFreq, int nValue)
{
	return g_mp3.SetEq(eFreq, nValue);
}

bool  mp3_reset_eq()
{
	return g_mp3.ReSetEq();
}

HBITMAP mp3_get_visual_snapshot()
{	
	return g_mp3.GetVisualSnapshot();
}	

void    mp3_release_visual_snapshot()
{
	return g_mp3.ReleaseVisualSnapshot();
}


bool  mp3_get_tag(const TCHAR* szPath, ITagParseCallback* pCallback)
{
	return g_mp3.GetTag(szPath, pCallback);
}

const wchar_t*  mp3_parse_genre(int n)
{
	return g_mp3.ParseGenreID(n);
}