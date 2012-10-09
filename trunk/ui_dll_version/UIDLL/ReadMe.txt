未考虑到的问题：
	1. 快捷键的映射，是否需要再添加一个xml文件
	2. DoModal的退出还没有做好
	
 未实现：
 1. Min max width/height 限制
 4. 由uibuilder启动时，ui的日志没有输出到 console 中
 5. 在拖动窗口大小的时候，EDIT位置变化时，CARAT没有随者变化
 7. 测试font的效果，是否可以改变
 10.可否考虑将ID类型都映射为一个整数，而不是用字符串去匹配？
 11.xml文件可否考虑分开，而不是全放在一个文件中？
 12.把所有的xmldao都用find_elem_in_xml/find_elem_under_style来实现
 13.load xml时，没有都转换成小写的
 17.CMainFrame::OnMenuSave只实现了保存所有的文件，没有实现保存当前文件
 18.使用快捷键资源来实现快捷键 ctrl+s save 
 19.style 编辑器不应该可以编辑继续得到的属性，也无法保存继续得到的属性问题
 25.优化SetWindowRgn缩放功能
 27.Gdi+下面的MeasureString偏大的原因，以及DrawString放不下的原因
 28.换肤时，崩溃。先打开三个窗口，再关闭第二个窗口，换肤，崩溃。
 30.提供一个方法：DoVerb( "action", xxx ); "press" "click" "unpress" "hover" "unhover"
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
52. 需要增加一下变量，保存当前皮肤的hue值，后面加载的图片都要进行转换 （同时将active skin功能也做一下）
56. LOG 支持__FUNCTION__ 参数！
57. 考虑支持缩放效果
58. icon在系统16位色下面显示全白
59. 去掉ResetAttribute方法
60. 将BkRender, ForeRender移到IObjectRender当中，子类controlRender可继承
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
73. 点击打开一个系统COMBOOX的下拉列表，然后将鼠标放在一个UI控件上面，点击一下，下拉列表消失，再继续点击UI控件，无反应。
    因为没有人去触发一个WM_MOUSEMOVE来set hover对象
78. 将layout.xml增加一个<#include>标签，允许将一些资源抽取出来 
83. 音乐频谱功能 
84. 实现新的一类换肤功能：背景主题图片	
89. 普通窗口拉伸优化！不平滑
92. playlistdlg resize的时候， listctrl 变化不正常，会变黑
93. WINDOW 的两个消息链有点混乱，能不能想办法都合入到processwindowmessage中，ui_begin_msg_map只由外部去处理
95. 逐步将句柄调用方式修改为接口调用方法
96. 点击任务栏上面的按钮不能最小化窗口
102.tooltip高级控件
106.layout.xxx 属性是否是以进行统一规划？
107.ccaret的分层窗口光标在第一行/最后一行显示不下的时候，位置不正确
108.drag drop移动richedit的文字，刷新不同步
109.richedit的横向滚动条range、page与ui控件不同步
110.scrollbar 的 sizebox 绘制，theme都有现成的API，由哪一层来负责绘制？
112.回车换行时，richedit的滚动条没有正确更新
118.复合列表框
	
==================================疑问==================================
1. Message类是否需要一个 m_pCurMsg成员变量？
 
==================================当前正在进行的任务==================================
95. 逐步将句柄调用方式修改为接口调用方法
    UI_GetBitmap... ，将返回值修改为 out 参数
113.实现上下翻动效果（文字）
114.实现gif背景（分层绘制）
116.mp3 wma文件时长读取
	下载   Windows   Media   Format   9.0   SDK   看看 
	WMFSDK里有个接口   IWMHeaderInfo   可以做到调用这个函数GetAttributeByName 
	The   GetAttributeByName   method   returns   a   descriptive   attribute   that   is   stored   in   the   header   section   of   the   Windows   Media   file. 
	
	创建元数据编辑器
打开源文件
通过查询获得头文件信息接口
通过头接口获取指定属性:pHeaderInfo->GetAttributeByName

GetAttributeByName:通过指定属性名字获取属性值 

