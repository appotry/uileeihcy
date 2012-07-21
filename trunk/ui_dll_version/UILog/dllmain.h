// dllmain.h : 模块类的声明。

class CUILogModule : public CAtlDllModuleT< CUILogModule >
{
public :
	DECLARE_LIBID(LIBID_UILogLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_UILOG, "{3CE94CFC-7D7D-437B-A03C-C244A3EE210A}")
};

extern class CUILogModule _AtlModule;
