#include "stdafx.h"
#include "mp3tag.h"

const char id3v1_mark[3] = {'T', 'A', 'G'};
const char id3v2_mark[3] = {'I', 'D', '3'};
const char lyrics3v2_end_mark[9]  = { 'L', 'Y', 'R', 'I', 'C', 'S', '2', '0', '0' };
const char lyrics3v2_beg_mark[11] = { 'L', 'Y', 'R', 'I', 'C', 'S', 'B', 'E', 'G', 'I', 'N' };
const char apev2_end_mark[8] = { 'A', 'P', 'E', 'T', 'A', 'G', 'E', 'X' };
const char apev2_beg_mark[8] = { 'A', 'P', 'E', 'T', 'A', 'G', 'E', 'X' };

// ȥ�����Ŀո��ַ�
void trim(char *str)
{
	char *ptr;

	ptr = str + strlen(str);
	while (ptr > str && ptr[-1] == ' ')
		--ptr;

	*ptr = 0;
}
void trim(wchar_t *str)
{
	wchar_t *ptr;

	ptr = str + wcslen(str);
	while (ptr > str && (ptr[-1] == L' ' || ptr[-1] == L' '))
		--ptr;

	*ptr = 0;
}

// ������������ ����λ��Hz��
int Mp3_Sampling_Table[4][4] =
{
/* index  MPEG2.5  ����   MPEG2    MPEG1 */ 
/* 00 */  11025,    0,    22050,   44100,   
/* 01 */  12000,    0,    24000,   48000,   
/* 10 */  8000,     0,    16000,   32000,   
/* 11 */  -1,      -1,    -1,      -1,      
};

// ����������(����mpeg�汾��Ϊ������)
// ����ֵ��λΪkbps������1kbit=1000bit������1024bit

int Mp3_Bitrate_Table_MPEG_1[][4] = 
{
/* index  ���� Layer3 Layer2 Layer1 */
/*  0 */  -1,    -1,    -1,    -1,   
/*  1 */   0,    32,    32,    32,  
/*  2 */   0,    40,    48,    64,  
/*  3 */   0,    48,    56,    96,  
/*  4 */   0,    56,    64,   128,  
/*  5 */   0,    64,    80,   160,  
/*  6 */   0,    80,    96,   192,  
/*  7 */   0,    96,   112,   224,  
/*  8 */   0,   112,   128,   256,  
/*  9 */   0,   128,   160,   288,  
/* 10 */   0,   160,   192,   320,  
/* 11 */   0,   192,   224,   352,  
/* 12 */   0,   224,   256,   384,  
/* 13 */   0,   256,   320,   416,  
/* 14 */   0,   320,   384,   448,  
/* 15 */  -2    -2,     -2,    -2,   
};

int Mp3_Bitrate_Table_MPEG_2_25[][4] = 
{
/* index  ���� Layer23 Layer23  Layer1*/
/*  0 */  -1,   -1,     -1,      -1,   
/*  1 */   0,    8,     8,       32,   
/*  2 */   0,   16,     16,      48,   
/*  3 */   0,   24,     24,      56,   
/*  4 */   0,   32,     32,      64,   
/*  5 */   0,   40,     40,      80,   
/*  6 */   0,   48,     48,      96,   
/*  7 */   0,   56,     56,     112,   
/*  8 */   0,   64,     64,     128,   
/*  9 */   0,   80,     80,     144,   
/* 10 */   0,   96,     96,     160,   
/* 11 */   0,  112,    112,     176,   
/* 12 */   0,  128,   128,      192,   
/* 13 */   0,  144,   144,      224,   
/* 14 */   0,  160,   160,      256,   
/* 15 */  -2    -2,    -2,       -2,   
};

// int g_BitrateTable[5][16] = 
// {
//     0,    0,   0,   0, 0,
//     32,  32,  32,  32, 8,
//     64,  48,  40,  48, 16,
//     96,  56,  48,  56, 24,
//     128,  64,  56,  64, 32,
//     160,  80,  64,  80, 40,
//     192,  96,  80,  96, 48,
//     224, 112,  96, 112, 56,
//     256, 128, 112, 128, 64,
//     288, 160, 128, 144, 80,
//     320, 192, 160, 160, 96,
//     352, 224, 192, 176, 112,
//     384, 256, 224, 192, 128,
//     416, 320, 256, 224, 144,
//     448, 384, 320, 256, 160,
//     -1,  -1,  -1,  -1,  -1,
// };

