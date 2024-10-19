#include "os.h"

namespace No {
  namespace OS {

    void Dup(V8_ARGS) {
        int fd = args[0].As<Uint32>()->Value();
        V8_RETURN(dup(fd));
    }


    static void GetConstrainedMemory(V8_ARGS) {
      uint64_t value = uv_get_constrained_memory();
      V8_RETURN(static_cast<double>(value));
    }

    static void GetAvailableMemory(V8_ARGS) {
      uint64_t value = uv_get_available_memory();
      V8_RETURN(static_cast<double>(value));
    }

    static void GetAvailableParallelism(V8_ARGS) {
      unsigned int parallelism = uv_available_parallelism();
      V8_RETURN(parallelism);
    }

    static void GetFreeMemory(V8_ARGS) {
      double amount = static_cast<double>(uv_get_free_memory());
      V8_RETURN(amount);
    }

    static void GetTotalMemory(V8_ARGS) {
      double amount = static_cast<double>(uv_get_total_memory());
      V8_RETURN(amount);
    }

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> obj = Object::New(isolate);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "dup"), No::Util::NewFunctionTemplate(isolate, Dup));
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "getConstrainedMemory"), No::Util::NewFunctionTemplate(isolate, GetConstrainedMemory));
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "getAvailableMemory"), No::Util::NewFunctionTemplate(isolate, GetAvailableMemory));
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "getAvailableParallelism"), No::Util::NewFunctionTemplate(isolate, GetAvailableParallelism));
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "getTotalMemory"), No::Util::NewFunctionTemplate(isolate, GetTotalMemory));
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "getFreeMemory"), No::Util::NewFunctionTemplate(isolate, GetFreeMemory));
      
      ObjectSet(isolate, target, "os", obj);
    }
  }
}