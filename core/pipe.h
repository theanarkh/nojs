#ifndef PIPE_H
#define PIPE_H

#include "util.h"
#include "common.h"
#include "env.h"
#include "async.h"
#include "req.h"

using namespace No::Env;
using namespace v8;
using namespace No::Util;
using namespace No::Async;

namespace No {
    namespace Pipe {
       
        void Init(Isolate* isolate, Local<Object> target);

        class PipeWrap: public AsyncWrap {
             public:
                PipeWrap(No::Env::Environment *env, Local<Object> obj, int ipc): AsyncWrap(env, obj){
                    uv_pipe_init(env->loop(), &handle_, ipc);
                    handle_.data = this;
                }
                static void New(V8_ARGS);
                static void Fchmod(V8_ARGS);
                static void Bind(V8_ARGS);
                static void Connect(V8_ARGS);
                static void Listen(V8_ARGS);
                static void Accept(V8_ARGS);
                static void ReadStart(V8_ARGS);
                static void ReadStop(V8_ARGS);
            private:
            uv_pipe_t  handle_;
        };

        class PipeConnectWrap : public ReqCallback<uv_connect_t> {
            public:
            PipeConnectWrap(No::Env::Environment* env,
                        v8::Local<v8::Object> req_wrap_obj): ReqCallback<uv_connect_t> (env, req_wrap_obj) {};

            };
    }
}
#endif 
