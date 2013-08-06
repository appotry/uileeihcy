#include "StdAfx.h"
#include "LyricMgr.h"
#include "App\Player\Player_UI\Mgr\MainMgr.h"
#include "App\Player\Player_UI\Mgr\Lyric\wininetwrap.h"
#include "App\Player\Player_UI\UI\LyricDlg.h"
#include "App\Player\Player_UI\Mgr\Lyric\lyricdownload.h"
#include "App\Player\Player_UI\Data\PlayerLyricData.h"

CLyricMgr::CLyricMgr(void)
{
    m_pLyricDlg = NULL;
    m_pPlayerLyricData = new CPlayerLyricData;
    m_pLyricDownloadMgr = new CLyricDownloadMgr;
}

CLyricMgr::~CLyricMgr(void)
{
    SAFE_DELETE_Ixxx(m_pLyricDlg);
    SAFE_DELETE(m_pLyricDownloadMgr);
    SAFE_DELETE(m_pPlayerLyricData);
}

bool CLyricMgr::Initialize()
{
    HWND hWndParent = GetMainMgr()->GetMainWnd();
    CLyricDlg::CreateInstance(g_pUIApp, &m_pLyricDlg);
    m_pLyricDlg->Create(g_pUIApp, _T("lyricdlg"), hWndParent);
    m_pLyricDlg->SetLyricMgr(this);

    RECT  rc;
    ::GetWindowRect(GetMainMgr()->GetMainWnd(), &rc);
    ::SetWindowPos(m_pLyricDlg->GetHWND(), NULL, rc.right, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE );

    this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_LYRICDLG_CREATE, (WPARAM)m_pLyricDlg->GetHWND());

    // 获取歌词服务器
    m_pLyricDownloadMgr->GetSvrList(_T("ttlrcct2.qianqian.com"), _T("dll/lyricsvr.dll?svrlst"));

    return true;
}
bool CLyricMgr::Release()
{
    if (m_pLyricDlg && ::IsWindow(m_pLyricDlg->GetHWND()))
    {
        ::DestroyWindow(m_pLyricDlg->GetHWND());
    }
    return true;
}

void CLyricMgr::ToggleShowLyricDlg()
{
    if (NULL == m_pLyricDlg)
        return;

    bool bVisible = m_pLyricDlg->IsVisible();
    if (bVisible)
    {
        ::ShowWindow(m_pLyricDlg->GetHWND(), SW_HIDE);
    }
    else
    {
        ::ShowWindow(m_pLyricDlg->GetHWND(),SW_SHOWNOACTIVATE);
    }

    this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_LYRICDLG_VISIBLE_CHANGED, (WPARAM)m_pLyricDlg->GetHWND(), (LPARAM)!bVisible);
}
void CLyricMgr::OnLyricDlgHide()
{
    this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_LYRICDLG_VISIBLE_CHANGED, (WPARAM)m_pLyricDlg->GetHWND(), (LPARAM)FALSE);
}
void CLyricMgr::HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam)
{
    if (EVENT_TYPE_PLAY == nEventType)
    {
        switch (nEventId)
        {
        case PLAY_EVENT_ID_ON_START:
            {
                this->OnPlay((IPlayerListItemInfo*)wParam);
                if (m_pLyricDlg)
                    m_pLyricDlg->OnMp3Start((IPlayerListItemInfo*)wParam);
            }
            break;

        case PLAY_EVENT_ID_ON_STOP:
            {
                if (m_pLyricDlg)
                    m_pLyricDlg->OnMp3Stop();
            }
            break;

        case PLAY_EVENT_ID_PROGRESS_IND:
            {
                if (NULL == m_pLyricDlg)
                    break;

                double* pdSeconds = ((double*)wParam);
                double* pdPecent = ((double*)lParam);

                if (pdSeconds || pdPecent)
                    m_pLyricDlg->OnMp3ProgressInd(*pdSeconds, *pdPecent);
            }
            break;
        }
    }
}

// http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?sh?Artist={0}&Title={1}&Flags=0
void  CLyricMgr::OnPlay(IPlayerListItemInfo*  pItem)
{
    const TCHAR*  szTitle  = pItem->GetTitle();
    const TCHAR*  szArtist = pItem->GetArtist();

    // 首先从默认的歌词目录中查找是否有该歌曲歌词文件
    {
        IBuffer*  pLyricBuffer = NULL;
        CreateBuffer(&pLyricBuffer);
        if (m_pPlayerLyricData->Load(pItem->GetFileName(), szArtist, szTitle, pLyricBuffer))
        {
            m_pLyricDlg->OnLyricLoad(pItem, pLyricBuffer);
            SAFE_RELEASE(pLyricBuffer);

            TTPLAYER_LOG_DEBUG(_T("%s load lyric from file: %s - %s.lrc"), FUNC_NAME, szArtist, szTitle);
            return;
        }
        SAFE_RELEASE(pLyricBuffer);
    }

    // 歌词列表
    IBuffer*  pBufferLyricList = NULL;
    CreateBuffer(&pBufferLyricList);
    if (false == m_pLyricDownloadMgr->GetSvrLyricList(szArtist, szTitle, pBufferLyricList))
    {
        SAFE_RELEASE(pBufferLyricList);
        return;
    }

    
    // 解析歌词列表
    CLyricDownloadList  lyricDownloadList;
    m_pLyricDownloadMgr->ParseLyricList((char*)pBufferLyricList->Get(), &lyricDownloadList);
    SAFE_RELEASE(pBufferLyricList);

    // 这里默认获取第一个歌词项进行下载
    LyricDownloadItemInfo*  pLyricItem = lyricDownloadList.GetItem(0);
    if (NULL == pLyricItem)
        return;

    IBuffer*  pLyricBuffer = NULL;
    CreateBuffer(&pLyricBuffer);
    if (false == m_pLyricDownloadMgr->Download(pLyricItem, pLyricBuffer))
    {
        SAFE_RELEASE(pLyricBuffer);
        return;
    }

    // 保存歌词
    m_pPlayerLyricData->Save(szArtist, szTitle, pLyricBuffer->Get(), pLyricBuffer->GetSize());
    m_pLyricDlg->OnLyricLoad(pItem, pLyricBuffer);

    SAFE_RELEASE(pLyricBuffer);
    TTPLAYER_LOG_DEBUG(_T("%s download lyric: %s - %s.lrc"), FUNC_NAME, szArtist, szTitle);
}