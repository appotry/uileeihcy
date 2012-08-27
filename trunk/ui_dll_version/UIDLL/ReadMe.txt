未考虑到的问题：
	1. 快捷键的映射，是否需要再添加一个xml文件
	2. DoModal的退出还没有做好
	
	
备注：
1. Q: 怎么解决用VS2008编译生成的程序对vc运行库的依赖？
   A: 造成这种现象即“使用标准Windows库”的工程要依赖于msvcr90.dll，是因为这个项目是由VC2008创建，自动有这个依赖关系。
	   怎么解决呢？很简单，步骤如下：
		a. 将项目默认值的MFC使用改成“在静态库中使用MFC”
		b. 编译一下，不成功不要紧。这里就没法成功
		c. 将项目默认值的MFC使用改回去，即“使用标准Windows库”；编译，成功，项目就不再有msvcr90.dll依赖了

 
2.Q: 多重继承的时候会出现什么问题？
  A: 如下程序代码：
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
			pMessage->ProcessMessage();		// 运行时错误
			 
			return 0;
		}
		
		若以为Window对象都继承了 Object和Message，那么就可以任意将 Window对象转换成Object或Message那就犯错了。
		若在某一个时候，Window对象被转换成一个Object*，你这个时候若再调用：
		Message* pMessage =  (Message*)pObject;
		那么就个时候的类型转换就没有意思，就只是一个指针的类型变化。
		
		所以在这里会做出一个改变，将Object去从Message派生。（若将Message从Object派生会有问题，首先Message的OBJ_TYPE
		就不正确，其次从对象继承意义角度来看都不正确。）
		
3.Q: 我在EXE中调用DLL中的一个SetName( String& str ){ m_str = str; }方法，最后崩溃了，什么原因？
  A: 1. 必须保证 m_str 的构造和析构要么都在 exe 中执行，要么都在 dll 中被执行
     2. 如果发现 SetName 显示的调用堆栈是在exe中的，而不是在dll中，检查这个方法是不是一个导出方法,
        （这个类有没有被导出），否则直接被exe工程include了
     3. 调用 GetName( String& str )也会导致崩溃，因此最后是采用  GetName( TCHAR* sz, int nSize )
        的方式，由调用者来负责new/delete内存   
        
        

 4.Q: 在对一个区域进行缩小的时候，stretch会导致有可能产生的颜色不纯正，即有可能RGB(255,0,255)会稍微变淡（可能是由于和附近的像素颜色混合），因此界面上会出现和透明色差不多的颜色，导致界面透明失败。
   A: 1. 解决的办法就是：响应OnGetMinMaxInfo消息，限制窗口的最小大小为你的背景图大小，使得在程序中尽量没有对 图片中带透明色区域进行 缩小。 在这种情况下，就算使用TransparentBlt也是没用的，因为透明色已经不是原来的RGB(255,0,255)了。
      2. 需要设置 HDC 的拉伸模式！有api可直接调用
   
5. Q: 是采用DLL导出函数好，还是采用导出一个个相应接口，再调用接口好？
   A: 暴露给用户的信息越少越好，因此只保留一个对象给用户使用即可，而不是暴露出更多的接口??
      但接口更灵活，更好扩展...
 
6. Q: 如何实现编辑多个配置使用一个xml的情况 
   A: 在打开别一个资源窗口时，先判断一下这个资源所使用的文件是否已经有打开过，如果打开过则弹出提示取消打开
 
7. Q: SetCursor为什么有1px的偏差
   A: 由于默认的WM_SETCURSOR函数是在WM_MOUSEMOVE消息之前触发，这将导致还没有计算出HOVER、PRESS对象就响应了SETCURSOR，因此会有误差
	  修改办法是，在原始的WM_SETCURSOR处直接返回，不处理。然后在处理完MOUSEMOVE之后POST一个WM_SETCURSOR，其中WPARAM为0，以区分于
	  原始的消息，在这个消息中再响应鼠标样式的设置 
	  
	  --> 方案已修改：在实现windowless richedit时，不能采用延时发送setcursor的办法，否则在
          选择文本时鼠标样式一直闪烁。因此修改方案为：先发送，然后在WM_MOUSEMOVE中检测如果hover obj
          发生了变化的话，重新再发送一次WM_SETCURSOR
	  
