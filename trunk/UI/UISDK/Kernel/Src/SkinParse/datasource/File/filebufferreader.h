#pragma once
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"

class FileBufferReader : public IFileBufferReader
{
public:
    FileBufferReader();
    ~FileBufferReader();

public:
    virtual void  Release();
    virtual int   read(char* pread, int nread);
    virtual void  seek(int npos, ios_base::seek_dir dir);
    virtual int   tell();
    virtual bool  load(const TCHAR* szPath);
    virtual bool  save(const TCHAR* szPath);

private:
    fstream  f;
};