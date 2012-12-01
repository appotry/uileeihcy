未考虑到的问题：
	1. 快捷键的映射，是否需要再添加一个xml文件
	2. DoModal的退出还没有做好
	
 未实现：
 4. 由uibuilder启动时，ui的日志没有输出到 console 中
 5. 在拖动窗口大小的时候，EDIT位置变化时，CARAT没有随者变化
 7. 测试font的效果，是否可以改变
 10.可否考虑将ID类型都映射为一个整数，而不是用字符串去匹配？
 11.xml文件可否考虑分开，而不是全放在一个文件中？
 12.把所有的xmldao都用find_elem_in_xml/find_elem_under_style来实现
 17.CMainFrame::OnMenuSave只实现了保存所有的文件，没有实现保存当前文件
 18.使用快捷键资源来实现快捷键 ctrl+s save 
 19.style 编辑器不应该可以编辑继续得到的属性，也无法保存继续得到的属性问题
 25.优化SetWindowRgn缩放功能
 27.Gdi+下面的MeasureString偏大的原因，以及DrawString放不下的原因
    通过使用Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());可以解决宽度偏大的问题
    但是高度偏大的问题还是没有找到解决方法。例如宋体字，绘制出来的效果下面就多出2px的空白
    
 28.换肤时，崩溃。先打开三个窗口，再关闭第二个窗口，换肤，崩溃。
    20121020， 仍然存在崩溃。
    
 30.提供一个方法：DoVerb( "action", xxx ); "press" "click" "unpress" "hover" "unhover"
    模拟IAccessible/MSAA，实现自动化TEST。可参考Window7 SDK的sample代码：
    C:\Program Files\Microsoft SDKs\Windows\v7.1\Samples\winui\msaa\CPP
    C:\Program Files\Microsoft SDKs\Windows\v7.1\Samples\winui\uiautomation\simpleuiaprovider
    
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

34. 从无主题切换到其他主题的时候，界面显示异常
 
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
45. 考虑将按钮的多态背景做成可复用的。例如EDIT的多态背景。
46. 考虑当对象隐藏时，模拟一个 MSG_WM_SHOWWINDOW的消息
47. 修改了GetHDC的字体选择，会对theme绘制造成什么影响？
51. FONTCOLORLISTTEXTRENDER <-- xml属性测试
56. LOG 支持__FUNCTION__ 参数
57. 考虑支持缩放效果
58. icon在系统16位色下面显示全白
59. 去掉ResetAttribute方法？？
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
73. 点击打开一个系统COMBOOX的下拉列表，然后将鼠标放在一个UI控件上面，点击一下，下拉列表消失，再继续点击UI控件，无反应。
    因为没有人去触发一个WM_MOUSEMOVE来set hover对象
78. 将layout.xml增加一个<#include>标签，允许将一些资源抽取出来 
89. 普通窗口拉伸优化！不平滑
92. playlistdlg resize的时候， listctrl 变化不正常，会变黑
93. WINDOW 的两个消息链有点混乱，能不能想办法都合入到processwindowmessage中，ui_begin_msg_map只由外部去处理
95. 逐步将句柄调用方式修改为接口调用方法
102.tooltip高级控件
106.layout.xxx 属性是否是以进行统一规划？
107.ccaret的分层窗口光标在第一行/最后一行显示不下的时候，位置不正确
108.drag drop移动richedit的文字，刷新不同步
109.richedit的横向滚动条range、page与ui控件不同步
110.scrollbar 的 sizebox 绘制，theme都有现成的API，由哪一层来负责绘制？
112.回车换行时，richedit的滚动条没有正确更新
118.复合列表框
123.mwa,wav文件的directsound支持
    已基本支持，但是WAVE文件非PCM格式的问题  5.1声道 C:\Program Files\Microsoft DirectX SDK (March 2009)\Samples\Media\Wavs\MusicSurround.wav
    
124.文件标签的读取，现在仍然不清楚业内是如何去实现的。完全靠读文件吗？

125.在分层窗口隐藏的时候，setwindowpos，然后再showwindow，会先显示旧的内容，然后刷新为新的内容 
	提取出虚函数CustomWindow::OnDrawWindow，在里面不判断窗口的visible属性。继续观察..
	
126.COMBOBOX_STYLE_DROPDOW类型，在改变当前选项后，设置EDIT text时，出现一次乱码。而且鼠标位置没有重新计算

128.将wma音乐文件的后缀名改成.mp3后，播放失败。但千千静音能够正常播放
    判断文件格式的方法太土了。而且directshow也无法识别出来，直接renderfile报错
    // 每一个WMA文件，它的头16个字节是固定的，为十六进制的“30 26 B2 75 8E 66 CF 11 A6 D9 00 AA 00 62 CE 6C”，用来标识这个是否为WMA文件。
    
