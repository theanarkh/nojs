#include "handle.h"

namespace No {
    namespace Handle {
        HandleWrap::HandleWrap(Environment* env, Local<Object> object, uv_handle_t *handle): AsyncWrap(env, object), handle_(handle) {
            handle_->data = this;
        }

        void HandleWrap::Ref(V8_ARGS) {
            HandleWrap* wrap = (HandleWrap*)Base::BaseObject::unwrap(args.Holder()) ;
            uv_ref(wrap->handle());
        }

        void HandleWrap::Unref(V8_ARGS) {
            HandleWrap* wrap = (HandleWrap*)Base::BaseObject::unwrap(args.Holder()) ;
            uv_unref(wrap->handle());
        }

        void HandleWrap::HasRef(V8_ARGS) {
            HandleWrap* wrap = (HandleWrap*)Base::BaseObject::unwrap(args.Holder()) ;
            args.GetReturnValue().Set(uv_has_ref(wrap->handle()));
        }

          void HandleWrap::Close(V8_ARGS) {
            HandleWrap* wrap = (HandleWrap*)Base::BaseObject::unwrap(args.Holder()) ;
            wrap->DoClose(args[0]);
        }

        void HandleWrap::OnClose(uv_handle_t* handle) {
            HandleWrap* wrap = (HandleWrap*)handle->data;
            Environment* env = wrap->env();
            HandleScope scope(env->GetIsolate());
            Context::Scope context_scope(env->GetContext());

            if (!wrap->persistent_object().IsEmpty() &&
                wrap->object()
                    ->Has(env->GetContext(), NewString(env->GetIsolate(), "onclose"))
                    .FromMaybe(false)) {
                wrap->MakeCallback("onclose", 0, nullptr);
            }
            wrap->Release();
        }

        void HandleWrap::DoClose(Local<Value> close_cb) {
            uv_close(handle_, OnClose);
            if (!close_cb.IsEmpty() && close_cb->IsFunction() &&
                !object().IsEmpty()) {
                object()->Set(env()->GetContext(),
                            NewString(env()->GetIsolate(), "onclose"),
                            close_cb).Check();
            }
        }

        Local<FunctionTemplate> HandleWrap::GetConstructorTemplate(Environment* env) {
            Local<FunctionTemplate> tmpl = env->handle_wrap_ctor_template();
            if (tmpl.IsEmpty()) {
                Isolate* isolate = env->GetIsolate();
                tmpl = NewFunctionTemplate(isolate, nullptr);
                tmpl->SetClassName(NewString(isolate, "HandleWrap"));
                SetProtoMethod(isolate, tmpl, "close", HandleWrap::Close);
                SetProtoMethod(isolate, tmpl, "hasRef", HandleWrap::HasRef);
                SetProtoMethod(isolate, tmpl, "ref", HandleWrap::Ref);
                SetProtoMethod(isolate, tmpl, "unref", HandleWrap::Unref);
                env->set_handle_wrap_ctor_template(tmpl);
            }
            return tmpl;
        }

        static void RegisterExternalReferences(ExternalReferenceRegistry* registry) {
            registry->Register(HandleWrap::Close);
            registry->Register(HandleWrap::Ref);
            registry->Register(HandleWrap::Unref);
            registry->Register(HandleWrap::HasRef);
        }
    }
}

NODE_BINDING_EXTERNAL_REFERENCE(handle, No::Handle::RegisterExternalReferences)
