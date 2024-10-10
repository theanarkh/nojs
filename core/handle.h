#ifndef HANDLE_H
#define HANDLE_H

#include "util.h"
#include "common.h"
#include "env.h"
#include "async.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Base;
using namespace No::Async;

namespace No {
    namespace Handle {
        class HandleWrap : public AsyncWrap {
            public:
                HandleWrap(Environment* env, Local<Object> object, uv_handle_t *handle);
                static void Ref(V8_ARGS);
                static void Unref(V8_ARGS);
                static void HasRef(V8_ARGS);
                static void Close(V8_ARGS); 
                static v8::Local<FunctionTemplate> GetConstructorTemplate(Environment* env);
                void Close(Local<Value> close_cb);
                static void OnClose(uv_handle_t* handle);
                uv_handle_t * handle() {
                    return handle_;
                }
            private:
                uv_handle_t *handle_;
        };
        
    }
}

#endif