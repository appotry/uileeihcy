#include "stdafx.h"
#include "GifImage.h"
#include "UISDK\Kernel\Inc\Interface\imessage.h"
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"
#include "UISDK\Kernel\Src\SkinParse\datasource\skindatasource.h"

Gif_Timer::Gif_Timer()
{
	m_nBestWaitTime = INFINITE;
	m_bQuitThread = false;
	m_hEvent_set_timer = ::CreateEvent(NULL,FALSE,FALSE,NULL);
	m_hEvent_post_message = ::CreateEvent(NULL,FALSE,FALSE, NULL);
	m_hMsgQueueEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hThread = ::CreateThread( NULL, 0, Gif_Timer::ThreadProc, this, 0, &m_dwThreadID );
}
Gif_Timer::~Gif_Timer()
{
	m_bQuitThread = true;
	::SetEvent(m_hEvent_set_timer);
	::WaitForSingleObject(m_hThread, 1000);
	::CloseHandle(m_hThread);

	if( NULL != m_hEvent_set_timer )
	{
		::CloseHandle(m_hEvent_set_timer);
		m_hEvent_set_timer = NULL;
	}
	if( NULL != m_hEvent_post_message )
	{
		::CloseHandle(m_hEvent_post_message);
		m_hEvent_post_message = NULL;
	}
	if( NULL != m_hMsgQueueEvent )
	{
		::CloseHandle(m_hMsgQueueEvent);
		m_hMsgQueueEvent = NULL;
	}
}

void Gif_Timer::post_message(UINT u,WPARAM w,LPARAM l)
{  
	if (m_hMsgQueueEvent)
	{
		::WaitForSingleObject(m_hMsgQueueEvent,INFINITE);   // 等待线程创建完消息队列了才能发送消息
		::CloseHandle(m_hMsgQueueEvent);
		m_hMsgQueueEvent = NULL;
	}

	BOOL bRet = ::PostThreadMessage(m_dwThreadID, u, w, l);
	if (FALSE == bRet)
	{
		return;
	}

	::SetEvent(m_hEvent_post_message);
}

//
//	由其它线程（主线程）调用，转发给gif动画线程来处理 --> 
//
void Gif_Timer::kill_timer(int id, bool bWait)
{
	HANDLE hEvent = NULL;
	if (bWait)
	{
		hEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL);
	}
	post_message(GT_KILL_TIMER, (WPARAM)id, (LPARAM)hEvent);

	if (bWait)
	{
		::WaitForSingleObject(hEvent, INFINITE);
		CloseHandle(hEvent);
		hEvent = NULL;
	}
}

// 位于gif动画线程中
void Gif_Timer::on_kill_timer(int id, HANDLE hWaitEvent)
{
	list<Gif_TimerItem*>::iterator iter = m_vItem.begin();

	for (; iter != m_vItem.end();)
	{
		Gif_TimerItem* pItem = (*iter);
		if (NULL == pItem)
		{
			iter ++;
			continue;
		}

		if (pItem->nID == id)
		{
			delete pItem;
			pItem = NULL;
			iter = m_vItem.erase(iter);
		}
		else
			iter ++ ;
	}

	if (hWaitEvent)
	{
		::SetEvent(hWaitEvent);
	}
	
}

void Gif_Timer::set_timer(const Gif_TimerItem& item)
{
	Gif_TimerItem* pItem = new Gif_TimerItem;
	pItem->nID = item.nID;
	pItem->nRepeat = item.nRepeat;
	pItem->nStart = item.nStart;
	pItem->nWait = item.nWait;
	pItem->pData = item.pData;

	post_message(GT_SET_TIMER, (WPARAM)pItem, NULL );
}
void Gif_Timer::on_set_timer(Gif_TimerItem* pItem)
{
	if (NULL == pItem || NULL == pItem->pData)
		return;

//	GIFTRACE1_int(_T("on_set_timer: id=%d\n"), pItem->nID );
	
	// 过滤相同ID
	list<Gif_TimerItem*>::iterator iter = m_vItem.begin();
	list<Gif_TimerItem*>::iterator iterEnd = m_vItem.end();
	for (; iter != iterEnd; iter++)
	{
		if ((*iter)->nID == pItem->nID)
			return;
	}

	m_vItem.push_back(pItem);
	update_wait_time();

	::SetEvent(m_hEvent_set_timer);   // 立即开始执行线程
}

//
//	其它地方更新了一条 gif_timer_item，需要设置event去重新检测
//
void Gif_Timer::on_modify_timer()
{
//	GIFTRACE0(_T("on_modify_timer\n"));

//	update_wait_time();  // 的on_timer的最后，会再update_wait_time一次，因此这里的去掉
	::SetEvent(m_hEvent_set_timer);   // 立即开始执行线程
}

// 更新下一次触发on_timer的等待时间
void Gif_Timer::update_wait_time()
{
	int oldWaitTime = m_nBestWaitTime;

	// 取最小时间为等待时间
	int nCount = (int)m_vItem.size();
	if( nCount == 0 )
	{
		m_nBestWaitTime = INFINITE;
	}
	else if(nCount == 1)
	{
		m_nBestWaitTime = (*m_vItem.begin())->get_remain();
		if( m_nBestWaitTime < 0 )
			m_nBestWaitTime = 0;
	}
	else
	{
		list<Gif_TimerItem*>::iterator iter = m_vItem.begin();
		list<Gif_TimerItem*>::iterator iterEnd = m_vItem.end();
		int a = (*iter)->get_remain();;

		for ( ; iter != iterEnd; iter ++ )
		{
			int b = (*iter)->get_remain();;
			a = min(a,b);
		}
		m_nBestWaitTime = a;

		if( m_nBestWaitTime < 0 )
			m_nBestWaitTime = 0;
	}
}

// 检查列表中的timer item，如果到达响应时间点，则触发on_timer
void Gif_Timer::check_timer()
{
	//GIFTRACE0(_T("Gif_Timer::check_timer\n"));

	list<Gif_TimerItem*>::iterator iter = m_vItem.begin();
	for (; iter != m_vItem.end();)
	{
		Gif_TimerItem* pItem = *iter;
		if (NULL == pItem)
		{
			iter++;
			continue;;
		}

		if (pItem->get_remain() <= 0)
		{
			this->on_timer(pItem);

			if (pItem->nRepeat != -1 && 0 == --pItem->nRepeat)
			{
				// 用完了循环次数，删除
				delete pItem;
				pItem = NULL;

				iter = m_vItem.erase(iter);
				continue;	
			}
			else
			{
				// 重新计算下一次的响应时间
				// pItem->update_repeat();    // gifimage在on_timer响应中只修改了wait值，在这里对start值做更新，
			}
		}
		iter++;
	}
	
	this->update_wait_time();
}

void Gif_Timer::on_timer(Gif_TimerItem* pItem)
{
	if( NULL == pItem )
		return;

	GifImageRender* pImageDrawItem = (GifImageRender*)pItem->pData;
	if (NULL == pImageDrawItem)
		return;

	pImageDrawItem->on_timer(pItem);
}

