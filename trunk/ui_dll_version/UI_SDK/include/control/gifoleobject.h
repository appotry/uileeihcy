#pragma 
#include "../util/GifImage.h"

//////////////////////////////////////////////////////////////////////////
//
//                      用于richedit中插入的ole对象
//
//////////////////////////////////////////////////////////////////////////


// http://www.cnblogs.com/wlwel/archive/2012/08/05/2623899.html
// 引用原文：根据我的调查（呃，通过实践，通过QueryInterface观察），我发现实现一个
// richedit中的动画控件只需要实现二个接口：IOleObject、IViewObject2，前者为了融入
// 到richedit环境中，后者为了渲染显示。由于richedit默认只喜好无窗口模式，所以针对
// IOleInPlaceSiteWindowless之类的，你去实现意义也不大，因为人家容器不认你，当然还
// 有IPersist系列接口，对于标准的环境有用（比如IDE），但这里并不是很需要，所以认清
// 核心问题能减少很多困惑。更显然的是我的控件没有用ATL框架，因为此控件脱离了richedit
// 环境生存的意义也不大，更有甚者我没必要让使其成为标准（也没可能），仅仅是为了在
// 一个系统中的richedit中更好地展示。实现的接口越少，引入的麻烦越少，这样才能使精力
// 集中在主要问题上。

//
// 更多的实现细节可以参考atl的源代码：CComControlBase  IOleObjectImpl
//
//

// {2EAE75F5-D78F-43ca-811D-8F8B01CCE05B}
static const GUID IID_IGifOleObject = { 0x2eae75f5, 0xd78f, 0x43ca, { 0x81, 0x1d, 0x8f, 0x8b, 0x1, 0xcc, 0xe0, 0x5b } };
class IGifOleObject
{
public:
	virtual HRESULT __stdcall LoadGif(const TCHAR* szPath) = 0;
	virtual HRESULT __stdcall Refresh() = 0;
};

class GifOleObject : public IGifOleObject, public RichEditOleObjectItem
{
public:
	GifOleObject();
	~GifOleObject();

// public:
// 	static  HRESULT CreateInstance(REFIID riid, void** ppv);

#pragma  region // igifoleobject
	virtual HRESULT __stdcall LoadGif(const TCHAR* szPath);
	virtual HRESULT __stdcall Refresh();
#pragma endregion

protected:
	GifImage*        m_pGif;
};