int  FindSampleInTable(int nIndex, int nVersion)
{
    return Mp3_Sampling_Table[nIndex][nVersion];
}

int  FindBitrateInTable(int nIndex, int nVersion, int nLayer)
{
    if (nVersion == MPEG_VERSION_1)
    {
        return Mp3_Bitrate_Table_MPEG_1[nIndex][nLayer];
    }else
    {
        return  Mp3_Bitrate_Table_MPEG_2_25[nIndex][nLayer];
    }
    return -1;
}

int  GetSideInfomation(int nVersion, int nChannelMode)
{
    if (CHANNEL_MODE_Single == nChannelMode)
    {
        if (MPEG_VERSION_1 == nVersion)
            return 17;
        else
            return 9;
    }
    else 
    {
        if (MPEG_VERSION_1 == nVersion)
            return 32;
        else
            return 17;
    }
}
//
// ���� ID3V2λ�ô����ļ���ǰ�棬Ϊ�˱�����֡ͷ��ͬ��λһ�£�����������ڱ����������11��1
// һ���ĸ��ֽڣ���ÿ���ֽ�ֻ��7λ�����λ��ʹ�ú�Ϊ0�����Ը�ʽ����
// 0xxxxxxx 0xxxxxxx 0xxxxxxx 0xxxxxxx
int ID3V2Header::CalcSize()
{
	int total_size = ((Size[0] & 0x7F) << 21)
					+((Size[1] & 0x7F) << 14)
					+((Size[2] & 0x7F) << 7)
					+ (Size[3] & 0x7F);
	return total_size;
}
int  ID3V2Header::GetTotalSize()
{
    if (!IsValid())
        return 0;

    return sizeof(ID3V2Header) + CalcSize();
}
bool ID3V2Header::IsValid()
{
    return Header[0] != '\0';
}

int ID3V2Frame::CalcTotalSize()
{
	int total_size = int(Size[0]<<24)
					+(Size[1]<<16)
					+(Size[2]<<8)
					+ Size[3];

	return total_size;
}

bool  ID3V1::IsValid()
{
    return Header[0] != '\0';
}
//////////////////////////////////////////////////////////////////////////

CMP3TagID3::CMP3TagID3()
{	
	ZeroMemory(&m_ID3v1, sizeof(ID3V1));
	ZeroMemory(&m_ID3V2Header, sizeof(ID3V2Header));
    ZeroMemory(&m_firstFrameHeader, sizeof(FrameHeader));

    m_nSizeID3V1 = 0;
    m_nFirstFramePos = 0;
    m_nLyrics3V2StartPos = -1;
    m_nLyrics3V2EndPos = -1;
    m_nApev2StartPos = -1;
    m_nApev2EndPos = -1;

    m_bNotified = false;
}
CMP3TagID3::~CMP3TagID3()
{
	
}

bool CMP3TagID3::ParseFile(const TCHAR* szFilePath, ITagParseCallback* pCallback)
{
	if (NULL == szFilePath || NULL == pCallback)
		return false;

	fstream  f;
	f.open(szFilePath, ios_base::in|ios_base::binary);
	if (f.fail())
		return false;

    // id3v2
	bool bRet = ParseID3v2(f, pCallback);

    // frame pos
    m_nFirstFramePos = FindFirstFramePos(f);
    if (-1 == m_nFirstFramePos)
        return false;

    // id3v1
	bRet = ParseID3v1(f, pCallback);
    
    // Lyrics3v2
	ParseLyrics3v2(f);

    // Apev2
    ParseApev2(f);

    // 
	ParseFirstFrame(f, pCallback);

	f.close();
	return true;
}


// http://wenku.baidu.com/view/ec4a4210a216147917112873.html  MPEG��� + ��μ���CBR VBR MP3�Ĳ���ʱ��
// http://blog.csdn.net/xsjm206/article/details/6719958     MP3 ֡����
//
// ֡ͷ��ÿһ֡��ǰ4���ֽڣ�32λ����ɡ�֡ͷ��ǰ11����
// ���ǹ̶��ĳ�����֡ͬ��������ˣ������������ļ��в��ҵ�
// һ��֡ͬ�������������ҵ�һ��ֵΪ255��������������
// �ĸ����λ��1���ֽڡ���Ȼ���ȡ����ͷ���ֵ�Ƿ���ȷ��
//


