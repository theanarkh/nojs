#include "libuv.h"

namespace No {
    namespace UV {
      void GetErrMap(const FunctionCallbackInfo<Value>& args) {
        Environment* env = Environment::GetCurrent(args);
        Isolate* isolate = env->GetIsolate();
        Local<Context> context = env->GetContext();

        Local<Map> err_map = Map::New(isolate);

        size_t errors_len = arraysize(uv_errors_map);
        for (size_t i = 0; i < errors_len; ++i) {
          const auto& error = uv_errors_map[i];
          Local<Value> arr[] = {NewString(isolate, error.name),
                                NewString(isolate, error.message)};
          if (err_map
                  ->Set(context,
                        Integer::New(isolate, error.value),
                        Array::New(isolate, arr, arraysize(arr)))
                  .IsEmpty()) {
            return;
          }
        }

        args.GetReturnValue().Set(err_map);
      }

      void Init(Isolate* isolate, Local<Object> target) {
        Local<Object> obj = Object::New(isolate);

        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "getErrMap"), No::Util::NewFunctionTemplate(isolate, GetErrMap));
            
        ObjectSet(isolate, target, "uv", obj);
      }
    }
}