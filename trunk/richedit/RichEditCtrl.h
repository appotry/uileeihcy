#pragma once

class RichEditCtrl : public CRichEditCtrl
{
public:
	RichEditCtrl(void);
	~RichEditCtrl(void);

public:
	// 对外接口
	bool    CreateControl(HWND hWndParent, CRect rect, UINT nCtrlID, bool bMultiLine=true);
	void    Destroy();


protected:
	// 内部方法
	void    InitRichEidtDll();
	void    ReleaseRichEidtDll();


protected:
	static HMODULE  s_RichEditDll;
	static LONG     s_refDll;
};