// ����ͬ��λѰ��Frame Head
int CMP3TagID3::FindFirstFramePos(fstream& f)
{
    f.seekg(m_ID3V2Header.GetTotalSize(), ios_base::beg);

    byte bSync = 0;
    byte b2 = 0, b3 = 0, b4 = 0;

    // ע�⣬FrameHead���ǰ����ڴ��ϵ����������ģ����Ǵ����ҵ�˳��һֱ������ȥ��

    bool bFind = false;
    while (f.good())
    {
        f.read((char*)&bSync, 1);
        if (0xFF == bSync)
        {
            f.read((char*)&b2, 1);
            if ((b2 & 0xC0) == 0xC0 && b2 != 0xFE) // 11λͬ��λ. FFFE������Ч��֡ͷ����Ҳ��֪��Ϊʲô������
            {
                f.read((char*)&b3, 1);
                f.read((char*)&b4, 1);

                // TODO: ���ﻹ��û���ҵ������жϸ���
                // ��Щ��̬�ļ����ļ�ͷǰ����һ��FF FF FF...����˳��˼��11λ1������ȥ�жϱ�ı�־
                int n = bSync << 24 | b2 << 16 | b3 << 8 | b4;
                memcpy(&m_firstFrameHeader, &n, 4);
                if (m_firstFrameHeader.bitrate_index == 0xF || m_firstFrameHeader.bitrate_index == 0) // Bitrate reversed or free
                {
                    f.seekg(-3, ios_base::cur);
                    continue;
                }
                if (m_firstFrameHeader.sampling_frequency == 3)  // Sample reversed
                {
                    f.seekg(-3, ios_base::cur);
                    continue;
                }
                // ע�� �����ж�m_firstFrameHeader.reserved�Ƿ�Ϊ0����Ϊ��Щ�ļ���ֵ����1
//              if (m_firstFrameHeader.reserved != 0)
//              {
//                  f.seekg(-3, ios_base::cur);
//                  continue;
//              }

                if (m_firstFrameHeader.mode_extension != 0 && m_firstFrameHeader.mode != CHANNEL_MODE_JointStereo)
                {
                    f.seekg(-3, ios_base::cur);
                    continue; 
                }

                bFind = true;
                break;
            }
            else
            {
                f.seekg(-1, ios_base::cur);  // ���¶�ȡ
            }
        }
    }

    if (!bFind)
    {
        memset(&m_firstFrameHeader, 0, sizeof(m_firstFrameHeader));
        return -1;
    }

    int nCurPos = f.tellg();
    return nCurPos - 4; // ��ȥ�Ѿ���ȡ����4���ֽ�
}