129.控件的动画效果，如按钮的状态渐变，进度条的渐变
130.SliderCtrlBase其它方向的绘制
131.立体声、单声道界面显示，剩余时间的显示
    注意，有一个5.1声道的wav文件.. 。--> 这个问题牵扯到如何读取一个文件的标签属性的问题。

132.使用GDI+绘制option dialog，效率非常的低。什么原因？
    GetHDC太慢的原因。可以考虑尝试vista/win7的新函数: BeginBufferedPaint  / SetAlpha
    
138.listctrl 的 ctrl+a, delete快捷键
139.richedit 的 gif 图片插入
    
142.存在正在播放某文件，突然显示该文件on_stop了，然后播放了下一首歌曲，这又是怎么回事？
143.均衡器没有实现。目前用的是IIR的基于时域的算法。而其它播放器都是用基本频域的10波段均衡器
144.梦幻星空频谱图是如何实现的？
145.频谱图的FFT结果放大系统究竟该怎么调才对？
146.使用ThemeTooltip后，再换肤。退出时font release崩溃了。应该是没有reset attr的原因吧。到时候查一下。
147.WMA的停止后再播放有问题。SetRange (0)之后，再pushbuffer(0,xxx)，结果WMAFile GetNextSample得到的值并不是0.是因为异步READER导致的吗？	

151.d2d bitmap drawbitmap 的disable处理

152.SetObjectPos里面的bMove||bSize，然后刷新父窗口，会导致滚动条刷新狂慢

153.在_OnPaint中返回0，会导致dialog类型的窗口，被其它窗口覆盖后移出来刷新异常!!!
    暂时修改为返回1

154.TxDraw，在第一次绘制时无视hdc的偏移，这是为嘛？？我又哪搞错了？
    
155.D2D的HLS改变    
	
156.Cursor资源测试

157.gif优化。1.大图片解码速度问题  2. 每个GIF占用的GDI还是偏多，IE中即使增加N个显示，GDI数量也不变
	
158. 延迟创建双缓存，会导致分层窗口在改变大小时，窗口形状也延迟更新
     分层窗口的限制帧数机制未完善，导致窗口刷新不正确    
     	
161.【严重】uipostmessage，例如GIF动画，这个时候窗口销毁，但退出还不退出，CForwardPostMessageWindow::ProcessWindowMessage将崩溃。
     有没有办法判断一个对象是否有效?
     
162. 灰度图的绘制效率需要再研究下 2012.11.29
     每次绘制时都会去创建一次灰度图

164. 滚动条代码优化！太绕了 2012.11.30  

165. 可以考虑为modify alpha的image增加一个属性，modifyalpharect，只对该区域中的alpha进行调整 2012.12.01
     
==================================疑问==================================

 
==================================当前正在进行的任务==================================
95. 逐步将句柄调用方式修改为接口调用方法
    UI_GetBitmap... ，将返回值修改为 out 参数
113.实现上下翻动效果（文字）
114.实现gif背景（分层绘制）

==================================Finish=============================
 
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
62. 需要一套更灵活的classname <-> xmlname 的注册映射关系
36. 考虑下，取消PrePareDC，每个对象都有一个FONT*属性，每次创建一个对象，，都会发送一个WM_SETFONT的消息
 3. PrepareDC( hDC ); 对应的SaveDC RestoreDC是否正确使用
29. CustomWindow收编LayeredWindow	
33. visible enable  的继承关系 
35. 去除一个窗口的透明属性
79. 换肤后，点击滚动条的line down，崩溃	             
    换肤前line down button 为 focus对象保存在keyboard mgr中，换肤过程中该对象被销毁，导致后面在切换焦点时
    继续引用原指针而崩溃
80. 需要解决鼠标同时在两个popup menuitem上移动时，需要将上一个popup menu隐藏
77. 菜单项radio check功能	
54. theme change消息的响应
53. 恢复skinh
48. 直接UpdateObject刷新子对象时，会刷新在父对象的外面。例如将progress高度调小，上面的
    按钮就会刷新到外面。将panel大小调小，上面的按钮会刷新到外面
44. 当父窗口的字体改变后，如何同步给人它的子对象？怎么区分子对象是有字体的？
14. 将pojp.h -> pojo.cpp
15. 优化mapXml2Class
85. 分层窗口为什么没有参与亮度的改变 : 错把gdiplus bitmap中的亮度改变的代码删除了
86. 分层窗口下面，对EDIT进行双击取词，发现选中部分会被第二次截断。就是因为caretwindow在mouse up之前又发送了一次mouse move的消息，
    导致的选区被修改...怎么办？  -- 在dbclick中将m_bDrag = false;
