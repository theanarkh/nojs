#ifndef DNS_H
#define DNS_H

#include "req.h"
#include "util.h"
#include "common.h"
#include "env.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;

namespace No {
    namespace DNS {
        void Init(Isolate* isolate, Local<Object> target);
        void Lookup(V8_ARGS);
        class LookupReq : public ReqCallback<uv_getaddrinfo_t> {
            public:
            LookupReq(No::Env::Environment *env, Local<Object> obj): ReqCallback<uv_getaddrinfo_t> (env, obj) {}
        };
    }
}

#endif
