#ifndef UDP_H
#define UDP_H

#include "util.h"
#include "common.h"
#include "env.h"
#include "req.h"
#include "async.h"

using namespace No::Env;
using namespace v8;
using namespace No::Util;
using namespace No::Async;

namespace No {
    namespace UDP {
       
        void Init(Isolate* isolate, Local<Object> target);

        class UDPWrap: public AsyncWrap {
             public:
                UDPWrap(No::Env::Environment *env, Local<Object> obj): AsyncWrap(env, obj){
                    uv_udp_init(env->loop(), &handle_);
                    handle_.data = this;
                }
                static void New(V8_ARGS);
                static void Bind(V8_ARGS);
                static void Bind6(V8_ARGS);
                static void Connect(V8_ARGS);
                static void Connect6(V8_ARGS);
                static void DisConnect(V8_ARGS);
                static void ReadStart(V8_ARGS);
                static void ReadStop(V8_ARGS);
            private:
                static void DoConnect(const FunctionCallbackInfo<Value>& args, int family);
                static void DoBind(const FunctionCallbackInfo<Value>& args, int family);
            uv_udp_t  handle_;
        };
    }
}
#endif 
