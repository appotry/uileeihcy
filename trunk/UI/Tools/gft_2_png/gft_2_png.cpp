// gft_2_png.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "gft_2_png.h"
#include <shlwapi.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#pragma comment(lib, "shlwapi.lib")
#pragma warning(disable:4996)

#ifdef _UNICODE
typedef wstring String;
#else
typedef string  String;
#endif

int ChangeFile(const TCHAR*  szFileName);
int ChangeGft2PngFile(const TCHAR*  szFileName);
int ChangeBmpFile(const TCHAR*  szFileName);
int ChangeDirectory(const TCHAR*  szFileName);
typedef bool (CALLBACK* EnumFileInDirProc)(const TCHAR*, const TCHAR*, WPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	if (NULL == lpCmdLine)
	{
		::MessageBox(NULL, _T("NULL == lpCmdLine"), _T("Error"), MB_OK|MB_ICONERROR);
		return -1;
	}

	int nFileNameLength = _tcslen(lpCmdLine);
	if (0 == nFileNameLength)
	{
		::MessageBox(NULL, _T("0 == nFileNameLength"), _T("Error"), MB_OK|MB_ICONERROR);
		return -2;
	}

	if (lpCmdLine[0] == _T('\"'))  // 去除命令行中的引号
	{
		lpCmdLine[nFileNameLength-1] = _T('\0');
		lpCmdLine++;
		nFileNameLength -= 2;
 	}

	// 判断是一个文件夹还是一个目录
	if (::PathIsDirectory(lpCmdLine))
	{
		return ChangeDirectory(lpCmdLine);
	}
	else
	{
		return ChangeFile(lpCmdLine);
	}
	return 0;
}

int ChangeFile(const TCHAR*  szFileName)
{
	int nFileNameLength = _tcslen(szFileName);
	fstream  fin, fout;
	fin.open(szFileName, ios_base::in|ios_base::binary);
	if (fin.fail())
	{
		::MessageBox(NULL, szFileName, _T("Error -- fin.fail()"), MB_OK|MB_ICONERROR);
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////
	// 寻找原始的PNG头的位置

	const byte* tagTGF  = (byte*)"TGF";
	const byte* tagKEY  = (byte*)"KEY";
	const byte* tagNINE = (byte*)"NINE";

	// http://www.garykessler.net/library/file_sigs.html 各种文件格式头
	byte  tagPNG[4] = {0x89, 'P', 'N', 'G'};
	byte  tagBMP[2] = {'B', 'M'};
	byte  tagJFIF[4]= {'J', 'F', 'I', 'F'};
	byte  tagExif[2]= {0xFF, 0xD8};
	
	byte   bytePrefix[128];
	fin.read((char*)bytePrefix, 64);  // 注：有可能一些文件就小于64字节

	byte*  p = bytePrefix;
	int    nRealFileStartPos = 0;

	if (0 == memcmp(p, tagTGF, 3))   // 不是一个gft文件，但有可能仅仅是一人重命名的文件
	{
		nRealFileStartPos += 20;

		if (0 == memcmp(p+nRealFileStartPos, tagNINE, 4))
		{
			nRealFileStartPos += 16;
		}

		if (0 == memcmp(p+nRealFileStartPos, tagKEY, 3))
		{
			nRealFileStartPos += 12;
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	// 现在接下来看图片文件的类型了
	String  strExt;
	if (0 == memcmp(p+nRealFileStartPos, tagPNG, 3))
	{
		strExt = _T("_.png");
	}
	else if (0 == memcmp(p+nRealFileStartPos, tagBMP, 2))
	{
		strExt = _T("_.bmp");
	}
	else if (0 == memcmp(p+nRealFileStartPos, tagJFIF, 4))
	{
		strExt = _T("_.jfif");
	}
	else if (0 == memcmp(p+nRealFileStartPos, tagExif, 2))
	{
		strExt = _T("_.jpg");
	}
	else
	{
		// 还剩下一个 0x 55 56 的文件格式，不知道是什么格式
		fin.close();
//		::MessageBox(NULL, _T(""), _T("Error, this file format is not supported."), MB_ICONERROR|MB_OK);
		return -1;    // 暂不支持
	}


	if (fin.eof())
	{
// 		fin.close();
// 		return -1;
		fin.clear(ios::goodbit);
	}

	// 在原文件名字的基础上，得出新的输出文件的名字
	TCHAR szOutFile[MAX_PATH];
	_tcscpy(szOutFile, szFileName);
	*_tcsrchr(szOutFile, _T('.')) = _T('\0');
	_tcscat(szOutFile, strExt.c_str());

	fout.open(szOutFile, ios_base::out|ios_base::binary);
	if (fout.fail())
	{
		::MessageBox(NULL, szOutFile, _T("Error fout.fail()"), MB_OK|MB_ICONERROR);
		return -4;
	}

	char byteBuffer[1024];
	fin.seekg(nRealFileStartPos, ios_base::beg);
	while(!fin.eof())
	{
		fin.read(byteBuffer, 1024);
		fout.write(byteBuffer, 1024);
	}
	fout.close();
	fin.close();
	::DeleteFile(szFileName);
	return 0;
}

// 遍历文件夹
BOOL EnumFileInDirectory(const TCHAR* szDir, EnumFileInDirProc proc, WPARAM wParam)
{
	WIN32_FIND_DATA  finddata;

	String strBaseDir = szDir;
	if (strBaseDir[strBaseDir.length()-1] != _T('\\') &&
		strBaseDir[strBaseDir.length()-1] != _T('/'))
	{
		strBaseDir += _T("\\");
	}
	String strFind = strBaseDir;
	strFind += _T("*.*");

	HANDLE hFind=::FindFirstFile(strFind.c_str(),&finddata);
	if(INVALID_HANDLE_VALUE == hFind)
		return FALSE;

	while(1)
	{
		if(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (finddata.cFileName[0] != '.')
			{
				String str = strBaseDir;
				str += finddata.cFileName;
				str += _T("\\");
				EnumFileInDirectory(str.c_str(), proc, wParam);
			}
		}
		else
		{
			proc(strBaseDir.c_str(), finddata.cFileName, wParam);
		}
		if(!FindNextFile(hFind,&finddata))
			break;
	}
	FindClose(hFind);

	return TRUE;
}

bool CALLBACK MyEnumFileInDirProc(const TCHAR* szDir, const TCHAR* szFileName, WPARAM wParam)
{
	String str = szDir;
	if (str[str.length()-1] != _T('\\'))
		str += _T("\\");
	str += szFileName;
	ChangeFile(str.c_str());
	return true;
}

int ChangeDirectory(const TCHAR*  szFileName)
{
	EnumFileInDirectory(szFileName, MyEnumFileInDirProc, 0);
	return 0;
}