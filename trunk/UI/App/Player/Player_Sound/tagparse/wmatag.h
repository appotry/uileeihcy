#pragma once
#include <wmsdk.h>
#include "..\Player_Sound.h"

class CWMATag
{
public:
    bool    ParseFile(const TCHAR* szFilePath, ITagParseCallback* pCallback);

protected:
    bool    ParseAttrib(WCHAR* pszName, BYTE* pBytes, int nByteSize, WMT_ATTR_DATATYPE dataType, ITagParseCallback* pCallback);

};