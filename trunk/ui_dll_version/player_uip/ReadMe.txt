Windows Media Format SDK

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
http://tieba.baidu.com/p/1906977428

这个buffer指的就是之前的SoundBuffer中的数据，因为SoundBuffer中的每一个字节（signed char）
就代表着采样声波某一点的振幅（假如是16位的采样质量那就代表高/低位）（强度） 


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
 
关于FFT 傅里叶变换
http://www.douban.com/note/164400821/
http://blog.csdn.net/v_JULY_v/article/details/6196862
但是对于非周期性的信号，我们需要用无穷多不同频率的正弦曲线来表示，这对于计算机来说是不可能实现的。所以对于离散信号的变换只有离散傅立叶变换（DFT）才能被适用，对于计算机来说只有离散的和有限长度的数据才能被处理，对于其它的变换类型只有在数学演算中才能用到，在计算机面前我们只能用DFT方法，

频谱demo(支持MP3,WMA,WAV)
http://www.codeproject.com/Articles/31375/Play-Wave-Files-with-DirectSound-and-Display-its-S
http://www.codeproject.com/Articles/31709/Play-Audio-Files-with-DirectSound-and-Display-its
http://www.cppblog.com/jackyxinli/archive/2009/02/04/72950.html
DirectSound Spectrum
可能是抄袭下面这个java代码
http://jukes.googlecode.com/svn-history/r25/trunk/src/main/java/com/melloware/jukes/gui/view/component/SpectrumTimeAnalyzer.java

http://www.zxxk.com/Article/192456.html   完美的数学声谱图案
声谱 可视化 小波转换图  声图

一定要记住，3D音效只支持单声道的wave文件，不要试图对一个多声道的wave文件实现三维立体声效果，不会成功的。
但是要听到3D音效，貌似必须有5个音箱的支持才行。

PCM 声道数：有单声道和立体声之分，单声道的声音只能使用一个喇叭发声（有的也处理成两个喇叭输出同一个声道的声音），
            立体声的PCM可以使两个喇叭都发声（一般左右声道有分工），更能感觉到空间效果。
要理解TransformSamples函数，就必须理解PCM格式，各声道数据分布及其范围。
?? FFT的buffer size为什么要取值为DEFAULT_FFT_SAMPLE_BUFFER_SIZE 2048?

实际上，你只要创建一个oldFFt记录下FFT上一次的结果，假如oldfft比较大，给它一个衰减的值再传递给bands，就是说，除了某频率声波突然变大，柱形都是缓慢衰减的，那么，看上去就和window media player的感觉一样好了，这点只要修改下上述源码，应该很容易实现
所以明白一点，做音乐播放器时的频谱图，你思考更多的应该是这个频谱如何看上去感觉更好（当然不能像某些劣质的mp3 mp4播放器那样，频谱图就是一段预先录制好的动画，至少你要让它看上去和当前播放的有关），而不是绞尽脑汁的让用户看懂你的傅立叶变换结果

timeSetEvent来取代开启线程？ -- 不行。timeSetEvent也是开启一个新线程
acm是什么？ acmstreamopen。。。

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

wmvcore.dll 这个是什么？使用WMF SDK时会从这个DLL中引用
就是引用WMCreateSyncReader的引用lib


拥有自主研发的全新音频引擎，支持DirectSound、Kernel Streaming和ASIO等高级音频流输出方式、
64比特混音、AddIn插件扩展技术，具有资源占用低、运行效率高，扩展能力强等特点。
 　　
千千静听支持几乎所有常见的音频格式，包括MP/mp3PRO、AAC/AAC+、M4A/MP4、WMA、APE、MPC、OGG、
WAVE、CD、FLAC、RM、TTA、AIFF、AU等音频格式以及多种MOD和MIDI音乐，以及AVI、VCD、DVD等多种
视频文件中的音频流，还支持CUE音轨索引文件。 格式转换：通过简单便捷的操作，可以在多种音频
格式之间进行轻松转换，包括上述所有格式（以及CD或DVD中的音频流）到WAVE、MP3、APE、WMA等格
式的转换；通过基于COM接口的AddIn插件或第三方提供的命令行编码器还能支持更多格式的播放和转换。 　　

