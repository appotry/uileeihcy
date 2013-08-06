#include "stdafx.h"
#include "ziputil.h"

bool CZIPUtil::Unzip2Dir(const TCHAR* szZipFile, const TCHAR* szDir)
{
	if (!PathIsDirectory(szDir))
		return false;

    HZIP hz = OpenZip(szZipFile, 0);
	if (!hz)
		return false;
	
	ZIPENTRY ze;
	GetZipItem(hz, -1, &ze);
	int numitems = ze.index;

	TCHAR szSafeDir[MAX_PATH] = _T("");
	_tcscpy(szSafeDir, szDir);

	int nLength = _tcslen(szSafeDir);
	if (szSafeDir[nLength-1] != _T('\\'))
	{
		_tcscat(szSafeDir, _T("\\"));
		nLength++;
	}

	for (int zi=0; zi<numitems; zi++)
	{  
		ZIPENTRY ze; 
		GetZipItem(hz,zi,&ze);  // fetch individual details

		_tcscpy(szSafeDir+nLength, ze.name);
		UnzipItem(hz, zi, szSafeDir);  // e.g. the item's name.
	}
	CloseZip(hz); 
	return true;
}

// void CZIPUtil::GetAllZipPath(CString strSourcePath, CStringList& strZipList)
// {
// 	DWORD dwAttributes = ::GetFileAttributes(strSourcePath);
// 	BOOL bIsADreictory =(dwAttributes == FILE_ATTRIBUTE_DIRECTORY);
// 	CFileFind fileFind;
// 	CString fileName = "";
// 
// 	//不是一个目录，返回
// 	if (bIsADreictory == FALSE)
// 	{
// 		return ;
// 	}
// 
// 	//查找文件
// 	fileName.Format("%s%s",strSourcePath,"*.zip");
// 	BOOL bFind = fileFind.FindFile(fileName);
// 	while (bFind)
// 	{  
// 		CString strName = "";
// 		CString strPath = "";
// 		CString strAimPath = "";
// 		bFind = fileFind.FindNextFile();
// 		if (fileFind.IsDots())
// 		{ 
// 			continue;
// 		}
// 		
// 		if (fileFind.IsDirectory())
// 		{
// 			continue;
// 		}
// 		strName = fileFind.GetFileName();
// 		strPath = fileFind.GetFilePath();
//         strZipList.AddHead(strPath);
//    }
// }
