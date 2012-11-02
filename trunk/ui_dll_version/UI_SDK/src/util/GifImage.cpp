#include "stdafx.h"
#include "util/GifImage.h"

Gif_Timer::Gif_Timer()
{
	m_nBestWaitTime = INFINITE;
	m_bQuitThread = false;
	m_hEvent_set_timer = ::CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hEvent_post_message = ::CreateEvent(NULL,TRUE,FALSE, NULL);
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
	if( NULL != m_hMsgQueueEvent )
	{
		::WaitForSingleObject(m_hMsgQueueEvent,INFINITE);   // 等待线程创建完消息队列了才能发送消息
		::CloseHandle(m_hMsgQueueEvent);
		m_hMsgQueueEvent = NULL;
	}

	BOOL bRet = ::PostThreadMessage( m_dwThreadID, u, w, l );
	if( FALSE == bRet )
	{
		int a = 0;
		return;
	}

	::SetEvent(m_hEvent_post_message);
}

//
//	由其它线程（主线程）调用，转发给gif动画线程来处理 --> 
//
void Gif_Timer::kill_timer(int id)
{
	post_message(GT_KILL_TIMER, (WPARAM)id, NULL );
}

// 位于gif动画线程中
void Gif_Timer::on_kill_timer(int id)
{
	list<Gif_TimerItem*>::iterator iter = m_vItem.begin();

	for ( ; iter !=  m_vItem.end(); )
	{
		Gif_TimerItem* pItem = (*iter);
		if( NULL == pItem )
		{
			iter ++;
			continue;
		}

		if( pItem->nID == id)
		{
			GifImage* pGifImage = (GifImage*)pItem->pData;

			delete pItem;
			pItem = NULL;
			iter = m_vItem.erase(iter);
			
			if( NULL != pGifImage )
				pGifImage->on_remove_from_timer_list();
		}
		else
			iter ++ ;
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
	if( NULL == pItem || NULL == pItem->pData )
		return;

//	GIFTRACE1_int(_T("on_set_timer: id=%d\n"), pItem->nID );
	
	// 过滤相同ID
	list<Gif_TimerItem*>::iterator iter = m_vItem.begin();
	list<Gif_TimerItem*>::iterator iterEnd = m_vItem.end();
	for ( ; iter != iterEnd; iter++ )
	{
		if( (*iter)->nID == pItem->nID)
			return;
	}

	((GifImage*)(pItem->pData))->on_add_to_timer_list();
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
void Gif_Timer::check_timer()
{
	
	//GIFTRACE0(_T("Gif_Timer::check_timer\n"));

	list<Gif_TimerItem*>::iterator iter = m_vItem.begin();
	for ( ; iter != m_vItem.end();  )
	{
		Gif_TimerItem* pItem = *iter;
		if( NULL == pItem )
		{
			iter++;
			continue;;
		}

		if( pItem->get_remain() <= 0 )
		{
			this->on_timer(pItem);

			if( pItem->nRepeat != -1 && 0 == --pItem->nRepeat )
			{
				// 用完了循环次数，删除
				GifImage* pGifImage = (GifImage*)pItem->pData;
				delete pItem;
				pItem = NULL;

				iter = m_vItem.erase(iter);

				if( NULL != pGifImage )
					pGifImage->on_remove_from_timer_list();

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

	GifImage* pImage = (GifImage*)pItem->pData;
	if (NULL==pImage)
		return;

	pImage->on_timer(pItem);
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
		::ResetEvent(pThis->m_hEvent_set_timer);        // 阻塞线程满负荷运行
		::ResetEvent(pThis->m_hEvent_post_message);     // 同上

		if( dwRet == WAIT_OBJECT_0 )
		{
			BOOL bRet = FALSE;
			MSG msg;
			while( (bRet = PeekMessage( &msg, NULL, 0, 0, PM_REMOVE)) != 0)
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
						pThis->on_kill_timer((int)msg.wParam);
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

GifImage::GifImage()
{
	m_nCurFrameIndex = 0;

	m_hWnd = NULL;
	m_hDC = NULL;
	m_hMemCanvasDC = NULL;
	m_hMemCanvasBitmap = NULL;
	m_hBrushTransparent = NULL;
	m_hMemPrevSaveDC = NULL;
	m_hMemPrevSaveBitmap = NULL;
	m_nDrawStatus = GIF_DRAW_STATUS_STOP;

	InitializeCriticalSection(&m_sect);
	m_hEvent_not_in_timer_list = ::CreateEvent(NULL,TRUE,TRUE,NULL);
}

GifImage::~GifImage()
{
	this->Destroy();

	WaitForSingleObject(m_hEvent_not_in_timer_list,INFINITE);  // 确保gif线程没有在访问m_vFrame数据。同时将状态改为了STOP，gif线程下次也将不会再访问这个数据了
	DeleteCriticalSection(&m_sect);

	::CloseHandle(m_hEvent_not_in_timer_list);
	m_hEvent_not_in_timer_list = NULL;
}
bool GifImage::Destroy()
{
	if( m_nDrawStatus != GIF_DRAW_STATUS_STOP )
		this->Stop();

	if( Gif_Timer_Factory::GetGifTimerEngine(false) )
		Gif_Timer_Factory::GetGifTimerEngine(false)->kill_timer((int)this);
	
	EnterCriticalSection(&m_sect);
	int nSize = (int) m_vFrame.size();
	for (int i = 0; i < nSize; i++ )
	{
		m_vFrame[i]->image.Destroy();
		delete m_vFrame[i];
		m_vFrame[i] = NULL;
	}
	m_vFrame.clear();
	LeaveCriticalSection(&m_sect);

	this->release_resource();

	return true;
}
void GifImage::release_resource()
{
	if (NULL != m_hMemCanvasBitmap)
	{
		DeleteObject(m_hMemCanvasBitmap);
		m_hMemCanvasBitmap = NULL;
	}
	if (NULL != m_hMemCanvasDC)
	{
		::ReleaseDC(m_hWnd,m_hMemCanvasDC);
		m_hMemCanvasDC = NULL;
	}
	if (NULL != m_hMemPrevSaveDC)
	{
		::ReleaseDC(m_hWnd,m_hMemPrevSaveDC);
		m_hMemPrevSaveDC = NULL;
	}
	if (NULL != m_hMemPrevSaveBitmap)
	{
		::DeleteObject(m_hMemPrevSaveBitmap);
		m_hMemPrevSaveBitmap = NULL;
	}
	if (NULL != m_hDC)
	{
		::ReleaseDC(m_hWnd, m_hDC);
		m_hDC = NULL;
	}
	if (NULL != m_hBrushTransparent)
	{
		::DeleteObject(m_hBrushTransparent);
		m_hBrushTransparent = NULL;
	}
}

//
// 跳过gif文件中的数据部分
//
int GifImage::skip_data_block(fstream& f, byte* pBits)
{
	int nDataLength = 0;              // 统计数据的总大小
	unsigned char cNextBlockLen = 0;  // 读取下一段的数据大小
	do 
	{
		f.read((char*)&cNextBlockLen, 1 );  // 本段数据长度 最大为FF，如果为0表示数据段结束
		if( 0 == cNextBlockLen )
			break;

		if (NULL != pBits)
		{
			f.read(((char*)pBits+nDataLength), cNextBlockLen);
		}
		else
		{
			f.seekg(cNextBlockLen, ios_base::cur);
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
//	使用路径进行文件GIF加载
//
bool GifImage::Load(const TCHAR* szPath)
{
	UIASSERT(0 == m_vFrame.size());

	fstream f;
	f.open(szPath, ios_base::in|ios_base::binary);
	if( f.fail() )
		return false;

	bool bRet = false;
	do 
	{
		// 文件标志
		GIF_FileMark header;
		f.read((char*)&header, sizeof(GIF_FileMark));
		if (f.fail())        break;
		if (!header.Valid()) break;

		// 文件全局信息
		GIF_LogicalScreenDescriptor logicScreenDesc;
		f.read((char*)&logicScreenDesc, sizeof(GIF_LogicalScreenDescriptor));
		if (f.fail())        break;

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
			f.read((char*)pGlobalColorTable, nGlobalColorTableSize);

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
			f.read((char*)&bNextBlockFlag, 1);

			if (bNextBlockFlag == GIF_BLOCK_FLAG_TRAILER)
			{
				bRet = true;
				break;   // 文件结束
			}

			if (NULL == pFrame)
			{
				pFrame = new GIF_Frame;
				nFrameStartPos = f.tellg();
				nFrameStartPos -= 1;
			}

			switch (bNextBlockFlag)
			{
			case GIF_BLOCK_FLAG_EXTENSION:
				{
					BYTE bNextExtFlag = 0;
					f.read ((char*)&bNextExtFlag, 1);

					switch (bNextExtFlag)
					{
					case GIF_EXT_FLAG_PLAIN:
						{
							// 跳过 
							f.seekg(13,ios_base::cur);
							skip_data_block(f);
						}
						break;
					case GIF_EXT_FLAG_CONTROL:
						{
							// 跳过
							f.read((char*)&pFrame->control, sizeof(GIF_GraphicControlExtension));
							pFrame->nRealDelayTime = pFrame->control.delay_time*10;
							if ( 0 == pFrame->nRealDelayTime )    // 有些gif该值为0
								pFrame->nRealDelayTime = 100;

							assert( pFrame->control.disposal_methold != 3 );// 这种GIF没有经没测试，没找到这样素材
						}
						break;
					case GIF_EXT_FLAG_COMMENT:
						{
							// 跳过
							skip_data_block(f);
						}
						break;
					case GIF_EXT_FLAG_APP:
						{
							// 跳过 
							f.seekg(12,ios_base::cur);
							skip_data_block(f);
						}
						break;
					};
				}
				break;
			case GIF_BLOCK_FLAG_IMAGE:
				{
					f.read((char*)&pFrame->descriptor, sizeof(GIF_ImageDescriptor));

					//////////////////////////////////////////////////////////////////////////
					// 本地色彩表

					byte* pLocalColorTable = NULL;
					int   nLocalColorTableSize = 0;

					if( pFrame->descriptor.local_color_table_flag )
						nLocalColorTableSize = static_cast<int>(3*pow((double)2,pFrame->descriptor.size_of_local_color_table+1));

					if( 0 != nLocalColorTableSize )
					{
						pLocalColorTable = new byte[nLocalColorTableSize];
						f.read((char*)pLocalColorTable, nLocalColorTableSize);
					}

					//////////////////////////////////////////////////////////////////////////
					// 解码

					byte* pColorTableForThisFrame     = pLocalColorTable==NULL? pGlobalColorTable:pLocalColorTable;
					int   nColorTableSizeForThisFrame = pLocalColorTable==NULL? nGlobalColorTableSize:nLocalColorTableSize;

					BYTE byte_LZW_code_size = 0;
					f.read((char*)&byte_LZW_code_size,1);   // 这个值是干什么的？  <-- 是LZW算法的初始位长度LZW code size 

					// 先尝试使用自己的解码算法
					bool bDecodeRet = this->decode_by_lzw(f, pFrame, byte_LZW_code_size, pColorTableForThisFrame, nColorTableSizeForThisFrame);
					if (false == bDecodeRet)
					{
						// 失败了再尝试用gdiplus的方法解码
						bDecodeRet = this->decode_by_gdiplus(f, pFrame, nFrameStartPos, header, logicScreenDesc, pColorTableForThisFrame, nColorTableSizeForThisFrame);
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
		if (NULL != pFrame)  // 剩余部分没有image data了，导致还new了一次，在这里释放掉
		{
			delete pFrame;
			pFrame = NULL;
		}

		delete[] pGlobalColorTable;
		pGlobalColorTable = NULL;
	} 
	while (0);

	f.close();
	return bRet;
}

bool GifImage::decode_by_lzw(fstream& f, GIF_Frame* pFrame, int byte_LZW_code_size, byte* pColorTable, int nColorTableSize)
{
	byte*   pGifFrameImageData = NULL;
	byte*   pOutputData = NULL;
	int     nOutputDataSize = pFrame->descriptor.image_width*pFrame->descriptor.image_height; 

	//////////////////////////////////////////////////////////////////////////
	// 读取图像数据用于解码（先读取数据大小）

	int nDataBeginPos = f.tellg();
	int nDataLength = skip_data_block(f);
	f.seekg(nDataBeginPos);

	pGifFrameImageData = new byte[nDataLength];
	skip_data_block(f, pGifFrameImageData);

	//////////////////////////////////////////////////////////////////////////
	// LZW解码

	pOutputData = new byte[nOutputDataSize];
	memset(pOutputData, 0, nOutputDataSize);

	GifLZWDecoder decoder(byte_LZW_code_size, pOutputData, nOutputDataSize);
	decoder.Decode(pGifFrameImageData, nDataLength);

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

#ifdef _DEBUG_x
	pFrame->image.Save(_T("c:\\aaaiidi.png"),Gdiplus::ImageFormatPNG);
#endif
	SAFE_ARRAY_DELETE(pGifFrameImageData);
	SAFE_ARRAY_DELETE(pOutputData);
	return true;
}

bool GifImage::decode_by_gdiplus(fstream& f, 
								 GIF_Frame* pFrame, 
								 int  nFrameStartPos,
								 GIF_FileMark& header, 
								 GIF_LogicalScreenDescriptor& logicScreenDesc,
								 byte* pColorTable, 
								 int nColorTableSize )
{
	skip_data_block(f);
	int nFrameEndPos = f.tellg();

	// 单帧图像数据
	int   nImageDataSize = nFrameEndPos-nFrameStartPos;
	void* pImageData = (void*)new char[nImageDataSize];

	f.seekg(nFrameStartPos, ios_base::beg);
	f.read((char*)pImageData, nImageDataSize);

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

int GifImage::get_next_frame_index()
{
	int nIndex = m_nCurFrameIndex;
	m_nCurFrameIndex++;
	if( m_nCurFrameIndex >= (int)m_vFrame.size() )
		m_nCurFrameIndex = 0;

	return nIndex;
}
GIF_Frame* GifImage::GetFrame(int nIndex)
{
	if( nIndex < 0 || nIndex >= (int)m_vFrame.size() )
		return NULL;

	return m_vFrame[nIndex];
}

//
//	设置绘制参数
//
//	Parameter
//		hWnd
//			[in]	在显示在哪个窗口上
//
//		x,y
//			[in]	绘制位置
//
//		colTransparent
//			[in]	透明背景颜色
//
//	Return
//		成功返回TRUE，失败返回FALSE
//
BOOL GifImage::SetDrawParam(HWND hWnd, int x, int y, COLORREF colorTransparent)
{
	if (m_vFrame.size() <= 0)
		return FALSE;

	this->release_resource();

	m_hWnd = hWnd;
	m_nDrawX = x;
	m_nDrawY = y;

	m_hDC = ::GetDC(m_hWnd);
	m_hMemCanvasDC = ::CreateCompatibleDC(m_hDC);
	m_hMemCanvasBitmap = ::CreateCompatibleBitmap(m_hDC,this->GetWidth(),this->GetHeight());
	/*HBITMAP hOldBmp = */(HBITMAP)::SelectObject(m_hMemCanvasDC, m_hMemCanvasBitmap);
	m_hMemPrevSaveDC =::CreateCompatibleDC(m_hDC);
	m_hMemPrevSaveBitmap = ::CreateCompatibleBitmap(m_hDC,this->GetWidth(),this->GetHeight());
	/*HBITMAP hOldBmp = */(HBITMAP)::SelectObject(m_hMemPrevSaveDC, m_hMemPrevSaveBitmap);

	m_hBrushTransparent = ::CreateSolidBrush(colorTransparent);

	RECT rcBack = {0,0, this->GetWidth(),this->GetHeight()};
	::FillRect( m_hMemCanvasDC,&rcBack,m_hBrushTransparent);
	::FillRect( m_hMemPrevSaveDC,&rcBack,m_hBrushTransparent);

	// 将DIB转换成DDB，因为TransparentBlt不支持DIB
// 	int nCount = m_vFrame.size();
// 	for ( int i = 0; i < nCount; i++ )
// 	{
// 		LONG nIndex = m_vFrame[i]->image.GetTransparentColor();
// 		if( -1 == nIndex )
// 			continue;
// 
// 		COLORREF colorSave = 0;
// 		if( nIndex != -1 )
// 			colorSave = m_vFrame[i]->image.GetTransparentRGB();
// 
// 		m_vFrame[i]->image.ChangeDIB2DDB(m_hMemCanvasDC);
// 		if( nIndex != -1 )
// 			m_vFrame[i]->image.SetTransparentColor(colorSave);
// 	}

	return TRUE;
}

//
//	更新显示位置
//
void GifImage::SetDrawPos( int x, int y )
{
	m_nDrawX = x;
	m_nDrawY = y;
}

//
//	TODO: 如何实现立即显示第一帧数据
//
void GifImage::Start()
{
	int nSize = (int)m_vFrame.size();
	if (nSize <= 0)
		return ;

	m_nDrawStatus = GIF_DRAW_STATUS_START;
	if (nSize == 1)    // 单帧gif图片不需要使用计时器
	{
		EnterCriticalSection(&m_sect);
		handle_disposal(NULL);                           // 刷背景
		draw_frame( this->GetFrame(m_nCurFrameIndex));   // 绘制第一帧到m_hMemCanvasDC中
		::BitBlt(m_hDC,this->GetDrawX(),this->GetDrawY(), this->GetWidth(), this->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);
		LeaveCriticalSection(&m_sect);
	}
	else
	{
		Gif_TimerItem timer_item = { (int)this, ::GetTickCount(), m_vFrame[m_nCurFrameIndex]->nRealDelayTime, -1, this };  // -1无限循环
		Gif_Timer_Factory::CreateGifTimerEngine()->set_timer(timer_item);
	}
}

void GifImage::Pause()
{
	if (m_vFrame.size() <= 0)
		return ;

	m_nDrawStatus = GIF_DRAW_STATUS_PAUSE;
}
void GifImage::Stop()
{
	if (m_vFrame.size() <= 0)
		return ;

	// 刷新父窗口，这里需要注意进行同步，线程中的最后一次on_timer可能将下面的刷背景又覆盖了
	EnterCriticalSection(&m_sect);
	m_nCurFrameIndex = 0;
	m_nDrawStatus = GIF_DRAW_STATUS_STOP;

	handle_disposal(NULL);  // 刷背景
	::BitBlt(m_hDC,this->GetDrawX(),this->GetDrawY(), this->GetWidth(), this->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);

	LeaveCriticalSection(&m_sect);
}

//
//	外部窗口调用
//
void GifImage::OnPaint(HDC hDC)
{
	if (GIF_DRAW_STATUS_STOP == m_nDrawStatus)
		return;

	EnterCriticalSection(&m_sect);
	::BitBlt(hDC, 0,0/*this->GetDrawX(),this->GetDrawY()*/, this->GetWidth(), this->GetHeight(), m_hMemCanvasDC,0,0,SRCCOPY);
	LeaveCriticalSection(&m_sect);
}

//
//	UI控件调用，x,y为控件内部坐标，如(0,0)
//
void GifImage::OnPaint(HDC hDC, int x, int y)
{
	if (GIF_DRAW_STATUS_STOP == m_nDrawStatus)
		return;

	EnterCriticalSection(&m_sect);
	::BitBlt(hDC, x, y, this->GetWidth(), this->GetHeight(), m_hMemCanvasDC,0,0,SRCCOPY);
	LeaveCriticalSection(&m_sect);
}

//
//	当一帧绘制完成后，并到达delay时间，将要绘制下一帧时，处理它的disposal
//
void GifImage::handle_disposal(GIF_Frame* pFrame)
{
	// 当pFrame为空时，默认就刷新整个图片的背景
	int  nDisposal = GIF_DISPOSAL_RESTORE_BACKGROUND;
	RECT rcBack = {0,0, this->GetWidth(),this->GetHeight()};

	if (NULL != pFrame)
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
			::FillRect( m_hMemCanvasDC,&rcBack,m_hBrushTransparent);
		}
		break;
	case GIF_DISPOSAL_RESTORE_PREVIOUS:
		{
			// 还原
			::BitBlt(m_hMemCanvasDC,0,0,this->GetWidth(),this->GetHeight(),m_hMemPrevSaveDC,0,0,SRCCOPY);
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
void GifImage::draw_frame(GIF_Frame* pFrame)
{
	if (NULL == pFrame)
		return;

	if( pFrame->control.disposal_methold == GIF_DISPOSAL_RESTORE_PREVIOUS )
	{
		// 备份
		::BitBlt(m_hMemPrevSaveDC,0,0,this->GetWidth(),this->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);
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
// 在另一个线程中被调用
//
void GifImage::on_timer(Gif_TimerItem* pTimerItem)  
{
	if (m_nDrawStatus != GIF_DRAW_STATUS_START)  
	{
		pTimerItem->nRepeat = 1;  // 暂停或停止状态下，在check_timer中将自动删除
		return;
	}
	EnterCriticalSection(&m_sect);
	if (m_nDrawStatus != GIF_DRAW_STATUS_START)
	{
		Gif_Timer_Factory::GetGifTimerEngine()->on_kill_timer((int)this);
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
		GIF_Frame* pPrevFrame = this->GetFrame(m_nCurFrameIndex-1);  // 当0 == m_nCurFrameIndex时将返回NULL，这时调用handle_disposal即可刷新整个背景
// 		if (NULL != pPrevFrame)
// 		{
			this->handle_disposal(pPrevFrame);
// 		}
// 	}

	GIF_Frame* pFrame = this->GetFrame(m_nCurFrameIndex);
	draw_frame(pFrame);
	::BitBlt(m_hDC,this->GetDrawX(),this->GetDrawY(), this->GetWidth(), this->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);

	// 为了避免频繁的更新列表，在这里每次仅更新Gif_timer_item里面的数据，而不是删除再添加一个
	int nNextFrameIndex = get_next_frame_index();
	if (nNextFrameIndex < (int) m_vFrame.size())
	{
		GIF_Frame* pFrame = m_vFrame[nNextFrameIndex];

		if (NULL != pFrame)
		{
			pTimerItem->update_repeat(); 
			pTimerItem->nWait = pFrame->nRealDelayTime;
			Gif_Timer_Factory::GetGifTimerEngine()->on_modify_timer();
		}
	}

	LeaveCriticalSection(&m_sect);
}
//
// 在另一个线程中被调用
//
void GifImage::on_add_to_timer_list()
{
	::ResetEvent(m_hEvent_not_in_timer_list);
}
//
// 在另一个线程中被调用
//
void GifImage::on_remove_from_timer_list()
{
	::SetEvent(m_hEvent_not_in_timer_list);
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

#ifdef _DEBUG_x // <-- 将每一帧保存为一个文件
	static int n = 0;
	TCHAR szPath[MAX_PATH] = _T("");
	_stprintf(szPath, _T("C:\\one_frame2\\%d.png"),n );
	pFrame->image.Save(szPath, Gdiplus::ImageFormatPNG);
#endif
	//nTransparentIndex++;   // ?? 为什么要+1? 不加1的话结果就对不上，什么时候要加1 ？什么时候不需要加1 ？

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
void  GifLZWDecoder::Decode(const byte* pSrcData, int nSrcDataSize)
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

