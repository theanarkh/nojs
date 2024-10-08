#include "micro_task.h"           

namespace No {
  namespace MicroTask {
    void SetMicroTaskCallback(V8_ARGS) {
      Environment* env = Environment::GetCurrent(args);
      env->set_micro_task_cb(args[0].As<Function>());
    }

    void SetMicroTaskFlag(V8_ARGS) {
      Environment* env = Environment::GetCurrent(args);
      env->set_micro_task_flag(args[0]->BooleanValue(args.GetIsolate()));
    }

    void RunMicrotasks(V8_ARGS) {
      Environment* env = Environment::GetCurrent(args);
      env->GetContext()->GetMicrotaskQueue()->PerformCheckpoint(env->GetIsolate());
    }

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> obj = Object::New(isolate);

      Local<FunctionTemplate> setMicroTaskCallback = No::Util::NewFunctionTemplate(isolate, SetMicroTaskCallback);
      Local<FunctionTemplate> setMicroTaskFlag = No::Util::NewFunctionTemplate(isolate, SetMicroTaskFlag);
      Local<FunctionTemplate> runMicrotasks = No::Util::NewFunctionTemplate(isolate, RunMicrotasks);
      
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "setMicroTaskCallback"), setMicroTaskCallback);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "setMicroTaskFlag"), setMicroTaskFlag);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "runMicrotasks"), runMicrotasks);
      
      ObjectSet(isolate, target, "microTask", obj);
    }
  }
}


