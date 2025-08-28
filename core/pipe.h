#ifndef PIPE_H
#define PIPE_H

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
    namespace Pipe {
       
        void Init(Isolate* isolate, Local<Object> target);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
        class PipeWrap: public HandleWrap {
             public:
                PipeWrap(No::Env::Environment *env, Local<Object> obj, int ipc): HandleWrap(env, obj, reinterpret_cast<uv_handle_t*>(&handle_)){
                    uv_pipe_init(env->loop(), &handle_, ipc);
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
