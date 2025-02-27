

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for DatabaseInterfaces.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0628 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __DatabaseInterfaces_h__
#define __DatabaseInterfaces_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __INativeDatabaseAccess_FWD_DEFINED__
#define __INativeDatabaseAccess_FWD_DEFINED__
typedef interface INativeDatabaseAccess INativeDatabaseAccess;

#endif 	/* __INativeDatabaseAccess_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __DatabaseInterfaces_LIBRARY_DEFINED__
#define __DatabaseInterfaces_LIBRARY_DEFINED__

/* library DatabaseInterfaces */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_DatabaseInterfaces;

#ifndef __INativeDatabaseAccess_INTERFACE_DEFINED__
#define __INativeDatabaseAccess_INTERFACE_DEFINED__

/* interface INativeDatabaseAccess */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_INativeDatabaseAccess;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("55c7ffa0-894d-4a5b-9fa9-166fe9e4a355")
    INativeDatabaseAccess : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OpenConnection( 
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *success) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CloseConnection( 
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *success) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetConnectionStatus( 
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *isConnected) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ExecuteQuery( 
            /* [in] */ BSTR connectionString,
            /* [in] */ BSTR query,
            /* [retval][out] */ BSTR *pVarResult) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ExecuteNonQuery( 
            /* [in] */ BSTR connectionString,
            /* [in] */ BSTR query,
            /* [retval][out] */ LONG *rowsAffected) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE BeginTransaction( 
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *success) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CommitTransaction( 
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *success) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RollbackTransaction( 
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *success) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreateParameterizedQuery( 
            /* [in] */ BSTR query,
            /* [retval][out] */ BSTR *queryId) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AddParameter( 
            /* [in] */ BSTR queryId,
            /* [in] */ BSTR paramName,
            /* [in] */ VARIANT paramValue) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ExecuteParameterizedQuery( 
            /* [in] */ BSTR connectionString,
            /* [in] */ BSTR queryId,
            /* [retval][out] */ BSTR *pVarResult) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ExecuteParameterizedNonQuery( 
            /* [in] */ BSTR connectionString,
            /* [in] */ BSTR queryId,
            /* [retval][out] */ LONG *rowsAffected) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct INativeDatabaseAccessVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INativeDatabaseAccess * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INativeDatabaseAccess * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INativeDatabaseAccess * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INativeDatabaseAccess * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INativeDatabaseAccess * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INativeDatabaseAccess * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INativeDatabaseAccess * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, OpenConnection)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *OpenConnection )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *success);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, CloseConnection)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CloseConnection )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *success);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, GetConnectionStatus)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetConnectionStatus )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *isConnected);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, ExecuteQuery)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ExecuteQuery )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR connectionString,
            /* [in] */ BSTR query,
            /* [retval][out] */ BSTR *pVarResult);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, ExecuteNonQuery)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ExecuteNonQuery )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR connectionString,
            /* [in] */ BSTR query,
            /* [retval][out] */ LONG *rowsAffected);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, BeginTransaction)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *BeginTransaction )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *success);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, CommitTransaction)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CommitTransaction )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *success);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, RollbackTransaction)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RollbackTransaction )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR connectionString,
            /* [retval][out] */ BOOL *success);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, CreateParameterizedQuery)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateParameterizedQuery )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR query,
            /* [retval][out] */ BSTR *queryId);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, AddParameter)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AddParameter )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR queryId,
            /* [in] */ BSTR paramName,
            /* [in] */ VARIANT paramValue);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, ExecuteParameterizedQuery)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ExecuteParameterizedQuery )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR connectionString,
            /* [in] */ BSTR queryId,
            /* [retval][out] */ BSTR *pVarResult);
        
        DECLSPEC_XFGVIRT(INativeDatabaseAccess, ExecuteParameterizedNonQuery)
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ExecuteParameterizedNonQuery )( 
            INativeDatabaseAccess * This,
            /* [in] */ BSTR connectionString,
            /* [in] */ BSTR queryId,
            /* [retval][out] */ LONG *rowsAffected);
        
        END_INTERFACE
    } INativeDatabaseAccessVtbl;

    interface INativeDatabaseAccess
    {
        CONST_VTBL struct INativeDatabaseAccessVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INativeDatabaseAccess_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INativeDatabaseAccess_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INativeDatabaseAccess_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INativeDatabaseAccess_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INativeDatabaseAccess_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INativeDatabaseAccess_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INativeDatabaseAccess_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INativeDatabaseAccess_OpenConnection(This,connectionString,success)	\
    ( (This)->lpVtbl -> OpenConnection(This,connectionString,success) ) 

#define INativeDatabaseAccess_CloseConnection(This,connectionString,success)	\
    ( (This)->lpVtbl -> CloseConnection(This,connectionString,success) ) 

#define INativeDatabaseAccess_GetConnectionStatus(This,connectionString,isConnected)	\
    ( (This)->lpVtbl -> GetConnectionStatus(This,connectionString,isConnected) ) 

#define INativeDatabaseAccess_ExecuteQuery(This,connectionString,query,pVarResult)	\
    ( (This)->lpVtbl -> ExecuteQuery(This,connectionString,query,pVarResult) ) 

#define INativeDatabaseAccess_ExecuteNonQuery(This,connectionString,query,rowsAffected)	\
    ( (This)->lpVtbl -> ExecuteNonQuery(This,connectionString,query,rowsAffected) ) 

#define INativeDatabaseAccess_BeginTransaction(This,connectionString,success)	\
    ( (This)->lpVtbl -> BeginTransaction(This,connectionString,success) ) 

#define INativeDatabaseAccess_CommitTransaction(This,connectionString,success)	\
    ( (This)->lpVtbl -> CommitTransaction(This,connectionString,success) ) 

#define INativeDatabaseAccess_RollbackTransaction(This,connectionString,success)	\
    ( (This)->lpVtbl -> RollbackTransaction(This,connectionString,success) ) 

#define INativeDatabaseAccess_CreateParameterizedQuery(This,query,queryId)	\
    ( (This)->lpVtbl -> CreateParameterizedQuery(This,query,queryId) ) 

#define INativeDatabaseAccess_AddParameter(This,queryId,paramName,paramValue)	\
    ( (This)->lpVtbl -> AddParameter(This,queryId,paramName,paramValue) ) 

#define INativeDatabaseAccess_ExecuteParameterizedQuery(This,connectionString,queryId,pVarResult)	\
    ( (This)->lpVtbl -> ExecuteParameterizedQuery(This,connectionString,queryId,pVarResult) ) 

#define INativeDatabaseAccess_ExecuteParameterizedNonQuery(This,connectionString,queryId,rowsAffected)	\
    ( (This)->lpVtbl -> ExecuteParameterizedNonQuery(This,connectionString,queryId,rowsAffected) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INativeDatabaseAccess_INTERFACE_DEFINED__ */

#endif /* __DatabaseInterfaces_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


