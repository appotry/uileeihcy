//////////////////////////////////////////////////////////////////////////
//
//	Gif Image 
//
//		使用 ATL::CImage作为解析器，将GIF图片中的每一帧单独抽取出来拼成一
//		张单帧GIF图片加载并显示
//
//	[version]
//		1.0  libo 2012.3.17 beijing  
//			完成基本功能（剩下TODO）
//
//	[usage]
//		1. 调用Load去加载资源
//		2. 调用SetDrawParam去设置绘制参数
//		3. 在WM_PAINT函数中调用OnPaint
//		4. 调用Start,Pause,Stop进行控制
//		5. 调用Destory释放
//
//	[TODO]
//		1. 优化
//		2. 缩略图模式
//		3. Delay Time 到底是哪一帧的时间<--应该是这一帧显示完后保留多长时间再显示下一帧
//		4. 一些图片在WIN7上加载出来的数据有问题，导致显示错误，如（res\\椭圆规.gif）
//		5. 需要支持从资源中加载图片
//
//	[Remark]
//		1. 在xp上面使用image加载gif会提示出异常
//		2. TransparentBlt MSDN解决只支持DDB，因此要将DIB转换成DDB进行透明处理
//		3. 如果存在local color table，要使用local color table，不能使用global color table
//		4. 结构的域中，不要使用 char，要使用unsigned char，否则会出错
//		5. GIF的透明色可能和背景色都是白色，但两者的索引是不一样的。因此在处理GIF透明的时候
//		   需要重新去解析索引值，将索引值==透明的特殊处理
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "image.h"
using namespace UI;
#include <fstream>
#include <math.h>
#include <list>
#include <vector>
using namespace std;

#ifdef _DEBUG
#define DEBUG_TRACE
#endif

#ifdef DEBUG_TRACE 
	#define GIFTRACE0(x) \
		::OutputDebugString(x); 
#else
	#define GIFTRACE0(x) 
#endif

#ifdef DEBUG_TRACE 
	#define GIFTRACE1_int(x,d) \
		{ \
			TCHAR szTrace[256] = _T(""); \
			_stprintf(szTrace, x, d ); \
			::OutputDebugString(szTrace); \
		} 
#else
	#define GIFTRACE1_int(x,d)
#endif

class  GifImage;
struct Gif_TimerItem  // 计时器数据
{
public:
	int  nID;       // 用于kill_timer
	int  nStart;    // 开始时戳
	int  nWait;     // 等待时长
	int  nRepeat;   // 循环次数 -1表示无限循环

	void* pData;    // GifImage*

public:
	// 获取剩余时间
	inline int  get_remain(){ return nWait - (::GetTickCount() - nStart); }
	// 更新下一次显示时间
//	inline void update_repeat() { nStart += nWait; };          // TODO: 使用该方法可保证准确性，但会导致在受到阻塞恢复时，gif会快进播放
    inline void update_repeat() { nStart = ::GetTickCount(); } // TODO: 使用该该当可保证平滑度，但不能保证响应的精度
};

// 线程消息
#define GT_SET_TIMER  101   // 注意不要使用1，否则postthreadmessage会报错：ERROR_MESSAGE_SYNC_ONLY
#define GT_KILL_TIMER 102

// 单实例的 Gif 播放计时器引擎
class Gif_Timer
{
public:
	Gif_Timer();
	~Gif_Timer();

	void set_timer(const Gif_TimerItem& item);
	void on_set_timer(Gif_TimerItem* pItem);
	void kill_timer(int id);
	void on_kill_timer(int id);
	void on_modify_timer();

	void update_wait_time();
	void check_timer();
	void on_timer(Gif_TimerItem* pItem);
	
	void post_message(UINT,WPARAM,LPARAM);
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);

public:  
	list<Gif_TimerItem*>     m_vItem;        // 当前需要计时的列表  

	HANDLE        m_hEvent_set_timer;        // 默认一直没有信号，当m_nBestWaitTime变化时，得到信号，立即检测列表
	HANDLE        m_hEvent_post_message;     // 默认一直没有信号，当有外部postthreadmessage时，得到信号，立即检测消息队列
	HANDLE        m_hMsgQueueEvent;          // 一次性对象。用于检测工作线程的消息队列是否创建完成

	HANDLE        m_hThread;                 // 工作线程
	DWORD         m_dwThreadID;              // 工作线程ID

	int           m_nBestWaitTime;           // 当前最佳等待时间
	bool          m_bQuitThread;             // 是否退出线程
};