hr = WMCreateReader( NULL, 0, &m_pReader );
hr = m_pReader->Open( pwszInputFile, this, NULL );
hr = m_pReader->QueryInterface( IID_IWMHeaderInfo, (void **)&m_pReaderHeaderInfo )
m_pReaderHeaderInfo ->GetAttributeByName(&wStream, g_wszWMDuration, &type, (BYTE*)&duration, &wBytes); 


http://tieba.baidu.com/p/1868393695
http://tieba.baidu.com/p/1885477077
http://tieba.baidu.com/p/1906977428?from=prin

117.mp3 wma频谱分析 
WAV 文件格式
http://wenku.baidu.com/view/801b1537f111f18583d05aa7.html

DSound
http://www.bairuitech.com/upimg/soft/documents/DirectSound_Develop_Manual.pdf

Direct2D
http://msdn.microsoft.com/zh-cn/library/dd756649(v=vs.85).aspx

Windows7 SDK/D2D 下载地址：
http://www.microsoft.com/en-us/download/details.aspx?id=8442

DirectSound + mgp123播放mp3
http://www.cppblog.com/codejie/archive/2009/03/26/77916.html

mpg123
http://sourceforge.net/projects/mpg123
http://mpg123.org/

问：千千使用的MP3解码器是哪一个？
	千千使用的是最新版本的mpg123-0.59s-pre，它是目前已经的开放源代码中最好的MP3
解码器之一，无论是精度来是速度都是很出色的。千千早期版本使用的是MAD，但在个人PC
上，MAD的解码速度比mpg123要慢70%甚至更多，而绝对精确和mpg123相比还略有差距，所以
后来就改用mpg123了。

问：是不是把播放时的缓冲长度设得越高播放越流畅？
	一般来说，只要设置1000-2000毫秒的长度就可以了，太长只会增加资源占用。
	
问：如果我的电脑上没有安装Windows Media Player 9或更新的版本，还能播放WMA吗？
	当然可以，只要在安装时选中支持WMA就行了，千千静听支持WMA只用到了微软的WMA解
码器组件，就是安装目录下的wmadmod.dll文件，只有400K左右:)，而且还支持WMA文件信息
的读取。

 “千千静听”作者的自我推荐
                                  ——献给所有使用汉字并且喜欢用电脑听音乐的人们

    前后已经发布了十几个版本了，觉得是时候写些说明性的东西了，想了半天，觉得还是
来聊聊音频播放器的现状以及千千静听的主要特点。
    本人开发千千静听(2.0版以前称为“MP3随身听”，以下简称“千千”)的初衷是为广大
的MP3爱好者提供一个全中文的纯音频播放软件，并以精致小巧、功能完善作为设计原则，
用过早期版本的用户应该知道，当时400k左右的程度已经可以播放MP3、视觉效果、均衡调
节、Winamp音效插件支持、歌词显示、皮肤等基本功能了，当时的版本只支持MP3和不压缩
的WAVE，但小巧的体积、极低的资源占用和完备的播放功能也赢得不少网友的喜爱，甚至一
度成为PCHOME(电脑之家)的热点推荐。
    如今千千已发展到3.6版，其稳定性和功能也与当时的版本不可同日而语，特别在格式
支持和音质还原上更是做了大量的工作，视觉效果更加丰富，歌词支持也更加完善，程序界
面也比以前漂亮得多，而程序体积和资源占用仍算得上是最小播放器之一。这中间得到了很
多热心网友的大力支持，他们在给我大力支持的同时更提出了大量宝贵的意思和建议，使得
千千更加成熟。
    如今的音频播放器已不再像当年的Winamp一枝独秀，几乎每个星期都有新的播放器诞生。
音频播放器最讲究音频还原的质量，通常所说的音质好不好就是指这个，播放核心和解码算
法决定了音质的优劣，网上出现的形形色色的音频播放器大都采用以下四类核心：
    1。DirectShow：Microsoft公司推出的多媒体解决方案，它自己的Media Player也是基
于这个平台的，虽然得到了广泛的支持，但缺少针对音频特性的优化，以及占用系统资源较
多缺点，它的最大优势是支持的格式最多、开发最容易，这也是使用最多的核心；
    2。XAudio：有人称它为最好的音频SDK，很多的音频播放器就使用这个核心，它提供音
