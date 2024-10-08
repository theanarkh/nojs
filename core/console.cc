#include "console.h"

namespace No {
    namespace Console {
      void Log(V8_ARGS) {
          V8_ISOLATE
          String::Utf8Value str(isolate, args[0]);
          No::Util::Log(*str);
      }

      void Init(Isolate* isolate, Local<Object> target) {
        Local<Object> obj = Object::New(isolate);

        Local<FunctionTemplate> tpl = No::Util::NewFunctionTemplate(isolate, Log);
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "log"), tpl);

        ObjectSet(isolate, target, "console", obj);
      }

    }
}
