#pragma once
#include "UISDK\Kernel\Src\XmlParse\uiresource.h"

namespace UI
{
    class Object;

	// ����
    class IParseBase : IRootInterface
	{
	protected:
		String   m_strDataSource;    // ����Դ����xml�ļ�·��
		IUIApplication*  m_pUIApp;   // UIʵ��

	public:
		void  SetUIApplicationPtr(IUIApplication* p) { m_pUIApp = p; }
		void  SetDataSource(const String& strDataSource) { m_strDataSource = strDataSource; }
		void  Release(){ delete this; }
		const String& GetDataSourceStringRef() { return m_strDataSource; }
	};

#if 0
	class IImageParse : public IParseBase
	{
	protected:
		virtual ~IImageParse() {};

	public:
		// ���������ļ�
		virtual  bool Create() = 0;
		// �����ļ�
		virtual  bool Save(ImageRes*  pImageInfo) = 0;
		// ����
		virtual  bool Load(ImageRes*  pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo) = 0;
		// ��temp�ļ������¼���
		virtual  bool Reload(ImageRes* pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo) = 0;
		// ����һ��ͼƬ
		virtual  bool InsertImage(ImageResItem *pImageItemInfo) = 0;
		// �޸�һ��ͼƬ
		virtual  bool ModifyImage(ImageResItem *pImageItemInfo) = 0;
		// ɾ��һ��ͼƬ
		virtual  bool RemoveImage(const String& strID) = 0;
		
	};


	class IColorParse : public IParseBase
	{
	protected:
		virtual ~IColorParse() {};

	public:
		// ���������ļ�
		virtual  bool Create() = 0;
		// �����ļ�
		virtual  bool Save( ColorRes*  pColorInfo ) = 0;
		// ����
		virtual  bool Load( ColorRes*  pColorInfo ) = 0;
		// ��temp�ļ������¼���
		virtual  bool Reload( ColorRes* pColorInfo ) = 0;
		// ����
		virtual  bool InsertColor( ColorResItem *pColorItemInfo ) = 0;
		// �޸�
		virtual  bool ModifyColor( ColorResItem *pColorItemInfo ) = 0;
		// ɾ��
		virtual  bool RemoveColor( const String& strID ) = 0;
	};

	class IFontParse : public IParseBase
	{
	protected:
		virtual ~IFontParse(){};
	public:
		// ���������ļ�
		virtual  bool Create() = 0;
		// �����ļ�
		virtual bool Save( FontRes* pFontInfo ) = 0;
		// ����
		virtual bool Load( FontRes* pFontInfo ) = 0;
		// ���¼���
		virtual bool Reload( FontRes* pFontInfo ) = 0;

		virtual  bool InsertFont( FontResItem *pFontItemInfo ) = 0;
		virtual  bool ModifyFont( FontResItem *pFontItemInfo ) = 0;
		virtual  bool RemoveFont( const String& strID ) = 0;

	};

	class IStyleParse : public IParseBase
	{
	protected:
		virtual ~IStyleParse(){};

	public:
		// ���������ļ�
		virtual bool Create() = 0;
		// �����ļ�
		virtual bool Save( StyleRes* pStyleInfo ) = 0;
		// ����
		virtual bool Load( StyleRes* pStyleInfo ) = 0;
		// ���¼���
		virtual bool Reload( StyleRes* pStyleInfo ) = 0;

		virtual  bool InsertStyle( StyleResItem *pStyleItemInfo ) = 0;
		virtual  bool ModifyStyle( StyleResItem *pStyleItemInfo ) = 0;
		virtual  bool RemoveStyle( STYLE_SELECTOR_TYPE type, const String& strID ) = 0;
		virtual  bool InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue ) = 0;
		virtual  bool ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue ) = 0;
		virtual  bool RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey ) = 0;
	};


	class ILayoutParse : public IParseBase
	{
	protected:
		virtual ~ILayoutParse(){};

	public:
		// ���������ļ�
		virtual bool Create() = 0;
		// �����ļ�
		virtual bool Save( LayoutRes* pLayoutInfo ) = 0;
		// ����
		virtual bool Load( LayoutRes* pLayoutInfo ) = 0;

		virtual bool LoadLayout(Object* pRootObj, LayoutRes* pResLayout ) = 0;

		virtual bool ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild ) = 0;
#if 0 // -- �ܹ�����
		virtual Menu* LoadMenu( const String& strMenuId ) = 0;
#endif

		// �༭��ר�ú���
		virtual bool LoadAllWindowNodeInfo( LayoutWindowNodeInfo* pInfoArray, int* pArraySize ) = 0;
	};
#endif
}