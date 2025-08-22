#include "perf.h"

namespace No {
  namespace Perf {

    void Hrtime(V8_ARGS) {
      V8_RETURN(static_cast<double>(uv_hrtime()));
    }

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> obj = Object::New(isolate);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "hrtime"), No::Util::NewFunctionTemplate(isolate, Hrtime));
      
      ObjectSet(isolate, target, "perf", obj);
    }
  }
}