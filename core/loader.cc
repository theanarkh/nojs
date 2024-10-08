#include "loader.h"      

namespace No {
  namespace Loader {
    void Compile(V8_ARGS) {
        V8_ISOLATE
        V8_CONTEXT
        String::Utf8Value filename(isolate, args[0].As<String>());
        int fd = open(*filename, 0 , O_RDONLY);
        if (fd == -1) {
          std::string msg("failed to open file: ");
          msg.append(*filename);
          args.GetReturnValue().Set(NewString(isolate, msg.c_str()));
          return;
        }
        std::string content;
        char buffer[4096];
        while (1)
        {
          memset(buffer, 0, 4096);
          int ret = read(fd, buffer, 4096);
          if (ret == -1) {
            std::string msg("failed to read file: ");
            msg.append(*filename);
            args.GetReturnValue().Set(NewString(isolate, msg.c_str()));
            return;
          }
          if (ret == 0) {
            break;
          }
          content.append(buffer, ret);
        }
        close(fd);
        ScriptCompiler::Source script_source(NewString(isolate, content.c_str()));
        Local<String> params[] = {
          NewString(isolate, "require"),
          NewString(isolate, "exports"),
          NewString(isolate, "module"),
          NewString(isolate, "No"),
        };
        MaybeLocal<Function> func = ScriptCompiler::CompileFunction(context, &script_source, 4, params, 0, nullptr);
        if (func.IsEmpty()) {
          args.GetReturnValue().Set(Undefined(isolate));
        } else {
          args.GetReturnValue().Set(func.ToLocalChecked());
        }
    }

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> obj = Object::New(isolate);

      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "compile"), No::Util::NewFunctionTemplate(isolate, Compile));
          
      ObjectSet(isolate, target, "loader", obj);
    }
  }
}