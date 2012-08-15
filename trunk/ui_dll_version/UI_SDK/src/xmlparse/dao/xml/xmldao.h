#pragma once
#include "3rd\markup\markup.h"

namespace UI
{
	//
	//	ui bulder�����ļ�
	//
	class CXmlProjectParse : public IProjectParse
	{
	protected:
		CXmlProjectParse( const String& strDataSource );
		~CXmlProjectParse();

		friend class IParseClassFactory;
	public:
		virtual  bool Create();
		virtual  bool Save( CPojo_Project*  pProject );
		virtual  bool Load( CPojo_Project*  pProject );
	};

	//
	//	ui.xml Ƥ����Ϣ�����ļ�
	//
	class CXmlSkinInfoParse : public ISkinInfoParse
	{
	protected:
		CXmlSkinInfoParse( const String& strDataSource );
		~CXmlSkinInfoParse();

		friend class IParseClassFactory;
	public:
		virtual  bool Create();
		virtual  bool Save( CPojo_SkinInfo*  pSkinInfo );
		virtual  bool Load( CPojo_SkinInfo*  pSkinInfo );
	};

	//
	//	image��Ϣ�����ļ�
	//
	class CXmlImageParse : public IImageParse
	{
	protected:
		CXmlImageParse( const String& strDataSource );
		~CXmlImageParse();

		friend class IParseClassFactory;
	public:
		virtual  bool Create();
		virtual  bool Save( CPojo_Image*  pImageInfo );
		virtual  bool Load( CPojo_Image*  pImageInfo );
		virtual  bool Reload( CPojo_Image* pImageInfo );

		virtual  bool InsertImage( CPojo_ImageItem *pImageItemInfo );
		virtual  bool ModifyImage( CPojo_ImageItem *pImageItemInfo );
		virtual  bool RemoveImage( const String& strID );

	private:
		bool  load_from_file( CMarkup* pXml, const String& strDataSource, CPojo_Image *pImageInfo );
		bool  insert_image_2_xml( CPojo_ImageItem *pImageItemInfo );
		bool  clear_save( CPojo_Image*  pImageInfo );
	private:
		CMarkup  m_xml;
	};

	//
	//	color��Ϣ�����ļ�
	//
	class CXmlColorParse : public IColorParse
	{
	protected:
		CXmlColorParse( const String& strDataSource );
		~CXmlColorParse();

		friend class IParseClassFactory;
	public:
		virtual  bool Create();
		virtual  bool Save( CPojo_Color*  pColorInfo );
		virtual  bool Load( CPojo_Color*  pColorInfo );
		// ��temp�ļ������¼���
		virtual  bool Reload( CPojo_Color* pColorInfo );
		// ����
		virtual  bool InsertColor( CPojo_ColorItem *pColorItemInfo );
		// �޸�
		virtual  bool ModifyColor( CPojo_ColorItem *pColorItemInfo );
		// ɾ��
		virtual  bool RemoveColor( const String& strID );

	private:
		bool  load_from_file( CMarkup* pXml, const String& strDataSource, CPojo_Color *pColorInfo );
		bool  insert_color_2_xml( CPojo_ColorItem *pColorInfoInfo );
		bool  clear_save(CPojo_Color* pColorInfo );

	private:
		CMarkup  m_xml;
	};

	//
	// font��Ϣ�����ļ�
	//
	class CXmlFontParse : public IFontParse
	{
	protected:
		CXmlFontParse( const String& strDataSource );
		~CXmlFontParse();

		friend class IParseClassFactory;
	public:
		virtual  bool Create();
		virtual  bool Save( CPojo_Font*  pColorInfo );
		virtual  bool Load( CPojo_Font* pFontInfo );
		virtual  bool Reload( CPojo_Font* pFontInfo );

		virtual  bool InsertFont( CPojo_FontItem *pFontItemInfo );
		virtual  bool ModifyFont( CPojo_FontItem *pFontItemInfo );
		virtual  bool RemoveFont( const String& strID );

	private:
		bool  load_from_file( CMarkup* pXml, const String& strDataSource, CPojo_Font *pFontInfo );
		bool  insert_font_2_xml( CPojo_FontItem *pFontItemInfo );
		bool  clear_save( CPojo_Font*  pFontInfo );
	private:
		CMarkup  m_xml;
	};
	