/*static*/ DWORD /*WINAPI*/ Gif_Timer::ThreadProc(LPVOID lpParameter)
{
	Gif_Timer* pThis = (Gif_Timer*) lpParameter;
	if( NULL == pThis )
		return 0;

	// 第一时间内，创建消息队列，避免外部发送消息失败
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	::SetEvent(pThis->m_hMsgQueueEvent);

	while (false == pThis->m_bQuitThread)
	{
		HANDLE arrHandles[2] = { pThis->m_hEvent_post_message, pThis->m_hEvent_set_timer };
		DWORD dwRet = ::WaitForMultipleObjects(2, arrHandles, FALSE, pThis->m_nBestWaitTime );   // 保持该event一直没有信息，去阻塞这个线程

		if (dwRet == WAIT_OBJECT_0)
		{
			BOOL bRet = FALSE;
			MSG msg;
			while ((bRet = PeekMessage( &msg, NULL, 0, 0, PM_REMOVE)) != 0)
			{ 
				if (bRet == -1)
				{
					// handle the error and possibly exit
				}
				else
				{
					switch(msg.message)
					{
					case GT_SET_TIMER:
						pThis->on_set_timer((Gif_TimerItem*)msg.wParam);
						break;

					case GT_KILL_TIMER:
						pThis->on_kill_timer((int)msg.wParam, (HANDLE)msg.lParam);
						break;
					}
				} 
			} 
		}
		else
		{
			pThis->check_timer();
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////

Gif_Timer_Factory g_gif_timer_factory; // 构造一个全局对象，在程序退出时保证析构s_giftimer成员

Gif_Timer* Gif_Timer_Factory::s_giftimer = NULL;

Gif_Timer_Factory::Gif_Timer_Factory()
{
}
Gif_Timer_Factory::~Gif_Timer_Factory()
{
	Gif_Timer_Factory::DestroyGifTimerEngine();
}

/*static*/ Gif_Timer* Gif_Timer_Factory::CreateGifTimerEngine()
{
	if( NULL == s_giftimer )
	{
		s_giftimer = new Gif_Timer();
	}
	return s_giftimer;
}

/*static*/ Gif_Timer* Gif_Timer_Factory::GetGifTimerEngine(bool bCreate)
{
	if( bCreate )
		return CreateGifTimerEngine();
	else
		return s_giftimer;
}

/*static*/ void Gif_Timer_Factory::DestroyGifTimerEngine()
{
	if( NULL != s_giftimer )
	{
		delete s_giftimer;
		s_giftimer = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
#pragma region

GifImageRender::GifImageRender(GifImageBase* pGifImage, Gif_Timer_Notify* pNotify, HWND hForwardMsgHWND)
{
	m_pGifImage = pGifImage;
	m_nCurFrameIndex = 0;
	m_nDrawStatus = GIF_DRAW_STATUS_STOP;

	m_hForwardMsgHWND = hForwardMsgHWND;

	m_hMemCanvasDC = NULL;
	m_hMemCanvasBitmap = NULL;

	m_hMemPrevSaveDC = NULL;
	m_hMemPrevSaveBitmap = NULL;

	m_notify.eType = Gif_Timer_Notify_Direct_Hwnd;
	m_notify.notify_hwnd.hDC = NULL;
	m_notify.notify_hwnd.hWnd = NULL;
	m_notify.notify_hwnd.x = m_notify.notify_hwnd.y = 0;
	m_notify.notify_ui_msg.nTimerID = 0;
	m_notify.notify_ui_msg.pNotifyMsgObj = NULL;
	m_notify.notify_ui_msg.lParam = 0;

	if (pNotify)
	{
		::memcpy(&m_notify, pNotify, sizeof(Gif_Timer_Notify));

		if (m_notify.notify_hwnd.hWnd)
			m_notify.notify_hwnd.hDC = ::GetDC(m_notify.notify_hwnd.hWnd);
	}

	m_hMemCanvasDC = ::CreateCompatibleDC(NULL);
	Image image; 
	image.Create(m_pGifImage->GetWidth(),m_pGifImage->GetHeight(), 32, Image::createAlphaChannel);
	m_hMemCanvasBitmap = image.Detach();

	/*HBITMAP hOldBmp = */(HBITMAP)::SelectObject(m_hMemCanvasDC, m_hMemCanvasBitmap);

	RECT rcBack = {0,0, m_pGifImage->GetWidth(),m_pGifImage->GetHeight()};
	::FillRect( m_hMemCanvasDC,&rcBack,m_pGifImage->m_hBrushTransparent);
//	::FillRect( m_hMemPrevSaveDC,&rcBack,m_hBrushTransparent);
}

GifImageRender::~GifImageRender()
{
	if (m_nDrawStatus != GIF_DRAW_STATUS_STOP)
		this->Stop();

	if (Gif_Timer_Factory::GetGifTimerEngine(false))
		Gif_Timer_Factory::GetGifTimerEngine(false)->kill_timer((int)this, true);

	if (m_hMemCanvasBitmap)
	{
		DeleteObject(m_hMemCanvasBitmap);
		m_hMemCanvasBitmap = NULL;
	}
	if (m_hMemCanvasDC)
	{
		::DeleteDC(m_hMemCanvasDC);
		m_hMemCanvasDC = NULL;
	}
	if (m_hMemPrevSaveDC)
	{
		::DeleteDC(m_hMemPrevSaveDC);
		m_hMemPrevSaveDC = NULL;
	}
	if (m_hMemPrevSaveBitmap)
	{
		::DeleteObject(m_hMemPrevSaveBitmap);
		m_hMemPrevSaveBitmap = NULL;
	}
	if (m_notify.notify_hwnd.hDC)
	{
		::ReleaseDC(m_notify.notify_hwnd.hWnd, m_notify.notify_hwnd.hDC);
		m_notify.notify_hwnd.hDC = NULL;
	}
}

bool GifImageRender::ModifyRenderParam(Gif_Timer_Notify* pNotify)
{
	if (NULL == pNotify)
		return false;

	EnterCriticalSection(&m_pGifImage->m_sect);
	if (m_notify.notify_hwnd.hDC)
	{
		::ReleaseDC(m_notify.notify_hwnd.hWnd, m_notify.notify_hwnd.hDC);
		m_notify.notify_hwnd.hDC = NULL;
	}

	::memcpy(&m_notify, pNotify, sizeof(Gif_Timer_Notify));
	if (m_notify.notify_hwnd.hWnd)
		m_notify.notify_hwnd.hDC = ::GetDC(m_notify.notify_hwnd.hWnd);

	LeaveCriticalSection(&m_pGifImage->m_sect);
	return true;
}

void GifImageRender::Release()
{
	if (NULL == m_pGifImage)
	{
		delete this;
		return;
	}

	m_pGifImage->DeleteRender(this);  // 注：该调用会触发delete this.因此后面不准再添加其它代码了
	return;
}

#pragma endregion
//////////////////////////////////////////////////////////////////////////

GifImageBase::GifImageBase(HWND hForwardMsgHWND)
{
	m_hBrushTransparent = NULL;
	m_nNextRenderItemIndex = 0;
	m_nImageWidth = 0;
	m_nImageHeight = 0;
	m_hForwardMsgHWND = hForwardMsgHWND;

    m_pIGifImage = NULL;
    m_bCreateIGifImage = false;
	InitializeCriticalSection(&m_sect);
}
GifImageBase::~GifImageBase()
{
    if (m_bCreateIGifImage)
    {
        SAFE_DELETE(m_pIGifImage);
    }
	DeleteCriticalSection(&m_sect);
}

void  GifImageBase::SetIGifImage(IGifImage* p)
{
    m_pIGifImage = p;
}
IGifImage*  GifImageBase::GetIGifImage()
{
    if (m_pIGifImage)
        return m_pIGifImage;

    m_pIGifImage = new IGifImage(this);
    m_bCreateIGifImage = true;
    return m_pIGifImage;
}

GifImage::GifImage(HWND hForwardMsgHWND):GifImageBase(hForwardMsgHWND)
{
    
}
GifImage::~GifImage()
{
   this->Destroy();
}

bool GifImage::Destroy()
{
	EnterCriticalSection(&m_sect);
	int nSize = (int) m_vFrame.size();
	for (int i = 0; i < nSize; i++)
	{
		m_vFrame[i]->image.Destroy();
		delete m_vFrame[i];
		m_vFrame[i] = NULL;
	}
	m_vFrame.clear();

	GifImageRenderItemMap::iterator iter = m_mapRenderItem.begin();
	GifImageRenderItemMap::iterator iterEnd = m_mapRenderItem.end();
	for (; iter != iterEnd; iter++)
	{
		GifImageRender* pItem = iter->second;
		SAFE_DELETE(pItem);
	}
	m_mapRenderItem.clear();

	if (m_hBrushTransparent)
	{
		::DeleteObject(m_hBrushTransparent);
		m_hBrushTransparent = NULL;
	}
	LeaveCriticalSection(&m_sect);

	return true;
}

//
// 跳过gif文件中的数据部分
//
int GifImage::skip_data_block(IStreamBufferReader*  pBuffer, byte* pBits)
{
	int nDataLength = 0;              // 统计数据的总大小
	unsigned char cNextBlockLen = 0;  // 读取下一段的数据大小
	do 
	{
		pBuffer->read((char*)&cNextBlockLen, 1);  // 本段数据长度 最大为FF，如果为0表示数据段结束
		if( 0 == cNextBlockLen )
			break;

		if (pBits)
		{
			pBuffer->read(((char*)pBits+nDataLength), cNextBlockLen);
		}
		else
		{
			pBuffer->seek(cNextBlockLen, ios_base::cur);
		}
		nDataLength += cNextBlockLen;
	} 
	while (1);
	return nDataLength;
}

//
//	组建一帧gif数据文件
//
void GifImage::build_one_frame_data(
			GIF_FileMark* pFileMark,                  // 
			GIF_LogicalScreenDescriptor* pDesc,       // 
			void* pColorTable, int nColorTableSize,   // 色彩表数据，当存在local color table时，使用local color table数据
			void* pImageData,  int nImageDataSize,    // 单帧数据
			void** ppOut, int* pOutSize               // 返回值,ppOut需要delete[]进行释放
		)
{
	assert( NULL != pFileMark );
	assert( NULL != pDesc );
	assert( NULL != pImageData );
	assert( nImageDataSize > 0 );
	assert( NULL != ppOut);
	assert( NULL != pOutSize) ;

	int nTotalSize = sizeof(GIF_FileMark) + sizeof(GIF_LogicalScreenDescriptor) + nColorTableSize + nImageDataSize + 1;
	char* one_frame_data = new char[nTotalSize];

	int pos = 0;
	memcpy(one_frame_data+pos, pFileMark, sizeof(GIF_FileMark));
	pos += sizeof(GIF_FileMark);

	memcpy(one_frame_data+pos, pDesc, sizeof(GIF_LogicalScreenDescriptor));
	pos += sizeof(GIF_LogicalScreenDescriptor);

	if( pColorTable > 0 )
	{
		memcpy(one_frame_data+pos, pColorTable, nColorTableSize );
		pos += nColorTableSize;
	}

	memcpy(one_frame_data+pos, pImageData, nImageDataSize);
	pos += nImageDataSize;

	byte byteTrailer = GIF_BLOCK_FLAG_TRAILER;
	memcpy(one_frame_data+pos, &byteTrailer, 1 );
	pos += 1;

	assert(pos == nTotalSize);

	*pOutSize = nTotalSize;
	*ppOut = one_frame_data;
}

//
//	使用路径进行文件GIF加载（只有外部请求一人render时，才真正调用load加载文件）
//
bool GifImage::Load(const TCHAR* szPath, IMapAttribute* pMapAttrib)
{
    IStreamBufferReader*  pStreamBuffer = NULL;

    CreateStreamBuffer(STREAM_TYPE_FILE, &pStreamBuffer);
    bool b = ((IFileBufferReader*)pStreamBuffer)->load(szPath);
    if (!b)
        return false;

    b = Load(pStreamBuffer, pMapAttrib);
    SAFE_RELEASE(pStreamBuffer);

	return b;
}

bool  GifImage::Load(IStreamBufferReader*  pBuffer, IMapAttribute* pMapAttrib)
{
    return RealLoad(pBuffer);
}

bool GifImage::RealLoad(IStreamBufferReader*  pBuffer)
{
	UIASSERT(0 == m_vFrame.size());
    if (NULL == pBuffer)
        return false;    

	bool bRet = false;
    int nRead = 0;
	do 
	{
		// 文件标志
		GIF_FileMark header;
		nRead = pBuffer->read((char*)&header, sizeof(GIF_FileMark));
		if (nRead < sizeof(GIF_FileMark) || !header.Valid()) 
            break;

		// 文件全局信息
		GIF_LogicalScreenDescriptor logicScreenDesc;
		nRead = pBuffer->read((char*)&logicScreenDesc, sizeof(GIF_LogicalScreenDescriptor));
        if (nRead < sizeof(GIF_LogicalScreenDescriptor))
            break;

		this->m_nImageWidth = logicScreenDesc.logical_screen_width;
		this->m_nImageHeight = logicScreenDesc.logical_screen_height;

		// 全局颜色表
		byte* pGlobalColorTable = NULL;
		int   nGlobalColorTableSize = 0;

		if (logicScreenDesc.global_color_table_flag)
			nGlobalColorTableSize = static_cast<int>(3*pow((double)2,(int)(logicScreenDesc.size_of_global_color_table+1)));

		if (0 != nGlobalColorTableSize)
		{
			pGlobalColorTable = new byte[nGlobalColorTableSize];
			nRead = pBuffer->read((char*)pGlobalColorTable, nGlobalColorTableSize);

			// 获取背景画刷
// 			int R = pGlobalColorTable[logicScreenDesc.background_color_index*3];
// 			int G = pGlobalColorTable[logicScreenDesc.background_color_index*3+1];
// 			int B = pGlobalColorTable[logicScreenDesc.background_color_index*3+2];
//			m_hBrushTransparent = CreateSolidBrush(RGB(R,G,B));
		}

		// 循环读取每一帧的数据，重组成一幅幅单帧的gif文件，让image加载。
		GIF_Frame* pFrame = NULL;
		int nFrameStartPos = 0;
	
		do 
		{
			BYTE bNextBlockFlag = 0;
			nRead = pBuffer->read((char*)&bNextBlockFlag, 1);
            if (nRead < 1)
                break;
            
			if (bNextBlockFlag == GIF_BLOCK_FLAG_TRAILER)
			{
				bRet = true;
				break;   // 文件结束
			}

			if (NULL == pFrame)
			{
				pFrame = new GIF_Frame;
				nFrameStartPos = pBuffer->tell();
				nFrameStartPos -= 1;
			}

			switch (bNextBlockFlag)
			{
			case GIF_BLOCK_FLAG_EXTENSION:
				{
					BYTE bNextExtFlag = 0;
					pBuffer->read ((char*)&bNextExtFlag, 1);

					switch (bNextExtFlag)
					{
					case GIF_EXT_FLAG_PLAIN:
						{
							// 跳过 
							pBuffer->seek(13,ios_base::cur);
							skip_data_block(pBuffer);
						}
						break;
					case GIF_EXT_FLAG_CONTROL:
						{
							// 跳过
							pBuffer->read((char*)&pFrame->control, sizeof(GIF_GraphicControlExtension));
							pFrame->nRealDelayTime = pFrame->control.delay_time*10;
							if ( 0 == pFrame->nRealDelayTime )    // 有些gif该值为0
								pFrame->nRealDelayTime = 100;

							assert( pFrame->control.disposal_methold != 3 );// 这种GIF没有经没测试，没找到这样素材
						}
						break;
					case GIF_EXT_FLAG_COMMENT:
						{
							// 跳过
							skip_data_block(pBuffer);
						}
						break;
					case GIF_EXT_FLAG_APP:
						{
							// 跳过 
							pBuffer->seek(12,ios_base::cur);
							skip_data_block(pBuffer);
						}
						break;
					};
				}
				break;
			case GIF_BLOCK_FLAG_IMAGE:
				{
					pBuffer->read((char*)&pFrame->descriptor, sizeof(GIF_ImageDescriptor));

					//////////////////////////////////////////////////////////////////////////
					// 本地色彩表

					byte* pLocalColorTable = NULL;
					int   nLocalColorTableSize = 0;

					if( pFrame->descriptor.local_color_table_flag )
						nLocalColorTableSize = static_cast<int>(3*pow((double)2,pFrame->descriptor.size_of_local_color_table+1));

					if( 0 != nLocalColorTableSize )
					{
						pLocalColorTable = new byte[nLocalColorTableSize];
						pBuffer->read((char*)pLocalColorTable, nLocalColorTableSize);
					}

					//////////////////////////////////////////////////////////////////////////
					// 解码

					byte* pColorTableForThisFrame     = pLocalColorTable==NULL? pGlobalColorTable:pLocalColorTable;
					int   nColorTableSizeForThisFrame = pLocalColorTable==NULL? nGlobalColorTableSize:nLocalColorTableSize;

					BYTE byte_LZW_code_size = 0;
					pBuffer->read((char*)&byte_LZW_code_size,1);   // 这个值是干什么的？  <-- 是LZW算法的初始位长度LZW code size 

					// 先尝试使用自己的解码算法
					bool bDecodeRet = this->decode_by_lzw(pBuffer, pFrame, byte_LZW_code_size, pColorTableForThisFrame, nColorTableSizeForThisFrame);
					if (false == bDecodeRet)
					{
						// 失败了再尝试用gdiplus的方法解码
						bDecodeRet = this->decode_by_gdiplus(pBuffer, pFrame, nFrameStartPos, header, logicScreenDesc, pColorTableForThisFrame, nColorTableSizeForThisFrame);
					}

					if( NULL != pLocalColorTable )
					{
						delete[] pLocalColorTable;
						pLocalColorTable = NULL;
						nLocalColorTableSize = 0;
					}

					if (false == bDecodeRet)
					{
						this->Destroy();
						goto PARSE_ERROR;
					}

					// 保存帧
					m_vFrame.push_back(pFrame);
					pFrame = NULL;
				}
				break;

			};  // switch(bNextBlockFlag)

		} while (1);  // do

PARSE_ERROR:
		if (pFrame)  // 剩余部分没有image data了，导致还new了一次，在这里释放掉
		{
			delete pFrame;
			pFrame = NULL;
		}

		delete[] pGlobalColorTable;
		pGlobalColorTable = NULL;
	} 
	while (0);

	return bRet;
}

bool GifImage::decode_by_lzw(IStreamBufferReader*  pBuffer, GIF_Frame* pFrame, int byte_LZW_code_size, byte* pColorTable, int nColorTableSize)
{
	byte*   pGifFrameImageData = NULL;
	byte*   pOutputData = NULL;
	int     nOutputDataSize = pFrame->descriptor.image_width*pFrame->descriptor.image_height; 

	//////////////////////////////////////////////////////////////////////////
	// 读取图像数据用于解码（先读取数据大小）

	int nDataBeginPos = pBuffer->tell();
	int nDataLength = skip_data_block(pBuffer);
    pBuffer->seek(nDataBeginPos, ios_base::beg);

	pGifFrameImageData = new byte[nDataLength];
	skip_data_block(pBuffer, pGifFrameImageData);

	//////////////////////////////////////////////////////////////////////////
	// LZW解码

	pOutputData = new byte[nOutputDataSize];
	memset(pOutputData, 0, nOutputDataSize);

	GifLZWDecoder decoder(byte_LZW_code_size, pOutputData, nOutputDataSize);
	if (false == decoder.Decode(pGifFrameImageData, nDataLength))
	{
		SAFE_ARRAY_DELETE(pGifFrameImageData);
		SAFE_ARRAY_DELETE(pOutputData);

        pBuffer->seek(nDataBeginPos, ios_base::beg);   // 重新让gdiplus解码的位置
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 解析交叉GIF类型的数据
	
	if (pFrame->descriptor.interlace_flag)
	{
		byte*   pInterlaceData = new byte[nOutputDataSize];
		int     nWriteCursor = 0;
		int     nReadCursor = 0;
		int     nRowDataSize = pFrame->descriptor.image_width;

		// Group 1，每隔8行取一次，从第0行开始写回
		for (int row = 0; row < pFrame->descriptor.image_height; row+=8)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}

		// Group 2，每隔8行取一次，从第4行开始写回
		for (int row = 4; row < pFrame->descriptor.image_height; row+=8)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}

		// Group 3，每隔4行取一次，从第2行开始写回
		for (int row = 2; row < pFrame->descriptor.image_height; row+=4)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}

		// Group 4，每隔2行取一次，从第1行开始写回
		for (int row = 1; row < pFrame->descriptor.image_height; row+=2)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}
		SAFE_ARRAY_DELETE(pOutputData);
		pOutputData = pInterlaceData;
	}

	//////////////////////////////////////////////////////////////////////////
	// 解析调色板和透明索引

	pFrame->image.Create(pFrame->descriptor.image_width,pFrame->descriptor.image_height, 32, Image::createAlphaChannel);

	BYTE*   pBits = (BYTE*)pFrame->image.GetBits();
	int     bytesperline = 4*pFrame->descriptor.image_width;  
	int     npxIndex = 0;

	for (int row = 0; row < pFrame->descriptor.image_height; row ++ )
	{
		for( int i = 0; i < bytesperline; i += 4 )
		{
			int nColorTableIndex = pOutputData[npxIndex++];    // 该像素对应的调色板的值
			int nColorTableIndexAddr = 3*nColorTableIndex;

			if (pFrame->control.transparent_color_flag && 
				pFrame->control.transparent_color_index == nColorTableIndex)
			{

				pBits[i]   = 0;          // 注：这里必须用0，相当于alpha通道的预乘，否则image.draw将变成白底，而不是透明
				pBits[i+1] = 0;
				pBits[i+2] = 0;

				pBits[i+3] = 0;          // 该像素透明
			}
			else
			{
				pBits[i]   = pColorTable[nColorTableIndexAddr+2]; // B
				pBits[i+1] = pColorTable[nColorTableIndexAddr+1]; // G
				pBits[i+2] = pColorTable[nColorTableIndexAddr];   // R

				pBits[i+3] = 255;
			}
		}

		pBits += pFrame->image.GetPitch();
	}

#ifdef _DEBUG // <-- 将每一帧保存为一个文件
	static int n = 0;
	TCHAR szPath[MAX_PATH] = _T("");
	_stprintf(szPath, _T("C:\\one_frame\\%d.png"),n );
	pFrame->image.Save(szPath, Gdiplus::ImageFormatPNG);
#endif

	SAFE_ARRAY_DELETE(pGifFrameImageData);
	SAFE_ARRAY_DELETE(pOutputData);
	return true;
}

bool GifImage::decode_by_gdiplus(IStreamBufferReader*  pBuffer, 
								 GIF_Frame* pFrame, 
								 int  nFrameStartPos,
								 GIF_FileMark& header, 
								 GIF_LogicalScreenDescriptor& logicScreenDesc,
								 byte* pColorTable, 
								 int nColorTableSize )
{
	skip_data_block(pBuffer);
	int nFrameEndPos = pBuffer->tell();

	// 单帧图像数据
	int   nImageDataSize = nFrameEndPos-nFrameStartPos;
	void* pImageData = (void*)new char[nImageDataSize];

	pBuffer->seek(nFrameStartPos, ios_base::beg);
	pBuffer->read((char*)pImageData, nImageDataSize);

	// 组装
	void* one_frame_gif_file_data = NULL;
	int   one_frame_gif_file_data_size = 0;

	// TODO: 真的有必要吗？Gdiplus内部应该会判断选哪个颜色表吧
// 	if( NULL == pLocalColorTable )
// 		this->build_one_frame_data(&header, &logicScreenDesc, pGlobalColorTable, nGlobalColorTableSize, pImageData, nImageDataSize, &one_frame_gif_file_data, &one_frame_gif_file_data_size );
// 	else
// 		this->build_one_frame_data(&header, &logicScreenDesc, pLocalColorTable, nLocalColorTableSize, pImageData, nImageDataSize, &one_frame_gif_file_data, &one_frame_gif_file_data_size );

	this->build_one_frame_data(&header, &logicScreenDesc, pColorTable, nColorTableSize, pImageData, nImageDataSize, &one_frame_gif_file_data, &one_frame_gif_file_data_size);

	if ( NULL != pImageData )
	{
		delete[] pImageData;
		pImageData = NULL;
		nImageDataSize = 0;
	}

#ifdef _DEBUG_x // <-- 将每一帧保存为一个文件
	static int n = 0;
	TCHAR szPath[MAX_PATH] = _T("");
	_stprintf(szPath, _T("C:\\one_frame\\%d.gif"),n++ );
	fstream temp;
	temp.open(szPath,ios_base::out|ios_base::binary);
	if( temp.good() )
	{
		temp.write((const char*)one_frame_gif_file_data, one_frame_gif_file_data_size);
		temp.close();
	}
#endif

	// 加载图片
	if (false == pFrame->image.LoadFromData(one_frame_gif_file_data, one_frame_gif_file_data_size))
	{
		delete [] one_frame_gif_file_data;
		one_frame_gif_file_data = NULL;

		return false;
	}
	delete [] one_frame_gif_file_data;
	one_frame_gif_file_data = NULL;


	// 设置背景透明色
	if (pFrame->control.transparent_color_flag)
	{
		if (false == this->decode_gif_image_transparent(pFrame, (LONG)pFrame->control.transparent_color_index))
			return false;

		//pFrame->image.SetTransparentColor((LONG)pFrame->control.transparent_color_index);
	}
	return true;
}

int GifImageRender::get_next_frame_index()
{
	int nIndex = m_nCurFrameIndex;
	m_nCurFrameIndex++;
	if (m_nCurFrameIndex >= (int)m_pGifImage->m_vFrame.size())
		m_nCurFrameIndex = 0;

	return nIndex;
}
GIF_Frame* GifImageBase::GetFrame(int nIndex)
{
	if( nIndex < 0 || nIndex >= (int)m_vFrame.size() )
		return NULL;

	return m_vFrame[nIndex];
}

//
//	设置绘制参数。在显示前必须调用该函数 
//
//	Return
//		成功返回TRUE，失败返回FALSE
//
GifImageRender* GifImageBase::AddRender(Gif_Timer_Notify* pNotify, int* pIndex)
{
	if (NULL == pNotify)
		return NULL;

	if (0 == m_mapRenderItem.size())
	{
        // 		if (false == this->RealLoad())   // 只有在真正有人请求一个Render时，才创建
        // 			return NULL;
        // 

        if (NULL == m_hBrushTransparent)
            this->SetTransparentColor();
    }

	if (m_vFrame.size() <= 0)
		return NULL;

	GifImageRender* pDrawItem = new GifImageRender(this, pNotify, m_hForwardMsgHWND);
	m_mapRenderItem.insert(make_pair(m_nNextRenderItemIndex, pDrawItem));
	
	if (pIndex)
	{
		*pIndex = m_nNextRenderItemIndex;
	}
	m_nNextRenderItemIndex++;

	return pDrawItem;
}
bool GifImageBase::ModifyRender(Gif_Timer_Notify* pNotify, int nIndex)
{
	if (m_vFrame.size() <= 0 || NULL == pNotify)
		return false;

	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (NULL == pItem)
		return false;

	return pItem->ModifyRenderParam(pNotify);
}

bool GifImageBase::DeleteRender(int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (NULL == pItem)
		return false;

	m_mapRenderItem.erase(nIndex);
	SAFE_DELETE(pItem);

	if (m_mapRenderItem.size() == 0)
	{
		this->Destroy();
	}
	return true;
}
void GifImageBase::DeleteRender(GifImageRender*  pItem)
{
	GifImageRenderItemMap::iterator iter = m_mapRenderItem.begin();	
	GifImageRenderItemMap::iterator iterEnd = m_mapRenderItem.end();

	for (; iter != iterEnd; iter++)
	{
		if (iter->second == pItem)
		{
			m_mapRenderItem.erase(iter);
			break;
		}
	}
	SAFE_DELETE(pItem);

	if (m_mapRenderItem.size() == 0)
	{
		this->Destroy();
	}
}

bool GifImageBase::SetTransparentColor(COLORREF colTransparent)
{
	SAFE_DELETE_GDIOBJECT(m_hBrushTransparent);
	m_hBrushTransparent = ::CreateSolidBrush(colTransparent);

	return true;
}


GifImageRender*  GifImageBase::GetDrawItemByIndex(int nIndex)
{
	if (-1 == nIndex)
		nIndex = 0;

	GifImageRenderItemMap::iterator iter= m_mapRenderItem.find(nIndex);
	if (iter == m_mapRenderItem.end())
		return NULL;
	else
		return iter->second;
}

int GifImageRender::GetWidth()
{
	if (NULL == m_pGifImage)
		return 0;

	return m_pGifImage->GetWidth();
}
int GifImageRender::GetHeight()
{
	if (NULL == m_pGifImage)
		return 0;

	return m_pGifImage->GetHeight();
}

//
//	TODO: 如何实现立即显示第一帧数据
//
void GifImageBase::Start(int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (NULL == pItem)
		return;
	
	pItem->Start();
}
void GifImageRender::Start()
{
	int nSize = (int)m_pGifImage->m_vFrame.size();
	if (nSize <= 0)
		return ;

	if (m_nDrawStatus == GIF_DRAW_STATUS_START)
		return;

	m_nDrawStatus = GIF_DRAW_STATUS_START;
	if (nSize == 1)    // 单帧gif图片不需要使用计时器
	{
		EnterCriticalSection(&m_pGifImage->m_sect);
		handle_disposal(NULL);                           // 刷背景
		draw_frame( m_pGifImage->GetFrame(m_nCurFrameIndex));   // 绘制第一帧到m_hMemCanvasDC中
		commit(m_notify.notify_hwnd.hDC, m_notify.notify_hwnd.x,m_notify.notify_hwnd.y);
		LeaveCriticalSection(&m_pGifImage->m_sect);
	}
	else
	{
		Gif_TimerItem timer_item = { (int)this, ::GetTickCount(), 0, -1, this };  // -1无限循环
		Gif_Timer_Factory::CreateGifTimerEngine()->set_timer(timer_item);
	}
}

void GifImageBase::Pause(int nIndex)
{
	if (m_vFrame.size() <= 0)
		return ;

	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (NULL == pItem)
		return;

	pItem->Pause();
}
void GifImageRender::Pause()
{
	m_nDrawStatus = GIF_DRAW_STATUS_PAUSE;
}
void GifImageBase::Stop(int nIndex)
{
	if (m_vFrame.size() <= 0)
		return ;

	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (NULL == pItem)
		return;

	pItem->Stop();
}
void GifImageRender::Stop()
{
	// 刷新父窗口，这里需要注意进行同步，线程中的最后一次on_timer可能将下面的刷背景又覆盖了
	EnterCriticalSection(&m_pGifImage->m_sect);
	m_nCurFrameIndex = 0;
	m_nDrawStatus = GIF_DRAW_STATUS_STOP;

	handle_disposal(NULL);  // 刷背景
	this->commit(m_notify.notify_hwnd.hDC, m_notify.notify_hwnd.x,m_notify.notify_hwnd.y);

	LeaveCriticalSection(&m_pGifImage->m_sect);
}

//
//	外部窗口调用
//
void GifImageBase::OnPaint(HDC hDC, int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (NULL == pItem)
		return;

	pItem->OnPaint(hDC, 0, 0);
}

//
//	UI控件调用，x,y为控件内部坐标，如(0,0)
//
void GifImageBase::OnPaint(HDC hDC, int x, int y, int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (NULL == pItem)
		return;

	pItem->OnPaint(hDC, x, y);
}
void GifImageRender::OnPaint(HDC hDC, int x, int y)
{
	if (GIF_DRAW_STATUS_STOP == m_nDrawStatus)
		return;

	EnterCriticalSection(&m_pGifImage->m_sect);
	::BitBlt(hDC, x, y, m_pGifImage->GetWidth(), m_pGifImage->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);
	LeaveCriticalSection(&m_pGifImage->m_sect);
}

void GifImageRender::OnAlphaPaint(HDC hDC, int x, int y)
{
    if (GIF_DRAW_STATUS_STOP == m_nDrawStatus)
        return;

    EnterCriticalSection(&m_pGifImage->m_sect);

    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    ::AlphaBlend( hDC, 
        x, y, m_pGifImage->GetWidth(), m_pGifImage->GetHeight(),
        m_hMemCanvasDC, 0, 0, m_pGifImage->GetWidth(), m_pGifImage->GetHeight(), bf);

    LeaveCriticalSection(&m_pGifImage->m_sect);
}

void GifImageRender::commit(HDC hDC, int x, int y)
{
	switch (m_notify.eType)
	{
	case Gif_Timer_Notify_Direct_Hwnd:
		if (hDC)
		{
			::BitBlt(hDC, x, y, 
				m_pGifImage->GetWidth(), m_pGifImage->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);
		}
		break;
	case Gif_Timer_Notify_Post_Thread_Msg:
		{		
			// 转发到界面线程
            UIMSG msg;
			msg.message = WM_TIMER;
			msg.wParam = m_notify.notify_ui_msg.nTimerID;
			msg.lParam = m_notify.notify_ui_msg.lParam;
			msg.pMsgTo = m_notify.notify_ui_msg.pNotifyMsgObj;
			UIPostMessage(m_hForwardMsgHWND, &msg);
		}
		break;
	}
}

GIF_DRAW_STATUS GifImageBase::GetStatus(int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (NULL == pItem)
		return GIF_DRAW_STATUS_STOP;

	return pItem->GetStatus();
}
GIF_DRAW_STATUS GifImageRender::GetStatus()
{
	return m_nDrawStatus; 
}

//
//	当一帧绘制完成后，并到达delay时间，将要绘制下一帧时，处理它的disposal
//
void GifImageRender::handle_disposal(GIF_Frame* pFrame)
{
	// 当pFrame为空时，默认就刷新整个图片的背景
	int  nDisposal = GIF_DISPOSAL_RESTORE_BACKGROUND;
	RECT rcBack = {0,0, m_pGifImage->GetWidth(),m_pGifImage->GetHeight()};

	if (pFrame)
	{
		nDisposal = pFrame->control.disposal_methold;
		::SetRect(&rcBack, 
			pFrame->descriptor.image_left_position,
			pFrame->descriptor.image_top_position,
			pFrame->descriptor.image_left_position+pFrame->descriptor.image_width,
			pFrame->descriptor.image_top_position+pFrame->descriptor.image_height);
	}

	switch (nDisposal)
	{
	case GIF_DISPOSAL_NULL:
	case GIF_DISPOSAL_LEFT:
		break;

	case GIF_DISPOSAL_RESTORE_BACKGROUND:
		{
			::FillRect(m_hMemCanvasDC, &rcBack, m_pGifImage->m_hBrushTransparent);
		}
		break;
	case GIF_DISPOSAL_RESTORE_PREVIOUS:
		{
			// 还原
			if (m_hMemPrevSaveDC)
				::BitBlt(m_hMemCanvasDC,0,0,m_pGifImage->GetWidth(),m_pGifImage->GetHeight(),m_hMemPrevSaveDC,0,0,SRCCOPY);
		}
		break;
	}
}
//
//	绘制一帧图像
//
//	[parameter]
//		pFrame
//			[in]	要绘制的当前帧
//
void GifImageRender::draw_frame(GIF_Frame* pFrame)
{
	if (NULL == pFrame)
		return;

	if (pFrame->control.disposal_methold == GIF_DISPOSAL_RESTORE_PREVIOUS)
	{
		if (NULL == m_hMemPrevSaveDC)  // 在这里进行m_hMemPrevSaveBitmap的创建
		{
			m_hMemPrevSaveDC =::CreateCompatibleDC(NULL);
			Image image;
			image.Create(m_pGifImage->GetWidth(),m_pGifImage->GetHeight(), 32, Image::createAlphaChannel);
			m_hMemPrevSaveBitmap = image.Detach();

			/*HBITMAP hOldBmp = */(HBITMAP)::SelectObject(m_hMemPrevSaveDC, m_hMemPrevSaveBitmap);
		}
		// 备份
		::BitBlt(m_hMemPrevSaveDC,0,0,m_pGifImage->GetWidth(),m_pGifImage->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);
	}

 	pFrame->image.Draw( m_hMemCanvasDC, 
		pFrame->descriptor.image_left_position,
 		pFrame->descriptor.image_top_position,
		pFrame->descriptor.image_width,
		pFrame->descriptor.image_height,
// 		pFrame->descriptor.image_left_position,
// 		pFrame->descriptor.image_top_position,
		0,0,
		pFrame->descriptor.image_width,
		pFrame->descriptor.image_height);
}
//
// 注：在另一个线程中被调用
//
void GifImageRender::on_timer(Gif_TimerItem* pTimerItem)  
{
	if (m_nDrawStatus != GIF_DRAW_STATUS_START)  
	{
		pTimerItem->nRepeat = 1;  // 暂停或停止状态下，在check_timer中将自动删除
		return;
	}
	EnterCriticalSection(&m_pGifImage->m_sect);
	if (m_nDrawStatus != GIF_DRAW_STATUS_START)
	{
		Gif_Timer_Factory::GetGifTimerEngine()->on_kill_timer((int)this, NULL);
		return;
	}

// 	if (0 == m_nCurFrameIndex)
// 	{
// 		// 遇到一个gif图，共14帧，前面所有帧的disposal都是2，最后一帧是1，导致循环结束时时最后一帧的图像和第一帧的
// 		// 图像一起显示了。这是不是意味着当画完所有帧时，要清空背景？
// 		if (0 == m_nCurFrameIndex)
// 		{
// 			this->handle_disposal(NULL);
// 		}
// 	}
// 	else
// 	{
		GIF_Frame* pPrevFrame = m_pGifImage->GetFrame(m_nCurFrameIndex-1);  // 当0 == m_nCurFrameIndex时将返回NULL，这时调用handle_disposal即可刷新整个背景
// 		if (pPrevFrame)
// 		{
			this->handle_disposal(pPrevFrame);
// 		}
// 	}

	GIF_Frame* pFrame = m_pGifImage->GetFrame(m_nCurFrameIndex);
	draw_frame(pFrame);
	this->commit(m_notify.notify_hwnd.hDC, m_notify.notify_hwnd.x,m_notify.notify_hwnd.y);

	// 为了避免频繁的更新列表，在这里每次仅更新Gif_timer_item里面的数据，而不是删除再添加一个
	int nNextFrameIndex = get_next_frame_index();
	if (nNextFrameIndex < (int) m_pGifImage->m_vFrame.size())
	{
		GIF_Frame* pFrame = m_pGifImage->m_vFrame[nNextFrameIndex];

		if (pFrame)
		{
			pTimerItem->update_repeat(); 
			pTimerItem->nWait = pFrame->nRealDelayTime;
			Gif_Timer_Factory::GetGifTimerEngine()->on_modify_timer();
		}
	}

	LeaveCriticalSection(&m_pGifImage->m_sect);
}

// <<--- 由于gdiplus加载单帧GIF出现透明色的值被改变的问题，现在已改用自己的LZW算法来解码
//
//	解析gif image中的透明信息
//
//	由于gif的透明是通过指定调色板中的一个索引来实现，而不能通过判断和透明色相等来实现
//	因为背景色可能和透明色为相同的颜色
//
//	备注:
//		image.GetWidth和image.GetHeight并不真实。真实的数据宽和高要读取LogicalScreenDescriptor中的width/height
//
bool GifImage::decode_gif_image_transparent(GIF_Frame* pFrame, int nTransparentIndex)
{
	if (NULL == pFrame)
		return false;

	int nDestRowBytes = 4*pFrame->descriptor.image_width;
	int nDestWidth = pFrame->descriptor.image_width;
	int nDestHeight= pFrame->descriptor.image_height;

	BITMAPINFOHEADER bmih;
	::ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biBitCount = 32;
	bmih.biWidth =  nDestWidth;
	bmih.biHeight = nDestHeight * -1;
	bmih.biCompression = BI_RGB;
	bmih.biPlanes = 1;
	BYTE* lpBits = NULL;
	
	HBITMAP hBitmap = ::CreateDIBSection(NULL,(BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&lpBits, NULL, 0 );
	if (NULL == hBitmap || NULL == lpBits)
		return false;

	RGBQUAD palettes[255];  // 取最大可能的大小
	pFrame->image.GetColorTable(0,255, palettes);

	BYTE*       pDestBits = lpBits;// + (pFrame->descriptor.image_height-1)*nDestRowBytes;  // 切换到最后一行，因为DIB是由下到上的（坐标顺序）
	const BYTE* pSrcBits  = ((const BYTE*)pFrame->image.GetBits()) + pFrame->descriptor.image_top_position*pFrame->image.GetPitch();

	for (int row = 0; row < nDestHeight; row ++ )
	{
		for (int i=0,j=0; i<nDestRowBytes; i+=4,j++)
		{
			BYTE index = pSrcBits[j+pFrame->descriptor.image_left_position];  // 取出这个位置的调色板索引值
			if (index >= nTransparentIndex)
			{
				pDestBits[i] = pDestBits[i+1] = pDestBits[i+2] = pDestBits[i+3]	= 0; // 将alpha置为0，透明掉
			}
			else
			{
				RGBQUAD& p = palettes[index];
				pDestBits[i]   = p.rgbBlue;
				pDestBits[i+1] = p.rgbGreen;
				pDestBits[i+2] = p.rgbRed;
				pDestBits[i+3] = 255;
			}
		}
		pDestBits += nDestRowBytes;
		pSrcBits  += pFrame->image.GetPitch();
	}

	pFrame->image.Destroy();
	pFrame->image.Attach(hBitmap);
	pFrame->image.ForceUseAlpha();
	
	return !pFrame->image.IsNull();
}


//////////////////////////////////////////////////////////////////////////
//
//  LZW 解码 GIF 数据
//
//////////////////////////////////////////////////////////////////////////

GifLZWDecoder::GifLZWDecoder(byte nInitBitLength, byte* pDecodeResultData, int nDecodeResultSize)
{
	memset(m_dict, 0, sizeof(DictItem)*4096);
	m_nInitBitLength   = nInitBitLength; 
	m_nCurBitLength    =  m_nInitBitLength+1;

	GIF_LZW_CLEAN_TAG = 1 << m_nInitBitLength;
	GIF_LZW_END_TAG   = GIF_LZW_CLEAN_TAG+1;

	m_pResultData = pDecodeResultData;
	m_nResultDataSize = nDecodeResultSize;
}


//
// 从当前数据流中读取下一个要进行运行的数字。
//
// 备注：
//	1. LZW是基于位的，不是基于字节的
//  2. LZW的位是变长的，不固定，由 lzw code size ~ 12，每当字典中的项上一个数量级时，取的位数加1，加到12后返回lzw code size
//  3. 例如  58         C1         05         D3         ， 当前nReadBitPosInByte=3，即01011,000这里，m_nCurBitLength=11
//          01011000   11000001   00000101   11010011
//
//     则下一个取的数为 58中的前5位加上C1中的后6位，000001 01011 ->43, nReadBitPosInByte=6
//     然后下一个取的数为 C1的前2两加上整个8位的05，还要加上D3的最低位， 1 00000101 11 -> 1047，整个跨度了3个字节才取到了需要的数字
//     因此下面代码中的 (*((int*)pDataCur) 就是先取出四个字节来进行移位/与操作。这里不能使用 byte或者WORD来代替
//
#define GET_NEXT_VALUE() \
	((*((int*)pDataCur))>>nReadBitPosInByte) & ((1<<m_nCurBitLength)-1); \
	nReadBitPosInByte += m_nCurBitLength;              \
	pDataCur += nReadBitPosInByte>>3;                  \
	nReadBitPosInByte %= 8;

// LZW算法解码
bool  GifLZWDecoder::Decode(const byte* pSrcData, int nSrcDataSize)
{
	int nRetSize = 0;

	// 注意：LZW是按位读取的数据，并不是按字节读取的
	const byte*  pDataEnd = pSrcData+nSrcDataSize;
	const byte*  pDataCur = pSrcData;
	byte  nReadBitPosInByte = 0;      // 当前读取pData字节的哪一位了

	WORD  wPrefix = 0, wSuffix = 0;

	wPrefix = GET_NEXT_VALUE();
	do 
	{
		if (wPrefix == GIF_LZW_END_TAG)   // 结束
			break;

		// 在实际的解码中, 第一个数据往往就是CLEAN标志, 这是为了算法而优化的设计,这样我们能直接进入解码循环,而不必在循环外部初始化.
		// : 的确，对于一个7位长的LZW，读取到的第一个数据真的是0x80 = 128
		if (wPrefix == GIF_LZW_CLEAN_TAG) // 重新初始化，这也是数据的第一个字节
		{
			memset(m_dict, 0, sizeof(DictItem)*4096);
			m_nDictLower      = (1<<m_nInitBitLength)+2;
			m_nDictUpper      =  m_nDictLower;
			m_nCurBitLength   =  m_nInitBitLength+1;
			m_nCurBitLengthEx = (1<<m_nCurBitLength)-1;

			wPrefix = GET_NEXT_VALUE();
			continue;
		}

		wSuffix = GET_NEXT_VALUE();

		if (wSuffix>m_nDictUpper)
		{
			UI_LOG_ERROR(_T("%s decode failed. wSuffix=%d, m_nDictUpper=%d"), FUNC_NAME, wSuffix, m_nDictUpper);
			return false;
		}

		// 为什么在这里要先存入一个前缀？这是为了解决如当第一个要push字典项为 dict[82] = {7F,82}时，
		// 82这个字典项根本就不存在，不能用自己来定义自己。 因此先写入 dict[82]={7F, }; suffix先不写入。
		// 这个时候能够获取到dict[82].prefix了。
		m_dict[m_nDictUpper].prefix = wPrefix;

		// 后缀必须是一个单独的字符
		WORD wSingleSuffix = wSuffix;
		while (wSingleSuffix > GIF_LZW_CLEAN_TAG)
			wSingleSuffix = m_dict[wSingleSuffix].prefix;
		
		Output(wPrefix);

		if (m_nDictUpper == wSuffix ||          // <--这是什么意思？当出现dict[82]={7F,82}这种自己定义自己时，必须直接将这项放入字典，而不是再检查一下CheckExist{7F,7F}是否存在。如果不存在则侥幸过关，如果存在，则82这一项则被丢掉了，导致下一次循环时dict[82].prefix=82, output(82)将死循环。
			false==CheckExist(wPrefix,wSingleSuffix))
		{
			PushDict(wPrefix, wSingleSuffix);   // 存入字典
		}
		wPrefix = wSuffix;

	} while (pDataEnd>pDataCur);

	UIASSERT(m_nResultDataSize==0);   // 保证将压缩的LZW数据解压缩为图片大小
	return m_nResultDataSize == 0;
}

// 检查prefix suffix是否在字典中存在
inline bool GifLZWDecoder::CheckExist(WORD wValue1, WORD wValue2)
{
	for (int i = m_nDictLower; i < m_nDictUpper; i++)
	{
		if (m_dict[i].prefix == wValue1 && m_dict[i].suffix == wValue2)
		{
			return true;
		}
	}
	return false;
}

// 输出一个有效结果。如果w值仍然是一个字典项，继续搜索字典。
// TODO: 将递归调用优化成循环调用
inline void GifLZWDecoder::Output(WORD w)
{
	if(w > GIF_LZW_CLEAN_TAG)
	{
		Output(m_dict[w].prefix);
		Output(m_dict[w].suffix);
	}
	else
	{
		*m_pResultData = (byte)w;
		m_pResultData++;
		m_nResultDataSize--;

#ifdef _DEBUG
// 		 	TCHAR szInfo[16];
// 		 	_stprintf(szInfo, _T("%d(%02X) "),w,w);
// 		 	::OutputDebugString(szInfo);
#endif
	}
}

// 向字典中添加一项
inline void GifLZWDecoder::PushDict(WORD wPrefix, WORD wSuffix)
{
	m_dict[m_nDictUpper].suffix = wSuffix;

	// GIF规范规定的是12位，超过12位的表达范围就推倒重来，并且GIF为了提高压缩率，采用的是变长的字长。
	// 比如说原始数据是8位，那么一开始，先加上一位再说，开始的字长就成了9位，然后开始加标号，当标号加到512时，
	// 也就是超过9为所能表达的最大数据时，也就意味着后面的标号要用10位字长才能表示了，那么从这里开始，
	// 后面的字长就是10位了。依此类推，到了2^12也就是4096时，在这里插一个清除标志，从后面开始，从9位再来。

	if (m_nDictUpper >= m_nCurBitLengthEx)
	{
		m_nCurBitLength++;
		if (m_nCurBitLength>12)
			m_nCurBitLength = 12;
		else
			m_nCurBitLengthEx = (1<<m_nCurBitLength)-1;
	}

	m_nDictUpper++;
}

