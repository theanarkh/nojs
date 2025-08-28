#include "vm.h"      

namespace No {
  namespace VM {
    void Run(V8_ARGS) {
        V8_ISOLATE
        V8_CONTEXT
        String::Utf8Value str(isolate, args[0].As<String>());
        Local<Object> options = args[1].As<Object>();
        Local<Value> mode;
        (void)options->Get(context, NewString(isolate, "mode")).ToLocal(&mode);
        MODE code_mode = (MODE)mode.As<Integer>()->Value();
        std::string code;
        if (code_mode == FILE_MODE) {
          int fd = open(*str, 0, O_RDONLY);
          if (fd == -1) {
            return V8_RETURN(NewString(isolate, "open file error"));
          }
          char buffer[4096];
          while (1)
          {
            memset(buffer, 0, 4096);
            int ret = read(fd, buffer, 4096);
            if (ret == -1) {
              return V8_RETURN(NewString(isolate, "read file error"));
            }
            if (ret == 0) {
              break;
            }
            code.append(buffer, ret);
          }
          close(fd);
        } else {
          code.append(*str, str.length());
        }

        Local<String> source = String::NewFromUtf8(isolate, code.c_str(),
                              NewStringType::kNormal,
                              code.length()).ToLocalChecked();

        // 编译
        MaybeLocal<Script> script = Script::Compile(context, source);
        if (script.IsEmpty()) {
          return V8_RETURN(NewString(isolate, "invalid code"));
        }
        // 执行
        Local<Value> result = script.ToLocalChecked()->Run(context).ToLocalChecked();
        V8_RETURN(result)
    }

    void CompileFunction(V8_ARGS) {
        V8_ISOLATE
        V8_CONTEXT
        String::Utf8Value str(isolate, args[0].As<String>());
        Local<Object> options = args[1].As<Object>();
        Local<Value> mode;
        (void)options->Get(context, NewString(isolate, "mode")).ToLocal(&mode);
        Local<Value> extensions;
        (void)options->Get(context, NewString(isolate, "extensions")).ToLocal(&extensions);
        Local<Value> paramNames;
        (void)options->Get(context, NewString(isolate, "paramNames")).ToLocal(&paramNames);
        Local<Value> paramValues;
        (void)options->Get(context, NewString(isolate, "paramValues")).ToLocal(&paramValues);
        Local<Value> call;
        (void)options->Get(context, NewString(isolate, "call")).ToLocal(&call);
        MODE code_mode = (MODE)mode.As<Integer>()->Value();
        std::string code;
        if (code_mode == FILE_MODE) {
          int fd = open(*str, 0, O_RDONLY);
          if (fd == -1) {
            return V8_RETURN(NewString(isolate, "open file error"));
          }
          char buffer[4096];
          while (1)
          {
            memset(buffer, 0, 4096);
            int ret = read(fd, buffer, 4096);
            if (ret == -1) {
              return V8_RETURN(NewString(isolate, "read file error"));
            }
            if (ret == 0) {
              break;
            }
            code.append(buffer, ret);
          }
          close(fd);
        } else {
          code.append(*str, str.length());
        }
      
        ScriptCompiler::Source script_source(NewString(isolate, code.c_str()));
        Local<String> * names = nullptr;
        int names_len = 0;
        vector<Local<String>> name_vector;
        if (!paramNames.IsEmpty()) {
          Local<Array> arr = paramNames.As<Array>();
          names_len = arr->Length();
          for (int i = 0; i < names_len; i++) {
            name_vector.push_back(arr->Get(context, i).ToLocalChecked().As<String>());
          }
          names = name_vector.data();
        }
        Local<Object> * exts = nullptr;
        int exts_len = 0;
        vector<Local<Object>> extension_vector;
        if (!extensions.IsEmpty()) {
          Local<Array> arr = extensions.As<Array>();
          exts_len = arr->Length();
          for (int i = 0; i < exts_len; i++) {
            extension_vector.push_back(arr->Get(context, i).ToLocalChecked().As<Object>());
          }
          exts = extension_vector.data();
        }
        MaybeLocal<Function> fun = ScriptCompiler::CompileFunction(context, &script_source, names_len, names, exts_len, exts);
        if (fun.IsEmpty()) {
          V8_RETURN(Undefined(isolate));
        } else {
          if (call.As<Boolean>()->Value()) {
            Local<Value> *argv = nullptr;
            int argc = 0;
            vector<Local<Value>> values_vector;
            if (!paramValues.IsEmpty()) {
              Local<Array> values = paramValues.As<Array>();
              argc = values->Length();
              for (int i = 0; i < argc; i++) {
                values_vector.push_back(values->Get(context, i).ToLocalChecked().As<Value>());
              }
              argv = values_vector.data();
            }
            MaybeLocal<Value> result = fun.ToLocalChecked()->Call(context, context->Global(), argc, argv);
            if (result.IsEmpty()) {
              V8_RETURN(Null(isolate));
            } else {
              V8_RETURN(result.ToLocalChecked());
            }
          } else {
            V8_RETURN(fun.ToLocalChecked());
          }
        }
    }