回放增益：千千静听支持高级采样频率转换(SSRC)和多种比特输出方式，并具有强大的回放增益功能，
可在播放时自动将音量调节到最佳水平以实现不同文件相同音量；基于频域的10波段均衡器、多级杜
比环绕、交叉淡入淡出音效，兼容并可同时激活多个Winamp2的音效插件。 　　

标签功能：支持所有常见的标签格式，包括ID3v1/v2、WMA、RM、APE和Vorbis等，支持批量修改标签
和以标签重命名文件，轻松管理播放列表；并且采用freedb接口实现自动在线获取CD的音轨信息的功能。 　　

歌词同步：千千静听倍受用户喜爱和推崇的是强大而完善的同步歌词功能。在播放歌曲的同时，可以自
动连接到千千静听庞大的歌词库服务器，下载相匹配的歌词，并且以卡拉OK式效果同步滚动显示，并支
持鼠标拖动定位播放；另有独具特色的歌词编辑功能，可以自己制作或修改同步歌词，还可以直接将自
己精心制作的歌词上传到服务器实现与他人共享！ 


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
 
 
 WMP visualizations
用DirectX实现粒子系统（三） 
http://www.cnblogs.com/graphics/archive/2012/07/09/2570431.html


///
特效注释

1、和声（Chorus）特技

typedef struct _DSFXChorus {
　FLOAT fWetDryMix;//压缩信号和未压缩信号的比率，缺省值50
　FLOAT fDepth; //低频率振荡器调制信号的延迟时间的百分比，缺省是10%
　FLOAT fFeedback;//输出信号对特技输入信号的反馈百分比，缺省25%
　FLOAT fFrequency;//LFO的频率，缺省是1.1
　LONG lWaveform;//LFO的波形类型，缺省是正弦波
　FLOAT fDelay;//音频播放时的延迟时间，缺省是16ms
　LONG lPhase; //左右声道声波的相位差别，取值为-180，-90，0，90，180 度
} DSFXChorus, *LPDSFXChorus;

2、压限特技（Compressor ）

typedef struct _DSFXCompressor 
{
　FLOAT fGain; //压限后输出信号的强度，缺省值是0db
　FLOAT fAttack;//压限达到最大值的时间，缺省是10ms
　FLOAT fRelease;//输入小于fThreshold后，压缩停止的时间，缺省是200ms
　FLOAT fThreshold;//压缩开始的临界点的db值，缺省是-20db
　FLOAT fRatio;//压缩的比率，缺省是3，即3:1
　FLOAT fPredelay;//lThreshold 达到临界点，attack phase开始前的延迟时间
} DSFXCompressor, *LPDSFXCompressor;

3、失真特技（Distorion）

typedef struct _DSFXDistortion 
{
　FLOAT fGain;//信号失真后的音量的强度变化，缺省是比原来衰减 18db。
　FLOAT fEdge;//失真的强度，用百分比来表示，缺省值是15
　FLOAT fPostEQCenterFrequency;//信号叠加的中心频率，缺省是2400hz
　FLOAT fPostEQBandwidth;//信号的带宽 ，缺省是2400hz
　FLOAT fPreLowpassCutoff;//高通滤波的最高值，缺省是8000hz
} DSFXDistortion, *LPDSFXDistortion; 


4、回声特技（ Echo）

typedef struct _DSFXEcho 
{
　FLOAT fWetDryMix;处理过的信号和没有处理的信号的比率，缺省值是50
　FLOAT fFeedback;//输出信号反馈到输入信号的百分比, 缺省值是50 
　FLOAT fLeftDelay;//左声道延迟的时间，缺省值500ms
　FLOAT fRightDelay;//右声道延迟的时间，缺省值500ms
　LONG lPanDelay;//是否左右声道交替延迟，缺省值 0，左右声道不交替
} DSFXEcho, *LPDSFXEcho;


5、波浪特技（Flanger）

