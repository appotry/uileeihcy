//////////////////////////////////////////////////////////////////////////
//
//	Gif Image 
//
//		ʹ�� ATL::CImage��Ϊ����������GIFͼƬ�е�ÿһ֡������ȡ����ƴ��һ
//		�ŵ�֡GIFͼƬ���ز���ʾ
//
//	[version]
//		1.0  libo 2012.3.17 beijing  
//			��ɻ������ܣ�ʣ��TODO��
//
//	[usage]
//		1. ����Loadȥ������Դ
//		2. ����SetDrawParamȥ���û��Ʋ���
//		3. ��WM_PAINT�����е���OnPaint
//		4. ����Start,Pause,Stop���п���
//		5. ����Destory�ͷ�
//
//	[TODO]
//		1. �Ż�
//		2. ����ͼģʽ
//		3. Delay Time ��������һ֡��ʱ��<--Ӧ������һ֡��ʾ������೤ʱ������ʾ��һ֡
//		4. һЩͼƬ��WIN7�ϼ��س��������������⣬������ʾ�����磨res\\��Բ��.gif��
//		5. ��Ҫ֧�ִ���Դ�м���ͼƬ
//
//	[Remark]
//		1. ��xp����ʹ��image����gif����ʾ���쳣
//		2. TransparentBlt MSDN���ֻ֧��DDB�����Ҫ��DIBת����DDB����͸������
//		3. �������local color table��Ҫʹ��local color table������ʹ��global color table
//		4. �ṹ�����У���Ҫʹ�� char��Ҫʹ��unsigned char����������
//		5. GIF��͸��ɫ���ܺͱ���ɫ���ǰ�ɫ�������ߵ������ǲ�һ���ġ�����ڴ���GIF͸����ʱ��
//		   ��Ҫ����ȥ��������ֵ��������ֵ==͸�������⴦��
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
struct Gif_TimerItem  // ��ʱ������
{
public:
	int  nID;       // ����kill_timer
	int  nStart;    // ��ʼʱ��
	int  nWait;     // �ȴ�ʱ��
	int  nRepeat;   // ѭ������ -1��ʾ����ѭ��

	void* pData;    // GifImage*

public:
	// ��ȡʣ��ʱ��
	inline int  get_remain(){ return nWait - (::GetTickCount() - nStart); }
	// ������һ����ʾʱ��
//	inline void update_repeat() { nStart += nWait; };          // TODO: ʹ�ø÷����ɱ�֤׼ȷ�ԣ����ᵼ�����ܵ������ָ�ʱ��gif��������
    inline void update_repeat() { nStart = ::GetTickCount(); } // TODO: ʹ�øøõ��ɱ�֤ƽ���ȣ������ܱ�֤��Ӧ�ľ���
};

// �߳���Ϣ
#define GT_SET_TIMER  101   // ע�ⲻҪʹ��1������postthreadmessage�ᱨ��ERROR_MESSAGE_SYNC_ONLY
#define GT_KILL_TIMER 102

// ��ʵ���� Gif ���ż�ʱ������
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
	list<Gif_TimerItem*>     m_vItem;        // ��ǰ��Ҫ��ʱ���б�  

	HANDLE        m_hEvent_set_timer;        // Ĭ��һֱû���źţ���m_nBestWaitTime�仯ʱ���õ��źţ���������б�
	HANDLE        m_hEvent_post_message;     // Ĭ��һֱû���źţ������ⲿpostthreadmessageʱ���õ��źţ����������Ϣ����
	HANDLE        m_hMsgQueueEvent;          // һ���Զ������ڼ�⹤���̵߳���Ϣ�����Ƿ񴴽����

	HANDLE        m_hThread;                 // �����߳�
	DWORD         m_dwThreadID;              // �����߳�ID

	int           m_nBestWaitTime;           // ��ǰ��ѵȴ�ʱ��
	bool          m_bQuitThread;             // �Ƿ��˳��߳�
};

//
//	ά��Gig_Timer��ʵ�ֵ��๤����ʹ��ȫ�ֱ���g_gif_timer_factory��֤�˳�ʱ�ͷ�
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
//	GIF����״̬
//	
enum GIF_DRAW_STATUS
{
	GIF_DRAW_STATUS_START,
	GIF_DRAW_STATUS_STOP,
	GIF_DRAW_STATUS_PAUSE
};

// [0]�ر�͸��������ÿһ֡��ȫ����ǰһ֡     No disposal specified. The decoder is not required to take any action.
// [1]������ÿһ֡�������ǰһ֡���ӵ������� Do not dispose. The graphic is to be left in place.
// [2]ʹ��͸���ȣ�����ÿ֡ǰˢ��͸������ɫ   Restore to background color. The area used by the graphic must be restored to the background color.
// [3]��ԭ��ǰһ��ͼ��                       Restore to previous. The decoder is required to restore the area overwritten by the graphic with what was there prior to rendering the graphic.
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

	char signature[3];                    //GIF��� "GIF"
	char version[3];                      //GIF�汾�� 87a��89a

	bool Valid()
	{
		if( memcmp(signature, "GIF", 3) )
			return false;

		if( memcmp(version, "87a", 3) && memcmp(version, "89a", 3) )
			return false;

		return true;
	}
};

