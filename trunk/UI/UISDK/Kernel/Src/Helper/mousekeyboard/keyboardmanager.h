#pragma once

namespace UI
{
class Object;
class WindowBase;
class KeyboardMgrBase
{
public:
    KeyboardMgrBase();
    

};

//
//	1. ���Ҫ����Ĭ�ϵ�һ���ؼ��Ľ��㣬������OnInitDialog����FALSE��
//
class WindowKeyboardMgr : public KeyboardMgrBase
{
public:
	WindowKeyboardMgr( );
	~WindowKeyboardMgr(void);

public:
	LRESULT  HandleMessage( UINT msg, WPARAM w, LPARAM l );
	

private:
	

private:
};

}