    void VMScript::New(const FunctionCallbackInfo<Value>& args) {
      V8_ISOLATE
      V8_CONTEXT
      VMScript* script_ptr  = new VMScript(Environment::GetCurrent(args), args.This());
     
      v8::String::Utf8Value source(args.GetIsolate(), args[0]);
      v8::String::Utf8Value filename(args.GetIsolate(), args[1]);

      v8::ScriptCompiler::CompileOptions option = v8::ScriptCompiler::kNoCompileOptions;
      ScriptCompiler::CachedData* cache = nullptr;

      if (args.Length() > 2 && args[2]->IsUint8Array()) {
        Local<Uint8Array> uint8_array = args[2].As<Uint8Array>();
        cache = new ScriptCompiler::CachedData(
            reinterpret_cast<const uint8_t*>(uint8_array->Buffer()->GetBackingStore()->Data()),
            reinterpret_cast<size_t>(uint8_array->Buffer()->ByteLength()));
        option = v8::ScriptCompiler::kConsumeCodeCache;
      }
      
      v8::ScriptOrigin origin(isolate, NewString(isolate, *filename));
      v8::ScriptCompiler::Source script_source(NewString(isolate, *source), origin, cache);
      v8::Local<v8::Script> script =
          v8::ScriptCompiler::Compile(context, &script_source, option).ToLocalChecked();
      script_ptr->script.Reset(isolate, script);
    }

    void VMScript::RunScript(V8_ARGS) {
      V8_ISOLATE
      V8_CONTEXT
      VMScript* script = (VMScript*)Base::BaseObject::unwrap(args.Holder()) ;
      Local<Value> result = script->script.Get(isolate)->Run(context).ToLocalChecked();
      V8_RETURN(result);
    }

    void VMScript::CreateCodeCache(V8_ARGS) {
      V8_ISOLATE
      V8_CONTEXT
      VMScript* script = (VMScript*)Base::BaseObject::unwrap(args.Holder()) ;
      ScriptCompiler::CachedData* cache = v8::ScriptCompiler::CreateCodeCache(script->script.Get(isolate)->GetUnboundScript());
      Local<ArrayBuffer> buffer = ArrayBuffer::New(isolate, cache->length);
      std::memcpy(buffer->GetBackingStore()->Data(), cache->data, cache->length);
      Local<Uint8Array> result = Uint8Array::New(buffer, 0, cache->length);
      V8_RETURN(result);
    }

    VMScript::~VMScript() {
      script.Reset();
    }

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> vm = Object::New(isolate);

      Local<Object> mode = Object::New(isolate);
      ObjectSet(isolate, mode, "CODE", Integer::New(isolate, CODE_MODE));
      ObjectSet(isolate, mode, "FILE", Integer::New(isolate, FILE_MODE));
      ObjectSet(isolate, vm, "MODE", mode);
      
      SetFunction(isolate->GetCurrentContext(), vm, NewString(isolate, "run"), No::Util::NewFunctionTemplate(isolate, Run));
      SetFunction(isolate->GetCurrentContext(), vm, NewString(isolate, "compileFunction"), No::Util::NewFunctionTemplate(isolate, CompileFunction));
      
      Local<FunctionTemplate> script = No::Util::NewFunctionTemplate(isolate, VMScript::New);
      SetProtoMethod(isolate, script, "run", VMScript::RunScript);
      SetProtoMethod(isolate, script, "createCodeCache", VMScript::CreateCodeCache);
      script->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
      SetFunction(isolate->GetCurrentContext(), vm, NewString(isolate, "Script"), script);
        
      ObjectSet(isolate, target, "vm", vm);
    }


    static void RegisterExternalReferences(ExternalReferenceRegistry* registry) {
      registry->Register(Run);
      registry->Register(CompileFunction);
      registry->Register(VMScript::New);
      registry->Register(VMScript::RunScript);
      registry->Register(VMScript::CreateCodeCache);
    }
  }
}

NODE_BINDING_EXTERNAL_REFERENCE(vm, No::VM::RegisterExternalReferences)
