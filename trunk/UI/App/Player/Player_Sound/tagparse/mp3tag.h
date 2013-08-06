#pragma once
#include "..\Player_Sound.h"

// http://id3.org/
// http://blog.csdn.net/zwj2pp/article/details/5283764  MP3 �ļ���ʽ
// http://hi.baidu.com/violet_sk23/item/ab028fe3e17047e6fa42bae5 MP3��ǩ��ʽ
// http://blog.chinaunix.net/uid-20792001-id-1841011.html   MPEG��Ƶ�ļ���ʽ(����MP3�ļ���ʽ)���---ת�� 
// http://wenku.baidu.com/view/69f54afdc8d376eeaeaa31c5.html id3v2
// http://id3.org/Lyrics3v2        Lyrics3v2

#pragma pack(push,1) 
typedef struct tagID3V1
{
	// ע�⣺
	char Header[3];     // ��ǩͷ������"TAG"������Ϊû�б�ǩ
	char Title[30];     // ����
	char Artist[30];    // ����
	char Album[30];     // ר��
	char Year[4];       // ��Ʒ���
	char Comment[28];   // ��ע
	char reserve;       // ����
	char track;;        // ����, ������ר���еı��
	char Genre;         // ��������

    bool  IsValid();

}ID3V1, *LPID3V1;

typedef struct tagID3V2Header
{
	char Header[3];    // ����Ϊ"ID3"������Ϊ��ǩ������
	char Ver;          // �汾��ID3V2.3�ͼ�¼3
	char Revision;     // ���汾�Ŵ˰汾��¼Ϊ0
	char Flag;         // ��ű�־���ֽڣ�����汾ֻ��������λ���Ժ���ϸ��˵
	byte Size[4];      // ����˵�Ĵ�С��ID3v2��Ϣ������ͬ�����룬������չ��ǩͷ(extended header),��������(padding)��
                       // ����������ǩͷ�Ĵ�С��Ҳ��������ID3v2��ǩ��Ϣ��С��ȥ10��ֻ��28λ(256MB)��Ϊ�˱�����MP3��ͬ���źų�ͻ

	int   CalcSize();     // ����Size��ֵ
    int   GetTotalSize(); // ����mp3�ļ�������id3v2ռ�Ĵ�С
    bool  IsValid();

}ID3V2Header;

// ��ID3V2Header::Flag������չ����У����������չ״��
typedef struct tagID3V2HeaderExt
{
	byte Size[4];
	byte Flag[2];
	byte Fill[4];

	int  CalcSize();
    int  GetTotalSize();

}ID3V2HeaderExt;

typedef struct tagID3V2Frame
{
	char FrameID[4];   // ���ĸ��ַ���ʶһ��֡��˵��������
	byte Size[4];      // ֡���ݵĴ�С��������֡ͷ������С��1 
	char Flags[2];     // ��ű�־��ֻ������6λ

	int CalcTotalSize();
}ID3V2Frame;

#pragma pack(pop)

// id3 v2 �ַ��������ʽ
enum id3_field_textencoding {
	ID3_FIELD_TEXTENCODING_ISO_8859_1 = 0x00,
	ID3_FIELD_TEXTENCODING_UTF_16     = 0x01,
	ID3_FIELD_TEXTENCODING_UTF_16BE   = 0x02,
	ID3_FIELD_TEXTENCODING_UTF_8      = 0x03
};

// id3 v2 �ַ��� UTF16�����ʽ���ֽ���
enum id3_utf16_byteorder {
	ID3_UTF16_BYTEORDER_ANY,
	ID3_UTF16_BYTEORDER_BE,
	ID3_UTF16_BYTEORDER_LE
};

struct FrameHeader {
	unsigned int emphasis:           2;   // ǿ��ģʽ
	unsigned int original:           1;   // ԭ���־
	unsigned int copyright:          1;   // ��Ȩ
    unsigned int mode_extension:     2;   // ����ģʽ Mode extension (Only if mode = Joint stereo)
	unsigned int mode:               2;   // ����ģʽ
	unsigned int reserved:           1;   // ������
	unsigned int padding:            1;   // ֡������
	unsigned int sampling_frequency: 2;   // ����Ƶ��
	unsigned int bitrate_index:      4;   // λ��
	unsigned int error_protection:   1;   // CRCУ��
	unsigned int layer:              2;   // ��
	unsigned int version:            2;   // �汾
	unsigned int sync:               11;  // ͬ����Ϣ
};

typedef struct APEData
{
    char  szID[8];   // { 'A', 'P', 'E', 'T', 'A', 'G', 'E', 'X' } 
    int  version;    // 1000 = Version 1.000 (old)  2000 = Version 2.000 (new) 
    int  size;       // Tag size in bytes including footer and all tag items excluding the header to be as compatible as possible with APE Tags 1.000 
    int  itemcount;  // Number of items in the Tag (n) 
    int  flag;       // Global flags of all items 
    int  reserved1;
    int  reserved2;
}APEFoot, APEHead;

class CMP3TagID3
{
public:
	CMP3TagID3();
	~CMP3TagID3();

	bool    ParseFile(const TCHAR* szFilePath, ITagParseCallback* pCallback);
	
protected:
	bool    ParseID3v2(fstream& f, ITagParseCallback* pCallback);
    int     FindFirstFramePos(fstream& f);
    bool    ParseID3v1(fstream& f, ITagParseCallback* pCallback);
    bool    ParseLyrics3v2(fstream& f);
    bool    ParseApev2(fstream& f);
	bool    ParseFirstFrame(fstream& f, ITagParseCallback* pCallback);

	bool    ParseFile_v2_frame(char* szFrameID, char* pBuffer, int nBufferSize, ITagParseCallback* pCallback);
	void    ParseV2String(String& strRet, char* pBuffer, char* pBufEnd);
	int     GetTextEncode(char* pBuffer, id3_field_textencoding& textencoding, id3_utf16_byteorder& byteorder);
	void    ParseV2String(String& strRet, char* pBuffer, char* pBufEnd, id3_field_textencoding textencoding, id3_utf16_byteorder byteorder);
	void    ParseV2String_ISO_8859_1(String& strRet, char* pBuffer, char* pBufEnd);
	void    ParseV2String_UTF16(String& strRet, char* pBuffer, char* pBufEnd);
	void    ParseV2String_UTF8(String& strRet, char* pBuffer, char* pBufEnd);
public:
	ID3V1  m_ID3v1;
    int  m_nSizeID3V1;  // Ϊ0��ʾû��id3v1, 128��ʾ��id3v1

	ID3V2Header  m_ID3V2Header;
    
    FrameHeader  m_firstFrameHeader;  // mp3�ļ��׸�֡ͷ
    int  m_nFirstFramePos;

    int  m_nLyrics3V2StartPos; // Ϊ-1��ʾû�� Lyrics3v2
    int  m_nLyrics3V2EndPos;

    int  m_nApev2StartPos;   // Ϊ-1��ʾû��Apev2
    int  m_nApev2EndPos;

    bool m_bNotified;  // �Ƿ��Ѿ���������
};