typedef struct _DSFXFlanger 
{
　FLOAT fWetDryMix;//处理过的信号和没有处理的信号的比率，取值范围 0~~100，缺省值是50
　FLOAT fDepth;//低频振荡器调整延迟时间的百分比，取值范围 0~~100，缺省值 是100
　FLOAT fFeedback;//输出信号反馈到输入信号的百分比,取值范围 -99~~99， 缺省值是-50 
　FLOAT fFrequency;//低频振荡器的频率， 缺省值是 0.25
　LONG lWaveform;//低频振荡器的波形，分为三角形，和正弦波形
　FLOAT fDelay;//播放之前的延迟，以毫秒作单位，缺省值是2ms
　LONG lPhase;//相位
} DSFXFlanger, *LPDSFXFlanger;

6、咕噜声效特技（Gargle）

typedef struct _DSFXGargle 
{
　DWORD dwRateHz;//调制波频率，取值范围 1 ~~1000 hz，缺省值是20 hz
　DWORD dwWaveShape;//调制波形状，分为三角形和正方形两种，DSFXGARGLE_WAVE_TRIANGLE DSFXGARGLE_WAVE_SQUARE
} DSFXGargle, *LPDSFXGargle;

7、参数平衡特技（ParamEQ）

typedef struct _DSFXParamEq 
{
　FLOAT fCenter;//中心的频率 ，单位是hz，取值范围是 80.0f hz~~16000.0f ，缺省值 是 8000hz
　FLOAT fBandwidth;//音的带宽，以半音为单位，取值范围是 1.0f~~36.0f,缺省值 是12
　FLOAT fGain;//音频的音量，以db为单位，取值范围是 -15.0db ~~15.0f db,缺省值是0db
} DSFXParamEq, *LPDSFXParamEq;
DSFXPARAMEQ_CENTER_MIN 
8、混响（Reverb）

typedef struct _DSFXWavesReverb 
{
　FLOAT fInGain; //输入信号的音量的大小,即db值,缺省值是 0db。
　FLOAT fReverbMix;//回响混合音量强度大小，用db值来表示，缺省值是 0db。
　FLOAT fReverbTime;//回响的时间，单位是ms，缺省值是 1000.0f ms
　FLOAT fHighFreqR//高频回响所占时间的比率 ， 缺省值 0.001f 。TRatio;
} DSFXWavesReverb, *LPDSFXWavesReverb;

///

说到EQ，相信只要是有后期基础的童鞋都不会陌生。

没错，这就是经常被后期们挂在嘴边的：均衡器！

说起来她的原理并不复杂，无非就是增加某一频段的音量，降低某一频段的音量，但往往刚上手的童鞋都会被这个小小的EQ弄得很抓狂。

我想做一个音频均衡器，可以调整低音、中音、高音，达到不同的效果。因为没做过这方面的，
看了些资料后，知道了大概过程：对采样数据经行傅立叶变换，即时频变换，再对不同频率进行处理，
最后再经过傅立叶逆变换。



/////

多线程协作技术：你的MP3播放器性能的好坏全看这一点做得如何了。
处理不好，你的MP3播放器就会像老牛拉破车一样。
为了提高效率，你要善于使用信号灯同步技术而不要使用Windows的消息系统。 

////
http://www.360doc.com/content/07/0504/14/11586_481582.shtml
　Equalizer(均衡效果)：玩过音响的朋友都知道EQ处理，通过精心调节各个独立频率段的增益，
　可适当地弥补音源的不足或提升其效果。一般调节范围为±20dB。在MP3播放器中实现EQ处理并
　不是很难，总的来说分为两种实现方法。 
　
　推荐：一类以WinAmp为代表的播放器，崇尚精确调节，其EQ对话框中对频率点的标定基本准确，
　用户在调节时较少出现因过增益而产生的阻塞现象。其实现原理为：将用户调节的10个频率点
　增益值适当地通过一对多映射对应到一个576个浮点数组成的数组，其中576是MP3解码原理中
　推导出来的常量。再实时地用这576个增益值同Huffman解码后，仍处于频域的MP3帧数据相乘，
　进行衰减/提升。这样做的优点是：频率准确，较少产生阻塞，缺点是：运算量稍大。有关这
　种EQ实现的具体方法，请参见X11AMP公布的源程序。
　 
　不推荐：另一类以XAudio为代表的播放器或许是还不懂WinAmp是怎么实现EQ的或许是认为平平淡
　淡才是真。在实现EQ时采用了较为不精确的处理。其实现原理为：直接用32个增益值同每帧中32
　个子带(sub－bands)分量相乘。其优点是运算简单，时滞小。缺点为频率调节极其不准确，由于
　MP3编码原理的限制，甚至于到现在还没人能准确地标定出各个子带的频率。Wplay大胆地在其EQ
　对话框中标出了频率值，但那是盲目地抄WinAmp的，你只要用几个单一频率的MP3文件测试一下就
　知道了。目前90%以上实现了EQ功能的MP3播放器都是采用了这类方法。 


