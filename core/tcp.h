#ifndef TCP_H
#define TCP_H

#include "util.h"
#include "common.h"
#include "env.h"
#include "handle.h"
#include "req.h"
#include "external_reference.h"

using namespace No::Env;
using namespace v8;
using namespace No::Util;
using namespace No::Handle;

namespace No {
    namespace TCP {
       
        void Init(Isolate* isolate, Local<Object> target);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
        class TCPWrap: public HandleWrap {
             public:
                TCPWrap(No::Env::Environment *env, Local<Object> obj): HandleWrap(env, obj, reinterpret_cast<uv_handle_t*>(&handle_)){
                    uv_tcp_init(env->loop(), &handle_);
                }
                static void New(V8_ARGS);
                template <typename T>
                static void DoBind(const FunctionCallbackInfo<Value>& args, int family, std::function<int(const char* ip_address, int port, T* addr)> uv_ip_addr);
                static void Bind(V8_ARGS);
                static void Bind6(V8_ARGS);
                template <typename T>
                static void DoConnect(const FunctionCallbackInfo<Value>& args,
                    std::function<int(const char* ip_address, T* addr)> uv_ip_addr);
                static void Connect(V8_ARGS);
                static void Connect6(V8_ARGS);
                static void Listen(V8_ARGS);
                static void Accept(V8_ARGS);
                static void ReadStart(V8_ARGS);
                static void ReadStop(V8_ARGS);
                static void Write(V8_ARGS);
                static void Open(V8_ARGS);
            private:
            uv_tcp_t  handle_;
        };

        class ConnectWrap : public ReqCallback<uv_connect_t> {
            public:
            ConnectWrap(No::Env::Environment* env,
                        v8::Local<v8::Object> req_wrap_obj): ReqCallback<uv_connect_t> (env, req_wrap_obj) {};

            };

        class WriteReq : public ReqCallback<uv_write_t> {
            public:
            WriteReq(No::Env::Environment *env, Local<Object> obj): ReqCallback<uv_write_t> (env, obj) {}
        };
    }
}
#endif 
