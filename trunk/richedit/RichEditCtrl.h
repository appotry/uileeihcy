#pragma once

class RichEditCtrl : public CRichEditCtrl
{
public:
	RichEditCtrl(void);
	~RichEditCtrl(void);

public:
	// ����ӿ�
	bool    CreateControl(HWND hWndParent, CRect rect, UINT nCtrlID, bool bMultiLine=true);
	void    Destroy();


protected:
	// �ڲ�����
	void    InitRichEidtDll();
	void    ReleaseRichEidtDll();


protected:
	static HMODULE  s_RichEditDll;
	static LONG     s_refDll;
};