bool CMP3TagID3::ParseFirstFrame(fstream& f, ITagParseCallback* pCallback)
{
	// ��������ʺ�����
    int nSample = FindSampleInTable(m_firstFrameHeader.sampling_frequency, m_firstFrameHeader.version);
    int nBitrate = FindBitrateInTable(m_firstFrameHeader.bitrate_index, m_firstFrameHeader.version, m_firstFrameHeader.layer);
    pCallback->SetBitrate(nBitrate*1000);
    pCallback->SetSapmleRate(nSample);

	pCallback->SetFormat(m_firstFrameHeader.version, m_firstFrameHeader.layer, m_firstFrameHeader.mode);

     // ����ʱ��
    if (nSample > 0 && nBitrate > 0)
    {
        // ��������Ϣ... (����ʲô����??)����λ��XINGͷ���ж�MP3���ͣ���CBR������VBR
        int nSideInfo = GetSideInfomation(m_firstFrameHeader.version, m_firstFrameHeader.mode);
        f.seekg(nSideInfo, ios_base::cur);

        const char szXING[4] = {'X', 'I', 'N', 'G'};
        const char szVBRI[4] = {'V', 'V', 'R', 'I'};

        char  szReadNext4[4] = {0};
        f.read((char*)szReadNext4, 4);

        if (0 == memcmp(szReadNext4, szXING, 4) || 
            0 == memcmp(szReadNext4, szVBRI, 4) ) // VBR
        {
            UIASSERT(0 && _T("VBR Mp3ʱ��������Ŀǰ��δ�õ��������ļ�"));
        }
        else
        {
            // �ж��ļ�ĩβ����ID3V1
            int nFileSize = 0;
            int nTotalFrameSize = 0;
            
            f.seekg(0, ios_base::end);
            nFileSize = f.tellg();

            nTotalFrameSize = nFileSize - m_nFirstFramePos;  // nTotalFrameSize = �ļ���С - ID3V1��С - ID3V2��С

           if (m_ID3v1.IsValid())
                nTotalFrameSize -= 128;
            
            if (-1 != m_nLyrics3V2StartPos)
                nTotalFrameSize -= (m_nLyrics3V2EndPos - m_nLyrics3V2StartPos);

            if (-1 != m_nApev2StartPos)
                nTotalFrameSize -= (m_nApev2EndPos - m_nApev2StartPos);

            // CBR ����ʱ��(ms) = ����֡��С(�ֽ�) * 8����/�ֽ� / (������ ǧ����/�� * 1000 ����/ǧ����) * 1000 ms/s
            int nDuration = (nTotalFrameSize << 3) / nBitrate;
            pCallback->SetDuration(nDuration);
        }
    }

	return true;
}

// id3ʵ�ִ���
// http://id3.org/Implementations
// ftp://ftp.mars.org/pub/mpeg/
//
// ID3v2�淶������������֡���ݵĺ������һЩ��������(padding)��
// ����Խ�ID3v2�Ĵ�С����ñ�������֡(frame)���ݵ��ܺ�Ҫ��
// ����ʣ������ݾ�Ҫ��䲹������(padding)���������ĺô�����
// ����Ԥ��һЩ�ռ�Ŷ����֡(frame)�������������Ӷ����֡
// (frame)ʱ��Ҫ����д�������ļ�������������$00��
//
bool CMP3TagID3::ParseID3v2(fstream& f, ITagParseCallback* pCallback)
{
	if (NULL == pCallback)
		return false;

	f.seekg(0, ios_base::beg);

	bool  bRet = false;
	do 
	{
		f.read((char*)&m_ID3V2Header, sizeof(ID3V2Header));
		if (0 != memcmp(m_ID3V2Header.Header, id3v2_mark, sizeof(m_ID3V2Header.Header)))
        {
            memset(&m_ID3V2Header, 0, sizeof(ID3V2Header));
			break;
        }

		int nTotalSize = m_ID3V2Header.CalcSize();

		// �ж��Ƿ�����չͷ
		if (m_ID3V2Header.Flag & 0x40)
		{
			ID3V2HeaderExt headerExt;
			f.read((char*)&headerExt, sizeof(ID3V2HeaderExt));
			nTotalSize -= sizeof(ID3V2HeaderExt);
		}

		// �ж��Ƿ���Footer
		if (m_ID3V2Header.Flag & 0x20)
		{

		}
 
		while (nTotalSize > 10)   // ����Ҫʣ���ȡһ��ID3V2Frame�Ĵ�С
		{			
			ID3V2Frame frame;
			f.read((char*)&frame, sizeof(ID3V2Frame));

			if (frame.FrameID[0] == 0)   // ʣ��padding���֣�����
				break;

			int nFrameSize = frame.CalcTotalSize();

			//f.seekg(nFrameSize, ios_base::cur);
			char* p = new char[nFrameSize];
			f.read(p, nFrameSize);

			char szBuffer[5] = "";
			memcpy(szBuffer, frame.FrameID, 4);
			this->ParseFile_v2_frame(szBuffer, p, nFrameSize, pCallback);
			delete [] p;

			nTotalSize -= sizeof(ID3V2Frame) + nFrameSize;
		}

        m_bNotified = true;
		bRet = true;
	}
	while (0);

	return bRet;
}

