#ifndef VM_H
#define VM_H

#include "common.h"
#include "util.h"
#include "env.h"
#include "base_object.h"
#include <vector>
#include <unistd.h>
#include <iostream>

enum MODE {
    CODE_MODE = 1,
    FILE_MODE = 2
};

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Base;

namespace No {
    namespace VM {
        class VMScript : public BaseObject {
            public:
                VMScript(No::Env::Environment *env, Local<Object> obj): BaseObject(env, obj){}
                ~VMScript();
              
                Local<Value> Run();
                static void New(V8_ARGS);
                static void Run(V8_ARGS);
                static void CreateCodeCache(V8_ARGS);
            private:
                Isolate* isolate;
                Global<v8::Script> script;
        };
        void Run(V8_ARGS);
        void CompileFunction(V8_ARGS);
        void Init(Isolate* isolate, Local<Object> target);
    }
}
#endif 
