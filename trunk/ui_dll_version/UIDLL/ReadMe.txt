δ���ǵ������⣺
	1. ��ݼ���ӳ�䣬�Ƿ���Ҫ�����һ��xml�ļ�
	2. DoModal���˳���û������
	
	
��ע��
1. Q: ��ô�����VS2008�������ɵĳ����vc���п��������
   A: ����������󼴡�ʹ�ñ�׼Windows�⡱�Ĺ���Ҫ������msvcr90.dll������Ϊ�����Ŀ����VC2008�������Զ������������ϵ��
	   ��ô����أ��ܼ򵥣��������£�
		a. ����ĿĬ��ֵ��MFCʹ�øĳɡ��ھ�̬����ʹ��MFC��
		b. ����һ�£����ɹ���Ҫ���������û���ɹ�
		c. ����ĿĬ��ֵ��MFCʹ�øĻ�ȥ������ʹ�ñ�׼Windows�⡱�����룬�ɹ�����Ŀ�Ͳ�����msvcr90.dll������

 
2.Q: ���ؼ̳е�ʱ������ʲô���⣿
  A: ���³�����룺
		class Object
		 {
		 };
		class Message
		 {
		 public:
			 virtual long ProcessMessage() {return 0;};
		 };
		 

		 class Window : public Message
		 {
		 };

		 class LoginWindow : public Object,public Window
		 {
		 public:
			 long ProcessMessage() 
			 {
				 cout<< "LoginWindow:ProcessMessage" << endl;
				 return 0;
			 }
		 };

		int _tmain(int argc, _TCHAR* argv[])
		{
			LoginWindow * p = new LoginWindow();

			Object* pObject = p;
			Message* pMessage =  (Message*)pObject;
			pMessage->ProcessMessage();		// ����ʱ����
			 
			return 0;
		}
		
		����ΪWindow���󶼼̳��� Object��Message����ô�Ϳ������⽫ Window����ת����Object��Message�Ǿͷ����ˡ�
		����ĳһ��ʱ��Window����ת����һ��Object*�������ʱ�����ٵ��ã�
		Message* pMessage =  (Message*)pObject;
		��ô�͸�ʱ�������ת����û����˼����ֻ��һ��ָ������ͱ仯��
		
		���������������һ���ı䣬��Objectȥ��Message������������Message��Object�����������⣬����Message��OBJ_TYPE
		�Ͳ���ȷ����δӶ���̳�����Ƕ�����������ȷ����
		
3.Q: ����EXE�е���DLL�е�һ��SetName( String& str ){ m_str = str; }�������������ˣ�ʲôԭ��
  A: 1. ���뱣֤ m_str �Ĺ��������Ҫô���� exe ��ִ�У�Ҫô���� dll �б�ִ��
     2. ������� SetName ��ʾ�ĵ��ö�ջ����exe�еģ���������dll�У������������ǲ���һ����������,
        ���������û�б�������������ֱ�ӱ�exe����include��
     3. ���� GetName( String& str )Ҳ�ᵼ�±������������ǲ���  GetName( TCHAR* sz, int nSize )
        �ķ�ʽ���ɵ�����������new/delete�ڴ�   
        
        

 4.Q: �ڶ�һ�����������С��ʱ��stretch�ᵼ���п��ܲ�������ɫ�����������п���RGB(255,0,255)����΢�䵭�����������ں͸�����������ɫ��ϣ�����˽����ϻ���ֺ�͸��ɫ������ɫ�����½���͸��ʧ�ܡ�
   A: 1. ����İ취���ǣ���ӦOnGetMinMaxInfo��Ϣ�����ƴ��ڵ���С��СΪ��ı���ͼ��С��ʹ���ڳ����о���û�ж� ͼƬ�д�͸��ɫ������� ��С�� ����������£�����ʹ��TransparentBltҲ��û�õģ���Ϊ͸��ɫ�Ѿ�����ԭ����RGB(255,0,255)�ˡ�
      2. ��Ҫ���� HDC ������ģʽ����api��ֱ�ӵ���
   
5. Q: �ǲ���DLL���������ã����ǲ��õ���һ������Ӧ�ӿڣ��ٵ��ýӿںã�
   A: ��¶���û�����ϢԽ��Խ�ã����ֻ����һ��������û�ʹ�ü��ɣ������Ǳ�¶������Ľӿ�??
      ���ӿڸ���������չ...
 
