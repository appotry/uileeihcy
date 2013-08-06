#include "stdafx.h"
#include "pluginmgr.h"
#include "3rd\markup\markup.h"
#include <algorithm>

CPluginManager*  CPluginManager::Get()
{
    static CPluginManager s;
    return &s;
}

CPluginManager::~CPluginManager()
{
    vector<PluginItemInfo*>::iterator iter = m_list.begin();
    for ( ; iter != m_list.end(); iter++)
    {
        SAFE_DELETE((*iter));
    }
    m_list.clear();
}

void  CPluginManager::GetPluginDir(TCHAR*  szPath)
{
    UI::Util::GetAppPath_(szPath);
    _tcscat(szPath, _T("IM\\plugin\\"));
}


bool PluginItemCompareProc(PluginItemInfo* p1, PluginItemInfo* p2)
{
    if (p1->nSort != p2->nSort)
        return p1->nSort < p2->nSort;

    return (_tcscoll(p1->strName.c_str(), p2->strName.c_str()) < 0);
}


void  CPluginManager::LoadPlugin()
{
    TCHAR  szPluginDir[MAX_PATH] = _T("");
    GetPluginDir(szPluginDir);

    String strFind = szPluginDir;
    strFind += _T("*.*");

    WIN32_FIND_DATA  finddata;
    HANDLE hFind=::FindFirstFile(strFind.c_str(),&finddata);
    if(INVALID_HANDLE_VALUE == hFind)
        return;

    while (1)
    {
        if (finddata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
        {
            if (finddata.cFileName[0] != '.')
            {
                // 判断目录下面是否存在skin.xml文件
                String strPath = szPluginDir;
                strPath.append(finddata.cFileName);

                String strFile = strPath;
                strFile.append(_T("\\plugin.xml"));

                if (PathFileExists(strFile.c_str()))
                {
                    OnFindPlugin(finddata.cFileName, strPath);
                }
            } 
        }

        if(!FindNextFile(hFind,&finddata))
            break;
    }
    FindClose(hFind);

    // 排序
    std::sort(m_list.begin(), m_list.end(), PluginItemCompareProc);
}


void  CPluginManager::OnFindPlugin(const String& strPluginName, const String& strDirPath)
{
    String strFile = strDirPath;
    strFile.append(_T("\\plugin.xml"));

    bool  bRet = false;
    PluginItemInfo*  p = new PluginItemInfo;
    p->strPlugin = strPluginName;
    do 
    {
        CMarkup  markup;
        if (!markup.Load(strFile))
            break;

        if (false == markup.FindElem(_T("plugin")))
            break;

        markup.IntoElem();
        
        // 注：markup只能按顺序，因此增加一个save/restore pos
#define GetElemData(key, value)       \
        markup.SavePos();             \
        if (markup.FindElem(key))     \
        {                             \
            value = markup.GetData(); \
        }                             \
        markup.RestorePos();

        String  strIcon, strIcon2, strSort;
        GetElemData(_T("name"),    p->strName);
        GetElemData(_T("dll"),     p->strDllPath);
        GetElemData(_T("desc"),    p->strDesc);
        GetElemData(_T("icon"),    strIcon);
        GetElemData(_T("icon2"),   strIcon2);
        GetElemData(_T("text"),    p->strText);
        GetElemData(_T("tooltip"), p->strTooltip);
        GetElemData(_T("sort"),    strSort);

        if (!strIcon.empty())
        {
            p->strIconPath = strDirPath;
            p->strIconPath.append(_T("\\"));
            p->strIconPath.append(strIcon);
        }
        if (!strIcon2.empty())
        {
            p->strIcon2Path = strDirPath;
            p->strIcon2Path.append(_T("\\"));
            p->strIcon2Path.append(strIcon2);
        }
        if (!strSort.empty())
        {
            p->nSort = _ttoi(strSort.c_str());
        }

        bRet = true;
    } while (0);

    if (!bRet)
    {
        SAFE_DELETE(p);
        return;
    }

    m_list.push_back(p);
}

PluginItemInfo*  CPluginManager::GetPluginItem(DWORD n)
{
    if (n >= m_list.size())
        return NULL;

    return m_list[n];
}