基于频域滤波数字均衡器的设计
http://wenku.baidu.com/view/8f71d97e27284b73f2425059.html

fftw
http://www.fftw.org/

将def编译成lib文件。在命令提示行中输入：
lib /machine:ix86 /def:libfftw3-3.def
lib /machine:ix86 /def:libfftw3f-3.def
lib /machine:ix86 /def:libfftw3l-3.def

fftw的用法
http://stackoverflow.com/questions/5818558/computing-fft-and-ifft-with-fftw-h-in-c
http://www.docin.com/p-71818494.html

创建完方案（fftw_plan）后，就可以用fftw_execute对指定的 数据in/out做任意次变换。
如果想变换一个相同大小（N相等）但数据不同的另外一个数组in，可以创建一个新方案，
FFTW会自动重用上次方案的信 息。这一点其实是非常好的，比如你首先用FFTW_MEASURE模
式创建了一个最优的变换方案，只要变换数据的大小不变，你可以用 fftw_plan_dft_1d创
建新的方案以对新数据执行变换，同时新变换仍然是最优的。一个fftw_plan只能对固定的
in/out进行变换， 但可以在变换后改变in的内容（大小不变）以用同一个方案执行新的变换。

根据手册，当正变换接着反变换后，会将原来的时域信号放大N位，因此要在反变换后，手动的
将所有的信息除以N。才能还原原始信号。

One-Dimensional DFTs of Real Data
the input is n real numbers, while the output is n/2+1 complex numbers 
??一个长度为N的信号可以分解成N/2 + 1个正余弦信息（16个信号=9个余弦+9个正弦）
??一个长度为N的信息，最多只能有N/2 + 1个不同频率，再多的频率就超过了计算机所能处理的精度范围
由于 FFT 结果的对称性，通常我们只使用前半部分的结果，即小于采样频率一半的结果。

假设FFT之后某点n用算数a+bi表示，那么这个算数的模就是
An=根号a*a+b*b，相位就是Pn=atan(b/a)

??假设我将N个点进行FFT变换得到频域上的复数数组A，A中除了第一个点，其他点的模除以（N/2）就代表振幅
??这是一个频谱图，横坐标表示频率大小，纵坐标表示振幅大小，原始信号长度为N（这里是32），经DFT转换后
  得到的17个频率的频谱，频谱密度表示每单位带宽中为多大的振幅，那么带宽是怎么计算出来的呢？看上图，
  除了头尾两个，其余点的所占的宽度是2/N，这个宽度便是每个点的带宽，头尾两个点的带宽是1/N,而Im X[k]
  和Re X[k]表示的是频谱密度，即每一个单位带宽的振幅大小，但表示2/N（或1/N）带宽的振幅大小，所以分别
  应当是Im X[k]和Re X[k]的2/N（或1/N）。

在离散信号处理中，运用复数形式来表示正余弦波是个常用的技术，这是因为利用复数进行各种运算得到的结果跟原来的正余弦运算结果是一致的

复数形式傅立叶变换把原始信号x[n]当成是一个用复数来表示的信号，其中实数部分表示原始信号值，虚数部分为0，变换结果X[k]也是个复数的形式，但这里的虚数部分是有值的。

/--
How to implement the FFT algorithm
http://www.codeproject.com/Articles/9388/How-to-implement-the-FFT-algorithm
用于ARM上的FFT与IFFT源代码（C语言，不依赖特定平台） 
http://blog.csdn.net/syrchina/article/details/6670517

一些基本数学公式
http://www.csie.ntnu.edu.tw/~u91029/Wave.html

