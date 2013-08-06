#pragma once

class CPlayerLyricData
{
public:
    bool  Save(const TCHAR* szArtist, const TCHAR* szTitle, byte* pByte, int nSize);
    bool  Load(const TCHAR* szMp3FileName, const TCHAR* szArtist, const TCHAR* szTitle, IBuffer* pBuffer);

private:
    void  GetDefaultLyricDir(String& str);
    bool  _LoadLrcFile(const String& strPath, IBuffer* pBuffer);
};