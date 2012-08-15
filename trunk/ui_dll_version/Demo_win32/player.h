#pragma once

class IMgr
{
public:
	virtual  bool  Initialize() = 0;
	virtual  bool  Release() = 0;
};