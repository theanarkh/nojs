#ifndef IMMEDIATE_H
#define IMMEDIATE_H

#include "util.h"
#include "common.h"
#include "env.h"

using namespace No::Env;
using namespace v8;
using namespace No::Util;

namespace No {
    namespace Immediate {
        void Init(Isolate* isolate, Local<Object> target);
        static void SetImmediateCallback(V8_ARGS);
        static void Start(V8_ARGS);
        static void Stop(V8_ARGS);
    }
}
#endif 