61. LISTBOX的局部刷新
90. map调用方法优化： map.count + map[] 效率低于 map.find	
87. 使用CaretWindow时，如果移动窗口，光标不会跟随
74. ListCtrlBase::ReDrawItem目前没用
55. render增加一个接口，判断DrawState，这个状态是否支持
    -- 修改为增加了一个 map映射字段，可以用于配置哪个状态对应哪个图片项
94. 删除播放列表所有项时，滚动条没有刷新          
88. 分层窗口优化！拖动窗口时，有明显的晃动
91. 当使用方向 → 时，弹出的子菜单又会向当前窗口发送一个WM_MOUSEMOVE消息，就可能导致当前父菜单另一个ITEM得到HOVER，使得子菜单被关闭
97. Menu Item为什么和xml中有1px padding差别 --- 默认设置了1px padding
75. 分层菜单设置
98. 透明菜单的实现，现在会崩溃
100. 普通image绘制icon时，转换成icon类型
99. gdiplus需要同步增加： DrawGray, Icon, imagelist Bitmap 类型
81. 图标列表的功能、icon大小读取的功能	
82. 音乐插件的功能，目前不支持WMA的文件  -- DirectShow已支持WMA
101. 将鼠标从hover item慢慢移动到submenu item时，会先收到submenumousemove，然后收到mouseleave，导致hover item闪烁 -- 重载onmouseleave函数，增加一个条件判断
21. PrepareDC的 oldFont 之类的东西如何恢复？ -- 该函数已被取消很多年了
31. 考虑将WndProc做成virtual -- 已经将processwindowmessage做成virtual了
76. 菜单提示条问题 -- 2012-09-28 将tooltip抽出来一个mgr，并实现了item提示
104.开始、暂停按钮的正常状态图片怎么成高亮图片了？ -- 由于触发了ondeleteobject，里面直接将m_pHover=NULL，导致按钮状态没有被重置。应该调用SetHoverObject(NULL);
103.option中的listbox的图片需要增加一个分隔线 -- 已修改图片
105.在xml中配置的\n，读取后，并不是一个转义字符，而是两个字符了。怎么转义？  --  xml中的回车换行必须用 &#xD; 或 &#xA; 而不是用\r\n
111.option window不是modal dialog，需要修改成modalless
115.color skin hls    
135.WAVE文件的时间显示问题，及SEEK时为什么必须用偶数的问题。
    因为是16bits的格式，一个sample有高低位
133.为什么有时候一开始播放一个文件后，当前时间就显示为00:01?
    因为该时间其实显示的是当前文件所读取到的位置，而不是播放到的位置。怎么解决？
    在调用GetCurPos时，将当前directsound buffer中的字节数给减掉再计算就行了。
    不过当前的时间显示仍然存在跳跃不均匀的问题。    
    
140.分层菜单的刷新又出问题了。！！Fuck1
    调用Gdiplus的FillRect/DrawRectangle时，貌似必须将right/bottom值减掉1？真心不懂呀    

141.mediaplayer皮肤的HLS变换又出现问题了，按钮有不正常的颜色。这又是闹哪样？
    --> 在获取图片数据时，忽略了图片如果是奇数宽时，最后有一位（或者三位）是对齐用的，不能用于循环当中    
    
 1. Min max width/height 限制
    --> 20121029. 为windowbase增加maxwidth maxheight minwidht minheight属性，同时设置分层窗口的窗口拉伸函数
    
96. 点击任务栏上面的按钮不能最小化窗口
    --> 根据是否有sys_min/sys_max按钮增加WS_MINIMIZEBOX/WS_MAXIMIAZEBOX属性

149.换肤时，paintobject中的updateobject，由于现在只绘制clip区域的内容，而当时的m_bSetWindowRgn为true，导致窗口被截断了
	--> 由于现在控件刷新时进行了裁剪，导致在控件刷新时刷父背景，发现m_bSetWindowRgn=true，就去设置
	    窗口形状，导致窗口被截断了。
	    现在将窗口异形的截取放在了onpaint中处理
119.放置一个控件用于显示频谱
120.如何得到换肤消息
121.换肤后自动设置属性、位置等等
122.透明背景频谱
123.设置频谱参数的线程同步
83. 音乐频谱功能 
134.换肤时，如果解决频谱图的另一个线程刷新问题？
    // 最后还是添加了一个cs来解决这个问题，再继续观察一下。