bool CMP3TagID3::ParseFile_v2_frame(char* szFrameID, char* pBuffer, int nBufferSize, ITagParseCallback* pCallback)
{
	if (NULL == szFrameID || NULL == pBuffer || NULL == pCallback)
		return false;

	char* pBufEnd = pBuffer + nBufferSize;

	if (0 == strcmp(szFrameID, "TIT2"))
	{
		String str;
		ParseV2String(str, pBuffer, pBufEnd);
		pCallback->SetTitle(str.c_str());
	}
	else if (0 == strcmp(szFrameID, "COMM"))
	{
// 		<Header for 'Comment', ID: "COMM">
// 		Text encoding           $xx
// 		Language                $xx xx xx
// 		Short content descrip.  <text string according to encoding> $00 (00)
// 		The actual text         <full text string according to encoding>

		// ��ȡ���뷽ʽ
		id3_field_textencoding textencoding;
		id3_utf16_byteorder    byteorder;

		int nReadCount = GetTextEncode(pBuffer, textencoding, byteorder);
		pBuffer += nReadCount;

		// ��ȡ����
		char szLang[4] = "";
		memcpy(szLang, pBuffer, 3);
		pBuffer += 3;

		// ������ȡdescrip
		while (*pBuffer++)
			;

		// ����
		String str;
		ParseV2String(str, pBuffer, pBufEnd, textencoding, byteorder);
		pCallback->SetComment(str.c_str());
	}
	else if (0 == strcmp(szFrameID, "TYER"))
	{
		String str;
		ParseV2String(str, pBuffer, pBufEnd);
		pCallback->SetYear(str.c_str());
	}
	else if (0 == strcmp(szFrameID, "TPE1")) // ����
	{
		String str;
		ParseV2String(str, pBuffer, pBufEnd);
		pCallback->SetArtist(str.c_str());
	}
	else if (0 == strcmp(szFrameID, "TALB")) // ר��
	{
		String str;
		ParseV2String(str, pBuffer, pBufEnd);
		pCallback->SetAlbum(str.c_str());
	}
	else if (0 == strcmp(szFrameID, "TRCK")) // ����
	{
		pCallback->SetTrack(*pBuffer);
	}
	else if (0 == strcmp(szFrameID, "TCON")) // ����
	{
		pCallback->SetGenre(*pBuffer);
	}
	return true;
}

// �ڲ�������һ��ParseV2String
void CMP3TagID3::ParseV2String(String& strRet, char* pBuffer, char* pBufEnd)
{
	id3_field_textencoding textencoding;
	id3_utf16_byteorder    byteorder;

	int nReadCount = GetTextEncode(pBuffer, textencoding, byteorder);
	pBuffer += nReadCount;

	ParseV2String(strRet, pBuffer, pBufEnd, textencoding, byteorder);
}
// ��ȡ�ַ����ı��뷽ʽ���ֽ��򣬷��ض�ȡ���ֽ���
int CMP3TagID3::GetTextEncode(char* pBuffer, id3_field_textencoding& textencoding, id3_utf16_byteorder& byteorder)
{
	int nReadBytes = 0;
	if (NULL == pBuffer)
		return nReadBytes;

	byte* p = (byte*)pBuffer;
	textencoding = (id3_field_textencoding)(*p++);
	byteorder = ID3_UTF16_BYTEORDER_ANY;

	nReadBytes ++;

	switch (textencoding)
	{
	case ID3_FIELD_TEXTENCODING_UTF_16BE:
		byteorder = ID3_UTF16_BYTEORDER_BE;
		break;

	case ID3_FIELD_TEXTENCODING_UTF_16:
		{
			byte b1 = *p ++;
			byte b2 = *p ++;
			unsigned short s = b1 + (b2 << 8);

			if (s == 0xFEFF)
			{
				byteorder = ID3_UTF16_BYTEORDER_BE;
			}
			else if (s == 0xFFFE)
			{
				byteorder = ID3_UTF16_BYTEORDER_LE;
			}
			nReadBytes += 2;
		}
		break;
	}

	return nReadBytes;
}

