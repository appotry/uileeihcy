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