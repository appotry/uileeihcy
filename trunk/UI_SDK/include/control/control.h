#pragma once

namespace UI
{

// 所有控件的基类
class UIAPI Control : public Object
{
public:
	Control(void);
	~Control(void);

	UI_DECLARE_OBJECT( Control, OBJ_CONTROL )

// 	UI_BEGIN_MSG_MAP
// 		UICHAIN_MSG_MAP(Object)
// 	UI_END_MSG_MAP

protected:

public:

	// Object 虚函数
public:
	virtual      bool        SetAttribute( map<String,String>& mapAttrib, bool bReload );
	virtual      SIZE        GetDesiredSize( HRDC hRDC ); // 内部调用GetAutoSize
	// 在没有指定控件的大小(width or height)的情况下，由自己计算自己所需要的大小，
	// 返回值不包括margin，margin的计算由GetDesiredSize完成，但padding则由子对象自己完成
	virtual      SIZE        GetAutoSize( HRDC hRDC ) = 0;

};


}