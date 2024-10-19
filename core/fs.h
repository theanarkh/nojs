#ifndef FS_H
#define FS_H

#include "req.h"
#include "util.h"
#include "common.h"

#include "env.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;

namespace No {
    namespace FS {
        void Open(V8_ARGS);
        void ReadSync(V8_ARGS);
        void Read(V8_ARGS);
        void WriteSync(V8_ARGS);
        void Init(Isolate* isolate, Local<Object> target);
        class FSReqCallback : public ReqCallback<uv_fs_t> {
            public:
            FSReqCallback(No::Env::Environment *env, Local<Object> obj): ReqCallback<uv_fs_t> (env, obj) {}
        };
    }
}

#endif