	// ���ڽ����̳й�ϵ�����ݽṹ��TODO: �������ʹ��
	template<class T>
	class tree
	{
	public:
		tree()
		{
			parent = NULL;
			first_child = NULL;
			first_neighbour = NULL;
		};

		T         data;
		tree<T>*  parent;
		tree<T>*  first_child;
		tree<T>*  first_neighbour;
	};

	//
	// style��Ϣ�����ļ�
	//
	class CXmlStyleParse : public IStyleParse
	{
	protected:
		CXmlStyleParse( const String& strDataSource );
		~CXmlStyleParse();

		friend class IParseClassFactory;
	public:
		virtual  bool Create();
		virtual  bool Save( CPojo_Style* pStyleInfo );
		virtual  bool Load( CPojo_Style* pStyleInfo );
		virtual  bool Reload( CPojo_Style* pStyleInfo );
		virtual  bool InsertStyle( CPojo_StyleItem *pStyleItemInfo );
		virtual  bool ModifyStyle( CPojo_StyleItem *pStyleItemInfo );
		virtual  bool RemoveStyle( STYLE_SELECTOR_TYPE type, const String& strID );
		virtual  bool InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
		virtual  bool ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
		virtual  bool RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey );

	private:
		bool  find_elem_in_xml( STYLE_SELECTOR_TYPE type, const String& strID );
		bool  find_elem_under_style( STYLE_SELECTOR_TYPE type, const String& strID );
		bool  load_from_file( CMarkup* pXml, const String& strDataSource, CPojo_Style *pStyleInfo );
		bool  insert_style_2_xml( CPojo_StyleItem *pStyleItemInfo );

		bool  parse_inherit( tree<CPojo_StyleItem*>* pTreeItem, CPojo_Style* pPojoStyle );
	private:
		CMarkup  m_xml;
	};

	//
	// layout��Ϣ�����ļ�
	//
	class CXmlLayoutParse : public ILayoutParse
	{
	protected:
		CXmlLayoutParse( const String& strDataSource );
		~CXmlLayoutParse();

		friend class IParseClassFactory;
	public:
		virtual  bool Create();
		virtual  bool Save( CPojo_Layout* pLayoutInfo );
		virtual  bool Load( CPojo_Layout* pLayoutInfo );
		virtual  bool LoadLayout(Object* pRootObj);
		virtual  bool ReLoadLayout( Object* pRootObj, list<Object*>& listAllChild );

	private:
		bool  load_from_file( CMarkup* pXml, const String& strDataSource, CPojo_Layout *pLayoutInfo );

		Object*    mapXmlToClass( String strXmlName );
		bool       loadAttributeForCurrentObjectInXml( Object* pObj, bool bReload=false );
		bool       loadObjects( Object* pObjParent );
		bool       reloadObjects(Object* pObjParent, list<Object*>& listAllChild);

	private:
		CMarkup  m_xml;
	};

	//
	//	�๤��
	//
	//		������Щparse��������service�����������������Լ��ͷŵ�
	//		��˽���Щ�����������ʽ��new/delete�ĳ� create/release��
	//		����ʹ��ʱ�������Գƣ�ֻ��new���Ҳ���delete
	//		��һ����Ҳ�����Ժ��ʵ���������滻
	//
	class IParseClassFactory
	{
	public:
		static IProjectParse*  CreateProjectParseInstance(const String& strDataSource) { return new CXmlProjectParse(strDataSource); }
		static ISkinInfoParse* CreateSkinInfoParseInstance(const String& strDataSource){ return new CXmlSkinInfoParse(strDataSource); }
		static IImageParse*    CreateImageParseInstance(const String& strDataSource )  { return new CXmlImageParse(strDataSource); }
		static IColorParse*    CreateColorParseInstance(const String& strDataSource )  { return new CXmlColorParse(strDataSource); }
		static IFontParse*     CreateFontParseInstance( const String& strDataSource )  { return new CXmlFontParse(strDataSource); }
		static IStyleParse*    CreateStyleParseInstance( const String& strDataSource ) { return new CXmlStyleParse(strDataSource); }
		static ILayoutParse*   CreateLayoutParseInstance( const String& strDataSource ){ return new CXmlLayoutParse(strDataSource); }

	};
}