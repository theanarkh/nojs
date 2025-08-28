#ifndef FS_WATCHER_H
#define FS_WATCHER_H

#include "common.h"
#include "env.h"
#include "handle.h"
#include "external_reference.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Handle;

namespace No {
    namespace FSWatcher {
        void Init(Isolate* isolate, Local<Object> target);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
        class Watcher: public HandleWrap {
            public:
            Watcher(No::Env::Environment *env, Local<Object> obj): 
                HandleWrap(env, obj, reinterpret_cast<uv_handle_t*>(&handle_)) {
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
