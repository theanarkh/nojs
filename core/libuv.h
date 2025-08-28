#ifndef LIBUV_H
#define LIBUV_H

#include "common.h"
#include "util.h"
#include "env.h"
#include "external_reference.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;

namespace No {
    namespace UV {
        struct UVError {
            int value;
            const char* name;
            const char* message;
        };

        static const struct UVError uv_errors_map[] = {
        #define V(name, message) {UV_##name, #name, message},
            UV_ERRNO_MAP(V)
        #undef V
        };

        void Init(Isolate* isolate, Local<Object> target);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
    }
}


#endif 