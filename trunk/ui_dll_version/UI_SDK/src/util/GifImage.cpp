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
		::WaitForSingleObject(m_hMsgQueueEvent,INFINITE);   // �ȴ��̴߳�������Ϣ�����˲��ܷ�����Ϣ
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
//	�������̣߳����̣߳����ã�ת����gif�����߳������� --> 
//
void Gif_Timer::kill_timer(int id)
{
	post_message(GT_KILL_TIMER, (WPARAM)id, NULL );
}

// λ��gif�����߳���
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
	
	// ������ͬID
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
				// ������ѭ��������ɾ��
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

	// ��һʱ���ڣ�������Ϣ���У������ⲿ������Ϣʧ��
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	::SetEvent(pThis->m_hMsgQueueEvent);

	while (false == pThis->m_bQuitThread)
	{
		HANDLE arrHandles[2] = { pThis->m_hEvent_post_message, pThis->m_hEvent_set_timer };
		DWORD dwRet = ::WaitForMultipleObjects(2, arrHandles, FALSE, pThis->m_nBestWaitTime );   // ���ָ�eventһֱû����Ϣ��ȥ��������߳�
		::ResetEvent(pThis->m_hEvent_set_timer);        // �����߳�����������
		::ResetEvent(pThis->m_hEvent_post_message);     // ͬ��

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

	WaitForSingleObject(m_hEvent_not_in_timer_list,INFINITE);  // ȷ��gif�߳�û���ڷ���m_vFrame���ݡ�ͬʱ��״̬��Ϊ��STOP��gif�߳��´�Ҳ�������ٷ������������
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
// ����gif�ļ��е����ݲ���
//
int GifImage::skip_data_block(fstream& f, byte* pBits)
{
	int nDataLength = 0;              // ͳ�����ݵ��ܴ�С
	unsigned char cNextBlockLen = 0;  // ��ȡ��һ�ε����ݴ�С
	do 
	{
		f.read((char*)&cNextBlockLen, 1 );  // �������ݳ��� ���ΪFF�����Ϊ0��ʾ���ݶν���
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
//	ʹ��·�������ļ�GIF����
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
		// �ļ���־
		GIF_FileMark header;
		f.read((char*)&header, sizeof(GIF_FileMark));
		if (f.fail())        break;
		if (!header.Valid()) break;

		// �ļ�ȫ����Ϣ
		GIF_LogicalScreenDescriptor logicScreenDesc;
		f.read((char*)&logicScreenDesc, sizeof(GIF_LogicalScreenDescriptor));
		if (f.fail())        break;

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
			f.read((char*)pGlobalColorTable, nGlobalColorTableSize);

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
			f.read((char*)&bNextBlockFlag, 1);

			if (bNextBlockFlag == GIF_BLOCK_FLAG_TRAILER)
			{
				bRet = true;
				break;   // �ļ�����
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
							// ���� 
							f.seekg(13,ios_base::cur);
							skip_data_block(f);
						}
						break;
					case GIF_EXT_FLAG_CONTROL:
						{
							// ����
							f.read((char*)&pFrame->control, sizeof(GIF_GraphicControlExtension));
							pFrame->nRealDelayTime = pFrame->control.delay_time*10;
							if ( 0 == pFrame->nRealDelayTime )    // ��Щgif��ֵΪ0
								pFrame->nRealDelayTime = 100;

							assert( pFrame->control.disposal_methold != 3 );// ����GIFû�о�û���ԣ�û�ҵ������ز�
						}
						break;
					case GIF_EXT_FLAG_COMMENT:
						{
							// ����
							skip_data_block(f);
						}
						break;
					case GIF_EXT_FLAG_APP:
						{
							// ���� 
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
					// ����ɫ�ʱ�

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
					// ����

					byte* pColorTableForThisFrame     = pLocalColorTable==NULL? pGlobalColorTable:pLocalColorTable;
					int   nColorTableSizeForThisFrame = pLocalColorTable==NULL? nGlobalColorTableSize:nLocalColorTableSize;

					BYTE byte_LZW_code_size = 0;
					f.read((char*)&byte_LZW_code_size,1);   // ���ֵ�Ǹ�ʲô�ģ�  <-- ��LZW�㷨�ĳ�ʼλ����LZW code size 

					// �ȳ���ʹ���Լ��Ľ����㷨
					bool bDecodeRet = this->decode_by_lzw(f, pFrame, byte_LZW_code_size, pColorTableForThisFrame, nColorTableSizeForThisFrame);
					if (false == bDecodeRet)
					{
						// ʧ�����ٳ�����gdiplus�ķ�������
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

					// ����֡
					m_vFrame.push_back(pFrame);
					pFrame = NULL;
				}
				break;

			};  // switch(bNextBlockFlag)

		} while (1);  // do

PARSE_ERROR:
		if (NULL != pFrame)  // ʣ�ಿ��û��image data�ˣ����»�new��һ�Σ��������ͷŵ�
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
	// ��ȡͼ���������ڽ��루�ȶ�ȡ���ݴ�С��

	int nDataBeginPos = f.tellg();
	int nDataLength = skip_data_block(f);
	f.seekg(nDataBeginPos);

	pGifFrameImageData = new byte[nDataLength];
	skip_data_block(f, pGifFrameImageData);

	//////////////////////////////////////////////////////////////////////////
	// LZW����

	pOutputData = new byte[nOutputDataSize];
	memset(pOutputData, 0, nOutputDataSize);

	GifLZWDecoder decoder(byte_LZW_code_size, pOutputData, nOutputDataSize);
	decoder.Decode(pGifFrameImageData, nDataLength);

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

	// ��֡ͼ������
	int   nImageDataSize = nFrameEndPos-nFrameStartPos;
	void* pImageData = (void*)new char[nImageDataSize];

	f.seekg(nFrameStartPos, ios_base::beg);
	f.read((char*)pImageData, nImageDataSize);

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
//	���û��Ʋ���
//
//	Parameter
//		hWnd
//			[in]	����ʾ���ĸ�������
//
//		x,y
//			[in]	����λ��
//
//		colTransparent
//			[in]	͸��������ɫ
//
//	Return
//		�ɹ�����TRUE��ʧ�ܷ���FALSE
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

	// ��DIBת����DDB����ΪTransparentBlt��֧��DIB
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
//	������ʾλ��
//
void GifImage::SetDrawPos( int x, int y )
{
	m_nDrawX = x;
	m_nDrawY = y;
}

//
//	TODO: ���ʵ��������ʾ��һ֡����
//
void GifImage::Start()
{
	int nSize = (int)m_vFrame.size();
	if (nSize <= 0)
		return ;

	m_nDrawStatus = GIF_DRAW_STATUS_START;
	if (nSize == 1)    // ��֡gifͼƬ����Ҫʹ�ü�ʱ��
	{
		EnterCriticalSection(&m_sect);
		handle_disposal(NULL);                           // ˢ����
		draw_frame( this->GetFrame(m_nCurFrameIndex));   // ���Ƶ�һ֡��m_hMemCanvasDC��
		::BitBlt(m_hDC,this->GetDrawX(),this->GetDrawY(), this->GetWidth(), this->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);
		LeaveCriticalSection(&m_sect);
	}
	else
	{
		Gif_TimerItem timer_item = { (int)this, ::GetTickCount(), m_vFrame[m_nCurFrameIndex]->nRealDelayTime, -1, this };  // -1����ѭ��
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

	// ˢ�¸����ڣ�������Ҫע�����ͬ�����߳��е����һ��on_timer���ܽ������ˢ�����ָ�����
	EnterCriticalSection(&m_sect);
	m_nCurFrameIndex = 0;
	m_nDrawStatus = GIF_DRAW_STATUS_STOP;

	handle_disposal(NULL);  // ˢ����
	::BitBlt(m_hDC,this->GetDrawX(),this->GetDrawY(), this->GetWidth(), this->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);

	LeaveCriticalSection(&m_sect);
}

//
//	�ⲿ���ڵ���
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
//	UI�ؼ����ã�x,yΪ�ؼ��ڲ����꣬��(0,0)
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
//	��һ֡������ɺ󣬲�����delayʱ�䣬��Ҫ������һ֡ʱ����������disposal
//
void GifImage::handle_disposal(GIF_Frame* pFrame)
{
	// ��pFrameΪ��ʱ��Ĭ�Ͼ�ˢ������ͼƬ�ı���
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
			// ��ԭ
			::BitBlt(m_hMemCanvasDC,0,0,this->GetWidth(),this->GetHeight(),m_hMemPrevSaveDC,0,0,SRCCOPY);
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
void GifImage::draw_frame(GIF_Frame* pFrame)
{
	if (NULL == pFrame)
		return;

	if( pFrame->control.disposal_methold == GIF_DISPOSAL_RESTORE_PREVIOUS )
	{
		// ����
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
// ����һ���߳��б�����
//
void GifImage::on_timer(Gif_TimerItem* pTimerItem)  
{
	if (m_nDrawStatus != GIF_DRAW_STATUS_START)  
	{
		pTimerItem->nRepeat = 1;  // ��ͣ��ֹͣ״̬�£���check_timer�н��Զ�ɾ��
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
// 		// ����һ��gifͼ����14֡��ǰ������֡��disposal����2�����һ֡��1������ѭ������ʱʱ���һ֡��ͼ��͵�һ֡��
// 		// ͼ��һ����ʾ�ˡ����ǲ�����ζ�ŵ���������֡ʱ��Ҫ��ձ�����
// 		if (0 == m_nCurFrameIndex)
// 		{
// 			this->handle_disposal(NULL);
// 		}
// 	}
// 	else
// 	{
		GIF_Frame* pPrevFrame = this->GetFrame(m_nCurFrameIndex-1);  // ��0 == m_nCurFrameIndexʱ������NULL����ʱ����handle_disposal����ˢ����������
// 		if (NULL != pPrevFrame)
// 		{
			this->handle_disposal(pPrevFrame);
// 		}
// 	}

	GIF_Frame* pFrame = this->GetFrame(m_nCurFrameIndex);
	draw_frame(pFrame);
	::BitBlt(m_hDC,this->GetDrawX(),this->GetDrawY(), this->GetWidth(), this->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);

	// Ϊ�˱���Ƶ���ĸ����б�������ÿ�ν�����Gif_timer_item��������ݣ�������ɾ�������һ��
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
// ����һ���߳��б�����
//
void GifImage::on_add_to_timer_list()
{
	::ResetEvent(m_hEvent_not_in_timer_list);
}
//
// ����һ���߳��б�����
//
void GifImage::on_remove_from_timer_list()
{
	::SetEvent(m_hEvent_not_in_timer_list);
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

#ifdef _DEBUG_x // <-- ��ÿһ֡����Ϊһ���ļ�
	static int n = 0;
	TCHAR szPath[MAX_PATH] = _T("");
	_stprintf(szPath, _T("C:\\one_frame2\\%d.png"),n );
	pFrame->image.Save(szPath, Gdiplus::ImageFormatPNG);
#endif
	//nTransparentIndex++;   // ?? ΪʲôҪ+1? ����1�Ļ�����ͶԲ��ϣ�ʲôʱ��Ҫ��1 ��ʲôʱ����Ҫ��1 ��

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
void  GifLZWDecoder::Decode(const byte* pSrcData, int nSrcDataSize)
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

