#ifndef BUFFER_H
#define BUFFER_H

#include "util.h"
#include "common.h"
#include "env.h"
#include "external_reference.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;

namespace No {
    namespace Buffer {
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
        void Init(Isolate* isolate, Local<Object> target);
        static void WriteUTF8(V8_ARGS);
        static void FromUTF8(V8_ARGS);
    }
}

#endif
