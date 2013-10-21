#pragma  once
#include "UISDK\Control\Inc\Interface\ILEDCtrl.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK\Control\Src\Control\Button\button.h"

namespace UI
{
    //
    //  1. ÿһ��Ŀ�ȿ��Բ�һ�������߶ȶ�һ�� ��Ĭ�Ͽؼ���ͼƬ���Ǻ���ģ�
    //  2. ֧�ֵ��������ͼƬ(normal/hover/press/disable)
    //

    class LEDExCtrl : public ButtonBase
    {
    public:
        LEDExCtrl();
        struct POSRANGE
        {
            int x;
            int w;
        };

        UI_DECLARE_OBJECT3(LEDExCtrl, OBJ_CONTROL|CONTROL_LEDCTRL, _T("UICtrl/Control"))

        UI_BEGIN_MSG_MAP
            UIMSG_WM_PAINT(OnPaint)	
            UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
            UIMSG_WM_QUERYINTERFACE(QueryInterface)
            UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
            UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
            UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UI_END_MSG_MAP_CHAIN_PARENT(ButtonBase)

        void  SetILEDExCtrl(ILEDExCtrl* p) { m_pILEDExCtrl = p;  }

    public:
        void  SetItemInfo(LEDEX_ITEM_INFO* pInfoArray, int nCount);
        void  SetItemInfo2(const TCHAR* szCharIndex, const TCHAR* szPosIndex);
        void  SetItemInfo3(const TCHAR* szCharIndex);
        void  SetText(const TCHAR* szText, bool bUpdate=true);

    protected:
        void  OnPaint(IRenderTarget*);
        void  ResetAttribute();
        void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
        void  GetDesiredSize(SIZE* pSize);

    protected:
        ILEDExCtrl*  m_pILEDExCtrl;

        String   m_strText; 
        bool     m_bSameWidth;

        map<TCHAR, POSRANGE>  m_mapIndex;    // �ַ�����ͼƬλ�õ�ӳ��
        vector<POSRANGE>      m_vecPos;      // ��ǰҪ��ʾ��λ���б�
    };


}