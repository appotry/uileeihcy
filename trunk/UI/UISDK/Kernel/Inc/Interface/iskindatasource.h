#ifndef ISKINDATASOURCE_H_1BE75159_FE4C_48cc_980A_BD613C7DA944
#define ISKINDATASOURCE_H_1BE75159_FE4C_48cc_980A_BD613C7DA944

namespace UI
{

enum  SKIN_PACKET_TYPE
{
    SKIN_PACKET_TYPE_DIR,
    SKIN_PACKET_TYPE_ZIP,
};

interface IImage;
interface IStreamBufferReader;
interface ISkinDataSource : public IRootInterface
{
    virtual void  Init() = 0;
    virtual void  Release() = 0;
    virtual void  SetDirection(const TCHAR* szDir) = 0;
    virtual void  SetName(const TCHAR* szName) = 0;

    virtual const TCHAR*  GetDirection() = 0;
    virtual const TCHAR*  GetName() = 0;
    virtual SKIN_PACKET_TYPE  GetType() = 0;

    virtual bool  Load_UIDocument(IUIDocument* pDocument, const TCHAR* szPath) = 0;
    virtual bool  Load_RenderBitmap(IRenderBitmap* pBitmap, const TCHAR* szPath, bool bCreateAlphaChannel) = 0;
    virtual bool  Load_Image(IImage* pImage, const TCHAR* szPath) = 0;
    virtual bool  Load_StreamBuffer(const TCHAR* szPath, IStreamBufferReader** pp) = 0;
};


enum  STREAM_TYPE
{
    STREAM_TYPE_FILE,
    STREAM_TYPE_BYTE,
};

interface IStreamBufferReader
{
    virtual void  Release() = 0;
    virtual int   read(char* pread, int nread) = 0;
    virtual void  seek(int npos, ios_base::seek_dir dir) = 0;
    virtual int   tell() = 0;
    virtual bool  save(const TCHAR* szPath) = 0;
};

interface IFileBufferReader : public IStreamBufferReader
{
    virtual bool  load(const TCHAR* szPath) = 0;
};
interface IByteBufferReader : public IStreamBufferReader
{
    virtual bool  load(const byte* pdata, int nsize, bool bdeletedata) = 0;
};

}

#endif // ISKINDATASOURCE_H_1BE75159_FE4C_48cc_980A_BD613C7DA944