#ifndef PROCESS_H
#define PROCESS_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "common.h"
#include "env.h"
#include "async.h"
#include "external_reference.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Async;

namespace No {
    namespace Process {
        void Init(Isolate* isolate, Local<Object> target);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
        static void Cwd(V8_ARGS);
        static void Exit(V8_ARGS);
        static void Execve(V8_ARGS);
        static void Kill(V8_ARGS);
        static void CPUUsage(V8_ARGS);
        static void MemoryUsage(V8_ARGS);
    }
}

#endif 