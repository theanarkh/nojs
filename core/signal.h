#ifndef SIGNAL_H
#define SIGNAL_H

#include <map> 
#include "util.h"
#include "common.h"
#include "env.h"
#include "handle.h"

using namespace v8;
using namespace No::Util;
using namespace No::Handle;
using namespace No::Env;

namespace No {
    namespace Signal {
        class SignalWrap: public HandleWrap {
             public:
                SignalWrap(No::Env::Environment *env, Local<Object> obj): HandleWrap(env, obj, reinterpret_cast<uv_handle_t*>(&handle_)){
                    uv_signal_init(env->loop(), &handle_);
                }
               
                static void New(V8_ARGS);
                static void Start(V8_ARGS);
                static void Stop(V8_ARGS);
            private:
                static void OnSignal(uv_signal_t* handle, int signum);
            uv_signal_t  handle_;
        };
        void Init(Isolate* isolate, Local<Object> target);
        void Kill(V8_ARGS);
    }
}

#endif