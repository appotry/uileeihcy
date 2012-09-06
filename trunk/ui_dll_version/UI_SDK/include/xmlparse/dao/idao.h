#pragma once
#include "xmlparse/pojo/pojo.h"

namespace UI
{
	// 基类
	class IParseBase
	{
	protected:
		IParseBase(const String& strDataSource):m_strDataSource(strDataSource){ }
		virtual ~IParseBase()=0{};
	
		String   m_strDataSource;    // 数据源，如xml文件路径
	public:

		void  Release(){ delete this; }
		const String& GetDataSourceStringRef() { return m_strDataSource; }
	};

	//
	//	ui bulder工程文件
	//
	class IProjectParse : public IParseBase
	{
	protected:
		IProjectParse( const String& strDataSource ):IParseBase(strDataSource){}
		virtual ~IProjectParse() {};
	
	public:
		// 创建
		virtual  bool Create() = 0;
		// 保存
		virtual  bool Save( CPojo_Project*  pProject ) = 0;
		// 加载
		virtual  bool Load( CPojo_Project*  pProject ) = 0;
	};


	//
	//	ui.xml 皮肤信息描述文件
	//
	class ISkinInfoParse : public IParseBase
	{
	protected:
		ISkinInfoParse( const String& strDataSource ):IParseBase(strDataSource){}
		virtual ~ISkinInfoParse() {};

	public:
		// 创建数据文件
		virtual  bool Create() = 0;
		// 保存文件
		virtual  bool Save( CPojo_SkinInfo*  pSkinInfo ) = 0;
		// 加载
		virtual  bool Load( CPojo_SkinInfo*  pSkinInfo ) = 0;
	};

	class IImageParse : public IParseBase
	{
	protected:
		IImageParse( const String& strDataSource ):IParseBase(strDataSource){}
		virtual ~IImageParse() {};

	public:
		// 创建数据文件
		virtual  bool Create() = 0;
		// 保存文件
		virtual  bool Save( CPojo_Image*  pImageInfo ) = 0;
		// 加载
		virtual  bool Load( CPojo_Image*  pImageInfo ) = 0;
		// 从temp文件中重新加载
		virtual  bool Reload( CPojo_Image* pImageInfo ) = 0;
		// 插入一张图片
		virtual  bool InsertImage( CPojo_ImageItem *pImageItemInfo ) = 0;
		// 修改一张图片
		virtual  bool ModifyImage( CPojo_ImageItem *pImageItemInfo ) = 0;
		// 删除一张图片
		virtual  bool RemoveImage( const String& strID ) = 0;
		
	};

	class IColorParse : public IParseBase
	{
	protected:
		IColorParse( const String& strDataSource ):IParseBase(strDataSource){}
		virtual ~IColorParse() {};

	public:
		// 创建数据文件
		virtual  bool Create() = 0;
		// 保存文件
		virtual  bool Save( CPojo_Color*  pColorInfo ) = 0;
		// 加载
		virtual  bool Load( CPojo_Color*  pColorInfo ) = 0;
		// 从temp文件中重新加载
		virtual  bool Reload( CPojo_Color* pColorInfo ) = 0;
		// 插入
		virtual  bool InsertColor( CPojo_ColorItem *pColorItemInfo ) = 0;
		// 修改
		virtual  bool ModifyColor( CPojo_ColorItem *pColorItemInfo ) = 0;
		// 删除
		virtual  bool RemoveColor( const String& strID ) = 0;
	};

	class IFontParse : public IParseBase
	{
	protected:
		IFontParse( const String& strDataSource ) : IParseBase(strDataSource){}
		virtual ~IFontParse(){};
	public:
		// 创建数据文件
		virtual  bool Create() = 0;
		// 保存文件
		virtual bool Save( CPojo_Font* pFontInfo ) = 0;
		// 加载
		virtual bool Load( CPojo_Font* pFontInfo ) = 0;
		// 重新加载
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
		// 创建数据文件
		virtual bool Create() = 0;
		// 保存文件
		virtual bool Save( CPojo_Style* pStyleInfo ) = 0;
		// 加载
		virtual bool Load( CPojo_Style* pStyleInfo ) = 0;
		// 重新加载
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
		// 创建数据文件
		virtual bool Create() = 0;
		// 保存文件
		virtual bool Save( CPojo_Layout* pLayoutInfo ) = 0;
		// 加载
		virtual bool Load( CPojo_Layout* pLayoutInfo ) = 0;

		virtual bool LoadLayout(Object* pRootObj) = 0;

		virtual  bool ReLoadLayout( Object* pRootObj, list<Object*>& listAllChild ) = 0;

		virtual  Menu* LoadMenu( const String& strMenuId ) = 0;
	};
}