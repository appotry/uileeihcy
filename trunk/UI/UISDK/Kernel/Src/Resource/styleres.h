#pragma once

namespace UI
{
	
class StyleResItem
{
public:
	StyleResItem();
	~StyleResItem();
    IStyleResItem*  GetIStyleResItem();

	DECLARE_STRING_SETGET( ID );

	STYLE_SELECTOR_TYPE GetSelectorType() { return m_eSelectorType; }
	void    SetSelectorType(const STYLE_SELECTOR_TYPE& type) { m_eSelectorType = type; }
	const TCHAR*  GetInherits()    { return m_strInherits.c_str(); }
	String& GetInheritsRef() { return m_strInherits; }

	void   SetInherits( const String& str );
	int    GetInheritCount();
	bool   GetInheritItem(int i, String& str);
	bool   RemoveInheritItem( const String& str );

    void   SetAttributeMap(IMapAttribute* pMapAttrib);
	void   GetAttributeMap(IMapAttribute** ppMapAttrib);
	void   SetAttribute( const String& key, const String& value );
	bool   GetAttribute( const String& key, String& retValue );
	bool   InsertAttribute( const String& key, const String& value );
	bool   ModifyAttribute( const String& key, const String& value );
	bool   RemoveAttribute( const String& key );

	bool   InheritAttribute( const String& key, const String& value );
	bool   InheritMyAttributesToAnother( StyleResItem* pChild );
	bool   ApplyMyAttributesToAnExistMap( IMapAttribute* pMapAttrib );

private:
    IStyleResItem*  m_pIStyleResItem;

	STYLE_SELECTOR_TYPE  m_eSelectorType;
	String  m_strID;          // 该选择器的ID
	String  m_strInherits;    // 继承列表
    IMapAttribute*  m_pMapAttrib;  // 该选择器的属性集

	// 非持久数据
	vector<String>  m_vInheritList;   // 继承列表，运行中解释m_strInherits
};

class StyleRes
{
public:
    StyleRes();
	~StyleRes();

    IStyleRes*  GetIStyleRes();

	long     GetStyleCount();
	IStyleResItem*  GetStyleResItem(long lIndex);
	IStyleResItem*  GetStyleResItem2(STYLE_SELECTOR_TYPE type, const TCHAR* szId);
public:

	void Clear();
	bool InsertStyle( STYLE_SELECTOR_TYPE type, const String& strID, const String& strInherit );
	bool InsertStyle( StyleResItem* pNewItem );
	bool ModifyStyle( STYLE_SELECTOR_TYPE type,const String& strID, const String& strInherit );
	bool RemoveStyle( STYLE_SELECTOR_TYPE type, const String& strID );
	
	StyleResItem* GetStyleItem( int nIndex );
	StyleResItem* GetStyleItem( STYLE_SELECTOR_TYPE type, const TCHAR* szID );
	StyleResItem* GetStyleItem( STYLE_SELECTOR_TYPE type, const String& strID );

	bool InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
	bool ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
	bool RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey );

	bool LoadStyle(const TCHAR* szTagName, const TCHAR* szStyleClass, const TCHAR* szID, IMapAttribute* pMapStyle);

private:
    IStyleRes*  m_pIStyleRes;
	vector<StyleResItem*>   m_vStyles;
};
}