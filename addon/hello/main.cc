#include <stdio.h>
#include <v8.h>

#ifdef _WIN32
# ifndef BUILDING_NODE_EXTENSION
#  define ADDON_EXTERN __declspec(dllexport)
# else
#  define ADDON_EXTERN __declspec(dllimport)
# endif
#else
# define ADDON_EXTERN __attribute__((visibility("default")))
#endif

static void Hello(const v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), 123));
}

extern "C" {
   ADDON_EXTERN void Init(v8::Local<v8::Object> module, v8::Local<v8::Object> exports, v8::Local<v8::Context> context) {
        v8::Isolate * isolate = context->GetIsolate();
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, "hello", v8::NewStringType::kNormal, strlen("hello")).ToLocalChecked();
        v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, Hello);
        tpl->SetClassName(name);
        v8::Local<v8::Function> func = tpl->GetFunction(context).ToLocalChecked();
        exports->Set(context, name, func).Check();
    }
}

