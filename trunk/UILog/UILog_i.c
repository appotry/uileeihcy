

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Sun Jul 01 22:43:02 2012
 */
/* Compiler settings for .\UILog.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_ILogItem,0xC63D124C,0x7D48,0x4fc8,0x9C,0x52,0x6B,0xF9,0x75,0x2C,0x17,0x65);


MIDL_DEFINE_GUID(IID, IID_ILog,0x47F812B2,0x1295,0x4115,0x92,0xB9,0x28,0x3A,0x31,0xDF,0x97,0x1A);


MIDL_DEFINE_GUID(IID, LIBID_UILogLib,0xF7715108,0x51D9,0x4CB2,0x9D,0x6D,0xA4,0xE5,0x30,0x84,0xA0,0xD5);


MIDL_DEFINE_GUID(CLSID, CLSID_Log,0x3A24D68D,0xEBB5,0x4836,0xB4,0x5A,0x3A,0x95,0x4B,0xFD,0x07,0xA1);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



