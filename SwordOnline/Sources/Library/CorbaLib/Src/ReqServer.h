// This file is generated by omniidl (C++ backend)- omniORB_3_0. Do not edit.
#ifndef __ReqServer_hh__
#define __ReqServer_hh__

#ifndef USE_omniORB_logStream
#define USE_omniORB_logStream
#endif

#ifndef __CORBA_H_EXTERNAL_GUARD__
#include <omniORB3/CORBA.h>
#endif

#ifndef  USE_core_stub_in_nt_dll
# define USE_core_stub_in_nt_dll_NOT_DEFINED_ReqServer
#endif
#ifndef  USE_dyn_stub_in_nt_dll
# define USE_dyn_stub_in_nt_dll_NOT_DEFINED_ReqServer
#endif

#ifdef USE_stub_in_nt_dll
#ifndef USE_core_stub_in_nt_dll
#define USE_core_stub_in_nt_dll
#endif
#ifndef USE_dyn_stub_in_nt_dll
#define USE_dyn_stub_in_nt_dll
#endif
#endif

#ifdef _core_attr
# error "A local CPP macro _core_attr has already been defined."
#else
# ifdef  USE_core_stub_in_nt_dll
#  define _core_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _core_attr
# endif
#endif

#ifdef _dyn_attr
# error "A local CPP macro _dyn_attr has already been defined."
#else
# ifdef  USE_dyn_stub_in_nt_dll
#  define _dyn_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _dyn_attr
# endif
#endif

#ifndef __ReqServer__
#define __ReqServer__

class ReqServer;
class _objref_ReqServer;
class _impl_ReqServer;

typedef _objref_ReqServer* ReqServer_ptr;
typedef ReqServer_ptr ReqServerRef;

class ReqServer_Helper {
public:
  typedef ReqServer_ptr _ptr_type;

  static _ptr_type _nil();
  static _CORBA_Boolean is_nil(_ptr_type);
  static void release(_ptr_type);
  static void duplicate(_ptr_type);
  static size_t NP_alignedSize(_ptr_type, size_t);
  static void marshalObjRef(_ptr_type, NetBufferedStream&);
  static _ptr_type unmarshalObjRef(NetBufferedStream&);
  static void marshalObjRef(_ptr_type, MemBufferedStream&);
  static _ptr_type unmarshalObjRef(MemBufferedStream&);
};

typedef _CORBA_ObjRef_Var<_objref_ReqServer, ReqServer_Helper> ReqServer_var;
typedef _CORBA_ObjRef_OUT_arg<_objref_ReqServer,ReqServer_Helper > ReqServer_out;

#endif

class ReqServer {
public:
  // Declarations for this interface type.
  typedef ReqServer_ptr _ptr_type;
  typedef ReqServer_var _var_type;

  static _ptr_type _duplicate(_ptr_type);
  static _ptr_type _narrow(CORBA::Object_ptr);
  static _ptr_type _nil();

  static inline size_t _alignedSize(_ptr_type, size_t);
  static inline void _marshalObjRef(_ptr_type, NetBufferedStream&);
  static inline void _marshalObjRef(_ptr_type, MemBufferedStream&);

  static inline _ptr_type _unmarshalObjRef(NetBufferedStream& s) {
    CORBA::Object_ptr obj = CORBA::UnMarshalObjRef(_PD_repoId, s);
    _ptr_type result = _narrow(obj);
    CORBA::release(obj);
    return result;
  }

  static inline _ptr_type _unmarshalObjRef(MemBufferedStream& s) {
    CORBA::Object_ptr obj = CORBA::UnMarshalObjRef(_PD_repoId, s);
    _ptr_type result = _narrow(obj);
    CORBA::release(obj);
    return result;
  }

  static _core_attr const char* _PD_repoId;

  // Other IDL defined within this scope.
  
};

class _objref_ReqServer :
  public virtual CORBA::Object, public virtual omniObjRef
{
public:
  void SendResult(CORBA::Long id, CORBA::Long index, CORBA::Long reserve);
  char* GetServerInfo();
  
  inline _objref_ReqServer() { _PR_setobj(0); }  // nil
  _objref_ReqServer(const char*, IOP::TaggedProfileList*, omniIdentity*, omniLocalIdentity*);

protected:
  virtual ~_objref_ReqServer();

private:
  virtual void* _ptrToObjRef(const char*);

  _objref_ReqServer(const _objref_ReqServer&);
  _objref_ReqServer& operator = (const _objref_ReqServer&);
  // not implemented
};

class _pof_ReqServer : public proxyObjectFactory {
public:
  inline _pof_ReqServer() : proxyObjectFactory(ReqServer::_PD_repoId) {}
  virtual ~_pof_ReqServer();

  virtual omniObjRef* newObjRef(const char*, IOP::TaggedProfileList*,
                                omniIdentity*, omniLocalIdentity*);
  virtual _CORBA_Boolean is_a(const char*) const;
};

class _impl_ReqServer :
  public virtual omniServant
{
public:
  virtual ~_impl_ReqServer();

  virtual void SendResult(CORBA::Long id, CORBA::Long index, CORBA::Long reserve) = 0;
  virtual char* GetServerInfo() = 0;
  
public:  // Really protected, workaround for xlC
  virtual _CORBA_Boolean _dispatch(GIOP_S&);

private:
  virtual void* _ptrToInterface(const char*);
  virtual const char* _mostDerivedRepoId();
};

_CORBA_GLOBAL_VAR _dyn_attr const CORBA::TypeCode_ptr _tc_ReqServer;

class POA_ReqServer :
  public virtual _impl_ReqServer,
  public virtual PortableServer::ServantBase
{
public:
  virtual ~POA_ReqServer();

  inline ReqServer_ptr _this() {
    return (ReqServer_ptr) _do_this(ReqServer::_PD_repoId);
  }
};

#undef _core_attr
#undef _dyn_attr

void operator<<=(CORBA::Any& _a, ReqServer_ptr _s);
void operator<<=(CORBA::Any& _a, ReqServer_ptr* _s);
CORBA::Boolean operator>>=(const CORBA::Any& _a, ReqServer_ptr& _s);

inline size_t
ReqServer::_alignedSize(ReqServer_ptr obj, size_t offset) {
  return CORBA::AlignedObjRef(obj, _PD_repoId, 18, offset);
}

inline void
ReqServer::_marshalObjRef(ReqServer_ptr obj, NetBufferedStream& s) {
  CORBA::MarshalObjRef(obj, _PD_repoId, 18, s);
}

inline void
ReqServer::_marshalObjRef(ReqServer_ptr obj, MemBufferedStream& s) {
  CORBA::MarshalObjRef(obj, _PD_repoId, 18, s);
}

#ifdef   USE_core_stub_in_nt_dll_NOT_DEFINED_ReqServer
# undef  USE_core_stub_in_nt_dll
# undef  USE_core_stub_in_nt_dll_NOT_DEFINED_ReqServer
#endif
#ifdef   USE_dyn_stub_in_nt_dll_NOT_DEFINED_ReqServer
# undef  USE_dyn_stub_in_nt_dll
# undef  USE_dyn_stub_in_nt_dll_NOT_DEFINED_ReqServer
#endif

#endif  // __ReqServer_hh__

