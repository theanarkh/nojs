#include "pipe.h"           

namespace No {
    namespace Pipe {
      void PipeWrap::New(const FunctionCallbackInfo<Value>& args) {
        int ipc = args[0].As<Int32>()->Value();
        new PipeWrap(Environment::GetCurrent(args), args.This(), ipc);
      }

      void PipeWrap::Fchmod(V8_ARGS) {
        PipeWrap* wrap = (PipeWrap*)Base::BaseObject::unwrap(args.Holder());
        int mode = args[0].As<Int32>()->Value();
        int err = uv_pipe_chmod(&wrap->handle_, mode);
        args.GetReturnValue().Set(err);
      }

      void PipeWrap::Bind(V8_ARGS) {
        PipeWrap* wrap = (PipeWrap*)Base::BaseObject::unwrap(args.Holder()) ;
        Environment* env = wrap->env();
        v8::String::Utf8Value name(args.GetIsolate(), args[0]);
        int err = uv_pipe_bind2(&wrap->handle_, *name, name.length(), UV_PIPE_NO_TRUNCATE);
        args.GetReturnValue().Set(err);
      }

      static void AfterConnect(uv_connect_t* req, int status){
        PipeConnectWrap* ctx = (PipeConnectWrap*)PipeConnectWrap::from_req(req);
        Isolate* isolate = ctx->env()->GetIsolate();
        HandleScope handle_scope(isolate);
        Context::Scope context_scope(ctx->env()->GetContext());
        Local<Value> argv[] = {
            Integer::New(isolate, status)
        };
        ctx->MakeCallback("onconnection", 1, argv);
      }

      void PipeWrap::Connect(const FunctionCallbackInfo<Value>& args) {

        PipeWrap* wrap = (PipeWrap*)Base::BaseObject::unwrap(args.Holder()) ;
        Environment* env = wrap->env();

        Local<Object> req_wrap_obj = args[0].As<Object>();
        v8::String::Utf8Value name(env->GetIsolate(), args[1]);

        PipeConnectWrap* req_wrap = new PipeConnectWrap(env, req_wrap_obj);
          int err = uv_pipe_connect2(req_wrap->req(), &wrap->handle_, *name, name.length(), UV_PIPE_NO_TRUNCATE,
                                  AfterConnect);
          if (err) {
            delete req_wrap;
          }

        args.GetReturnValue().Set(err);
      }

      static void OnConnection(uv_stream_t* handle, int status) {
        PipeWrap* wrap = (PipeWrap*)handle->data;
        Isolate* isolate = wrap->env()->GetIsolate();
        HandleScope handle_scope(isolate);
        Context::Scope context_scope(wrap->env()->GetContext());
        Local<Value> argv[] = {
            Integer::New(isolate, status)
        };
        wrap->MakeCallback("onconnection", 1, argv);
      }

      void PipeWrap::Listen(V8_ARGS) {
        PipeWrap* wrap = (PipeWrap*)Base::BaseObject::unwrap(args.Holder()) ;
        Environment* env = wrap->env();
        int backlog;
        if (!args[0]->Int32Value(env->GetContext()).To(&backlog)) return;
        int err = uv_listen(reinterpret_cast<uv_stream_t*>(&wrap->handle_),
                            backlog,
                            OnConnection);
        args.GetReturnValue().Set(err);
      }

      void PipeWrap::Accept(V8_ARGS) {
        V8_ISOLATE
        PipeWrap* wrap = (PipeWrap*)Base::BaseObject::unwrap(args.Holder());
        PipeWrap* new_wrap = (PipeWrap*)Base::BaseObject::unwrap(args[0].As<Object>()) ;
        uv_stream_t* client = reinterpret_cast<uv_stream_t*>(&new_wrap->handle_);
        int ret = uv_accept((uv_stream_t *)&wrap->handle_, client);
        V8_RETURN(Integer::New(isolate, ret));
      }

      void PipeWrap::ReadStart(V8_ARGS) {
        PipeWrap* wrap = (PipeWrap*)Base::BaseObject::unwrap(args.Holder());
        uv_read_start(
            (uv_stream_t* )&wrap->handle_,
            [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
              PipeWrap* wrap = static_cast<PipeWrap*>(handle->data);
              *buf = wrap->env()->allocate_managed_buffer(suggested_size);
            },
            [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
              if (nread > 0) {
                PipeWrap* wrap = static_cast<PipeWrap*>(stream->data);
                Isolate * isolate= wrap->env()->GetIsolate();
            
                std::unique_ptr<BackingStore> bs = wrap->env()->release_managed_buffer(buf);

                if (static_cast<size_t>(nread) != buf->len) {
                  std::unique_ptr<BackingStore> old_bs = std::move(bs);
                  bs = ArrayBuffer::NewBackingStore(isolate, nread);
                  memcpy(static_cast<char*>(bs->Data()),
                        static_cast<char*>(old_bs->Data()),
                        nread);
                }
                Local<ArrayBuffer> buffer = ArrayBuffer::New(isolate, std::move(bs));
                
                Local<Value> argv[] = {
                    Integer::New(isolate, nread),
                    buffer.As<Value>()
                };
                wrap->MakeCallback("onread", 1, argv);
              }
            });
      }

      void PipeWrap::ReadStop(V8_ARGS) {
        V8_ISOLATE
        PipeWrap* wrap = (PipeWrap*)Base::BaseObject::unwrap(args.Holder());
        int ret = uv_read_stop((uv_stream_t*)&wrap->handle_);
        V8_RETURN(Integer::New(isolate, ret));
      }

      void Init(Isolate* isolate, Local<Object> target) {
        Local<Object> obj = Object::New(isolate);
        Environment *env = Environment::GetCurrent(isolate);
        Local<FunctionTemplate> tpl = No::Util::NewFunctionTemplate(isolate, PipeWrap::New);
        SetProtoMethod(isolate, tpl, "listen", PipeWrap::Listen);
        SetProtoMethod(isolate, tpl, "bind", PipeWrap::Bind);
        SetProtoMethod(isolate, tpl, "connect", PipeWrap::Connect);
        SetProtoMethod(isolate, tpl, "accept", PipeWrap::Accept);
        SetProtoMethod(isolate, tpl, "readStart", PipeWrap::ReadStart);
        SetProtoMethod(isolate, tpl, "readStop", PipeWrap::ReadStop);
        SetProtoMethod(isolate, tpl, "fchmod", PipeWrap::Fchmod);
        tpl->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
        tpl->Inherit(HandleWrap::GetConstructorTemplate(env));
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "Pipe"), tpl);

        Local<FunctionTemplate> t = No::Util::NewDefaultFunctionTemplate(isolate);
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "PipeConenctReq"), t);

        ObjectSet(isolate, target, "pipe", obj);
      }

      static void RegisterExternalReferences(ExternalReferenceRegistry* registry) {
        registry->Register(PipeWrap::New);
        registry->Register(PipeWrap::Bind);
        registry->Register(PipeWrap::Listen);
        registry->Register(PipeWrap::Connect);
        registry->Register(PipeWrap::Accept);
        registry->Register(PipeWrap::ReadStart);
        registry->Register(PipeWrap::ReadStop);
        registry->Register(PipeWrap::Fchmod);
      }
    }
}

NODE_BINDING_EXTERNAL_REFERENCE(pipe, No::Pipe::RegisterExternalReferences)