void  CMP3TagID3::ParseV2String(String& strRet, char* pBuffer, char* pBufEnd, id3_field_textencoding textencoding, id3_utf16_byteorder byteorder)
{
	if (NULL == pBuffer)
		return;

	switch (textencoding)
	{
	case ID3_FIELD_TEXTENCODING_ISO_8859_1:
		ParseV2String_ISO_8859_1(strRet, pBuffer, pBufEnd);
		break;

	case ID3_FIELD_TEXTENCODING_UTF_16BE:
		ParseV2String_UTF16(strRet, pBuffer, pBufEnd);
		break;

	case ID3_FIELD_TEXTENCODING_UTF_16:
		ParseV2String_UTF16(strRet, pBuffer, pBufEnd);
		break;

	case ID3_FIELD_TEXTENCODING_UTF_8:
		ParseV2String_UTF8(strRet, pBuffer, pBufEnd);
		break;
	}
}

// TODO: Ϊʲô����ʹ�� 28591 ���������أ�����������
#define CP_ISO_8859_1 28591
void CMP3TagID3::ParseV2String_ISO_8859_1(String& strRet, char* pBuffer, char* pBufEnd)
{
// 	char* p = new char[nBufferSize+1];
// 	memcpy(p, pBuffer, nBufferSize);
// 	p[nBufferSize] = 0;
// 	strRet = CA2W(p);

	int nBufferSize = pBufEnd - pBuffer;
	int nSize =  ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pBuffer, nBufferSize, 0,0);
	if (0 == nSize)
		return;

	WCHAR* pDst = new WCHAR[nSize+1];
	memset(pDst, 0, (nSize+1)*sizeof(WCHAR));
	nSize = ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pBuffer, nBufferSize, pDst, nSize+1);

 	trim(pDst);
	strRet.assign(pDst);
	SAFE_ARRAY_DELETE(pDst);
}
void CMP3TagID3::ParseV2String_UTF16(String& strRet, char* pBuffer, char* pBufEnd)
{
	// 	WCHAR* pText = new WCHAR[nBufferSize/2 +1];
	// 	if (ID3_FIELD_TEXTENCODING_UTF_16 == bEncode)
	// 	{

	// 	}
	// 	SAFE_ARRAY_DELETE(pText);
	int nBufferSize = pBufEnd - pBuffer;
	strRet.insert(0, (const wchar_t*)pBuffer, nBufferSize/2);
}
void CMP3TagID3::ParseV2String_UTF8(String& strRet, char* pBuffer, char* pBufEnd)
{
	int nBufferSize = pBufEnd - pBuffer;
	int nLen = ::MultiByteToWideChar(CP_UTF8, NULL, (LPCSTR)pBuffer, nBufferSize, NULL, 0) ;
	if (0 == nLen)
		return;

	WCHAR* pWideBuffer = new WCHAR[nLen+1];
	memset(pWideBuffer, 0, sizeof(WCHAR)*(nLen+1));
	nLen = ::MultiByteToWideChar(CP_UTF8, NULL, (LPCSTR)pBuffer, nBufferSize, pWideBuffer, nLen+1);
	
	strRet.assign(pWideBuffer);
	SAFE_ARRAY_DELETE(pWideBuffer);
}
bool CMP3TagID3::ParseID3v1(fstream& f, ITagParseCallback* pCallback)
{
	if (NULL == pCallback)
		return false;

    bool  bRet = false;
	f.seekg(-128, ios_base::end);
	do 
	{
		if (f.fail())
			break;

		f.read((char*)&m_ID3v1, 128);
		if (0 != memcmp(m_ID3v1.Header, id3v1_mark, sizeof(m_ID3v1.Header)))
        {
            memset(&m_ID3v1, 0, sizeof(ID3V1));
			break;
        }

        if (m_bNotified)
        {
            bRet = true;
            break;
        }

		char szBuffer[64] = "";
		
		memcpy(szBuffer, m_ID3v1.Title, sizeof(m_ID3v1.Title));
		trim(szBuffer);
		pCallback->SetTitle(CA2W(szBuffer));

		memset(szBuffer, 0, sizeof(szBuffer));
		memcpy(szBuffer, m_ID3v1.Artist, sizeof(m_ID3v1.Artist));
		trim(szBuffer);
		pCallback->SetArtist(CA2W(szBuffer));

		memset(szBuffer, 0, sizeof(szBuffer));
		memcpy(szBuffer, m_ID3v1.Album, sizeof(m_ID3v1.Album));
		trim(szBuffer);
		pCallback->SetAlbum(CA2W(szBuffer));

		memset(szBuffer, 0, sizeof(szBuffer));
		memcpy(szBuffer, m_ID3v1.Year, sizeof(m_ID3v1.Year));
		trim(szBuffer);
		pCallback->SetYear(CA2W(szBuffer));

		memset(szBuffer, 0, sizeof(szBuffer));
		memcpy(szBuffer, m_ID3v1.Comment, sizeof(m_ID3v1.Comment));
		trim(szBuffer);
		pCallback->SetComment(CA2W(szBuffer));

		pCallback->SetGenre(m_ID3v1.Genre);
		pCallback->SetTrack(m_ID3v1.track);

		bRet = true;
	} 
	while (0);

	return bRet;
}
// 
// How to read a Lyrics 3 v2.00 tag Here is a step by step description of how to read the lyrics3 v2.00 tag: 
// 1.Read the 9 bytes before the ID3v1 tag, if any. Those 9 bytes must be LYRICS200. 
// 2.Read the previous 6 bytes, which are text digits that, when interpreted as a number,
//   give you the total number of bytes in the Lyrics3 v2.00 tag field, including the 
//   LYRICSBEGIN header, but not including the trailing Tag size and LYRICS200 end string. 
// 3.Seek back in the file from the beginning of the tag size field, the number of bytes 
//   read in the previous step. 
// 4.Read 11 bytes forward. These 11 bytes must read LYRICSBEGIN. 
// 5.Start reading fields until you have read the number of bytes retrieved in step 2. 
//
bool  CMP3TagID3::ParseLyrics3v2(fstream& f)
{
    if (m_ID3v1.IsValid())
        f.seekg(-128, ios_base::end);
    else
        f.seekg(0, ios_base::end);

    int nEndPos = f.tellg();

    // ��ȡβ����ʶ
    char  szEndMark[9] = {0};
    f.seekg(-9, ios_base::cur);
    f.read(szEndMark, sizeof(szEndMark));
    if (0 != memcmp(szEndMark, lyrics3v2_end_mark, sizeof(szEndMark)))
        return false;

    // ��ȡ��С
    char szSize[7] = {0}; // 6+1
    f.seekg(-9-6, ios_base::cur);
    f.read(szSize, 6);

    int nSize = atoi(szSize);

    // ��ȡͷ����ʶ
    f.seekg(-nSize-6, ios_base::cur);
    if (f.fail())
    {
        f.clear();
        return false;
    }

    int nBeginPos = f.tellg();

    char szBegMark[11] = {0};
    f.read(szBegMark, sizeof(szBegMark));
    if (0 != memcmp(szBegMark, lyrics3v2_beg_mark, 11))
        return false;

    m_nLyrics3V2StartPos = nBeginPos;
    m_nLyrics3V2EndPos = nEndPos;

    return true;
}