频解码、视频效果(频谱分析的数据)和回放输出的功能，早期的木子播放器就采用了这个
核心。它的缺点是扩展性较差，支持的格式少；
    3。Winamp的内核：使用这个内核的人真的很聪明，其实Nullsoft并没有推出过Winamp的
内核SDK，有些人利用Winamp的plugins公开的接口组建了一个新的播放器，利用了大量Winamp
的现有资源，当然同时也“利用”了Winamp一些过时的或有缺陷的设计；
    4。最后一种当然是完全自主开发的核心，包括从文件格式的解析、音频格式的解码、音
效的处理到最后的回放，虽然开发工作量大，但可以最大限度的优化结构和资源占用，并可
加入最新的音频特性，千千就是完全自主开发的核心，当然这种核心的弱势就是文件格式的
支持比较困难，有时需要第三方提供格式的说明和相应的解码库，例如对WMA的格式的支持就
需要从Microsoft那里得到ASF文件格式的规格说明，并采用它提供的解码组件。
    当然还有基于其它的如BASS(http://un4seen.com/)、Real Player等的内核，而且很多
播放器还同时使用多个内核来支持不同的文件格式，例如目前很流行的Media Player Classic
就是同时使用三种内核，播放RM用Real Player、播放MOV用QuickTime、其它用DirectShow。
    正因为千千有自主开发的核心，才能在根本上优化程序的结构和性能，千千采用了一个
很小的但扩展性很好的内核，在增加新格式时和音效时可以非常容易。并且采用了基于MAD算
法的高精度MP3解码库(http://www.underbit.com/products/mad/)，同时还采用了32bit浮
点来作为各种格式的解码输出(当然包括MP3)，相比16/24bit的整型输出更精确也更适合作
音效处理，在回放输出上也支持各种bit的格式，能最大限度的支持不同层次的声卡，一般来
说高bit的格式能提供最高精度的回放。
    千千提供了一个相当优秀的均衡器，它采用了IIR数字信号处理算法(有关IIR可以参照
相关数字信号处理的资料)，并且采用了先进的ISO 10波段分法，Windows Media Player 9
就是采用这种分法，相比老的10波段在音色上大有改进，特别强调低音和高音部分的增强。
千千还提供了一个分级杜比环绕效果。
    千千对Winamp2的音效插件的支持也很到位，甚至使用这类插件有着比Winamp5还要好的
支持，就是同时可激活多个插件，并且可以设置音频过滤的顺序，而Winamp2甚至Winamp5都
只能同时激活一个。
    千千对歌词(LRC格式)的支持也很完善，除了自动搜索和同步滚动歌词外，还能用左键
拖动歌词来实现定位播放，这对于那些喜欢唱歌的用户来说真的很方便，想听哪一句就只要
用鼠标拖动就行了。还针对不同版本的歌曲和歌词文件不对应所带来的不同步，提供了时间
调整功能，可以加快或减速来调整歌词显示的时间，以达到同步显示。最新的版本还提供了
自动下载歌词的功能，只要连在网上就可以自动搜索并下载歌词文件，并且还内置了一个歌
词编辑器，能够最方便的制作或者修改歌词文件。
    考虑到大多数上网用户的带宽限制和安全因素等原因，千千暂时还未提供网络实时点播
功能，随着宽带的普及，会适时考虑增加这个功能的。
    和老牌播放器Winamp相比，千千静听还相当年轻，但千千静听的高起点和高要求又在某
些方面占有一定的优势。希望大家都能喜欢上这来自国内的一个年轻程序员两年多来精心打
造的又完全免费的全中文数字音频播放器。

    注：考虑到打包尽可能小，所以没把Real Codec放在发布包里，只要你的电脑上安装了
Real Player一般就可以播放Real的音频了，但这项功能目前还未做到完美的兼容性。发布
包里的wmadmod.dll和msdmo.dll是用来解压WMA的，当你的电脑上没有安装Windows Media
Player 8/9时会用到。如果Windows98上不能播放CD或不能CD格式转换成MP3，请先下载并安
装http://alen.27h.com/aspi32.exe. 
 

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