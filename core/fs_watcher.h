#ifndef FS_WATCHER_H
#define FS_WATCHER_H

#include "common.h"
#include "env.h"
#include "async.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Async;

namespace No {
    namespace FSWatcher {
        void Init(Isolate* isolate, Local<Object> target);
        class Watcher: public AsyncWrap {
            public:
            Watcher(No::Env::Environment *env, Local<Object> obj): 
                AsyncWrap(env, obj) {
                uv_fs_event_init(env->loop(), &handle_);
                handle_.data = this;
            }
            static void New(const FunctionCallbackInfo<Value>& args);
            static void Start(const FunctionCallbackInfo<Value>& args);
            static void OnChange(uv_fs_event_t* handle, const char* filename, int events, int status);
            private:
            uv_fs_event_t handle_;
        };
    }
}

#endif
