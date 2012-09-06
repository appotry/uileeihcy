#pragma once
#include "xmlparse/pojo/pojo.h"

namespace UI
{
	// ����
	class IParseBase
	{
	protected:
		IParseBase(const String& strDataSource):m_strDataSource(strDataSource){ }
		virtual ~IParseBase()=0{};
	
		String   m_strDataSource;    // ����Դ����xml�ļ�·��
	public:

		void  Release(){ delete this; }
		const String& GetDataSourceStringRef() { return m_strDataSource; }
	};

	//
	//	ui bulder�����ļ�
	//
	class IProjectParse : public IParseBase
	{
	protected:
		IProjectParse( const String& strDataSource ):IParseBase(strDataSource){}
		virtual ~IProjectParse() {};
	
	public:
		// ����
		virtual  bool Create() = 0;
		// ����
		virtual  bool Save( CPojo_Project*  pProject ) = 0;
		// ����
		virtual  bool Load( CPojo_Project*  pProject ) = 0;
	};


	//
	//	ui.xml Ƥ����Ϣ�����ļ�
	//
	class ISkinInfoParse : public IParseBase
	{
	protected:
		ISkinInfoParse( const String& strDataSource ):IParseBase(strDataSource){}
		virtual ~ISkinInfoParse() {};

	public:
		// ���������ļ�
		virtual  bool Create() = 0;
		// �����ļ�
		virtual  bool Save( CPojo_SkinInfo*  pSkinInfo ) = 0;
		// ����
		virtual  bool Load( CPojo_SkinInfo*  pSkinInfo ) = 0;
	};

	class IImageParse : public IParseBase
	{
	protected:
		IImageParse( const String& strDataSource ):IParseBase(strDataSource){}
		virtual ~IImageParse() {};

	public:
		// ���������ļ�
		virtual  bool Create() = 0;
		// �����ļ�
		virtual  bool Save( CPojo_Image*  pImageInfo ) = 0;
		// ����
		virtual  bool Load( CPojo_Image*  pImageInfo ) = 0;
		// ��temp�ļ������¼���
		virtual  bool Reload( CPojo_Image* pImageInfo ) = 0;
		// ����һ��ͼƬ
		virtual  bool InsertImage( CPojo_ImageItem *pImageItemInfo ) = 0;
		// �޸�һ��ͼƬ
		virtual  bool ModifyImage( CPojo_ImageItem *pImageItemInfo ) = 0;
		// ɾ��һ��ͼƬ
		virtual  bool RemoveImage( const String& strID ) = 0;
		
	};

	class IColorParse : public IParseBase
	{
	protected:
		IColorParse( const String& strDataSource ):IParseBase(strDataSource){}
		virtual ~IColorParse() {};

	public:
		// ���������ļ�
		virtual  bool Create() = 0;
		// �����ļ�
		virtual  bool Save( CPojo_Color*  pColorInfo ) = 0;
		// ����
		virtual  bool Load( CPojo_Color*  pColorInfo ) = 0;
		// ��temp�ļ������¼���
		virtual  bool Reload( CPojo_Color* pColorInfo ) = 0;
		// ����
		virtual  bool InsertColor( CPojo_ColorItem *pColorItemInfo ) = 0;
		// �޸�
		virtual  bool ModifyColor( CPojo_ColorItem *pColorItemInfo ) = 0;
		// ɾ��
		virtual  bool RemoveColor( const String& strID ) = 0;
	};

	class IFontParse : public IParseBase
	{
	protected:
		IFontParse( const String& strDataSource ) : IParseBase(strDataSource){}
		virtual ~IFontParse(){};
	public:
		// ���������ļ�
		virtual  bool Create() = 0;
		// �����ļ�
		virtual bool Save( CPojo_Font* pFontInfo ) = 0;
		// ����
		virtual bool Load( CPojo_Font* pFontInfo ) = 0;
		// ���¼���
		virtual bool Reload( CPojo_Font* pFontInfo ) = 0;

		virtual  bool InsertFont( CPojo_FontItem *pFontItemInfo ) = 0;
		virtual  bool ModifyFont( CPojo_FontItem *pFontItemInfo ) = 0;
		virtual  bool RemoveFont( const String& strID ) = 0;

	};


	class IStyleParse : public IParseBase
	{
	protected:
		IStyleParse( const String& strDataSource ) : IParseBase(strDataSource){}
		virtual ~IStyleParse(){};

	public:
		// ���������ļ�
		virtual bool Create() = 0;
		// �����ļ�
		virtual bool Save( CPojo_Style* pStyleInfo ) = 0;
		// ����
		virtual bool Load( CPojo_Style* pStyleInfo ) = 0;
		// ���¼���
		virtual bool Reload( CPojo_Style* pStyleInfo ) = 0;

		virtual  bool InsertStyle( CPojo_StyleItem *pStyleItemInfo ) = 0;
		virtual  bool ModifyStyle( CPojo_StyleItem *pStyleItemInfo ) = 0;
		virtual  bool RemoveStyle( STYLE_SELECTOR_TYPE type, const String& strID ) = 0;
		virtual  bool InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue ) = 0;
		virtual  bool ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue ) = 0;
		virtual  bool RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey ) = 0;
	};
	
	class ILayoutParse : public IParseBase
	{
	protected:
		ILayoutParse( const String& strDataSource ) : IParseBase(strDataSource){}
		virtual ~ILayoutParse(){};

	public:
		// ���������ļ�
		virtual bool Create() = 0;
		// �����ļ�
		virtual bool Save( CPojo_Layout* pLayoutInfo ) = 0;
		// ����
		virtual bool Load( CPojo_Layout* pLayoutInfo ) = 0;

		virtual bool LoadLayout(Object* pRootObj) = 0;

		virtual  bool ReLoadLayout( Object* pRootObj, list<Object*>& listAllChild ) = 0;

		virtual  Menu* LoadMenu( const String& strMenuId ) = 0;
	};
}