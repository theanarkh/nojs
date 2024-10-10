#ifndef PROCESS_H
#define PROCESS_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "common.h"
#include "env.h"
#include "async.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Async;

namespace No {
    namespace Process {
        void Init(Isolate* isolate, Local<Object> target);
        void Cwd(V8_ARGS);
        void Exit(V8_ARGS);
        void Execve(V8_ARGS);
    }
}

#endif 