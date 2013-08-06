#pragma once
#include "..\Player_Sound.h"

// http://id3.org/
// http://blog.csdn.net/zwj2pp/article/details/5283764  MP3 文件格式
// http://hi.baidu.com/violet_sk23/item/ab028fe3e17047e6fa42bae5 MP3标签格式
// http://blog.chinaunix.net/uid-20792001-id-1841011.html   MPEG音频文件格式(包括MP3文件格式)详解---转载 
// http://wenku.baidu.com/view/69f54afdc8d376eeaeaa31c5.html id3v2
// http://id3.org/Lyrics3v2        Lyrics3v2

#pragma pack(push,1) 
typedef struct tagID3V1
{
	// 注意：
	char Header[3];     // 标签头必须是"TAG"否则认为没有标签
	char Title[30];     // 标题
	char Artist[30];    // 作者
	char Album[30];     // 专集
	char Year[4];       // 出品年代
	char Comment[28];   // 备注
	char reserve;       // 保留
	char track;;        // 音轨, 歌曲在专辑中的编号
	char Genre;         // 流派类型

    bool  IsValid();

}ID3V1, *LPID3V1;

typedef struct tagID3V2Header
{
	char Header[3];    // 必须为"ID3"否则认为标签不存在
	char Ver;          // 版本号ID3V2.3就记录3
	char Revision;     // 副版本号此版本记录为0
	char Flag;         // 存放标志的字节，这个版本只定义了三位，稍后详细解说
	byte Size[4];      // 这里说的大小是ID3v2信息经过非同步编码，包括扩展标签头(extended header),补白数据(padding)，
                       // 但不包括标签头的大小，也就是整个ID3v2标签信息大小减去10。只用28位(256MB)是为了避免与MP3的同步信号冲突

	int   CalcSize();     // 计算Size的值
    int   GetTotalSize(); // 计算mp3文件中整个id3v2占的大小
    bool  IsValid();

}ID3V2Header;

// 当ID3V2Header::Flag包含扩展标记有，后面会有扩展状况
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
	char FrameID[4];   // 用四个字符标识一个帧，说明其内容
	byte Size[4];      // 帧内容的大小，不包括帧头，不得小于1 
	char Flags[2];     // 存放标志，只定义了6位

	int CalcTotalSize();
}ID3V2Frame;

#pragma pack(pop)

// id3 v2 字符串编码格式
enum id3_field_textencoding {
	ID3_FIELD_TEXTENCODING_ISO_8859_1 = 0x00,
	ID3_FIELD_TEXTENCODING_UTF_16     = 0x01,
	ID3_FIELD_TEXTENCODING_UTF_16BE   = 0x02,
	ID3_FIELD_TEXTENCODING_UTF_8      = 0x03
};

// id3 v2 字符串 UTF16编码格式的字节序
enum id3_utf16_byteorder {
	ID3_UTF16_BYTEORDER_ANY,
	ID3_UTF16_BYTEORDER_BE,
	ID3_UTF16_BYTEORDER_LE
};

struct FrameHeader {
	unsigned int emphasis:           2;   // 强调模式
	unsigned int original:           1;   // 原版标志
	unsigned int copyright:          1;   // 版权
    unsigned int mode_extension:     2;   // 扩充模式 Mode extension (Only if mode = Joint stereo)
	unsigned int mode:               2;   // 声道模式
	unsigned int reserved:           1;   // 保留字
	unsigned int padding:            1;   // 帧长调节
	unsigned int sampling_frequency: 2;   // 采样频率
	unsigned int bitrate_index:      4;   // 位率
	unsigned int error_protection:   1;   // CRC校验
	unsigned int layer:              2;   // 层
	unsigned int version:            2;   // 版本
	unsigned int sync:               11;  // 同步信息
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
    int  m_nSizeID3V1;  // 为0表示没有id3v1, 128表示有id3v1

	ID3V2Header  m_ID3V2Header;
    
    FrameHeader  m_firstFrameHeader;  // mp3文件首个帧头
    int  m_nFirstFramePos;

    int  m_nLyrics3V2StartPos; // 为-1表示没有 Lyrics3v2
    int  m_nLyrics3V2EndPos;

    int  m_nApev2StartPos;   // 为-1表示没有Apev2
    int  m_nApev2EndPos;

    bool m_bNotified;  // 是否已经解析过了
};


