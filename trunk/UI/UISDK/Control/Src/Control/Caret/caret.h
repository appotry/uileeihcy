#pragma once
#include "icaret.h"

//////////////////////////////////////////////////////////////////////////
// ���ⲿ�ṩһ�׹��������������Ҫ�ڷֲ㴰��������ʾһ�����ʱ��
// ������һ��CCaretWindow������Ҫ����ͨ����������ʾһ�����ʱ����
// ����ϵͳ��API
// 
//  2016.6.17 11:08
//
namespace UI
{

class CCaret
{
public:
	CCaret();
    ~CCaret();

	BOOL  CreateCaret(IObject* pObj, HBITMAP hbmp, int nWidth, int nHeight, CARET_TYPE eType = CARET_TYPE_UNKNOWN);
	void  DestroyCaret();
	void  SetCaretPos(int x, int y, bool bRedraw=true);
	void  ShowCaret(bool bRedraw=true);
	void  HideCaret(bool bRedraw=true);
    void  OnControlPaint(IRenderTarget* p);
//     void  PreCommitWindowBuffer(HDC hMemDC);
//     void  PostCommitWindowBuffer(HDC hMemDC);

    CARET_TYPE  GetCaretType();

public:
    ICaret*  m_pCaret;
    POINT  m_ptLast;
};

}