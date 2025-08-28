#ifndef OS_H
#define OS_H

#include "util.h"
#include "common.h"
#include "env.h"
#include <unistd.h>
#include "external_reference.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;

namespace No {
    namespace OS {
        void Init(Isolate* isolate, Local<Object> target);
        static void Dup(V8_ARGS);
        static void GetAvailableParallelism(V8_ARGS);
        static void GetFreeMemory(V8_ARGS);
        static void GetTotalMemory(V8_ARGS);
        static void GetConstrainedMemory(V8_ARGS);
        static void GetAvailableMemory(V8_ARGS);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
    }

}

#endif