bool  CMP3TagID3::ParseApev2(fstream& f)
{
    // ��λ
    if (-1 != m_nLyrics3V2StartPos)
    {
        f.seekg(m_nLyrics3V2StartPos, ios_base::beg);
    }
    else if (m_ID3v1.IsValid())
    {
        f.seekg(-128, ios_base::end);
    }
    else
    {
        f.seekg(0, ios_base::end);
    }

    int nEndPos = f.tellg();

    // ��ȡ32byte footer
    APEFoot  apefoot = {0};
    f.seekg(-32, ios_base::cur);
    if (f.fail())
    {
        f.clear();
        return false;
    }
    f.read((char*)&apefoot, 32);
    if (0 != memcmp(apefoot.szID, apev2_end_mark, sizeof(apev2_end_mark)))
        return false;

    f.seekg(-apefoot.size-32, ios_base::cur);  // ??? Ϊʲô���ﻹҪ��ȥ32��size�����Ѿ�������������С��
    if (f.fail())
    {
        f.clear();
        return false;
    }
    int nBegPos = f.tellg();

    APEHead apehead = {0};
    f.read((char*)&apehead, 32);
    if (0 != memcmp(apehead.szID, apev2_beg_mark, sizeof(apev2_beg_mark)))
        return false;

    m_nApev2StartPos = nBegPos;
    m_nApev2EndPos = nEndPos;
    return true;
       
}