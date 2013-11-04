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
		::WaitForSingleObject(m_hMsgQueueEvent,INFINITE);   // �ȴ��̴߳�������Ϣ�����˲��ܷ�����Ϣ
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
//	�������̣߳����̣߳����ã�ת����gif�����߳������� --> 
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

// λ��gif�����߳���
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
	
	// ������ͬID
	list<Gif_TimerItem*>::iterator iter = m_vItem.begin();
	list<Gif_TimerItem*>::iterator iterEnd = m_vItem.end();
	for (; iter != iterEnd; iter++)
	{
		if ((*iter)->nID == pItem->nID)
			return;
	}

	m_vItem.push_back(pItem);
	update_wait_time();

	::SetEvent(m_hEvent_set_timer);   // ������ʼִ���߳�
}

//
//	�����ط�������һ�� gif_timer_item����Ҫ����eventȥ���¼��
//
void Gif_Timer::on_modify_timer()
{
//	GIFTRACE0(_T("on_modify_timer\n"));

//	update_wait_time();  // ��on_timer����󣬻���update_wait_timeһ�Σ���������ȥ��
	::SetEvent(m_hEvent_set_timer);   // ������ʼִ���߳�
}

// ������һ�δ���on_timer�ĵȴ�ʱ��
void Gif_Timer::update_wait_time()
{
	int oldWaitTime = m_nBestWaitTime;

	// ȡ��Сʱ��Ϊ�ȴ�ʱ��
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

// ����б��е�timer item�����������Ӧʱ��㣬�򴥷�on_timer
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
				// ������ѭ��������ɾ��
				delete pItem;
				pItem = NULL;

				iter = m_vItem.erase(iter);
				continue;	
			}
			else
			{
				// ���¼�����һ�ε���Ӧʱ��
				// pItem->update_repeat();    // gifimage��on_timer��Ӧ��ֻ�޸���waitֵ���������startֵ�����£�
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

	// ��һʱ���ڣ�������Ϣ���У������ⲿ������Ϣʧ��
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	::SetEvent(pThis->m_hMsgQueueEvent);

	while (false == pThis->m_bQuitThread)
	{
		HANDLE arrHandles[2] = { pThis->m_hEvent_post_message, pThis->m_hEvent_set_timer };
		DWORD dwRet = ::WaitForMultipleObjects(2, arrHandles, FALSE, pThis->m_nBestWaitTime );   // ���ָ�eventһֱû����Ϣ��ȥ��������߳�

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

Gif_Timer_Factory g_gif_timer_factory; // ����һ��ȫ�ֶ����ڳ����˳�ʱ��֤����s_giftimer��Ա

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

	m_pGifImage->DeleteRender(this);  // ע���õ��ûᴥ��delete this.��˺��治׼���������������
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
// ����gif�ļ��е����ݲ���
//
int GifImage::skip_data_block(IStreamBufferReader*  pBuffer, byte* pBits)
{
	int nDataLength = 0;              // ͳ�����ݵ��ܴ�С
	unsigned char cNextBlockLen = 0;  // ��ȡ��һ�ε����ݴ�С
	do 
	{
		pBuffer->read((char*)&cNextBlockLen, 1);  // �������ݳ��� ���ΪFF�����Ϊ0��ʾ���ݶν���
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
//	�齨һ֡gif�����ļ�
//
void GifImage::build_one_frame_data(
			GIF_FileMark* pFileMark,                  // 
			GIF_LogicalScreenDescriptor* pDesc,       // 
			void* pColorTable, int nColorTableSize,   // ɫ�ʱ����ݣ�������local color tableʱ��ʹ��local color table����
			void* pImageData,  int nImageDataSize,    // ��֡����
			void** ppOut, int* pOutSize               // ����ֵ,ppOut��Ҫdelete[]�����ͷ�
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
//	ʹ��·�������ļ�GIF���أ�ֻ���ⲿ����һ��renderʱ������������load�����ļ���
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
		// �ļ���־
		GIF_FileMark header;
		nRead = pBuffer->read((char*)&header, sizeof(GIF_FileMark));
		if (nRead < sizeof(GIF_FileMark) || !header.Valid()) 
            break;

		// �ļ�ȫ����Ϣ
		GIF_LogicalScreenDescriptor logicScreenDesc;
		nRead = pBuffer->read((char*)&logicScreenDesc, sizeof(GIF_LogicalScreenDescriptor));
        if (nRead < sizeof(GIF_LogicalScreenDescriptor))
            break;

		this->m_nImageWidth = logicScreenDesc.logical_screen_width;
		this->m_nImageHeight = logicScreenDesc.logical_screen_height;

		// ȫ����ɫ��
		byte* pGlobalColorTable = NULL;
		int   nGlobalColorTableSize = 0;

		if (logicScreenDesc.global_color_table_flag)
			nGlobalColorTableSize = static_cast<int>(3*pow((double)2,(int)(logicScreenDesc.size_of_global_color_table+1)));

		if (0 != nGlobalColorTableSize)
		{
			pGlobalColorTable = new byte[nGlobalColorTableSize];
			nRead = pBuffer->read((char*)pGlobalColorTable, nGlobalColorTableSize);

			// ��ȡ������ˢ
// 			int R = pGlobalColorTable[logicScreenDesc.background_color_index*3];
// 			int G = pGlobalColorTable[logicScreenDesc.background_color_index*3+1];
// 			int B = pGlobalColorTable[logicScreenDesc.background_color_index*3+2];
//			m_hBrushTransparent = CreateSolidBrush(RGB(R,G,B));
		}

		// ѭ����ȡÿһ֡�����ݣ������һ������֡��gif�ļ�����image���ء�
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
				break;   // �ļ�����
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
							// ���� 
							pBuffer->seek(13,ios_base::cur);
							skip_data_block(pBuffer);
						}
						break;
					case GIF_EXT_FLAG_CONTROL:
						{
							// ����
							pBuffer->read((char*)&pFrame->control, sizeof(GIF_GraphicControlExtension));
							pFrame->nRealDelayTime = pFrame->control.delay_time*10;
							if ( 0 == pFrame->nRealDelayTime )    // ��Щgif��ֵΪ0
								pFrame->nRealDelayTime = 100;

							assert( pFrame->control.disposal_methold != 3 );// ����GIFû�о�û���ԣ�û�ҵ������ز�
						}
						break;
					case GIF_EXT_FLAG_COMMENT:
						{
							// ����
							skip_data_block(pBuffer);
						}
						break;
					case GIF_EXT_FLAG_APP:
						{
							// ���� 
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
					// ����ɫ�ʱ�

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
					// ����

					byte* pColorTableForThisFrame     = pLocalColorTable==NULL? pGlobalColorTable:pLocalColorTable;
					int   nColorTableSizeForThisFrame = pLocalColorTable==NULL? nGlobalColorTableSize:nLocalColorTableSize;

					BYTE byte_LZW_code_size = 0;
					pBuffer->read((char*)&byte_LZW_code_size,1);   // ���ֵ�Ǹ�ʲô�ģ�  <-- ��LZW�㷨�ĳ�ʼλ����LZW code size 

					// �ȳ���ʹ���Լ��Ľ����㷨
					bool bDecodeRet = this->decode_by_lzw(pBuffer, pFrame, byte_LZW_code_size, pColorTableForThisFrame, nColorTableSizeForThisFrame);
					if (false == bDecodeRet)
					{
						// ʧ�����ٳ�����gdiplus�ķ�������
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

					// ����֡
					m_vFrame.push_back(pFrame);
					pFrame = NULL;
				}
				break;

			};  // switch(bNextBlockFlag)

		} while (1);  // do

PARSE_ERROR:
		if (pFrame)  // ʣ�ಿ��û��image data�ˣ����»�new��һ�Σ��������ͷŵ�
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
	// ��ȡͼ���������ڽ��루�ȶ�ȡ���ݴ�С��

	int nDataBeginPos = pBuffer->tell();
	int nDataLength = skip_data_block(pBuffer);
    pBuffer->seek(nDataBeginPos, ios_base::beg);

	pGifFrameImageData = new byte[nDataLength];
	skip_data_block(pBuffer, pGifFrameImageData);

	//////////////////////////////////////////////////////////////////////////
	// LZW����

	pOutputData = new byte[nOutputDataSize];
	memset(pOutputData, 0, nOutputDataSize);

	GifLZWDecoder decoder(byte_LZW_code_size, pOutputData, nOutputDataSize);
	if (false == decoder.Decode(pGifFrameImageData, nDataLength))
	{
		SAFE_ARRAY_DELETE(pGifFrameImageData);
		SAFE_ARRAY_DELETE(pOutputData);

        pBuffer->seek(nDataBeginPos, ios_base::beg);   // ������gdiplus�����λ��
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// ��������GIF���͵�����
	
	if (pFrame->descriptor.interlace_flag)
	{
		byte*   pInterlaceData = new byte[nOutputDataSize];
		int     nWriteCursor = 0;
		int     nReadCursor = 0;
		int     nRowDataSize = pFrame->descriptor.image_width;

		// Group 1��ÿ��8��ȡһ�Σ��ӵ�0�п�ʼд��
		for (int row = 0; row < pFrame->descriptor.image_height; row+=8)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}

		// Group 2��ÿ��8��ȡһ�Σ��ӵ�4�п�ʼд��
		for (int row = 4; row < pFrame->descriptor.image_height; row+=8)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}

		// Group 3��ÿ��4��ȡһ�Σ��ӵ�2�п�ʼд��
		for (int row = 2; row < pFrame->descriptor.image_height; row+=4)
		{
			nWriteCursor = row*nRowDataSize;
			memcpy(pInterlaceData+nWriteCursor, pOutputData+nReadCursor, nRowDataSize);
			nReadCursor += nRowDataSize;
		}

		// Group 4��ÿ��2��ȡһ�Σ��ӵ�1�п�ʼд��
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
	// ������ɫ���͸������

	pFrame->image.Create(pFrame->descriptor.image_width,pFrame->descriptor.image_height, 32, Image::createAlphaChannel);

	BYTE*   pBits = (BYTE*)pFrame->image.GetBits();
	int     bytesperline = 4*pFrame->descriptor.image_width;  
	int     npxIndex = 0;

	for (int row = 0; row < pFrame->descriptor.image_height; row ++ )
	{
		for( int i = 0; i < bytesperline; i += 4 )
		{
			int nColorTableIndex = pOutputData[npxIndex++];    // �����ض�Ӧ�ĵ�ɫ���ֵ
			int nColorTableIndexAddr = 3*nColorTableIndex;

			if (pFrame->control.transparent_color_flag && 
				pFrame->control.transparent_color_index == nColorTableIndex)
			{

				pBits[i]   = 0;          // ע�����������0���൱��alphaͨ����Ԥ�ˣ�����image.draw����ɰ׵ף�������͸��
				pBits[i+1] = 0;
				pBits[i+2] = 0;

				pBits[i+3] = 0;          // ������͸��
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

#ifdef _DEBUG // <-- ��ÿһ֡����Ϊһ���ļ�
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

	// ��֡ͼ������
	int   nImageDataSize = nFrameEndPos-nFrameStartPos;
	void* pImageData = (void*)new char[nImageDataSize];

	pBuffer->seek(nFrameStartPos, ios_base::beg);
	pBuffer->read((char*)pImageData, nImageDataSize);

	// ��װ
	void* one_frame_gif_file_data = NULL;
	int   one_frame_gif_file_data_size = 0;

	// TODO: ����б�Ҫ��Gdiplus�ڲ�Ӧ�û��ж�ѡ�ĸ���ɫ���
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

#ifdef _DEBUG_x // <-- ��ÿһ֡����Ϊһ���ļ�
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

	// ����ͼƬ
	if (false == pFrame->image.LoadFromData(one_frame_gif_file_data, one_frame_gif_file_data_size))
	{
		delete [] one_frame_gif_file_data;
		one_frame_gif_file_data = NULL;

		return false;
	}
	delete [] one_frame_gif_file_data;
	one_frame_gif_file_data = NULL;


	// ���ñ���͸��ɫ
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
//	���û��Ʋ���������ʾǰ������øú��� 
//
//	Return
//		�ɹ�����TRUE��ʧ�ܷ���FALSE
//
GifImageRender* GifImageBase::AddRender(Gif_Timer_Notify* pNotify, int* pIndex)
{
	if (NULL == pNotify)
		return NULL;

	if (0 == m_mapRenderItem.size())
	{
        // 		if (false == this->RealLoad())   // ֻ����������������һ��Renderʱ���Ŵ���
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
//	TODO: ���ʵ��������ʾ��һ֡����
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
	if (nSize == 1)    // ��֡gifͼƬ����Ҫʹ�ü�ʱ��
	{
		EnterCriticalSection(&m_pGifImage->m_sect);
		handle_disposal(NULL);                           // ˢ����
		draw_frame( m_pGifImage->GetFrame(m_nCurFrameIndex));   // ���Ƶ�һ֡��m_hMemCanvasDC��
		commit(m_notify.notify_hwnd.hDC, m_notify.notify_hwnd.x,m_notify.notify_hwnd.y);
		LeaveCriticalSection(&m_pGifImage->m_sect);
	}
	else
	{
		Gif_TimerItem timer_item = { (int)this, ::GetTickCount(), 0, -1, this };  // -1����ѭ��
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
	// ˢ�¸����ڣ�������Ҫע�����ͬ�����߳��е����һ��on_timer���ܽ������ˢ�����ָ�����
	EnterCriticalSection(&m_pGifImage->m_sect);
	m_nCurFrameIndex = 0;
	m_nDrawStatus = GIF_DRAW_STATUS_STOP;

	handle_disposal(NULL);  // ˢ����
	this->commit(m_notify.notify_hwnd.hDC, m_notify.notify_hwnd.x,m_notify.notify_hwnd.y);

	LeaveCriticalSection(&m_pGifImage->m_sect);
}

//
//	�ⲿ���ڵ���
//
void GifImageBase::OnPaint(HDC hDC, int nIndex)
{
	GifImageRender* pItem = this->GetDrawItemByIndex(nIndex);
	if (NULL == pItem)
		return;

	pItem->OnPaint(hDC, 0, 0);
}

//
//	UI�ؼ����ã�x,yΪ�ؼ��ڲ����꣬��(0,0)
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
			// ת���������߳�
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
//	��һ֡������ɺ󣬲�����delayʱ�䣬��Ҫ������һ֡ʱ����������disposal
//
void GifImageRender::handle_disposal(GIF_Frame* pFrame)
{
	// ��pFrameΪ��ʱ��Ĭ�Ͼ�ˢ������ͼƬ�ı���
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
			// ��ԭ
			if (m_hMemPrevSaveDC)
				::BitBlt(m_hMemCanvasDC,0,0,m_pGifImage->GetWidth(),m_pGifImage->GetHeight(),m_hMemPrevSaveDC,0,0,SRCCOPY);
		}
		break;
	}
}
//
//	����һ֡ͼ��
//
//	[parameter]
//		pFrame
//			[in]	Ҫ���Ƶĵ�ǰ֡
//
void GifImageRender::draw_frame(GIF_Frame* pFrame)
{
	if (NULL == pFrame)
		return;

	if (pFrame->control.disposal_methold == GIF_DISPOSAL_RESTORE_PREVIOUS)
	{
		if (NULL == m_hMemPrevSaveDC)  // ���������m_hMemPrevSaveBitmap�Ĵ���
		{
			m_hMemPrevSaveDC =::CreateCompatibleDC(NULL);
			Image image;
			image.Create(m_pGifImage->GetWidth(),m_pGifImage->GetHeight(), 32, Image::createAlphaChannel);
			m_hMemPrevSaveBitmap = image.Detach();

			/*HBITMAP hOldBmp = */(HBITMAP)::SelectObject(m_hMemPrevSaveDC, m_hMemPrevSaveBitmap);
		}
		// ����
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
// ע������һ���߳��б�����
//
void GifImageRender::on_timer(Gif_TimerItem* pTimerItem)  
{
	if (m_nDrawStatus != GIF_DRAW_STATUS_START)  
	{
		pTimerItem->nRepeat = 1;  // ��ͣ��ֹͣ״̬�£���check_timer�н��Զ�ɾ��
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
// 		// ����һ��gifͼ����14֡��ǰ������֡��disposal����2�����һ֡��1������ѭ������ʱʱ���һ֡��ͼ��͵�һ֡��
// 		// ͼ��һ����ʾ�ˡ����ǲ�����ζ�ŵ���������֡ʱ��Ҫ��ձ�����
// 		if (0 == m_nCurFrameIndex)
// 		{
// 			this->handle_disposal(NULL);
// 		}
// 	}
// 	else
// 	{
		GIF_Frame* pPrevFrame = m_pGifImage->GetFrame(m_nCurFrameIndex-1);  // ��0 == m_nCurFrameIndexʱ������NULL����ʱ����handle_disposal����ˢ����������
// 		if (pPrevFrame)
// 		{
			this->handle_disposal(pPrevFrame);
// 		}
// 	}

	GIF_Frame* pFrame = m_pGifImage->GetFrame(m_nCurFrameIndex);
	draw_frame(pFrame);
	this->commit(m_notify.notify_hwnd.hDC, m_notify.notify_hwnd.x,m_notify.notify_hwnd.y);

	// Ϊ�˱���Ƶ���ĸ����б�������ÿ�ν�����Gif_timer_item��������ݣ�������ɾ�������һ��
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

// <<--- ����gdiplus���ص�֡GIF����͸��ɫ��ֵ���ı�����⣬�����Ѹ����Լ���LZW�㷨������
//
//	����gif image�е�͸����Ϣ
//
//	����gif��͸����ͨ��ָ����ɫ���е�һ��������ʵ�֣�������ͨ���жϺ�͸��ɫ�����ʵ��
//	��Ϊ����ɫ���ܺ�͸��ɫΪ��ͬ����ɫ
//
//	��ע:
//		image.GetWidth��image.GetHeight������ʵ����ʵ�����ݿ�͸�Ҫ��ȡLogicalScreenDescriptor�е�width/height
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

	RGBQUAD palettes[255];  // ȡ�����ܵĴ�С
	pFrame->image.GetColorTable(0,255, palettes);

	BYTE*       pDestBits = lpBits;// + (pFrame->descriptor.image_height-1)*nDestRowBytes;  // �л������һ�У���ΪDIB�����µ��ϵģ�����˳��
	const BYTE* pSrcBits  = ((const BYTE*)pFrame->image.GetBits()) + pFrame->descriptor.image_top_position*pFrame->image.GetPitch();

	for (int row = 0; row < nDestHeight; row ++ )
	{
		for (int i=0,j=0; i<nDestRowBytes; i+=4,j++)
		{
			BYTE index = pSrcBits[j+pFrame->descriptor.image_left_position];  // ȡ�����λ�õĵ�ɫ������ֵ
			if (index >= nTransparentIndex)
			{
				pDestBits[i] = pDestBits[i+1] = pDestBits[i+2] = pDestBits[i+3]	= 0; // ��alpha��Ϊ0��͸����
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
//  LZW ���� GIF ����
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
// �ӵ�ǰ�������ж�ȡ��һ��Ҫ�������е����֡�
//
// ��ע��
//	1. LZW�ǻ���λ�ģ����ǻ����ֽڵ�
//  2. LZW��λ�Ǳ䳤�ģ����̶����� lzw code size ~ 12��ÿ���ֵ��е�����һ��������ʱ��ȡ��λ����1���ӵ�12�󷵻�lzw code size
//  3. ����  58         C1         05         D3         �� ��ǰnReadBitPosInByte=3����01011,000���m_nCurBitLength=11
//          01011000   11000001   00000101   11010011
//
//     ����һ��ȡ����Ϊ 58�е�ǰ5λ����C1�еĺ�6λ��000001 01011 ->43, nReadBitPosInByte=6
//     Ȼ����һ��ȡ����Ϊ C1��ǰ2����������8λ��05����Ҫ����D3�����λ�� 1 00000101 11 -> 1047�����������3���ֽڲ�ȡ������Ҫ������
//     �����������е� (*((int*)pDataCur) ������ȡ���ĸ��ֽ���������λ/����������ﲻ��ʹ�� byte����WORD������
//
#define GET_NEXT_VALUE() \
	((*((int*)pDataCur))>>nReadBitPosInByte) & ((1<<m_nCurBitLength)-1); \
	nReadBitPosInByte += m_nCurBitLength;              \
	pDataCur += nReadBitPosInByte>>3;                  \
	nReadBitPosInByte %= 8;

// LZW�㷨����
bool  GifLZWDecoder::Decode(const byte* pSrcData, int nSrcDataSize)
{
	int nRetSize = 0;

	// ע�⣺LZW�ǰ�λ��ȡ�����ݣ������ǰ��ֽڶ�ȡ��
	const byte*  pDataEnd = pSrcData+nSrcDataSize;
	const byte*  pDataCur = pSrcData;
	byte  nReadBitPosInByte = 0;      // ��ǰ��ȡpData�ֽڵ���һλ��

	WORD  wPrefix = 0, wSuffix = 0;

	wPrefix = GET_NEXT_VALUE();
	do 
	{
		if (wPrefix == GIF_LZW_END_TAG)   // ����
			break;

		// ��ʵ�ʵĽ�����, ��һ��������������CLEAN��־, ����Ϊ���㷨���Ż������,����������ֱ�ӽ������ѭ��,��������ѭ���ⲿ��ʼ��.
		// : ��ȷ������һ��7λ����LZW����ȡ���ĵ�һ�����������0x80 = 128
		if (wPrefix == GIF_LZW_CLEAN_TAG) // ���³�ʼ������Ҳ�����ݵĵ�һ���ֽ�
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

		// Ϊʲô������Ҫ�ȴ���һ��ǰ׺������Ϊ�˽���統��һ��Ҫpush�ֵ���Ϊ dict[82] = {7F,82}ʱ��
		// 82����ֵ�������Ͳ����ڣ��������Լ��������Լ��� �����д�� dict[82]={7F, }; suffix�Ȳ�д�롣
		// ���ʱ���ܹ���ȡ��dict[82].prefix�ˡ�
		m_dict[m_nDictUpper].prefix = wPrefix;

		// ��׺������һ���������ַ�
		WORD wSingleSuffix = wSuffix;
		while (wSingleSuffix > GIF_LZW_CLEAN_TAG)
			wSingleSuffix = m_dict[wSingleSuffix].prefix;
		
		Output(wPrefix);

		if (m_nDictUpper == wSuffix ||          // <--����ʲô��˼��������dict[82]={7F,82}�����Լ������Լ�ʱ������ֱ�ӽ���������ֵ䣬�������ټ��һ��CheckExist{7F,7F}�Ƿ���ڡ��������������ҹ��أ�������ڣ���82��һ���򱻶����ˣ�������һ��ѭ��ʱdict[82].prefix=82, output(82)����ѭ����
			false==CheckExist(wPrefix,wSingleSuffix))
		{
			PushDict(wPrefix, wSingleSuffix);   // �����ֵ�
		}
		wPrefix = wSuffix;

	} while (pDataEnd>pDataCur);

	UIASSERT(m_nResultDataSize==0);   // ��֤��ѹ����LZW���ݽ�ѹ��ΪͼƬ��С
	return m_nResultDataSize == 0;
}

// ���prefix suffix�Ƿ����ֵ��д���
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

// ���һ����Ч��������wֵ��Ȼ��һ���ֵ�����������ֵ䡣
// TODO: ���ݹ�����Ż���ѭ������
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

// ���ֵ������һ��
inline void GifLZWDecoder::PushDict(WORD wPrefix, WORD wSuffix)
{
	m_dict[m_nDictUpper].suffix = wSuffix;

	// GIF�淶�涨����12λ������12λ�ı�ﷶΧ���Ƶ�����������GIFΪ�����ѹ���ʣ����õ��Ǳ䳤���ֳ���
	// ����˵ԭʼ������8λ����ôһ��ʼ���ȼ���һλ��˵����ʼ���ֳ��ͳ���9λ��Ȼ��ʼ�ӱ�ţ�����żӵ�512ʱ��
	// Ҳ���ǳ���9Ϊ���ܱ����������ʱ��Ҳ����ζ�ź���ı��Ҫ��10λ�ֳ����ܱ�ʾ�ˣ���ô�����￪ʼ��
	// ������ֳ�����10λ�ˡ��������ƣ�����2^12Ҳ����4096ʱ���������һ�������־���Ӻ��濪ʼ����9λ������

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