137.重新架构direct sound,sa的线程模型。现在的结构已经应付不了了。很多的崩溃。
    不能完全将所有的消息都放在buffer thread来做。因为主线程中要根据directsoundengin::renderfile结果来判断是否使用该engine
    但如果只将部分控制通知放在buffer thread中的话，又可能造成空指针崩溃。即一个线程释放了，但另一个线程还没收到通知。
    
    // 将原来的给线程postmessage的方法全改成了cs锁。但理论上仍然会存在其它问题。例如：
       关于文件结束前的一瞬间，我又播放了其它文件。那么上一个文件的on_mp3_stop消息有可能会将新文件的播放停止掉。
       继续观察吧。
52. 需要增加一下变量，保存当前皮肤的hue值，后面加载的图片都要进行转换 （同时将active skin功能也做一下）

	
150.在任务栏上右击，选择”关闭“ 无反应
    1. 在_OnHandleKeyboardMessage，不要让控件处理 SYSKEYDOWN+VK_F4消息
    2. 将EndDialog放在SYSCOMMAND消息中，而不是OnSysClose中

160. 从一个模态窗口中再弹出一个模态窗口，结果后面那个模态窗口无法响应鼠标点击事件了... WM_MOUSEACTIVE一直返0有关系?
     由于customwindow中拦截了WM_NCACTIVATE消息并强制返回1，导致子窗口无法被激活
       
163. 需要在退出时记录下当前播放曲目 2012.11.29
     已添加. 保存在配置文件中 2012.11.29

84. 实现新的一类换肤功能：背景主题图片	
    2012.11.29 已实现基本功能    
    
148.窗口最大化后，覆盖任务栏的问题，应该是GETMINMAXINFO没有去年任务栏高度的问题
    另外，window类中处理的系统按钮命令是不是应该移到custom window当中？
                  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
       	         	
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
	 
10.Q: 为什么我有一个窗口拉伸时效率特别的低？
   A: 可能是因为这个窗口采用的graphics_render是gdiplus，导致效率低下。
      测试得出，绘制同样的一张PNG 100次，gdiplus需要800ms，而gdi的alphablend只需要15ms
      将窗口的graphics render type换成gdi后，效率明显变快。但同时却失去了成为分层窗口的机会
  
11.Q: 如何实现高级列表功能？
   A: 2012.9.28
      目前先保持listitembase的功能，即传统的列表项，里面保存数据，由listctrl负责排版
      后期需要再增加object类型的listitem功能。但如果列表项很多的话，如果增强遍历效率？
      另外传统的listitembase如何实现tooltip的功能？
      另外菜单项中的某一项是否可以为object类型，其它为传统类型？
  
/====================双屏坐标处理代码=================================================	 
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
	
	
///////////////////////////////////////////////////////////////////////////////////////
//
//                               HLS 色调换肤实验数据
// 
///////////////////////////////////////////////////////////////////////////////////////	


/============================H值的规律========================================	
RGB                PS       HLS

RGB(255,0,0) 红色 +0    ->  HLS(0,120,240)

RGB(0,255,255)    +180  ->  HLS(120,120,240)
RGB(0,255,0)      +120  ->  HLS(80,120,240)
RGB(255,255,0)    +60   ->  HLS(40,120,240)

RGB(255,0,0)      +0    ->  HLS(0,120,240)

RGB(255,0,255)    -60   ->  HLS(200,120,240)
RGB(0,0,255)      -120  ->  HLS(160,120,240)
RGB(0,255,255)    -180  ->  HLS(120,120,240)

==>可以推断出来，H色调真的是一个环形的结构。 0->120相当于ps的 +0 -> +180； 240->120相当于ps的 +0 -> -180
   最后都在120处交接。
   240就是0，0就是240，H的范围应该是从(0 - 240)
   其它PS中的+180 -180应该是 +180度角度，-180度角度的意思。加起来就是360度了

  ==》着色模式应该是以前的算法，将所有像素的H都替换为newH
-------------------   
   
RGB(0,255,0) 绿色 +0    ->  HLS(80,120,240)

RGB(255,0,255)    +180  ->  HLS(200,120,240)

==>直接将H加了半个圆:120

-------------------
RGB(255,0,255)粉色+0    ->  HLS(200,120,240)

RGB(0,255,0)      +180  ->  HLS(80,120,240)
  
==> 也相当于是将H加了半个圆:320 - 240 = 80


/============================L值的规律========================================	

RGB(255,0,0) 红色 +0    ->  HLS(0,120,240)

