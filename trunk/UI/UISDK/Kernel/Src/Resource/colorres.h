#pragma once

namespace UI
{

//
//	color�е�һ����Ϣ����
//
class  ColorResItem
{
public:
	ColorResItem();
	~ColorResItem();

    IColorResItem*  GetIColorResItem();
	const TCHAR* GetColorString();

private:
	String    m_strID;
	String    m_strColor;     // ��ɫ�ַ���
    ColorPtr  m_pColor;

	COLORREF* m_pOriginColorValue;  // ��COLORδ�ı�ɫ��ǰ����ɫֵ�����ڻ�ԭ

	// TODO: �����ٽ�����չ���������ֵĺڰ���ɫ�任���Ǹ������ֺ���ı���ɫ�������ģ�������������
	bool      m_bUseSkinHLS;  // ��COLOR�Ƿ����Ƥ��ɫ���ı� 

public:
	DECLARE_STRING_SETGET( ID );
	DECLARE_bool_SETGET(UseSkinHLS);
	const String&   GetColorStringRef();
	bool  GetColor(ColorPtr* pColor, bool *bFirstTimeCreate = NULL);
	void  SetColor(const String& strColor); 

	void  SetAttribute(IMapAttribute* p);
	bool  ModifyHLS(short h, short l, short s, int nFlag);

private:
    IColorResItem*  m_pIColorResItem;
};

//
//	color�б�
//
class ColorRes
{
public:
	ColorRes(SkinRes* p);
	~ColorRes();

    IColorRes*  GetIColorRes();

	long  GetColorCount();
	IColorResItem*  GetColorResItem(long lIndex);
	void  GetColor(const TCHAR* szColorId, Color** pp);

public:
	ColorResItem* GetColorItem( int nIndex );
	ColorResItem* GetColorItem( const String& strID );
	bool  InsertColor( const String& strID, const TCHAR* szColor, ColorResItem** pItem );
	bool  ModifyColor( const String& strID, const String& strColor );
	bool  RemoveColor( const String& strID );
	void  Clear();

	bool  LoadItem(IMapAttribute* pMapAttrib, const TCHAR* szData);
	bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

private:
	SkinRes*  m_pSkinRes;
    IColorRes*  m_pIColorRes;
    vector<ColorResItem*>   m_vColors;

};

}