8. Q: 为什么调用 ::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)pProc); 子类化一个对话框总是显示不正常？
   A: 因为要使用DWLP_DLGPROC，而不是使用 GWLP_WNDPROC; 另外，模态对话框的窗口过程只要直接返回0即可，其内部会做
      进一步的处理。例如你可以不用响应WM_PAINT消息也不会导致CPU 100%；不用调用IsDialogMessage也会自动处理键盘消息。
 
9. Q: 如果更好的做控件换肤？
   A: 将与换肤无关的数据保存在控件类中，与换肤相当的数据放在IObjectView派生类中。
      每次换肤时，只需要去delete/new IObjectView，而控件类不用修改。同样这样也能更好的去进行扩展
	  
  
 未实现：
 1. Min max width/height 限制
 3. PrepareDC( hDC ); 对应的SaveDC RestoreDC是否正确使用
 4. 由uibuilder启动时，ui的日志没有输出到 console 中
 5. 在拖动窗口大小的时候，EDIT位置变化时，CARAT没有随者变化
 7. 测试font的效果，是否可以改变
 10.可否考虑将ID类型都映射为一个整数，而不是用字符串去匹配？
 11.xml文件可否考虑分开，而不是全放在一个文件中？
 12.把所有的xmldao都用find_elem_in_xml/find_elem_under_style来实现
 13.load xml时，没有都转换成小写的
 14.将pojp.h -> pojo.cpp
 15.优化mapXml2Class
 17.CMainFrame::OnMenuSave只实现了保存所有的文件，没有实现保存当前文件
 18.使用快捷键资源来实现快捷键 ctrl+s save 
 19.style 编辑器不应该可以编辑继续得到的属性，也无法保存继续得到的属性问题
 21.PrepareDC的 oldFont 之类的东西如何恢复？
 25.优化SetWindowRgn缩放功能
 27.Gdi+下面的MeasureString偏大的原因，以及DrawString放不下的原因
 28.换肤时，崩溃。先打开三个窗口，再关闭第二个窗口，换肤，崩溃。
 29.CustomWindow收编LayeredWindow
 30.提供一个方法：DoVerb( "action", xxx ); "press" "click" "unpress" "hover" "unhover"
 31.考虑将WndProc做成virtual
 32.Direct3D + GDI 
    1). 在Render当中的 EndScene前添加：
		IDirect3DSurface9*  pSurface = NULL;
		g_pD3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pSurface);
		
		HDC hDC = NULL;
		pSurface->GetDC(&hDC);
		....
		UpdateLayeredWindow(,NULL,,.... hDC,... );
		pSurface->ReleaseDC(hDC);
		
	2). d3dpp.Flags = D3DPRESENTFALG_LOCKABLE_BACKBUFFER;
33. visible enable  的继承关系 	
34. 从无主题切换到其他主题的时候，界面显示异常
35. 去除一个窗口的透明属性
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
 
36. 考虑下，取消PrePareDC，每个对象都有一个FONT*属性，每次创建一个对象，，都会发送一个WM_SETFONT的消息
39. 考虑所有的virtual函数，能不需要的全都不做成virtual函数！
	为什么不能用虚函数来实现消息映射？ -- 深入解析MFC	
	虽然这一方法符合C++和OOP风格，但它仍存在问题。请记住，虚函数是用类的虚函数表(virtual fuction table)
	实现的，而每个派生类都会带一个虚函数表的拷贝。虚函数表中的每个入口都是一个4字节的指针。数一下虚函数
	要处理的消息个数，将这个数目乘4，这样每个类就会在虚函数表中带来大量的额外字节。
	
	但析构函数要是虚函数，这样就可以通过多态机制调用析构函数。换句话说，MFC打算自己创建和删除很多从CObject
	派生的类。

40. Windows 快捷键大全
	http://msdn.microsoft.com/en-us/library/ms971323.aspx
41. 当窗口失活时，不显示FOCUS
42. 按钮的空格键
43. Win7 Style Builder <-- win7按钮素材
44. 当父窗口的字体改变后，如何同步给人它的子对象？怎么区分子对象是有字体的？
45. 考虑将按钮的多态背景做成可复用的。例如EDIT的多态背景。
46. 考虑当对象隐藏时，模拟一个 MSG_WM_SHOWWINDOW的消息
47. 修改了GetHDC的字体选择，会对theme绘制造成什么影响？
48. 直接UpdateObject刷新子对象时，会刷新在父对象的外面。例如将progress高度调小，上面的
    按钮就会刷新到外面。将panel大小调小，上面的按钮会刷新到外面
