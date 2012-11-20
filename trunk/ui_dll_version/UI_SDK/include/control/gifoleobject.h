#pragma 
#include "../util/GifImage.h"

//////////////////////////////////////////////////////////////////////////
//
//                      ����richedit�в����ole����
//
//////////////////////////////////////////////////////////////////////////


// http://www.cnblogs.com/wlwel/archive/2012/08/05/2623899.html
// ����ԭ�ģ������ҵĵ��飨����ͨ��ʵ����ͨ��QueryInterface�۲죩���ҷ���ʵ��һ��
// richedit�еĶ����ؼ�ֻ��Ҫʵ�ֶ����ӿڣ�IOleObject��IViewObject2��ǰ��Ϊ������
// ��richedit�����У�����Ϊ����Ⱦ��ʾ������richeditĬ��ֻϲ���޴���ģʽ���������
// IOleInPlaceSiteWindowless֮��ģ���ȥʵ������Ҳ������Ϊ�˼����������㣬��Ȼ��
// ��IPersistϵ�нӿڣ����ڱ�׼�Ļ������ã�����IDE���������ﲢ���Ǻ���Ҫ����������
// ���������ܼ��ٺܶ����󡣸���Ȼ�����ҵĿؼ�û����ATL��ܣ���Ϊ�˿ؼ�������richedit
// �������������Ҳ���󣬸���������û��Ҫ��ʹ���Ϊ��׼��Ҳû���ܣ���������Ϊ����
// һ��ϵͳ�е�richedit�и��õ�չʾ��ʵ�ֵĽӿ�Խ�٣�������鷳Խ�٣���������ʹ����
// ��������Ҫ�����ϡ�

//
// �����ʵ��ϸ�ڿ��Բο�atl��Դ���룺CComControlBase  IOleObjectImpl
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
