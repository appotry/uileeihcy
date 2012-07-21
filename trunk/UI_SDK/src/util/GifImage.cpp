#include "stdafx.h"
#include "GifImage.h"

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
	if( NULL != m_hMemCanvasBitmap )
	{
		DeleteObject(m_hMemCanvasBitmap);
		m_hMemCanvasBitmap = NULL;
	}
	if( NULL != m_hMemCanvasDC )
	{
		::ReleaseDC(m_hWnd,m_hMemCanvasDC);
		m_hMemCanvasDC = NULL;
	}
	if( NULL != m_hMemPrevSaveDC )
	{
		::ReleaseDC(m_hWnd,m_hMemPrevSaveDC);
		m_hMemPrevSaveDC = NULL;
	}
	if( NULL != m_hMemPrevSaveBitmap )
	{
		::DeleteObject(m_hMemPrevSaveBitmap);
		m_hMemPrevSaveBitmap = NULL;
	}
	if( NULL != m_hDC )
	{
		::ReleaseDC(m_hWnd, m_hDC);
		m_hDC = NULL;
	}
	if( NULL != m_hBrushTransparent )
	{
		::DeleteObject(m_hBrushTransparent);
		m_hBrushTransparent = NULL;
	}
}

//
// ����gif�ļ��е����ݲ���
//
void GifImage::skip_data_block(fstream& f)
{
	unsigned char cNextBlockLen = 0;
	do 
	{
		f.read((char*)&cNextBlockLen, 1 );  // �������ݳ��� ���ΪFF�����Ϊ0��ʾ���ݶν���
		if( 0 == cNextBlockLen )
			break;

		f.seekg(cNextBlockLen, ios_base::cur);
	} 
	while (1);
}

//
//	�齨һ֡gif����
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

	memset(one_frame_data+pos, GIF_BLOCK_FLAG_TRAILER, 1 );
	pos += 1;

	assert(pos == nTotalSize);

	*pOutSize = nTotalSize;
	*ppOut = one_frame_data;
}

