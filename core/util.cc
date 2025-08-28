
#include "util.h"
#include "env.h"
#include "base_object.h"
 
void No::Util::Log(const char * str) {
    write(STDOUT_FILENO, str, strlen(str));
}

Local<String> No::Util::NewString(Isolate * isolate, const char * str, NewStringType type) {
    return String::NewFromUtf8(isolate, str, type, strlen(str)).ToLocalChecked();
}

// 设置对象的属性，属性为函数
void No::Util::setMethod(Isolate *isolate, 
                Local<ObjectTemplate> 
                recv, const char *name, 
                FunctionCallback callback) {
    recv->Set(String::NewFromUtf8(isolate, name, 
        NewStringType::kNormal).ToLocalChecked(), 
        FunctionTemplate::New(isolate, callback));
}

void No::Util::SetProtoMethod(v8::Isolate* isolate,
                    Local<v8::FunctionTemplate> that,
                    const char* name,
                    v8::FunctionCallback callback) {
  Local<v8::FunctionTemplate> t =
      NewFunctionTemplate(isolate,
                          callback);
  // kInternalized strings are created in the old space.
  const v8::NewStringType type = v8::NewStringType::kInternalized;
  Local<v8::String> name_string =
      v8::String::NewFromUtf8(isolate, name, type, strlen(name))
          .ToLocalChecked();
  that->PrototypeTemplate()->Set(name_string, t);

  t->SetClassName(name_string); 
}

// 设置对象的属性，属性为非函数
void No::Util::setObjectTemplateValue(Isolate *isolate, 
                Local<ObjectTemplate> 
                recv, const char *name, 
                // TEMPLATE OR PRIVATE
                Local<Data> value) {
    recv->Set(String::NewFromUtf8(isolate, name, 
        NewStringType::kNormal).ToLocalChecked(), 
        value);
}

// 设置对象的属性，属性为非函数
void No::Util::ObjectSet(Isolate *isolate, 
                Local<Object> recv, const char *name, 
                Local<Value> value) {
    recv->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, name, 
        NewStringType::kNormal).ToLocalChecked(), 
        value).Check();
}

void No::Util::Init(Isolate* isolate, Local<Object> target) {}

static void DefaultFunctionCallback(const FunctionCallbackInfo<Value>& args) {
    No::Env::Environment* env = No::Env::Environment::GetCurrent(args);
    args.This()->SetAlignedPointerInInternalField(No::Base::BaseObject::Slot, nullptr);
}

Local<v8::FunctionTemplate>  No::Util::NewDefaultFunctionTemplate(v8::Isolate* isolate) {
    return NewFunctionTemplate(isolate, DefaultFunctionCallback);
}

Local<v8::FunctionTemplate>  No::Util::NewFunctionTemplate(
    v8::Isolate* isolate, v8::FunctionCallback callback) {
  Local<FunctionTemplate> t = v8::FunctionTemplate::New(isolate,
                                  callback,
                                   Local<v8::Value>(),
                                   v8::Local<v8::Signature>(),
                                   0,
                                    v8::ConstructorBehavior::kAllow,
                                    v8::SideEffectType::kHasSideEffect,
                                    nullptr);
  t->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
  return t;
}

void No::Util::SetFunction(Local<Context> context,
                            Local<Object> that,
                            Local<String> name,
                            Local<FunctionTemplate> tmpl) {
  tmpl->SetClassName(name);
  that->Set(context, name, tmpl->GetFunction(context).ToLocalChecked()).Check();
}

