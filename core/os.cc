#include "os.h"

namespace No {
  namespace OS {

    void Dup(V8_ARGS) {
        int fd = args[0].As<Uint32>()->Value();
        V8_RETURN(dup(fd));
    }

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> obj = Object::New(isolate);
      Local<FunctionTemplate> dup = No::Util::NewFunctionTemplate(isolate, Dup);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "dup"), dup);
      ObjectSet(isolate, target, "os", obj);
    }
  }
}