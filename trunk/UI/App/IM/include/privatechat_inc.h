#ifndef PRIVATECHAT_INC_H_3CF03C58_0A52_4d45_A094_D915DF7C1364
#define PRIVATECHAT_INC_H_3CF03C58_0A52_4d45_A094_D915DF7C1364

#include "App\IM\include\framework_inc.h"
#include "App\IM\include\task.h"

#ifdef PRIVATECHAT_EXPORTS
#define IMPRIVATECHATAPI __declspec(dllexport)
#else
#define IMPRIVATECHATAPI __declspec(dllimport)
#endif

namespace IM
{
    class OpenPrivateChatDlgReqTask;


    struct IPrivateChatBiz : public IBiz
    {
        virtual long  OpenPrivateChatDlg(OpenPrivateChatDlgReqTask* p) = 0;
    };

    // UI COMMAND
#define PRIVATECHAT_UI_CMD_DESTROYPRIVATECHATDLG  138311416

    // BIZ Command


    enum OPEN_PRIVATECHATDLG_MODE 
    {
        NORMAL,
        SENDTEXT,   // wParam: BSTR initText
        SENDFILE,
        VIDEO,
        AUDIO,
    };

    class OpenPrivateChatDlgReqTask : public TaskImpl<ITask>
    {
    public:
        OpenPrivateChatDlgReqTask(IM::IUiBizBase* pDest, const TCHAR* szUserId)
        {
            m_lOpenMode = NORMAL;
            m_wParam = 0;
            m_lParam = 0;

            m_pDest = pDest;
            if (szUserId)
            {
                _tcsncpy(m_szUserId, szUserId, sizeof(m_szUserId)/sizeof(TCHAR) - 1);
                m_szUserId[255] = _T('');
            }
            else
            {
                memset(m_szUserId, 0, sizeof(m_szUserId));
            }
        }
        ~OpenPrivateChatDlgReqTask()
        {
            switch (m_lOpenMode)
            {
            case SENDTEXT:
                {
                    BSTR bstrText = (BSTR)m_wParam;
                    if (bstrText)
                    {
                        SysFreeString(bstrText);
                    }
                }
                break;
            }
        }
        TCHAR  m_szUserId[256];
        long   m_lOpenMode;
    };
}

#endif // PRIVATECHAT_INC_H_3CF03C58_0A52_4d45_A094_D915DF7C1364