6. Q: ���ʵ�ֱ༭�������ʹ��һ��xml����� 
   A: �ڴ򿪱�һ����Դ����ʱ�����ж�һ�������Դ��ʹ�õ��ļ��Ƿ��Ѿ��д򿪹�������򿪹��򵯳���ʾȡ����
 
7. Q: SetCursorΪʲô��1px��ƫ��
   A: ����Ĭ�ϵ�WM_SETCURSOR��������WM_MOUSEMOVE��Ϣ֮ǰ�������⽫���»�û�м����HOVER��PRESS�������Ӧ��SETCURSOR����˻������
	  �޸İ취�ǣ���ԭʼ��WM_SETCURSOR��ֱ�ӷ��أ�������Ȼ���ڴ�����MOUSEMOVE֮��POSTһ��WM_SETCURSOR������WPARAMΪ0����������
	  ԭʼ����Ϣ���������Ϣ������Ӧ�����ʽ������ 
	  
	  --> �������޸ģ���ʵ��windowless richeditʱ�����ܲ�����ʱ����setcursor�İ취��������
          ѡ���ı�ʱ�����ʽһֱ��˸������޸ķ���Ϊ���ȷ��ͣ�Ȼ����WM_MOUSEMOVE�м�����hover obj
          �����˱仯�Ļ��������ٷ���һ��WM_SETCURSOR
	  
8. Q: Ϊʲô���� ::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)pProc); ���໯һ���Ի���������ʾ��������
   A: ��ΪҪʹ��DWLP_DLGPROC��������ʹ�� GWLP_WNDPROC; ���⣬ģ̬�Ի���Ĵ��ڹ���ֻҪֱ�ӷ���0���ɣ����ڲ�����
      ��һ���Ĵ�����������Բ�����ӦWM_PAINT��ϢҲ���ᵼ��CPU 100%�����õ���IsDialogMessageҲ���Զ����������Ϣ��
 
9. Q: ������õ����ؼ�������
   A: ���뻻���޹ص����ݱ����ڿؼ����У��뻻���൱�����ݷ���IObjectView�������С�
      ÿ�λ���ʱ��ֻ��Ҫȥdelete/new IObjectView�����ؼ��಻���޸ġ�ͬ������Ҳ�ܸ��õ�ȥ������չ
	  
  
 δʵ�֣�
 1. Min max width/height ����
 3. PrepareDC( hDC ); ��Ӧ��SaveDC RestoreDC�Ƿ���ȷʹ��
 4. ��uibuilder����ʱ��ui����־û������� console ��
 5. ���϶����ڴ�С��ʱ��EDITλ�ñ仯ʱ��CARATû�����߱仯
 7. ����font��Ч�����Ƿ���Ըı�
 10.�ɷ��ǽ�ID���Ͷ�ӳ��Ϊһ�����������������ַ���ȥƥ�䣿
 11.xml�ļ��ɷ��Ƿֿ���������ȫ����һ���ļ��У�
 12.�����е�xmldao����find_elem_in_xml/find_elem_under_style��ʵ��
 13.load xmlʱ��û�ж�ת����Сд��
 14.��pojp.h -> pojo.cpp
 15.�Ż�mapXml2Class
 17.CMainFrame::OnMenuSaveֻʵ���˱������е��ļ���û��ʵ�ֱ��浱ǰ�ļ�
 18.ʹ�ÿ�ݼ���Դ��ʵ�ֿ�ݼ� ctrl+s save 
 19.style �༭����Ӧ�ÿ��Ա༭�����õ������ԣ�Ҳ�޷���������õ�����������
 21.PrepareDC�� oldFont ֮��Ķ�����λָ���
 25.�Ż�SetWindowRgn���Ź���
 27.Gdi+�����MeasureStringƫ���ԭ���Լ�DrawString�Ų��µ�ԭ��
 28.����ʱ���������ȴ��������ڣ��ٹرյڶ������ڣ�������������
 29.CustomWindow�ձ�LayeredWindow
 30.�ṩһ��������DoVerb( "action", xxx ); "press" "click" "unpress" "hover" "unhover"
 31.���ǽ�WndProc����virtual
 32.Direct3D + GDI 
    1). ��Render���е� EndSceneǰ��ӣ�
		IDirect3DSurface9*  pSurface = NULL;
		g_pD3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pSurface);
		
		HDC hDC = NULL;
		pSurface->GetDC(&hDC);
		....
		UpdateLayeredWindow(,NULL,,.... hDC,... );
		pSurface->ReleaseDC(hDC);
		
	2). d3dpp.Flags = D3DPRESENTFALG_LOCKABLE_BACKBUFFER;
