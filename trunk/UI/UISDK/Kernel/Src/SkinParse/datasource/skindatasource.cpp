#include "stdafx.h"
#include "skindatasource.h"
#include "Zip/zipdatasource.h"
#include "File/filedatasource.h"
#include "Zip/bytebufferreader.h"
#include "File/filebufferreader.h"

namespace UI
{

void  CreateDataSourceInstance(SKIN_PACKET_TYPE e, ISkinDataSource** pp)
{
    if (NULL == pp)
        return;

    ISkinDataSource* p = NULL;
    switch (e)
    {
    case SKIN_PACKET_TYPE_DIR:
        {
            p = new FileDataSource;
        }
        break;;

    case SKIN_PACKET_TYPE_ZIP:
        {
            p = new ZipDataSource;
        }
        break;
    }
    
    *pp = p;
}

void  CreateStreamBuffer(STREAM_TYPE e, IStreamBufferReader** pp)
{
    if (NULL == pp)
        return;

    IStreamBufferReader* p = NULL;
    switch (e)
    {
    case STREAM_TYPE_FILE:
        {
            p = new FileBufferReader;
        }
        break;;

    case STREAM_TYPE_BYTE:
        {
            p = new ByteBufferReader;
        }
        break;
    }

    *pp = p;
}

}