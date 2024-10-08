#ifndef TIMER_H
#define TIMER_H

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
    namespace Timer {
        void Init(Isolate* isolate, Local<Object> target);
        class TimerWrap: public AsyncWrap {
             public:
                TimerWrap(No::Env::Environment *env, Local<Object> obj): AsyncWrap(env, obj){
                    uv_timer_init(env->loop(), &handle_);
                    handle_.data = this;
                }
                static void New(V8_ARGS);
                static void Start(V8_ARGS);
                static void Stop(V8_ARGS);
            private:
            uv_timer_t  handle_;
        };
    }
}
#endif 
