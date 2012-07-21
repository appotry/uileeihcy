

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __UILog_i_h__
#define __UILog_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ILogItem_FWD_DEFINED__
#define __ILogItem_FWD_DEFINED__
typedef interface ILogItem ILogItem;
#endif 	/* __ILogItem_FWD_DEFINED__ */


#ifndef __ILog_FWD_DEFINED__
#define __ILog_FWD_DEFINED__
typedef interface ILog ILog;
#endif 	/* __ILog_FWD_DEFINED__ */


#ifndef __Log_FWD_DEFINED__
#define __Log_FWD_DEFINED__

#ifdef __cplusplus
typedef class Log Log;
#else
typedef struct Log Log;
#endif /* __cplusplus */

#endif 	/* __Log_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ILogItem_INTERFACE_DEFINED__
#define __ILogItem_INTERFACE_DEFINED__

/* interface ILogItem */
/* [unique][helpstring][nonextensible][uuid][object] */ 


EXTERN_C const IID IID_ILogItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C63D124C-7D48-4fc8-9C52-6BF9752C1765")
    ILogItem : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSwitch( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetName( 
            /* [retval][out] */ BSTR *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPath( 
            /* [retval][out] */ BSTR *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLevel( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogFlag( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFileFlag( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILogItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILogItem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILogItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILogItem * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSwitch )( 
            ILogItem * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetName )( 
            ILogItem * This,
            /* [retval][out] */ BSTR *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPath )( 
            ILogItem * This,
            /* [retval][out] */ BSTR *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
            ILogItem * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogFlag )( 
            ILogItem * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFileFlag )( 
            ILogItem * This,
            /* [retval][out] */ LONG *pRetVal);
        
        END_INTERFACE
    } ILogItemVtbl;

    interface ILogItem
    {
        CONST_VTBL struct ILogItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILogItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILogItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILogItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILogItem_GetSwitch(This,pRetVal)	\
    ( (This)->lpVtbl -> GetSwitch(This,pRetVal) ) 

#define ILogItem_GetName(This,pRetVal)	\
    ( (This)->lpVtbl -> GetName(This,pRetVal) ) 

#define ILogItem_GetPath(This,pRetVal)	\
    ( (This)->lpVtbl -> GetPath(This,pRetVal) ) 

#define ILogItem_GetLevel(This,pRetVal)	\
    ( (This)->lpVtbl -> GetLevel(This,pRetVal) ) 

#define ILogItem_GetLogFlag(This,pRetVal)	\
    ( (This)->lpVtbl -> GetLogFlag(This,pRetVal) ) 

#define ILogItem_GetFileFlag(This,pRetVal)	\
    ( (This)->lpVtbl -> GetFileFlag(This,pRetVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILogItem_INTERFACE_DEFINED__ */


#ifndef __ILog_INTERFACE_DEFINED__
#define __ILog_INTERFACE_DEFINED__

/* interface ILog */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ILog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("47F812B2-1295-4115-92B9-283A31DF971A")
    ILog : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ BSTR strXmlPath,
            /* [retval][out] */ LONG *pIsDirty) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Load( 
            /* [in] */ BSTR strXmlPath,
            /* [retval][out] */ LONG *pIsDirty) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Log( 
            /* [in] */ BSTR bstrModule,
            /* [in] */ LONG level,
            /* [in] */ BSTR bstrContent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnableLog( 
            /* [in] */ LONG bSwitch,
            /* [retval][out] */ LONG *pIsDirty) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddLogItem( 
            /* [in] */ BSTR bstrModule,
            /* [in] */ LONG bSwitch,
            /* [in] */ LONG level,
            /* [in] */ BSTR bstrLogPath,
            /* [in] */ LONG lLogFlag,
            /* [in] */ LONG lFileLog,
            /* [retval][out] */ LONG *pIsDirty) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ModifyLogItem( 
            /* [in] */ BSTR bstrOldModuleName,
            /* [in] */ BSTR bstrNewModuleName,
            /* [in] */ LONG bSwitch,
            /* [in] */ LONG level,
            /* [in] */ BSTR bstrLogPath,
            /* [in] */ LONG lLogFlag,
            /* [in] */ LONG lFileLog,
            /* [retval][out] */ LONG *pIsDirty) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveLogItem( 
            /* [in] */ BSTR bstrModuleName,
            /* [retval][out] */ LONG *pIsDirty) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsDirty( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsEnable( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogItemCount( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogItemByIndex( 
            /* [in] */ LONG lIndex,
            /* [retval][out] */ ILogItem **ppRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogItemByName( 
            /* [in] */ BSTR bstrLogItemName,
            /* [retval][out] */ ILogItem **ppRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TestCanLog( 
            /* [in] */ BSTR bstrModule,
            /* [in] */ LONG level,
            /* [retval][out] */ LONG *lRet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILog * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILog * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILog * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ILog * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ILog * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ILog * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ILog * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Create )( 
            ILog * This,
            /* [in] */ BSTR strXmlPath,
            /* [retval][out] */ LONG *pIsDirty);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Load )( 
            ILog * This,
            /* [in] */ BSTR strXmlPath,
            /* [retval][out] */ LONG *pIsDirty);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Log )( 
            ILog * This,
            /* [in] */ BSTR bstrModule,
            /* [in] */ LONG level,
            /* [in] */ BSTR bstrContent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnableLog )( 
            ILog * This,
            /* [in] */ LONG bSwitch,
            /* [retval][out] */ LONG *pIsDirty);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddLogItem )( 
            ILog * This,
            /* [in] */ BSTR bstrModule,
            /* [in] */ LONG bSwitch,
            /* [in] */ LONG level,
            /* [in] */ BSTR bstrLogPath,
            /* [in] */ LONG lLogFlag,
            /* [in] */ LONG lFileLog,
            /* [retval][out] */ LONG *pIsDirty);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ModifyLogItem )( 
            ILog * This,
            /* [in] */ BSTR bstrOldModuleName,
            /* [in] */ BSTR bstrNewModuleName,
            /* [in] */ LONG bSwitch,
            /* [in] */ LONG level,
            /* [in] */ BSTR bstrLogPath,
            /* [in] */ LONG lLogFlag,
            /* [in] */ LONG lFileLog,
            /* [retval][out] */ LONG *pIsDirty);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveLogItem )( 
            ILog * This,
            /* [in] */ BSTR bstrModuleName,
            /* [retval][out] */ LONG *pIsDirty);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            ILog * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsDirty )( 
            ILog * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsEnable )( 
            ILog * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogItemCount )( 
            ILog * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogItemByIndex )( 
            ILog * This,
            /* [in] */ LONG lIndex,
            /* [retval][out] */ ILogItem **ppRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogItemByName )( 
            ILog * This,
            /* [in] */ BSTR bstrLogItemName,
            /* [retval][out] */ ILogItem **ppRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TestCanLog )( 
            ILog * This,
            /* [in] */ BSTR bstrModule,
            /* [in] */ LONG level,
            /* [retval][out] */ LONG *lRet);
        
        END_INTERFACE
    } ILogVtbl;

    interface ILog
    {
        CONST_VTBL struct ILogVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILog_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILog_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILog_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILog_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ILog_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ILog_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ILog_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ILog_Create(This,strXmlPath,pIsDirty)	\
    ( (This)->lpVtbl -> Create(This,strXmlPath,pIsDirty) ) 

#define ILog_Load(This,strXmlPath,pIsDirty)	\
    ( (This)->lpVtbl -> Load(This,strXmlPath,pIsDirty) ) 

#define ILog_Log(This,bstrModule,level,bstrContent)	\
    ( (This)->lpVtbl -> Log(This,bstrModule,level,bstrContent) ) 

#define ILog_EnableLog(This,bSwitch,pIsDirty)	\
    ( (This)->lpVtbl -> EnableLog(This,bSwitch,pIsDirty) ) 

#define ILog_AddLogItem(This,bstrModule,bSwitch,level,bstrLogPath,lLogFlag,lFileLog,pIsDirty)	\
    ( (This)->lpVtbl -> AddLogItem(This,bstrModule,bSwitch,level,bstrLogPath,lLogFlag,lFileLog,pIsDirty) ) 

#define ILog_ModifyLogItem(This,bstrOldModuleName,bstrNewModuleName,bSwitch,level,bstrLogPath,lLogFlag,lFileLog,pIsDirty)	\
    ( (This)->lpVtbl -> ModifyLogItem(This,bstrOldModuleName,bstrNewModuleName,bSwitch,level,bstrLogPath,lLogFlag,lFileLog,pIsDirty) ) 

#define ILog_RemoveLogItem(This,bstrModuleName,pIsDirty)	\
    ( (This)->lpVtbl -> RemoveLogItem(This,bstrModuleName,pIsDirty) ) 

#define ILog_Save(This,pRetVal)	\
    ( (This)->lpVtbl -> Save(This,pRetVal) ) 

#define ILog_IsDirty(This,pRetVal)	\
    ( (This)->lpVtbl -> IsDirty(This,pRetVal) ) 

#define ILog_IsEnable(This,pRetVal)	\
    ( (This)->lpVtbl -> IsEnable(This,pRetVal) ) 

#define ILog_GetLogItemCount(This,pRetVal)	\
    ( (This)->lpVtbl -> GetLogItemCount(This,pRetVal) ) 

#define ILog_GetLogItemByIndex(This,lIndex,ppRetVal)	\
    ( (This)->lpVtbl -> GetLogItemByIndex(This,lIndex,ppRetVal) ) 

#define ILog_GetLogItemByName(This,bstrLogItemName,ppRetVal)	\
    ( (This)->lpVtbl -> GetLogItemByName(This,bstrLogItemName,ppRetVal) ) 

#define ILog_TestCanLog(This,bstrModule,level,lRet)	\
    ( (This)->lpVtbl -> TestCanLog(This,bstrModule,level,lRet) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILog_INTERFACE_DEFINED__ */



#ifndef __UILogLib_LIBRARY_DEFINED__
#define __UILogLib_LIBRARY_DEFINED__

/* library UILogLib */
/* [helpstring][version][uuid] */ 

typedef 
enum eLogLevel
    {	LOG_LEVEL_DEBUG	= 0,
	LOG_LEVEL_INFO	= 1,
	LOG_LEVEL_WARN	= 2,
	LOG_LEVEL_ERROR	= 3,
	LOG_LEVEL_FATAL	= 4
    } 	LOG_LEVEL;

typedef 
enum eLogFlag
    {	LOG_FLAG_THREADID	= 0x1,
	LOG_FLAG_CONSOLE	= 0x2,
	LOG_FALG_DBGVIEW	= 0x4
    } 	LOG_FLAG;

typedef 
enum eLogFileFlag
    {	LOG_FLAG_FILE_DISABLE	= 0x1,
	LOG_FLAG_FILE_SIMPLE	= 0x2,
	LOG_FLAG_FILE_TIME	= 0x4,
	LOG_FLAG_FILE_LOOP	= 0x8
    } 	LOG_FILE_FLAG;


EXTERN_C const IID LIBID_UILogLib;

EXTERN_C const CLSID CLSID_Log;

#ifdef __cplusplus

class DECLSPEC_UUID("3A24D68D-EBB5-4836-B45A-3A954BFD07A1")
Log;
#endif
#endif /* __UILogLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