struct GIF_LogicalScreenDescriptor        // �߼���Ļ������
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

	unsigned short logical_screen_width;  // �߼���Ļ���
	unsigned short logical_screen_height; // �߼���Ļ�߶�

	// ��װ��
	unsigned char  size_of_global_color_table : 3;  // ȫ�ֲ�ɫ���С��ȫ�ֲ�ɫ��ĳ���Ϊ��3*pow(2,this+1)��
	unsigned char  sort_flag : 1;                   // ��ɫ�����־��0��ʾû����Ҫ������1��ʾ����Ҫ����ɫ��ǰ��
	unsigned char  color_resolution : 3;            // ��ɫ�ֱ��ʣ�??��
	unsigned char  global_color_table_flag : 1;     // ȫ�ֲ�ɫ��־���Ƿ����ȫ�ֲ�ɫ��

	unsigned char  background_color_index;          // ������ɫ����
	unsigned char  pixel_aspect_ratio;              // ���ؿ�߱�
};


struct GIF_GraphicControlExtension    // gifͼƬ֡��ť����
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

	unsigned char  block_size;             // �̶�Ϊ4
	unsigned char  transparent_color_flag : 1;
	unsigned char  user_input_flag : 1;
	unsigned char  disposal_methold : 3;   // ��Ҫ����GIF_DISPOSAL_NULL�Ķ��塣�ڻ����굱ǰ֡ͼ��֮�󣨲���֮ǰ������Ҫ��δ���Indicates the way in which the graphic is to be treated after being displayed.
	unsigned char  reserved : 3;
	unsigned short delay_time;             // ��Ҫ��ÿһ֡����ʾʱ��
	unsigned char  transparent_color_index;// ��Ҫ��ÿһ֡��͸��ɫ
	unsigned char  block_terminator;
};

struct GIF_ImageDescriptor                // ͼ��������
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

	unsigned short image_left_position;    // ͼ�����λ��(px)
	unsigned short image_top_position;     // ͼ�񶥲�λ��(px)
	unsigned short image_width;            // ͼ����
	unsigned short image_height;           // ͼ��߶�

	unsigned char size_of_local_color_table:3;      // ���ڼ���ֲ���ɫ���а������ֽ���
	unsigned char reserved : 2;                     // ����
	unsigned char sort_flag : 1;                    // ��ȫ�ֲ�ɫ���е�sort flag������ͬ
	unsigned char interlace_flag : 1;               // ��ʾ��ͼ���ǲ��ǽ���ͼ��
	unsigned char local_color_table_flag : 1;       // �ֲ���ɫ���־
};
#pragma pack()

//
//	gif֡����
//	
struct GIF_Frame
{
	GIF_Frame()
	{
		nRealDelayTime = 100;   // ��Ȼ���е�gifû��control��..
	}

	GIF_GraphicControlExtension  control;      // ��������һ֡ͼ�����ʱ
	GIF_ImageDescriptor          descriptor;   // ��������һ֡ͼ���ƫ�ƺʹ�С

	int           nRealDelayTime;              // = control.delay_time*10�������ظ�����
	UI::Image     image;                       // ��Ҫ��ͼƬ������
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

public:  // Gif�����̵߳��ú���
	void   on_add_to_timer_list();
	void   on_remove_from_timer_list();
	void   on_timer(Gif_TimerItem* pTimerItem);   

public:  // �ⲿ�ӿ�
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
	vector<GIF_Frame*>  m_vFrame;             // gifͼƬ֡�б�
	int                 m_nCurFrameIndex;     // ��ǰ����֡����

	GIF_DRAW_STATUS     m_nDrawStatus;        // ��ǰgif����״̬:��ʼ����ͣ��ֹͣ
	CRITICAL_SECTION    m_sect;               // �߳����ݱ���
	HANDLE              m_hEvent_not_in_timer_list;  // ��ʶ�ö��󻹴����ڼ�ʱ���б��У���ʱ����ɾ��

	int                 m_nImageWidth;        // ͼƬ���
	int                 m_nImageHeight;       // ͼƬ�߶�
	int                 m_nDrawX;             // ͼƬ��ʾλ��
	int                 m_nDrawY;             // ͼƬ��ʾλ��

	HWND                m_hWnd;               // Ҫ�������ĸ��������棨��ȡ�ĸ����ڵ�DC��
	HDC                 m_hDC;                // m_hWnd��DC
	HDC                 m_hMemCanvasDC;       // m_hDC�ļ���DC��˫����
	HBITMAP             m_hMemCanvasBitmap;   // ˫����
	HDC                 m_hMemPrevSaveDC;     // ������һ֡ͼƬ�ļ���DC, disposal=3�����
	HBITMAP             m_hMemPrevSaveBitmap; // ������һ֡ͼƬ, disposal=3�����
	HBRUSH              m_hBrushTransparent;  // ����ʵ��͸���Ļ�ˢ
};