RGB(0,0,0)        -100  ->  HLS(160,0,0)  
RGB(64,0,0)       -75   ->  HLS(0,30,240)
RGB(128,0,0)      -50   ->  HLS(0,60,240)
RGB(192,0,0)      -25   ->  HLS(0,90,240)
RGB(255,0,0)      +0    ->  HLS(0,120,240)
RGB(255,63,63)    +25   ->  HLS(0,150,240)
RGB(255,127,127)  +50   ->  HLS(0,180,240)
RGB(255,191,191)  +75   ->  HLS(0,210,240)
RGB(255,255,255)  +100  ->  HLS(160,240,0) 

----------------------

RGB(16,25,24)     -90   ->  HLS(116,19,53)
RGB(38,63,59)     -75   ->  HLS(114,48,59)
RGB(76,122,116)   -50   ->  HLS(115,93,56)
RGB(112,181,171)  -25   ->  HLS(114,138,76)
RGB(161,214,211)  -1
RGB(163,215,212)  +0    ->  HLS(118,178,95)
RGB(164,215,212)  +1
RGB(164,215,212)  +2
RGB(167,216,214)  +5
RGB(182,223,221)  +25   ->  HLS(118,191,94)
RGB(204,232,231)  +50   ->  HLS(119 205 91)
RGB(228,242,242)  +75   ->  HLS(120,221,84)
RGB(255,255,255)  +100  ->  HLS(160,240,0) 

--> 可以推断出亮度的修改貌似和HLS没有直接关系，而是RGB呈现出一种线性变化的关系
    超过255时取值255，低于0时，取值0
    
http://blog.csdn.net/maozefa/article/details/4155835


/============================S值的规律========================================	
RGB(255,0,0) 红色 +0    ->  HLS(0,120,240)
RGB(255,0,0)      +100  ->  HLS(0,120,240)
RGB(255,0,0)      +50   ->  HLS(0,120,240)
RGB(255,0,0)      +25   ->  HLS(0,120,240)
 
RGB(223,32,32)    -25   ->  HLS(0,120,180)
RGB(191,64,64)    -50   ->  HLS(0,120,120)
RGB(159,95,95)    -75   ->  HLS(0,120,60)
RGB(127,127,127)  -100  ->  HLS(160,120,0)


RGB(97,159,202)   +0    ->  HLS(136,141,119)

RGB(44,168,255)   +100  ->  HLS(136,141,240)
RGB(44,168,255)   +75   ->  HLS(136,141,240)
RGB(45,168,254)   +50   ->  HLS(136,141,238)
RGB(80,162,219)   +25   ->  HLS(126,141,158)
RGB(97,159,202)   +0    ->  HLS(136,141,119)
RGB(110,157,189)  -25   ->  HLS(136,141,90)
RGB(123,154,175)  -50   ->  HLS(136,140,59)
RGB(136,151,162)  -75   ->  HLS(137,140,29)
RGB(149,149,149)  -100  ->  HLS(160,140,0)


RGB(44,169,255)   +75  ->  HLS(136,141,240)
RGB(71,164,228)   +50  ->  HLS(136,141,179)
RGB(97,159,202)   +25  ->  HLS(136,141,119)
RGB(110,157,189)  +0   ->  HLS(136,141,90)
RGB(120,155,179)  -25  ->  HLS(136,141,67)
RGB(130,153,169)  -50  ->  HLS(136,141,44)
RGB(139,151,159)  -75  ->  HLS(136,140,23)

RGB(32,126,1)     +75  ->  HLS(70,60,236)
RGB(48,96,32)     +50  ->  HLS(70,60,120)
RGB(53,85,43)     +25  ->  HLS(70,60,79)
RGB(56,80,48)     +0   ->  HLS(70,60,60)

-->通过参考网上的资料和数据推断,饱和度的变化规律是： dS = s/100;
   当dS > 0, newS = S / (1-dS)
   当dS < 0, newS = S * (1+dS)

Windows下S值为0时，H值始终为160（2/3*240）

饱和度调节算法
http://blog.csdn.net/maozefa/article/details/1781208




///////////////////////////////////////////////////////////////////////////////////////
//
//      RichEdit 实现 OLE 控件插入的知识点
// 
///////////////////////////////////////////////////////////////////////////////////////

IAdviseSink
   该接口通常是由窗口(Container)来实现，用于接收数据变更的通知。













///////////////////////////////////////////////////////////////////////////////////////

// 其它一些界面库的架构
http://blog.csdn.net/jameshooo/article/details/6677272
http://blog.csdn.net/jameshooo/article/details/5953365
// 关于是否开源的讨论
http://topic.csdn.net/u/20100321/15/a93390d2-8022-4154-a019-1faa76bf5202.html