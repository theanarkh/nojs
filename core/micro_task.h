#ifndef MICRO_TASK_H
#define MICRO_TASK_H

#include "util.h"
#include "common.h"
#include "env.h"
#include "external_reference.h"

using namespace No::Env;
using namespace v8;
using namespace No::Util;

namespace No {
    namespace MicroTask {
        void Init(Isolate* isolate, Local<Object> target);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
        static void SetMicroTaskCallback(V8_ARGS);
        static void SetMicroTaskFlag(V8_ARGS);
        static void RunMicrotasks(V8_ARGS);
        static void EnqueueMicrotask(V8_ARGS);
        class MicroTaskScope {
            public:
            MicroTaskScope(Environment * env): _env(env) {}
            ~MicroTaskScope() {
                if (_env->micro_task_flag()) {
                    _env->micro_task_cb()->Call(_env->GetContext(), _env->GetContext()->Global(), 0, nullptr).ToLocalChecked();
                }
                _env->GetContext()->GetMicrotaskQueue()->PerformCheckpoint(_env->GetIsolate());
            }
            private:
            Environment * _env;
        };
    }
}
#endif 
