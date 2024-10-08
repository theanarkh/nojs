#ifndef BASE_OBJECT_H
#define BASE_OBJECT_H

#include "util.h"
#include "common.h"
#include "env.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;

namespace No {
    namespace Base {
        class BaseObject {
            public:
                enum InternalFields { Slot, kInternalFieldCount };
                BaseObject(Environment* env, Local<Object> object);
                virtual ~BaseObject();
                void MakeWeak();
                Environment* env() const;
                Local<Object> object();
                static void * unwrap(Local<Object> object);
            private:
                static void WeakCallback(const WeakCallbackInfo<BaseObject>& data);
                Environment * _env;
                Global<Object> _object;
        };
    }
}

#endif