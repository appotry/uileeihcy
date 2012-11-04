#pragma once

class CLoginDlg : public CustomWindow
{
public:
	CLoginDlg(void);
	~CLoginDlg(void);

	UI_BEGIN_MSG_MAP
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

};
