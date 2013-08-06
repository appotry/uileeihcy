// ZIPUtil.h: interface for the CZIPUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIPUTIL_H__ACE59C57_DBCC_42D1_9317_D23176E5EE7E__INCLUDED_)
#define AFX_ZIPUTIL_H__ACE59C57_DBCC_42D1_9317_D23176E5EE7E__INCLUDED_
#include "zip.h"
#include "unzip.h"

class CZIPUtil  
{
public:
//	static void GetAllZipPath(CString strSourcePath,CStringList& strZipList);
	static bool Unzip2Dir(const TCHAR* szZipFile, const TCHAR* szDir);
};

#endif // !defined(AFX_ZIPUTIL_H__ACE59C57_DBCC_42D1_9317_D23176E5EE7E__INCLUDED_)