const double EQ_Frecs[][40] = {          
{20, 1000, 5000, 20000}, // EQ of 3 bands         
 {20, 80, 320, 1280, 5120, 16000}, // EQ of 5 bands          
 {20, 200, 600, 1000, 3000, 5000, 10000, 16000}, // EQ of 7 bands          
 {15.75, 31.5, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000}, // EQ of 10 bands          
 {15.75, 25, 40, 63, 100, 160, 250, 400, 630, 1000, 1600, 2500,  4000, 6300, 10000, 16000}, // EQ of 15 bands          
 {15.84, 22.4, 31.5, 45, 63, 90, 125, 180, 250, 355, 500, 710, 1000, 1400, 2000, 2800, 4000, 5600, 8000, 11200, 16000}, // EQ of 20 bands          
 {15.87, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000,                  6300, 8000, 10000, 12500, 16000, 20000}, // EQ of 31 bands using 31 peaking EQ filters  };  
 
 DirectX与VB.NET编程（八）*EQ均衡器 
 http://hi.baidu.com/jadechoice/item/096446bf8d0276d285dd7935
 
 audition 操作——滤波和均衡
 http://majingyinyue.blog.163.com/blog/static/17078000820110201119761/
 
 IIR均衡器。(在FFT均衡器之下，IIR均衡器的质量和精度都是最高的)。【这个意思是不是就是说FFT均衡器更加的牛掰?】
 
 xmms X Multimedia System (XMMS)是一套自由的音频播放器，其外表极像Winamp，但是运行在Unix-like 操作系统。
 虽然原来的授权是闭源的，但现已变为开源，采用GNU General Public License。
 http://www.xmms.org/
 iir 能找到的唯一一个相关代码及基使用方法工程,提供一个简单的时域算法的均衡器。 但我需要的是基于频域的10波段均衡器
 http://repository-of-projects.googlecode.com/svn/trunk/Win32/oggdemo/ra_ogg/
 
 
 http://topic.csdn.net/u/20100430/09/97E72305-6207-4315-8A6C-77315BEC254C.html
 最近我在做EQ程序，基本的思想是将一段采样进行快速傅立叶变换，变成频域数据，然后进行快速傅立叶变换的逆变换。
 问题就是，当进行完快速傅立叶变换之后，我不知道该如何处理变换后的数据以达到调速EQ的目的。比如我用128个采样进行傅立叶变换，
 得到的频谱有128个频率，如何与实际的音乐频率（20－22K）联系起来呢？
 
 比如44100Hz的音频，每次对1024个sample进行FFT变换，变换后还是1024个数据。由于傅立叶变换是对称的，只取前512个分析，对应的音频频率就是0~22050Hz
 如果你的采样率是16K，那么0就是对应0Hz，63就对应8KHz。

Equalizer 等化器/均衡器
http://www.sounderpro.com.tw/reviw/equalizer/equalizer.html

Why EQ Is Done In the Time Domain 
http://blog.bjornroche.com/2012/08/why-eq-is-done-in-time-domain.html

http://www.eccn.com/design_2011041915054540.htm
常采用的均衡器算法是使用IIR或者FIR滤波器滤波的设计方法。这种方法有几个不足之处：IIR滤波器具有反馈回路，会出现相位偏差；FIR滤波器会造成很大的时间延迟，这对于实时滤波是非常不利的。另外，如果使用IIR或者FIR滤波器，所调节的频段越多，增加的滤波器的个数也越多，运算量也明显增大。而通过傅里叶变换设计均衡器，不但在滤波的过程中具有很大的优越性，不存在相位误差、时间延迟这些问题，对调节多段均衡程序运算量上也有明显的减少。
对于频点的调节是调节频点周围这一段的频率幅度的大小，以最终达到调节频率的目的。为了防止在抽取频点时，因某一点的调节范围过大而使这一段的声音听起来不和谐，在对频点进行调节时，采用的方法是调节该频点及其附近的频点以达到最终的调节效果。
算法的实现：取频率点周围的点，将所取的点调节的范围是该点与对应频率点的距离的反比，这样就避免该点频率的影响太强烈。算法流程如图2所示。针对某一个频点的处理的程序如下：


