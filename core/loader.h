#ifndef LOADER_H
#define LOADER_H

#include "common.h"
#include "util.h"
#include "env.h"
#include <string>
#include <unistd.h>
#include "external_reference.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;

namespace No {
    namespace Loader {
        void Compile(V8_ARGS);
        void CompileNative(V8_ARGS);
        std::string GetJsCode(const char *filename);
        void Init(Isolate* isolate, Local<Object> target);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
    }
}
#endif 
