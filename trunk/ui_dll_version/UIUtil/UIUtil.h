// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� UIUTIL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// UIUTIL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef UIUTIL_EXPORTS
#define UIUTIL_API __declspec(dllexport)
#else
#define UIUTIL_API __declspec(dllimport)
#endif

// �����Ǵ� UIUtil.dll ������
// class UIUTIL_API CUIUtil {
// public:
// 	CUIUtil(void);
// 	// TODO: �ڴ�������ķ�����
// };
// 
// extern UIUTIL_API int nUIUtil;
// 
// UIUTIL_API int fnUIUtil(void);