//
//	维护Gig_Timer单实现的类工厂，使用全局变量g_gif_timer_factory保证退出时释放
//	
class Gif_Timer_Factory
{
public:
	Gif_Timer_Factory();
	~Gif_Timer_Factory();

	static Gif_Timer* CreateGifTimerEngine();
	static Gif_Timer* GetGifTimerEngine(bool bCreate=true);
	static void       DestroyGifTimerEngine();

private:
	static Gif_Timer*  s_giftimer;
};

//
//	GIF动画状态
//	
enum GIF_DRAW_STATUS
{
	GIF_DRAW_STATUS_START,
	GIF_DRAW_STATUS_STOP,
	GIF_DRAW_STATUS_PAUSE
};

// [0]关闭透明背景，每一帧完全覆盖前一帧     No disposal specified. The decoder is not required to take any action.
// [1]增量，每一帧相对于是前一帧增加的新内容 Do not dispose. The graphic is to be left in place.
// [2]使用透明度，绘制每帧前刷上透明背景色   Restore to background color. The area used by the graphic must be restored to the background color.
// [3]还原到前一幅图像                       Restore to previous. The decoder is required to restore the area overwritten by the graphic with what was there prior to rendering the graphic.
#define GIF_DISPOSAL_NULL               0
#define GIF_DISPOSAL_LEFT               1
#define GIF_DISPOSAL_RESTORE_BACKGROUND 2
#define GIF_DISPOSAL_RESTORE_PREVIOUS   3

#define GIF_BLOCK_FLAG_EXTENSION   0x21
#define GIF_BLOCK_FLAG_TRAILER     0x3B
#define GIF_BLOCK_FLAG_IMAGE       0x2C

#define GIF_EXT_FLAG_PLAIN         0x01
#define GIF_EXT_FLAG_CONTROL       0xF9
#define GIF_EXT_FLAG_COMMENT       0xFE
#define GIF_EXT_FLAG_APP           0xFF

#pragma pack(1)
struct GIF_FileMark
{
	GIF_FileMark()
	{
		strcpy(signature, "");
		strcpy(version, "");
	}

	char signature[3];                    //GIF标记 "GIF"
	char version[3];                      //GIF版本号 87a或89a

	bool Valid()
	{
		if( memcmp(signature, "GIF", 3) )
			return false;

		if( memcmp(version, "87a", 3) && memcmp(version, "89a", 3) )
			return false;

		return true;
	}
};

struct GIF_LogicalScreenDescriptor        // 逻辑屏幕描述块
{
	GIF_LogicalScreenDescriptor()
	{
		logical_screen_width = logical_screen_height = 0;
		size_of_global_color_table = 0;
		sort_flag = 0;
		color_resolution = 0;
		global_color_table_flag = 0;
		background_color_index = 0;
		pixel_aspect_ratio = 0;
	}

	unsigned short logical_screen_width;  // 逻辑屏幕宽度
	unsigned short logical_screen_height; // 逻辑屏幕高度

	// 包装域
	unsigned char  size_of_global_color_table : 3;  // 全局彩色表大小（全局彩色表的长度为：3*pow(2,this+1)）
	unsigned char  sort_flag : 1;                   // 彩色排序标志（0表示没有重要性排序，1表示最重要的颜色在前）
	unsigned char  color_resolution : 3;            // 彩色分辨率（??）
	unsigned char  global_color_table_flag : 1;     // 全局彩色标志（是否存在全局彩色表）

	unsigned char  background_color_index;          // 背景颜色索引
	unsigned char  pixel_aspect_ratio;              // 像素宽高比
};


struct GIF_GraphicControlExtension    // gif图片帧按钮数据
{
	GIF_GraphicControlExtension()
	{
		block_size = 0;
		transparent_color_flag = 0;
		user_input_flag = 0;
		disposal_methold = 0;
		reserved = 0;
		delay_time = 0;
		transparent_color_index = 0;
		block_terminator = 0;
	}

	unsigned char  block_size;             // 固定为4
	unsigned char  transparent_color_flag : 1;
	unsigned char  user_input_flag : 1;
	unsigned char  disposal_methold : 3;   // 重要：见GIF_DISPOSAL_NULL的定义。在绘制完当前帧图像之后（不是之前），需要如何处理：Indicates the way in which the graphic is to be treated after being displayed.
	unsigned char  reserved : 3;
	unsigned short delay_time;             // 重要：每一帧的显示时间
	unsigned char  transparent_color_index;// 重要：每一帧的透明色
	unsigned char  block_terminator;
};

