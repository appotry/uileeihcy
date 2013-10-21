#ifndef IBUFFER_H_98BE9A4D_7F2D_40ea_87AB_A4DB060E30E6
#define IBUFFER_H_98BE9A4D_7F2D_40ea_87AB_A4DB060E30E6

namespace UI
{
interface IBuffer
{
    virtual void  AddRef() = 0;
    virtual void  Release() = 0;
    virtual void  SetBuffer(void* pBuffer, ULONG nSize) = 0;
    virtual ULONG  GetSize() = 0;
    virtual void*  GetBuffer() = 0;
};

}

#endif // IBUFFER_H_98BE9A4D_7F2D_40ea_87AB_A4DB060E30E6