// Copy from Node.js src/api/utils.cc
const char* No::Util::SignoTostring(int signo) {
#define SIGNO_CASE(e)                                                          \
  case e:                                                                      \
    return #e;
  switch (signo) {
#ifdef SIGHUP
    SIGNO_CASE(SIGHUP);
#endif

#ifdef SIGINT
    SIGNO_CASE(SIGINT);
#endif

#ifdef SIGQUIT
    SIGNO_CASE(SIGQUIT);
#endif

#ifdef SIGILL
    SIGNO_CASE(SIGILL);
#endif

#ifdef SIGTRAP
    SIGNO_CASE(SIGTRAP);
#endif

#ifdef SIGABRT
    SIGNO_CASE(SIGABRT);
#endif

#ifdef SIGIOT
#if SIGABRT != SIGIOT
    SIGNO_CASE(SIGIOT);
#endif
#endif

#ifdef SIGBUS
    SIGNO_CASE(SIGBUS);
#endif

#ifdef SIGFPE
    SIGNO_CASE(SIGFPE);
#endif

#ifdef SIGKILL
    SIGNO_CASE(SIGKILL);
#endif

#ifdef SIGUSR1
    SIGNO_CASE(SIGUSR1);
#endif

#ifdef SIGSEGV
    SIGNO_CASE(SIGSEGV);
#endif

#ifdef SIGUSR2
    SIGNO_CASE(SIGUSR2);
#endif

#ifdef SIGPIPE
    SIGNO_CASE(SIGPIPE);
#endif

#ifdef SIGALRM
    SIGNO_CASE(SIGALRM);
#endif

    SIGNO_CASE(SIGTERM);

#ifdef SIGCHLD
    SIGNO_CASE(SIGCHLD);
#endif

#ifdef SIGSTKFLT
    SIGNO_CASE(SIGSTKFLT);
#endif

#ifdef SIGCONT
    SIGNO_CASE(SIGCONT);
#endif

#ifdef SIGSTOP
    SIGNO_CASE(SIGSTOP);
#endif

#ifdef SIGTSTP
    SIGNO_CASE(SIGTSTP);
#endif

#ifdef SIGBREAK
    SIGNO_CASE(SIGBREAK);
#endif

#ifdef SIGTTIN
    SIGNO_CASE(SIGTTIN);
#endif

#ifdef SIGTTOU
    SIGNO_CASE(SIGTTOU);
#endif

#ifdef SIGURG
    SIGNO_CASE(SIGURG);
#endif

#ifdef SIGXCPU
    SIGNO_CASE(SIGXCPU);
#endif

#ifdef SIGXFSZ
    SIGNO_CASE(SIGXFSZ);
#endif

#ifdef SIGVTALRM
    SIGNO_CASE(SIGVTALRM);
#endif

#ifdef SIGPROF
    SIGNO_CASE(SIGPROF);
#endif

#ifdef SIGWINCH
    SIGNO_CASE(SIGWINCH);
#endif

#ifdef SIGIO
    SIGNO_CASE(SIGIO);
#endif

#ifdef SIGPOLL
#if SIGPOLL != SIGIO
    SIGNO_CASE(SIGPOLL);
#endif
#endif

#ifdef SIGLOST
#if SIGLOST != SIGABRT
    SIGNO_CASE(SIGLOST);
#endif
#endif

#ifdef SIGPWR
#if SIGPWR != SIGLOST
    SIGNO_CASE(SIGPWR);
#endif
#endif

#ifdef SIGINFO
#if !defined(SIGPWR) || SIGINFO != SIGPWR
    SIGNO_CASE(SIGINFO);
#endif
#endif

#ifdef SIGSYS
    SIGNO_CASE(SIGSYS);
#endif

    default:
      return "";
  }
}

int No::Util::SockaddrForfamily(int address_family,
                     const char* address,
                     const unsigned short port,
                     struct sockaddr_storage* addr) {
    switch (address_family) {
        case AF_INET:
        return uv_ip4_addr(address, port, reinterpret_cast<sockaddr_in*>(addr));
        case AF_INET6:
        return uv_ip6_addr(address, port, reinterpret_cast<sockaddr_in6*>(addr));
        default: return -1;
    }
}

static void No::Util::RegisterExternalReferences(ExternalReferenceRegistry* registry) {
    registry->Register(DefaultFunctionCallback);
}

NODE_BINDING_EXTERNAL_REFERENCE(util, No::Util::RegisterExternalReferences)
