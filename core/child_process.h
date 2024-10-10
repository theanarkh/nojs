#ifndef CHILD_PROCESS_H
#define CHILD_PROCESS_H

#include "util.h"
#include "common.h"
#include "env.h"
#include "handle.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Handle;

namespace No {
    namespace ChildProcess {
        void Init(Isolate* isolate, Local<Object> target);
        class ProcessWrap: public HandleWrap {
            public:
                ProcessWrap(Environment *env, Local<Object> obj): HandleWrap(env, obj, reinterpret_cast<uv_handle_t*>(&_handle)) {
                    _handle.data = this;
                }
                DEFIND_JS_FUNC(New);
                DEFIND_JS_FUNC(Spawn);
                DEFIND_JS_FUNC(Kill);
                static void OnExit(uv_process_t* handle,
                                   int64_t exit_status,
                                   int term_signal);
            private:
                uv_process_t _handle;
        };

         class SyncProcessWrap {
            public:
                SyncProcessWrap() {
                    uv_loop_init(&_loop);
                    _exit_status = 0;
                    _term_signal = 0;
                    _handle.data = this;
                }
                static void SpawnSync(V8_ARGS);
                static void OnSpawnSyncExit(uv_process_t* handle,
                            int64_t exit_status,
                            int term_signal);
            private:
                int64_t _exit_status;
                int _term_signal;
                uv_process_t _handle;
                uv_loop_t _loop;
        };
    }
}

#endif 