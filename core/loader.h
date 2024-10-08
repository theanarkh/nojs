#ifndef LOADER_H
#define LOADER_H

#include "common.h"
#include "util.h"
#include "env.h"
#include <string>
#include <unistd.h>

using namespace v8;
using namespace No::Util;
using namespace No::Env;

namespace No {
    namespace Loader {
        void Compile(V8_ARGS);
        void Init(Isolate* isolate, Local<Object> target);
    }
}
#endif 
