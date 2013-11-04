#pragma once

//
//	鼠标样式
//
namespace  UI
{
class UICursor;

class CursorResItem
{
public:
	CursorResItem();
	~CursorResItem();

	DECLARE_STRING_SETGET( ID );
	DECLARE_STRING_SETGET( CurFilePath );

	IUICursor*  GetCursor();
	bool  ModifyCursor( const String& strCurFilePath );
	void  SetAttribute(IMapAttribute* p){}

	ICursorResItem*  GetICursorResItem();
public:
	String     m_strID;
	String     m_strCurFilePath;     // .cur文件路径
	UICursor*  m_pCursor;  
	ICursorResItem*  m_pICursorResItem;
};


class CursorRes
{
public:
	CursorRes();
	~CursorRes();

	ICursorRes*  GetICursorRes();
	void  GetCursor(const TCHAR* szCursorId, IUICursor** pp);

public:
	CursorResItem* GetCursorItem( int nIndex );
	CursorResItem* GetCursorItem( const String& strID );
	int   GetCursorCount();
	CursorResItem*  LoadItem(IMapAttribute* pMapAttrib, const TCHAR* szFullPath);
	bool  InsertCursor(const TCHAR* strID, const TCHAR* strCurFilePath, CursorResItem** ppItem);
	bool  ModifyCursor(const TCHAR* strID, const TCHAR* strCursor );
	bool  RemoveCursor(const TCHAR* strID );
	void  Clear();

private:
	ICursorRes*  m_pICursorRes;
	vector<CursorResItem*>   m_vCursors;
};

}