51. FONTCOLORLISTTEXTRENDER <-- xml属性测试
52. 需要增加一下变量，保存当前皮肤的hue值，后面加载的图片都要进行转换 （同时将active skin功能也做一下）
53. 恢复skinh
54. theme change消息的响应
55. render增加一个接口，判断DrawState，这个状态是否支持
56. LOG 支持__FUNCTION__ 参数！
57. 考虑支持缩放效果
58. icon在系统16位色下面显示全白
59. 去掉ResetAttribute方法
60. 将BkRender, ForeRender移到IObjectRender当中，子类controlRender可继承
61. LISTBOX的局部刷新
62. 需要一套更灵活的classname <-> xmlname 的注册映射关系
64. 关于DrawThemexxx的Gdiplus，可以考虑先画在一个HDC的BITMAP上面，然后转成Gdiplus::Bitmap
65. TextRender可以考虑增加一个文字阴影效果，仿XP STYLE
66. Edit中，字体改变后/换肤后，m_nCaretHeight没有更新，将导致绘制位置出错
67. WPF中的元素Visibility可视性有三种值,之前我也没有仔细看过,一般就都用Hidden了(Flash的习惯),但事实上呢,Collapsed也有非常大的用处,hidden仅仅将元素设为不可视,但是元素在画面上依然将占有空间,而Collapsed的话,在不可视的基础上,还能将元素在画面上的占位符清除,元素彻底不影响画面.所以,某些时候可能用Collapsed更为合理.在做一个StackPanel的时候最明显,三个对象在StackPanel中的时候,中间的对象如果Hidden,还将占有Stack中的位置,而Collapsed的话,下面的对象就会挤上了(有点像DIV了..)  
	VISIBILITY_HIDDEN,     // 不可见，占用布局
	VISIBILITY_Visible     // 可见，  占用布局
	VISIBILITY_COLLAPSED,  // 不可见，不占用布局
68. 考虑下m_pLayout是否可以不作为一个成员变量出现，而是作为一个helper类出现？
69. 为什么在customwindow上面点击一下，还会产生一个WM_MOUSEMOVE的消息？
70. RICHEDIT在光标出现的时候，隐藏RICHEDIT，结果窗口上会遗留一个光标的图形不刷新。	
71. Windowless richedit：字体定义，滚动条显示，拖拽文字，只读模式，插入图片，GIF
72. 如何实现菜单的换扶（不是所有的菜单都配置在XML中的，那么没有配置在XML中的对象是否就没法实现换肤？）
	
疑问：
1. Message类是否需要一个 m_pCurMsg成员变量？
 
当前正在进行的任务

 
 
Finish
 
8. 子控件可以超出parent的区域进行绘制，但不会刷新，例如sliderctrl的滑动按钮
 
9. 为每一个控件提供一个能直接刷新界面的方法，而不用去调用UpdateObject而触发所有的控件更新
 
38. <紧急>，将对象的创建放在 WM_CREATE/WM_INITDIALOG中，将对象的释放放在 WM_NCDESTROY中    
 
37. 将WTL的StartDialogProc模式搬进WindowBase::Create中，这样就能拦截所有的消息了。
    至于this指针的传递，可以考虑map<HTREAD,pThis>的方式，+ 线程同步
    --> 使用ATL提供的方式，在创建窗口之前使用UI_AddCreateWndData/UI_ExtractCreateWndData
    
6. ToolTip 插件
	--> 未使用插件的方式，而是在MouseManager当中使用SetTimer来实现
    
 WM_IME_STARTCOMPOSITION,WM_IME_COMPOSITION,WM_IME_ENDCOMPOSITION
 
49. 取消ImageSliderCtrl ->  backrender!
50. GDIPLUS pojo_imageitem changeskinH

63. 修改optiondlg中焦点切换不刷新的问题 --> Groupbox没有设置为透明导致

*69. 现在重置属性，需要的构造函数中和resetattrib函数中都写一份代码，很容易导致不匹配，急需修改	
*70. 如何在析构列表控件的时候，还能调用虚函数OnDeleteItem??,同时解决DestroyUI的虚函数继承问题
     增加一人UIObjCreator InitialConstruct FinalConstruct FinalRelease

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