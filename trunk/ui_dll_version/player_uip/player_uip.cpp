// player_uip.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "player_uip.h"
#include "MP3.h"

CMP3 g_mp3;

bool  mp3_init()
{
	return g_mp3.Init();
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
bool  mp3_set_file(const String& strPath)
{
	return g_mp3.RenderFile(strPath);
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

bool  mp3_set_volumn( double percent )
{
	return g_mp3.SetVolume(percent);
}

bool  mp3_mute( bool bMute )
{
	return g_mp3.Mute(bMute);
}