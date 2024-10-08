#ifndef ASYNC_H
#define ASYNC_H

#include "env.h"
#include "micro_task.h"
#include "base_object.h"
#include "common.h"

using namespace v8;
using namespace No::Env;
using namespace No::Base;

namespace No {
    namespace Async {
        class AsyncWrap: public BaseObject {
            public: 
            AsyncWrap(Environment* env, Local<Object> object): BaseObject(env, object) {}
            void MakeCallback(const char* event, int argc, Local<Value> *argv);
        };
    }
}

#endif