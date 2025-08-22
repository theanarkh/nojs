#ifndef PERF_H
#define PERF_H

#include "common.h"
#include "env.h"

using namespace v8;
using namespace No::Env;

namespace No {
    namespace Perf {
        void Init(Isolate* isolate, Local<Object> target);
        static void Hrtime(V8_ARGS);
    }
}

#endif
