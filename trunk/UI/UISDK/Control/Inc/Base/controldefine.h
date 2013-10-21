#ifndef CONTROLDEFINE_H_92376BDC_456B_4a0a_9C9F_3656DEDD4F79
#define CONTROLDEFINE_H_92376BDC_456B_4a0a_9C9F_3656DEDD4F79

// ���뵼������
#ifdef UICTRLS_EXPORTS
#define UICTRLAPI __declspec(dllexport)
#else
#define UICTRLAPI __declspec(dllimport)
#endif

namespace UI
{
// �����λ��Control Sub Type��ʶ

// button style
#define BUTTON_STYLE_HAVE_TEXT_FIRST    0x00    // ��־�����ݵİ�ť����Щ��ť����Ҫ��������textrender
#define BUTTON_STYLE_PUSHBUTTON         0x00    // ��ͨ��ť
#define BUTTON_STYLE_RADIOBUTTON        0x01    // ��ѡ��ť
#define BUTTON_STYLE_CHECKBUTTON        0x02    // ��ѡ��ť
#define BUTTON_STYLE_HYPERLINK          0x03    // �����Ӱ�ť
#define BUTTON_STYLE_MENUBUTTON         0x04    // �˵���ť
#define BUTTON_STYLE_SPLITMENUBUTTON    0x05    // ��ť+�˵�
#define BUTTON_STYLE_TABCTRLBUTTON      0x06    // TAB�ؼ��ϵ�ӭ����
#define BUTTON_STYLE_HAVE_TEXT_LAST     0x80

#define BUTTON_STYLE_COMBOBOX           0x81    // ��Ͽ��еİ�ť
#define BUTTON_STYLE_SCROLLLINEUP       0x82    // ��������ť
#define BUTTON_STYLE_SCROLLLINEDOWN     0x83
#define BUTTON_STYLE_SCROLLLINELEFT     0x84
#define BUTTON_STYLE_SCROLLLINERIGHT    0x85
#define BUTTON_STYLE_HSCROLLTHUMB       0x86    // ��������ק��ť
#define BUTTON_STYLE_VSCROLLTHUMB       0x87    // ��������ק��ť
#define BUTOTN_STYLE_SLIDERTRACKBUTTON  0x88    // ����������İ�ť

#define BUTTON_STYLE_NOTDEFPUSH         0x0100  // ����ΪĬ�ϰ�ť
#define BUTTON_STYLE_CLICK_ONDOWN       0x0200  // ����갴��ʱ�ʹ���click����������굯��ʱ����

#define SLIDER_STYLE_POINT_LEFT         0x0100    // ���鰴ť��ͷָ����� Ĭ��Ϊ��ָ��
#define SLIDER_STYLE_POINT_TOP          0x0100    // ���鰴ť��ͷָ���ϲ�
#define SLIDER_STYLE_POINT_RIGHT        0x0200    // ���鰴ť��ͷָ���Ҳ�
#define SLIDER_STYLE_POINT_BOTTOM       0x0200    // ���鰴ť��ͷָ���²�
#define SLIDER_STYLE_SHOWTICKS          0x0400    // ��ʾ�̶ȣ�Ĭ��ֻ��ʾ��һ�������һ��ֵ�Ŀ̶�
#define SLIDER_STYLE_AUTOTICKS          0x0800    // �Զ���ʾ�̶ȣ�Ĭ��ÿ��һ��ֵ��ʾһ���̶�

    // edit style
#define EDIT_STYLE_MASK                 0x00FF    // �༭������ʹ�õ�λ
#define EDIT_STYLE_COMBOBOX             0x0001    // ��Ͽ���ʹ�õı༭��
#define EDIT_STYLE_WANTTAB              0x0100    // ����TAB����Ϣ
#define EDIT_STYLE_READONNLY            0x0200    // ֻ��

    // tree style
#define TREE_STYLE_HASLINE              0x0100    // ��������

    // listbox style

    // menu style
#define MENU_STYLE_AUTO_DELETE_SUBMENU  0x1000    // ����������ʱ���Ƿ��Զ��ͷ��Ӳ˵��ڴ档��Ҫ���ڴ�xml��load�Ĳ˵�
    // ��Ϊ���ڲ��������Ӳ˵����ڲ����������Ӳ˵������ⲿ�Լ�create����
    // combobox style
#define COMBOBOX_STYLE_DROPDOWN         0x0100    // ���ɱ༭�����б�
#define COMBOBOX_STYLE_EDITDROPDOWNLIST 0x0200    // �ɱ༭�����б�

    // headerctrl style
#define HEADERCTRL_STYLE_CANDRAG        0x0100    // ����ק���п��

// tab ctrl
#define TABCTRL_STYLE_BOTTOM            0x0100    // ��ťλ���²�

}
#endif  // CONTROLDEFINE_H_92376BDC_456B_4a0a_9C9F_3656DEDD4F79