#ifndef ITRAYICON_H_252A5BCF_41B2_4e57_97ED_EECF4E76BE4D
#define ITRAYICON_H_252A5BCF_41B2_4e57_97ED_EECF4E76BE4D

namespace UI
{
interface ITrayIcon
{
    virtual long  AddRef() = 0;
    virtual long  Release() = 0;

	virtual HRESULT  AddTrayIcon(HWND hWndNotify, HICON hIcon, BSTR bstrToolTip) = 0;
	virtual BOOL  nvProcessMessage(UIMSG* pMsg, UINT nMsgID, bool) = 0;
};
}

#endif // ITRAYICON_H_252A5BCF_41B2_4e57_97ED_EECF4E76BE4D