//
//	ʹ��·�������ļ�GIF����
//
bool GifImage::Load(TCHAR* szPath)
{
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
		if( f.fail() )      break;
		if(!header.Valid()) break;

		// �ļ�ȫ����Ϣ
		GIF_LogicalScreenDescriptor logicScreenDesc;
		f.read((char*)&logicScreenDesc, sizeof(GIF_LogicalScreenDescriptor));
		if(f.fail())        break;

		this->m_nImageWidth = logicScreenDesc.logical_screen_width;
		this->m_nImageHeight = logicScreenDesc.logical_screen_height;

		// ȫ����ɫ��
		void* pGlobalColorTable = NULL;
		int   nGlobalColorTableSize = 0;

		if( logicScreenDesc.global_color_table_flag )
			nGlobalColorTableSize = static_cast<int>(3*pow((double)2,(int)(logicScreenDesc.size_of_global_color_table+1)));

		if( 0 != nGlobalColorTableSize )
		{
			pGlobalColorTable = (void*)new char[nGlobalColorTableSize];
			f.read((char*)pGlobalColorTable, nGlobalColorTableSize);
		}

		// ѭ����ȡÿһ֡�����ݣ������һ������֡��gif�ļ�����image���ء�
		GIF_Frame* pFrame = NULL;
		int nFrameStartPos = 0;
	
		do 
		{
			BYTE bNextBlockFlag = 0;
			f.read((char*)&bNextBlockFlag, 1);

			if( bNextBlockFlag == GIF_BLOCK_FLAG_TRAILER )
			{
				bRet = true;
				break;   // �ļ�����
			}

			if( NULL == pFrame )
			{
				pFrame = new GIF_Frame;
				nFrameStartPos = f.tellg();
				nFrameStartPos -= 1;
			}

			switch(bNextBlockFlag)
			{
			case GIF_BLOCK_FLAG_EXTENSION:
				{
					BYTE bNextExtFlag = 0;
					f.read( (char*)&bNextExtFlag, 1);

					switch(bNextExtFlag)
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

					// ����ɫ�ʱ�
					void* pLocalColorTable = NULL;
					int   nLocalColorTableSize = 0;

					if( pFrame->descriptor.local_color_table_flag )
						nLocalColorTableSize = static_cast<int>(3*pow((double)2,pFrame->descriptor.size_of_local_color_table+1));

					if( 0 != nLocalColorTableSize )
					{
						pLocalColorTable = (void*)new char[nLocalColorTableSize];
						f.read((char*)pLocalColorTable, nLocalColorTableSize);
					}

					BYTE bUnknown = 0;
					f.read((char*)&bUnknown,1);   // TODO: ���ֵ�Ǹ�ʲô�ģ�

					skip_data_block(f);
					int nFrameEndPos = f.tellg();

					// ��֡ͼ������
					int   nImageDataSize = nFrameEndPos-nFrameStartPos;
					void* pImageData = (void*)new char[nImageDataSize];

					f.seekg(nFrameStartPos, ios_base::beg);
					f.read((char*)pImageData, nImageDataSize);

					//
					// ��װ
					//
					void* one_frame_gif_file_data = NULL;
					int   one_frame_gif_file_data_size = 0;

					if( NULL == pLocalColorTable )
						this->build_one_frame_data(&header, &logicScreenDesc, pGlobalColorTable, nGlobalColorTableSize, pImageData, nImageDataSize, &one_frame_gif_file_data, &one_frame_gif_file_data_size );
					else
						this->build_one_frame_data(&header, &logicScreenDesc, pLocalColorTable, nLocalColorTableSize, pImageData, nImageDataSize, &one_frame_gif_file_data, &one_frame_gif_file_data_size );

					if( NULL != pLocalColorTable )
					{
						delete[] pLocalColorTable;
						pLocalColorTable = NULL;
						nLocalColorTableSize = 0;
					}
					if ( NULL != pImageData )
					{
						delete[] pImageData;
						pImageData = NULL;
						nImageDataSize = 0;
					}

#ifdef _DEBUG__
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
					if( false == pFrame->image.LoadFromData(one_frame_gif_file_data, one_frame_gif_file_data_size) )
					{
						delete [] one_frame_gif_file_data;
						one_frame_gif_file_data = NULL;

						this->Destroy();
						goto PARSE_ERROR;
						break;
					}
					delete [] one_frame_gif_file_data;
					one_frame_gif_file_data = NULL;
					

					// ���ñ���͸��ɫ
					if( pFrame->control.transparent_color_flag )
					{
						this->decode_gif_image_transparent(pFrame->image, (LONG)pFrame->control.transparent_color_index);
						//pFrame->image.SetTransparentColor((LONG)pFrame->control.transparent_color_index);
					}

					m_vFrame.push_back(pFrame);
					pFrame = NULL;
				}
				break;

			};  // switch(bNextBlockFlag)

		} while (1);  // do

PARSE_ERROR:
		if( NULL != pFrame )  // ʣ�ಿ��û��image data�ˣ����»�new��һ�Σ��������ͷŵ�
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

int GifImage::get_next_frame_index()
{
	int nIndex = m_nCurFrameIndex;
	m_nCurFrameIndex++;
	if( m_nCurFrameIndex >= (int)m_vFrame.size() )
		m_nCurFrameIndex = 0;

	return nIndex;
}
GIF_Frame* GifImage::GetFrame( int nIndex )
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
BOOL GifImage::SetDrawParam( HWND hWnd, int x, int y, COLORREF colorTransparent )
{
	if( m_vFrame.size() <= 0 )
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
	if( nSize <= 0 )
		return ;

	m_nDrawStatus = GIF_DRAW_STATUS_START;
	if( nSize == 1 )    // ��֡gifͼƬ����Ҫʹ�ü�ʱ��
	{
		EnterCriticalSection(&m_sect);
		draw_frame( GIF_DISPOSAL_NULL, this->GetFrame(m_nCurFrameIndex));
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
	if( m_vFrame.size() <= 0 )
		return ;

	m_nDrawStatus = GIF_DRAW_STATUS_PAUSE;
}
void GifImage::Stop()
{
	if( m_vFrame.size() <= 0 )
		return ;

	// ˢ�¸����ڣ�������Ҫע�����ͬ�����߳��е����һ��on_timer���ܽ������ˢ�����ָ�����
	EnterCriticalSection(&m_sect);
	m_nCurFrameIndex = 0;
	m_nDrawStatus = GIF_DRAW_STATUS_STOP;

	draw_frame( GIF_DISPOSAL_RESTORE_BACKGROUND, NULL);
	::BitBlt(m_hDC,this->GetDrawX(),this->GetDrawY(), this->GetWidth(), this->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);

	LeaveCriticalSection(&m_sect);
}

//
//	�ⲿ���ڵ���
//
void GifImage::OnPaint(HDC hDC)
{
	if( GIF_DRAW_STATUS_STOP == m_nDrawStatus )
		return;

	EnterCriticalSection(&m_sect);
	::BitBlt(hDC,this->GetDrawX(),this->GetDrawY(), this->GetWidth(), this->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);
	LeaveCriticalSection(&m_sect);
}

//
//	����һ֡ͼ��
//
//	[parameter]
//		nPrevFrameDisposal
//			[in]	��һ֡��dispose����
//
//		pFrame
//			[in]	Ҫ���Ƶĵ�ǰ֡
//
void GifImage::draw_frame(int nPrevFrameDisposal, GIF_Frame* pFrame)
{
	switch (nPrevFrameDisposal)
	{
	case GIF_DISPOSAL_NULL:
	case GIF_DISPOSAL_LEFT:
		break;

	case GIF_DISPOSAL_RESTORE_BACKGROUND:
		{
			RECT rcBack = {0,0, this->GetWidth(),this->GetHeight()};
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

	if (NULL == pFrame)   // ���ں���ȥ�жϣ���������֧�ֻ���͸������ˢ�½���
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
		pFrame->descriptor.image_left_position,
		pFrame->descriptor.image_top_position,
		pFrame->descriptor.image_width,
		pFrame->descriptor.image_height);
}
//
// ����һ���߳��б�����
//
void GifImage::on_timer(Gif_TimerItem* pTimerItem)  
{
	if( m_nDrawStatus != GIF_DRAW_STATUS_START )  
	{
		pTimerItem->nRepeat = 1;  // ��ͣ��ֹͣ״̬�£���check_timer�н��Զ�ɾ��
		return;
	}
	EnterCriticalSection(&m_sect);
	if( m_nDrawStatus != GIF_DRAW_STATUS_START )
	{
		Gif_Timer_Factory::GetGifTimerEngine()->on_kill_timer((int)this);
		return;
	}

	int nDisposal = GIF_DISPOSAL_NULL;
	GIF_Frame* pPrevFrame = this->GetFrame(m_nCurFrameIndex-1);
	if( NULL != pPrevFrame )
	{
		nDisposal = pPrevFrame->control.disposal_methold;
	}

	GIF_Frame* pFrame = this->GetFrame(m_nCurFrameIndex);
	draw_frame( nDisposal, pFrame );
	::BitBlt(m_hDC,this->GetDrawX(),this->GetDrawY(), this->GetWidth(), this->GetHeight(),m_hMemCanvasDC,0,0,SRCCOPY);


	// Ϊ�˱���Ƶ���ĸ����б�������ÿ�ν�����Gif_timer_item��������ݣ�������ɾ�������һ��
	int nNextFrameIndex = get_next_frame_index();
	if( nNextFrameIndex < (int) m_vFrame.size() )
	{
		GIF_Frame* pFrame = m_vFrame[nNextFrameIndex];

		if( NULL != pFrame )
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

//
//	����gif image�е�͸����Ϣ
//
//	����gif��͸����ͨ��ָ����ɫ���е�һ��������ʵ�֣�������ͨ���жϺ�͸��ɫ�����ʵ��
//	��Ϊ����ɫ���ܺ�͸��ɫΪ��ͬ����ɫ
//
bool GifImage::decode_gif_image_transparent(Image& image, int nTransparentIndex)
{
	if( image.IsNull() )
		return false;

	BITMAPINFOHEADER bmih;
	::ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biBitCount = 32;
	bmih.biWidth = image.GetWidth();
	bmih.biHeight = image.GetHeight() * -1;
	bmih.biCompression = BI_RGB;
	bmih.biPlanes = 1;
	BYTE* lpBits = NULL;
	
	HBITMAP hBitmap = ::CreateDIBSection(NULL,(BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&lpBits, NULL, 0 );
	if(NULL == hBitmap || NULL == lpBits)
		return false;

	const BYTE* lpPaletteBits = (const BYTE*)image.GetBits();
	int npxCount = image.GetWidth()*image.GetHeight();  // gif�е����ظ�����ÿ��������һ����ɫ�������

	RGBQUAD palettes[255];  // ȡ�����ܵĴ�С
	image.GetColorTable(0,255, palettes);

	int rowbytes = 4*image.GetWidth();
	int height = image.GetHeight();

	BYTE*       pDestBits = lpBits;
	const BYTE* pSrcBits  = lpPaletteBits;
	for (int row = 0; row < height; row ++ )
	{
		for( int i=0,j=0; i<rowbytes; i+=4,j++)
		{
			BYTE index = pSrcBits[j];  // ȡ�����λ�õĵ�ɫ������ֵ
			if(index == nTransparentIndex)
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
		pDestBits += rowbytes;
		pSrcBits  += image.GetPitch();
	}

	image.Destroy();
	image.Attach(hBitmap);
	image.ForceUseAlpha();

#ifdef _DEBUG_
	image.Save(_T("C:\\test.png"), Gdiplus::ImageFormatPNG);
#endif

	return !image.IsNull();
}