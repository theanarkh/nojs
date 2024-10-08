#ifndef CONSOLE_H
#define CONSOLE_H

#include "util.h"
#include "common.h"

using namespace v8;
using namespace No::Util;

namespace No {
    namespace Console {
        void Log(V8_ARGS);
        void Init(Isolate* isolate, Local<Object> target);
    }
}

#endif