33. visible enable  �ļ̳й�ϵ 	
34. ���������л������������ʱ�򣬽�����ʾ�쳣
35. ȥ��һ�����ڵ�͸������
To make this window completely opaque again, remove the WS_EX_LAYERED bit by calling SetWindowLong and then ask the window to repaint. Removing the bit is desired to let the system know that it can free up some memory associated with layering and redirection. The code might look like this:

// Remove WS_EX_LAYERED from this window styles
SetWindowLong(hwnd, 
              GWL_EXSTYLE,
              GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);

// Ask the window and its children to repaint
RedrawWindow(hwnd, 
             NULL, 
             NULL, 
             RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
 
36. �����£�ȡ��PrePareDC��ÿ��������һ��FONT*���ԣ�ÿ�δ���һ�����󣬣����ᷢ��һ��WM_SETFONT����Ϣ
39. �������е�virtual�������ܲ���Ҫ��ȫ��������virtual������
	Ϊʲô�������麯����ʵ����Ϣӳ�䣿 -- �������MFC	
	��Ȼ��һ��������C++��OOP��񣬵����Դ������⡣���ס���麯����������麯����(virtual fuction table)
	ʵ�ֵģ���ÿ�������඼���һ���麯����Ŀ������麯�����е�ÿ����ڶ���һ��4�ֽڵ�ָ�롣��һ���麯��
	Ҫ�������Ϣ�������������Ŀ��4������ÿ����ͻ����麯�����д��������Ķ����ֽڡ�
	
	����������Ҫ���麯���������Ϳ���ͨ����̬���Ƶ����������������仰˵��MFC�����Լ�������ɾ���ܶ��CObject
	�������ࡣ

40. Windows ��ݼ���ȫ
	http://msdn.microsoft.com/en-us/library/ms971323.aspx
41. ������ʧ��ʱ������ʾFOCUS
42. ��ť�Ŀո��
43. Win7 Style Builder <-- win7��ť�ز�
44. �������ڵ�����ı�����ͬ�����������Ӷ�����ô�����Ӷ�����������ģ�
45. ���ǽ���ť�Ķ�̬�������ɿɸ��õġ�����EDIT�Ķ�̬������
46. ���ǵ���������ʱ��ģ��һ�� MSG_WM_SHOWWINDOW����Ϣ
47. �޸���GetHDC������ѡ�񣬻��theme�������ʲôӰ�죿
48. ֱ��UpdateObjectˢ���Ӷ���ʱ����ˢ���ڸ���������档���罫progress�߶ȵ�С�������
    ��ť�ͻ�ˢ�µ����档��panel��С��С������İ�ť��ˢ�µ�����
51. FONTCOLORLISTTEXTRENDER <-- xml���Բ���
52. ��Ҫ����һ�±��������浱ǰƤ����hueֵ��������ص�ͼƬ��Ҫ����ת�� ��ͬʱ��active skin����Ҳ��һ�£�
53. �ָ�skinh
54. theme change��Ϣ����Ӧ
55. render����һ���ӿڣ��ж�DrawState�����״̬�Ƿ�֧��
56. LOG ֧��__FUNCTION__ ������
57. ����֧������Ч��
58. icon��ϵͳ16λɫ������ʾȫ��
59. ȥ��ResetAttribute����
60. ��BkRender, ForeRender�Ƶ�IObjectRender���У�����controlRender�ɼ̳�
61. LISTBOX�ľֲ�ˢ��
62. ��Ҫһ�׸�����classname <-> xmlname ��ע��ӳ���ϵ
64. ����DrawThemexxx��Gdiplus�����Կ����Ȼ���һ��HDC��BITMAP���棬Ȼ��ת��Gdiplus::Bitmap
65. TextRender���Կ�������һ��������ӰЧ������XP STYLE
66. Edit�У�����ı��/������m_nCaretHeightû�и��£������»���λ�ó���
67. WPF�е�Ԫ��Visibility������������ֵ,֮ǰ��Ҳû����ϸ����,һ��Ͷ���Hidden��(Flash��ϰ��),����ʵ����,CollapsedҲ�зǳ�����ô�,hidden������Ԫ����Ϊ������,����Ԫ���ڻ�������Ȼ��ռ�пռ�,��Collapsed�Ļ�,�ڲ����ӵĻ�����,���ܽ�Ԫ���ڻ����ϵ�ռλ�����,Ԫ�س��ײ�Ӱ�컭��.����,ĳЩʱ�������Collapsed��Ϊ����.����һ��StackPanel��ʱ��������,����������StackPanel�е�ʱ��,�м�Ķ������Hidden,����ռ��Stack�е�λ��,��Collapsed�Ļ�,����Ķ���ͻἷ����(�е���DIV��..)  
	VISIBILITY_HIDDEN,     // ���ɼ���ռ�ò���
	VISIBILITY_Visible     // �ɼ���  ռ�ò���
	VISIBILITY_COLLAPSED,  // ���ɼ�����ռ�ò���
68. ������m_pLayout�Ƿ���Բ���Ϊһ����Ա�������֣�������Ϊһ��helper����֣�
69. Ϊʲô��customwindow������һ�£��������һ��WM_MOUSEMOVE����Ϣ��
70. RICHEDIT�ڹ����ֵ�ʱ������RICHEDIT����������ϻ�����һ������ͼ�β�ˢ�¡�	
71. Windowless richedit�����嶨�壬��������ʾ����ק���֣�ֻ��ģʽ������ͼƬ��GIF
72. ���ʵ�ֲ˵��Ļ������������еĲ˵���������XML�еģ���ôû��������XML�еĶ����Ƿ��û��ʵ�ֻ�������
	
���ʣ�
1. Message���Ƿ���Ҫһ�� m_pCurMsg��Ա������
 
��ǰ���ڽ��е�����

 
 
Finish
 
8. �ӿؼ����Գ���parent��������л��ƣ�������ˢ�£�����sliderctrl�Ļ�����ť
 
9. Ϊÿһ���ؼ��ṩһ����ֱ��ˢ�½���ķ�����������ȥ����UpdateObject���������еĿؼ�����
 
38. <����>��������Ĵ������� WM_CREATE/WM_INITDIALOG�У���������ͷŷ��� WM_NCDESTROY��    
 
37. ��WTL��StartDialogProcģʽ���WindowBase::Create�У����������������е���Ϣ�ˡ�
    ����thisָ��Ĵ��ݣ����Կ���map<HTREAD,pThis>�ķ�ʽ��+ �߳�ͬ��
    --> ʹ��ATL�ṩ�ķ�ʽ���ڴ�������֮ǰʹ��UI_AddCreateWndData/UI_ExtractCreateWndData
    
6. ToolTip ���
	--> δʹ�ò���ķ�ʽ��������MouseManager����ʹ��SetTimer��ʵ��
    
 WM_IME_STARTCOMPOSITION,WM_IME_COMPOSITION,WM_IME_ENDCOMPOSITION
 
49. ȡ��ImageSliderCtrl ->  backrender!
50. GDIPLUS pojo_imageitem changeskinH

63. �޸�optiondlg�н����л���ˢ�µ����� --> Groupboxû������Ϊ͸������

*69. �����������ԣ���Ҫ�Ĺ��캯���к�resetattrib�����ж�дһ�ݴ��룬�����׵��²�ƥ�䣬�����޸�	
*70. ����������б�ؼ���ʱ�򣬻��ܵ����麯��OnDeleteItem??,ͬʱ���DestroyUI���麯���̳�����
     ����һ��UIObjCreator InitialConstruct FinalConstruct FinalRelease

CMFCPopupMenu::RecalcLayout, afxmenupopup.cpp L630
CRect rectScreen;

	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	if (GetMonitorInfo(MonitorFromPoint(m_ptLocation, MONITOR_DEFAULTTONEAREST), &mi))
	{
		rectScreen = mi.rcWork;
	}
	else
	{
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0);
	}