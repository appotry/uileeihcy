#pragma  once
#include "UISDK\Control\Inc\Interface\iledctrl.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
namespace UI
{
//
//  Ĭ��ÿһ��Ŀ�ȡ��߶ȶ�һ��
//

class LEDCtrl : public MessageProxy
{
public:
	LEDCtrl();

    UI_DECLARE_OBJECT3(LEDCtrl, OBJ_CONTROL|CONTROL_LEDCTRL, _T("UICtrl/Control"))

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)	
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(LEDCtrl, IControl)

    void  SetILEDCtrl(ILEDCtrl* p) { m_pILEDCtrl = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

public:
    void     SetIndexMap(const TCHAR* szText);
    void     SetText(const TCHAR* szText, bool bUpdate=true);

protected:
	void  OnPaint(IRenderTarget*);
	void  ResetAttribute();
	void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
	void  GetDesiredSize(SIZE* pSize);
    void  OnLButtonUp(UINT nFlags, POINT point);

    void  OnClicked(POINT* pt);

protected:
    ILEDCtrl*  m_pILEDCtrl;
	IImageListRender*  m_pImagelist;  // �ô�ֱ�ӽ���m_foregndRender�����Ҫ�����ļ���Ӧ�ý�foregnd render����Ϊimage list �б�
	map<TCHAR,int>    m_mapIndex;    // �ַ�����ͼƬ������ӳ��
	
	vector<int>       m_vecIndex;    // ��ǰҪ��ʾ����������
	String            m_strText; 
};


}