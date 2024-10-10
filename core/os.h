#ifndef OS_H
#define OS_H

#include "util.h"
#include "common.h"
#include "env.h"
#include <unistd.h>

using namespace v8;
using namespace No::Util;
using namespace No::Env;

namespace No {
    namespace OS {
        void Init(Isolate* isolate, Local<Object> target);
        void Dup(V8_ARGS);
    }
}

#endif
