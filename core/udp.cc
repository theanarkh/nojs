#include "udp.h"           

namespace No {
    namespace UDP {

      void UDPWrap::New(const FunctionCallbackInfo<Value>& args) {
        new UDPWrap(Environment::GetCurrent(args), args.This());
      }

      void UDPWrap::Bind(const FunctionCallbackInfo<Value>& args) {
        DoBind(args, AF_INET);
      }
      void UDPWrap::Bind6(const FunctionCallbackInfo<Value>& args) {
        DoBind(args, AF_INET6);
      }

      void UDPWrap::DoBind(const FunctionCallbackInfo<Value>& args, int family) {
        UDPWrap* wrap = (UDPWrap*)Base::BaseObject::unwrap(args.Holder()) ;
        Environment* env = wrap->env();

        v8::String::Utf8Value address(args.GetIsolate(), args[0]);
        Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
        uint32_t port;
        if (!args[1]->Uint32Value(ctx).To(&port))
          return;
        uint32_t flags = 0;
        if (!args[2]->Uint32Value(ctx).To(&flags))
          return;
        struct sockaddr_storage addr_storage;
        int err = SockaddrForfamily(family,*address, port, &addr_storage);
        if (err == 0) {
          err = uv_udp_bind(&wrap->handle_,
                            reinterpret_cast<const sockaddr*>(&addr_storage),
                            flags);
        }

        args.GetReturnValue().Set(err);
      }

      void UDPWrap::Connect(const FunctionCallbackInfo<Value>& args){
        DoConnect(args, AF_INET);
      }

      void UDPWrap::Connect6(const FunctionCallbackInfo<Value>& args){
        DoConnect(args, AF_INET6);
      }

      void UDPWrap::DoConnect(const FunctionCallbackInfo<Value>& args, int family) {

        UDPWrap* wrap = (UDPWrap*)Base::BaseObject::unwrap(args.Holder()) ;
        Environment* env = wrap->env();

        v8::String::Utf8Value address(args.GetIsolate(), args[0]);
        Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
        uint32_t port;
        if (!args[1]->Uint32Value(ctx).To(&port))
          return;
        struct sockaddr_storage addr_storage;
        int err = SockaddrForfamily(family, *address, port, &addr_storage);
        if (err == 0) {
          err = uv_udp_connect(&wrap->handle_,
                              reinterpret_cast<const sockaddr*>(&addr_storage));
        }
        args.GetReturnValue().Set(err);
      }

      void UDPWrap::DisConnect(const FunctionCallbackInfo<Value>& args) {
        UDPWrap* wrap = (UDPWrap*)Base::BaseObject::unwrap(args.Holder()) ;
        int err = uv_udp_connect(&wrap->handle_, nullptr);
        args.GetReturnValue().Set(err);
      }

      void UDPWrap::ReadStart(V8_ARGS) {
        UDPWrap* wrap = (UDPWrap*)Base::BaseObject::unwrap(args.Holder());
        int ret = uv_udp_recv_start(
            &wrap->handle_,
            [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
            UDPWrap* wrap = static_cast<UDPWrap*>(handle->data);
            *buf = wrap->env()->allocate_managed_buffer(suggested_size);
            },
            [](uv_udp_t* handle,
              ssize_t nread,
              const uv_buf_t* buf,
              const struct sockaddr* addr,
              unsigned flags) {
              if (nread > 0) {
                UDPWrap* wrap = static_cast<UDPWrap*>(handle->data);
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
                wrap->MakeCallback("onmessage", 2, argv);
              }
            });
            V8_ISOLATE
            V8_RETURN(Integer::New(isolate, ret));
      }

      void UDPWrap::ReadStop(V8_ARGS) {
        V8_ISOLATE
        UDPWrap* wrap = (UDPWrap*)Base::BaseObject::unwrap(args.Holder());
        int ret = uv_udp_recv_stop(&wrap->handle_);
        V8_RETURN(Integer::New(isolate, ret));
      }

      static void InitConstant(Isolate* isolate, Local<Object> target) {
        Local<Object> constant = Object::New(isolate);
        Local<Object> flag = Object::New(isolate);

        #define FLAG_LIST(Set) \
          Set(UV_UDP_IPV6ONLY) \
          Set(UV_UDP_REUSEADDR) \
          Set(UV_UDP_REUSEPORT)
          #define Set(val) ObjectSet(isolate, flag, #val, Number::New(isolate, val));
              FLAG_LIST(Set)
          #undef Set
        #undef FLAG_LIST

        ObjectSet(isolate, constant, "FLAG", flag);
        ObjectSet(isolate, target, "constant", constant);
      }

      void Init(Isolate* isolate, Local<Object> target) {
        Local<Object> obj = Object::New(isolate);
        Environment *env = Environment::GetCurrent(isolate);
        Local<FunctionTemplate> tpl = No::Util::NewFunctionTemplate(isolate, UDPWrap::New);
        SetProtoMethod(isolate, tpl, "bind", UDPWrap::Bind);
        SetProtoMethod(isolate, tpl, "bind6", UDPWrap::Bind6);
        SetProtoMethod(isolate, tpl, "connect", UDPWrap::Connect);
        SetProtoMethod(isolate, tpl, "connect6", UDPWrap::Connect6);
        SetProtoMethod(isolate, tpl, "disconnect", UDPWrap::DisConnect);
        SetProtoMethod(isolate, tpl, "readStart", UDPWrap::ReadStart);
        SetProtoMethod(isolate, tpl, "readStop", UDPWrap::ReadStop);
        tpl->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
        tpl->Inherit(HandleWrap::GetConstructorTemplate(env));
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "UDP"), tpl);

        InitConstant(isolate, obj);

        ObjectSet(isolate, target, "udp", obj);
      }
    } 
}
