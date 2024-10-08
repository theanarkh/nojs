#include "micro_task.h"           

namespace No {
  namespace Immediate {
    void SetImmediateCallback(V8_ARGS) {
      Environment* env = Environment::GetCurrent(args);
      env->set_immediate_cb(args[0].As<Function>());
    }

    void Start(V8_ARGS) {
      Environment* env = Environment::GetCurrent(args);
      env->start_immediate_task();
    }

    void Stop(V8_ARGS) {
      Environment* env = Environment::GetCurrent(args);
      env->stop_immediate_task();
    }

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> obj = Object::New(isolate);

      Local<FunctionTemplate> setImmediateCallback = No::Util::NewFunctionTemplate(isolate, SetImmediateCallback);
      Local<FunctionTemplate> start = No::Util::NewFunctionTemplate(isolate, Start);
      Local<FunctionTemplate> stop = No::Util::NewFunctionTemplate(isolate, Stop);
      
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "setImmediateCallback"), setImmediateCallback);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "start"), start);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "stop"), stop);
      
      ObjectSet(isolate, target, "immediate", obj);
    }
  }
}


