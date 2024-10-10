#include "child_process.h"

namespace No {
    namespace ChildProcess {

      void ProcessWrap::New(V8_ARGS) {
          new ProcessWrap(Environment::GetCurrent(args), args.This());
      }

      // Copy from Node.js src/process_wrap.cc
      static void ParseOptions(uv_process_options_t *options, Local<Object> js_options) {
          Isolate *isolate = js_options->GetIsolate();
          Local<Context> context = isolate->GetCurrentContext();
          // options.uid
          Local<Value> uid_v =
              js_options->Get(context, NewString(isolate, "uid")).ToLocalChecked();
          if (!uid_v->IsUndefined() && !uid_v->IsNull()) {
            const int32_t uid = uid_v.As<Int32>()->Value();
            options->flags |= UV_PROCESS_SETUID;
            options->uid = static_cast<uv_uid_t>(uid);
          }

          // options.gid
          Local<Value> gid_v =
              js_options->Get(context, NewString(isolate, "gid")).ToLocalChecked();
          if (!gid_v->IsUndefined() && !gid_v->IsNull()) {
            const int32_t gid = gid_v.As<Int32>()->Value();
            options->flags |= UV_PROCESS_SETGID;
            options->gid = static_cast<uv_gid_t>(gid);
          }

          // options.file
          Local<Value> file_v = js_options->Get(context, NewString(isolate, "file")).ToLocalChecked();
          v8::String::Utf8Value file(isolate, file_v);
          options->file = *file;

          // options.args
          Local<Value> argv_v = js_options->Get(context, NewString(isolate, "args")).ToLocalChecked();
          if (!argv_v.IsEmpty() && argv_v->IsArray()) {
            Local<Array> js_argv = argv_v.As<Array>();
            int argc = js_argv->Length();
            // Heap allocate to detect errors. +1 is for nullptr.
            options->args = new char*[argc + 1];
            for (int i = 0; i < argc; i++) {
              v8::String::Utf8Value arg(isolate, js_argv->Get(context, i).ToLocalChecked());
              options->args[i] = strdup(*arg);
            }
            options->args[argc] = nullptr;
          }

          // options.cwd
          Local<Value> cwd_v =
              js_options->Get(context, NewString(isolate, "cwd")).ToLocalChecked();
          v8::String::Utf8Value cwd(isolate, cwd_v->IsString() ? cwd_v : Local<Value>());
          if (cwd.length() > 0) {
            options->cwd = *cwd;
          }

          // options.env
          Local<Value> env_v = js_options->Get(context, NewString(isolate, "env")).ToLocalChecked();
          if (!env_v.IsEmpty() && env_v->IsArray()) {
            Local<Array> env_opt = env_v.As<Array>();
            int envc = env_opt->Length();
            options->env = new char*[envc + 1];  // Heap allocated to detect errors.
            for (int i = 0; i < envc; i++) {
              v8::String::Utf8Value pair(isolate, env_opt->Get(context, i).ToLocalChecked());
              options->env[i] = strdup(*pair);
            }
            options->env[envc] = nullptr;
          }

          // options.detached
          Local<Value> detached_v = js_options->Get(context, NewString(isolate, "detached")).ToLocalChecked();
          if (detached_v->IsTrue()) {
            options->flags |= UV_PROCESS_DETACHED;
          }
      }

      static void ReleaseOptions(uv_process_options_t *options) {
          if (options->args) {
              for (int i = 0; options->args[i]; i++) free(options->args[i]);
              delete [] options->args;
          }

          if (options->env) {
              for (int i = 0; options->env[i]; i++) free(options->env[i]);
              delete [] options->env;
          }

          delete[] options->stdio;
      }

