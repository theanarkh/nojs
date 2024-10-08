#include "fs_watcher.h"

namespace No {
    namespace FSWatcher {
        void Watcher::New(const FunctionCallbackInfo<Value>& args) {
            new Watcher(Environment::GetCurrent(args), args.This());
        }

        void Watcher::OnChange(uv_fs_event_t* handle,
                               const char* filename,
                               int events,
                               int status) {
            Watcher* wrap = static_cast<Watcher*>(handle->data);
            Environment* env = wrap->env();
            Isolate* isolate = env->GetIsolate();
            Local<Context> context = env->GetContext();
            HandleScope handle_scope(isolate);
            Context::Scope context_scope(context);
            Local<Array> event_names = Array::New(isolate);
            int index = 0;
            if (status == 0) {
                if (events & UV_RENAME) {
                    if (event_names->Set(context, index++, NewString(isolate, "rename")).IsNothing()) {
                        return;
                    }
                } else if (events & UV_CHANGE) {
                    if (event_names->Set(context, index++, NewString(isolate, "change")).IsNothing()) {
                        return;
                    }
                } else {
                    return;
                }
            }
            Local<Value> argv[] = {
                Integer::New(isolate, status),
                event_names,
                filename == nullptr ? String::Empty(isolate) : NewString(isolate, filename)
            };

            wrap->MakeCallback("onchange", 3, argv);
       }

        void Watcher::Start(const FunctionCallbackInfo<Value>& args) {
            Watcher* wrap = (Watcher*)Base::BaseObject::unwrap(args.Holder());
            v8::String::Utf8Value path(args.GetIsolate(), args[0]);
            unsigned int flags = 0;
            if (args[1]->IsTrue())
                flags |= UV_FS_EVENT_RECURSIVE;
            int err = uv_fs_event_start(&wrap->handle_, OnChange, *path, flags);
            args.GetReturnValue().Set(err);
        }

        void Init(Isolate* isolate, Local<Object> target) {
            Local<Object> obj = Object::New(isolate);

            Local<FunctionTemplate> tpl = No::Util::NewFunctionTemplate(isolate, Watcher::New);
            SetProtoMethod(isolate, tpl, "start", Watcher::Start);
            tpl->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
            SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "Watcher"), tpl);
            
            ObjectSet(isolate, target, "fswatcher", obj);
        }
    }
}