struct GIF_ImageDescriptor                // 图像描述块
{
	GIF_ImageDescriptor() 
	{
		image_left_position = image_top_position = 0;
		image_width = image_height = 0;
		size_of_local_color_table = 0;
		reserved = 0;
		sort_flag = 0;
		interlace_flag = 0;
		local_color_table_flag = 0;
	}

	unsigned short image_left_position;    // 图像左边位置(px)
	unsigned short image_top_position;     // 图像顶部位置(px)
	unsigned short image_width;            // 图像宽度
	unsigned short image_height;           // 图像高度

	unsigned char size_of_local_color_table:3;      // 用于计算局部彩色表中包含的字节数
	unsigned char reserved : 2;                     // 保留
	unsigned char sort_flag : 1;                    // 与全局彩色表中的sort flag域含义相同
	unsigned char interlace_flag : 1;               // 表示该图标是不是交插图像
	unsigned char local_color_table_flag : 1;       // 局部彩色表标志
};
#pragma pack()

//
//	gif帧数据
//	
struct GIF_Frame
{
	GIF_Frame()
	{
		nRealDelayTime = 100;   // 居然还有的gif没有control块..
	}

	GIF_GraphicControlExtension  control;      // 包含了这一帧图像的延时
	GIF_ImageDescriptor          descriptor;   // 包含了这一帧图像的偏移和大小

	int           nRealDelayTime;              // = control.delay_time*10，避免重复计算
	UI::Image     image;                       // 重要：图片解析器
};


class GifImage
{
public:
	GifImage();
	~GifImage();

protected:
	void   skip_data_block(fstream& f);
	void   build_one_frame_data(GIF_FileMark*, GIF_LogicalScreenDescriptor*, 
								void* pColorTable, int nColorTableSize, void* pImageData, int nImageDataSize,
								void** ppOut, int* pOutSize  );
	int    get_next_frame_index();

	void   release_resource();
	void   draw_frame(int nPrevFrameDisposal, GIF_Frame* pFrame);
	bool   decode_gif_image_transparent(Image& image, int nTransparentIndex);

public:  // Gif绘制线程调用函数
	void   on_add_to_timer_list();
	void   on_remove_from_timer_list();
	void   on_timer(Gif_TimerItem* pTimerItem);   

public:  // 外部接口
	bool   Load(TCHAR* szPath);
	bool   Destroy();
	BOOL   SetDrawParam( HWND hWnd, int x, int y, COLORREF colTransparent );
	void   SetDrawPos( int x, int y );

	void   Start();
	void   Pause();
	void   Stop();
	void   OnPaint(HDC hDC);

	int    GetWidth()  { return m_nImageWidth; }
	int    GetHeight() { return m_nImageHeight; }
	int    GetDrawX()  { return m_nDrawX; }
	int    GetDrawY()  { return m_nDrawY; }
	HWND   GetHWnd()   { return m_hWnd; }
	int    GetFrameCount()      { return (int)m_vFrame.size(); }
	GIF_DRAW_STATUS GetStatus() { return m_nDrawStatus; }
	GIF_Frame*      GetFrame( int nIndex );

private:
	vector<GIF_Frame*>  m_vFrame;             // gif图片帧列表
	int                 m_nCurFrameIndex;     // 当前绘制帧索引

	GIF_DRAW_STATUS     m_nDrawStatus;        // 当前gif绘制状态:开始、暂停、停止
	CRITICAL_SECTION    m_sect;               // 线程数据保护
	HANDLE              m_hEvent_not_in_timer_list;  // 标识该对象还存在于计时器列表中，暂时不能删除

	int                 m_nImageWidth;        // 图片宽度
	int                 m_nImageHeight;       // 图片高度
	int                 m_nDrawX;             // 图片显示位置
	int                 m_nDrawY;             // 图片显示位置

	HWND                m_hWnd;               // 要绘制在哪个窗口上面（获取哪个窗口的DC）
	HDC                 m_hDC;                // m_hWnd的DC
	HDC                 m_hMemCanvasDC;       // m_hDC的兼容DC，双缓冲
	HBITMAP             m_hMemCanvasBitmap;   // 双缓冲
	HDC                 m_hMemPrevSaveDC;     // 保存上一帧图片的兼容DC, disposal=3的情况
	HBITMAP             m_hMemPrevSaveBitmap; // 保存上一帧图片, disposal=3的情况
	HBRUSH              m_hBrushTransparent;  // 用于实现透明的画刷
};