      void ProcessWrap::Spawn(V8_ARGS) {
          ProcessWrap* wrap = (ProcessWrap*)Base::BaseObject::unwrap(args.Holder()) ;
          Environment *env = Environment::GetCurrent(args);
          Isolate *isolate = env->GetIsolate();
          Local<Context> context = env->GetContext();
          Local<Object> js_options = args[0]->ToObject(context).ToLocalChecked();

          uv_process_options_t options;
          memset(&options, 0, sizeof(uv_process_options_t));

          options.exit_cb = OnExit;

          ParseOptions(&options, js_options);

          int err = uv_spawn(env->loop(), &wrap->_handle, &options);

          if (err == 0) {
            wrap->object()->Set(context,NewString(isolate, "pid"),
                                Integer::New(isolate,
                                            wrap->_handle.pid)).Check();
          }

          ReleaseOptions(&options);
          args.GetReturnValue().Set(err);
      }

      void SyncProcessWrap::SpawnSync(V8_ARGS) {
          Environment *env = Environment::GetCurrent(args);
          Isolate *isolate = env->GetIsolate();
          Local<Context> context = env->GetContext();
          Local<Object> js_options = args[0]->ToObject(context).ToLocalChecked();
          SyncProcessWrap sync_process;
          uv_process_options_t options;
          memset(&options, 0, sizeof(uv_process_options_t));

          options.exit_cb = OnSpawnSyncExit;

          ParseOptions(&options, js_options);

          int err = uv_spawn(&sync_process._loop, &sync_process._handle, &options);

          if (err < 0){
              return;
          }
          uv_run(&sync_process._loop, UV_RUN_DEFAULT);
          ReleaseOptions(&options);
          Local<Object> result = Object::New(isolate);
          result->Set(context, NewString(isolate, "status"),
                          Number::New(isolate,
                                      static_cast<double>(sync_process._exit_status))).Check();
          result->Set(context, NewString(isolate, "pid"),
                          Number::New(isolate,
                                      sync_process._handle.pid)).Check();
          if (sync_process._term_signal > 0) {
              result->Set(context, NewString(isolate, "signal"),
                          NewString(isolate, SignoTostring(sync_process._term_signal))).Check();
          }

          args.GetReturnValue().Set(result);
      }

      void SyncProcessWrap::OnSpawnSyncExit(uv_process_t* handle,
                                            int64_t exit_status,
                                            int term_signal) {
          SyncProcessWrap * wrap = (SyncProcessWrap *)handle->data;
          uv_close((uv_handle_t *)&wrap->_handle, NULL);
          wrap->_exit_status = exit_status;
          wrap->_term_signal = term_signal;
      }

      void ProcessWrap::Kill(V8_ARGS) {
          ProcessWrap* wrap = (ProcessWrap*)Base::BaseObject::unwrap(args.Holder()) ;
          int signal = args[0]->Int32Value(wrap->env()->GetContext()).FromJust();
          int err = uv_process_kill(&wrap->_handle, signal);
          args.GetReturnValue().Set(err);
      }

      void ProcessWrap::OnExit(uv_process_t* handle,
                               int64_t exit_status,
                               int term_signal) {
          ProcessWrap * wrap = (ProcessWrap *)handle->data;
          Environment* env = wrap->env();
          HandleScope handle_scope(env->GetIsolate());
          Context::Scope context_scope(env->GetContext());

          Local<Value> argv[] = {
            Number::New(env->GetIsolate(), static_cast<double>(exit_status)),
            NewString(env->GetIsolate(), No::Util::SignoTostring(term_signal))
          };
          wrap->MakeCallback("onexit", 2, argv);
      }

      void Init(Isolate* isolate, Local<Object> target) {
        Local<Object> obj = Object::New(isolate);
        Environment *env = Environment::GetCurrent(isolate);
        Local<FunctionTemplate> tpl = No::Util::NewFunctionTemplate(isolate, ProcessWrap::New);
        SetProtoMethod(isolate, tpl, "spawn", ProcessWrap::Spawn);
        SetProtoMethod(isolate, tpl, "kill", ProcessWrap::Kill);
        tpl->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
        tpl->Inherit(HandleWrap::GetConstructorTemplate(env));
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "ChildProcess"), tpl);

        Local<FunctionTemplate> sync_tpl = No::Util::NewFunctionTemplate(isolate, SyncProcessWrap::SpawnSync);
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "spawnSync"), sync_tpl);

        ObjectSet(isolate, target, "child_process", obj);
      }

    }
}