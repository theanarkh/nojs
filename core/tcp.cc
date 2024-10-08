#include "tcp.h"           

namespace No {
    namespace TCP {
      void TCPWrap::New(const FunctionCallbackInfo<Value>& args) {
        new TCPWrap(Environment::GetCurrent(args), args.This());
      }

      template <typename T>
      void TCPWrap::Bind(
          const FunctionCallbackInfo<Value>& args,
          int family,
          std::function<int(const char* ip_address, int port, T* addr)> uv_ip_addr) {
        TCPWrap* wrap = (TCPWrap*)Base::BaseObject::unwrap(args.Holder()) ;
        
        Environment* env = wrap->env();
        v8::String::Utf8Value ip_address(env->GetIsolate(), args[0]);
        int port;
        if (!args[1]->Int32Value(env->GetContext()).To(&port)) return;

        T addr;
        int err = uv_ip_addr(*ip_address, port, &addr);

        if (err == 0) {
          err = uv_tcp_bind(&wrap->handle_,
                            reinterpret_cast<const sockaddr*>(&addr),
                            0);
        }
        args.GetReturnValue().Set(err);
      }

      void TCPWrap::Bind(V8_ARGS) {
        Bind<sockaddr_in>(args, AF_INET, uv_ip4_addr);
      }


      void TCPWrap::Bind6(V8_ARGS) {
        Bind<sockaddr_in6>(args, AF_INET6, uv_ip6_addr);
      }

      void TCPWrap::Connect(const FunctionCallbackInfo<Value>& args) {
        int port = static_cast<int>(args[2].As<Uint32>()->Value());
        Connect<sockaddr_in>(args,
                            [port](const char* ip_address, sockaddr_in* addr) {
            return uv_ip4_addr(ip_address, port, addr);
        });
      }

      void TCPWrap::Connect6(const FunctionCallbackInfo<Value>& args) {
        Environment* env = Environment::GetCurrent(args);
        int port;
        if (!args[2]->Int32Value(env->GetContext()).To(&port)) return;
        Connect<sockaddr_in6>(args,
                              [port](const char* ip_address, sockaddr_in6* addr) {
            return uv_ip6_addr(ip_address, port, addr);
        });
      }

      static void AfterConnect(uv_connect_t* req, int status){
        ConnectWrap* ctx = (ConnectWrap*)ConnectWrap::from_req(req);
        Local<Value> argv[] = {
            Integer::New(ctx->env()->GetIsolate(), status)
        };
        ctx->MakeCallback("onconnection", 1, argv);
      }

      template <typename T>
      void TCPWrap::Connect(const FunctionCallbackInfo<Value>& args,
          std::function<int(const char* ip_address, T* addr)> uv_ip_addr) {

        TCPWrap* wrap = (TCPWrap*)Base::BaseObject::unwrap(args.Holder()) ;
        Environment* env = wrap->env();

        Local<Object> req_wrap_obj = args[0].As<Object>();
        v8::String::Utf8Value ip_address(env->GetIsolate(), args[1]);

        T addr;
        int err = uv_ip_addr(*ip_address, &addr);

        if (err == 0) {
          ConnectWrap* req_wrap = new ConnectWrap(env, req_wrap_obj);
          int err = uv_tcp_connect(req_wrap->req(), &wrap->handle_, reinterpret_cast<const sockaddr*>(&addr),
                                  AfterConnect);
          if (err) {
            delete req_wrap;
          }
        }

        args.GetReturnValue().Set(err);
      }

      static void OnConnection(uv_stream_t* handle, int status) {
        TCPWrap* wrap = (TCPWrap*)handle->data;
        Local<Value> argv[] = {
            Integer::New(wrap->env()->GetIsolate(), status)
        };
        wrap->MakeCallback("onconnection", 1, argv);
      }

      void TCPWrap::Listen(V8_ARGS) {
        TCPWrap* wrap = (TCPWrap*)Base::BaseObject::unwrap(args.Holder()) ;
        Environment* env = wrap->env();
        int backlog;
        if (!args[0]->Int32Value(env->GetContext()).To(&backlog)) return;
        int err = uv_listen(reinterpret_cast<uv_stream_t*>(&wrap->handle_),
                            backlog,
                            OnConnection);
        args.GetReturnValue().Set(err);
      }

      void AfterWrite(uv_write_t* req, int status) {
        WriteReq* ctx = (WriteReq*)WriteReq::from_req(req);
        Local<Value> argv[] = {
            Integer::New(ctx->env()->GetIsolate(), status)
        };
        ctx->MakeCallback("oncomplete", 1, argv);
      }

      void TCPWrap::Write(V8_ARGS) {
        TCPWrap* wrap = (TCPWrap*)Base::BaseObject::unwrap(args.Holder()) ;
        Environment* env = wrap->env();

        Local<Object> req_wrap_obj = args[0].As<Object>();
        WriteReq *ctx = new WriteReq(No::Env::Environment::GetCurrent(args), req_wrap_obj);
          
        Local<Uint8Array> uint8Array = args[1].As<Uint8Array>();
        Local<ArrayBuffer> arrayBuffer = uint8Array->Buffer();
        std::shared_ptr<BackingStore> backing = arrayBuffer->GetBackingStore();
        const uv_buf_t bufs[] = {
          {
            (char*)backing->Data(),
            backing->ByteLength()
          }
        };

        int err = uv_write(ctx->req(), (uv_stream_t*)&wrap->handle_, bufs, 1, AfterWrite);
        
        args.GetReturnValue().Set(err);
      }

      void TCPWrap::Accept(V8_ARGS) {
        V8_ISOLATE
        TCPWrap* wrap = (TCPWrap*)Base::BaseObject::unwrap(args.Holder());
        TCPWrap* new_wrap = (TCPWrap*)Base::BaseObject::unwrap(args[0].As<Object>()) ;
        uv_stream_t* client = reinterpret_cast<uv_stream_t*>(&new_wrap->handle_);
        int ret = uv_accept((uv_stream_t *)&wrap->handle_, client);
        V8_RETURN(Integer::New(isolate, ret));
      }

      void TCPWrap::ReadStart(V8_ARGS) {
        TCPWrap* wrap = (TCPWrap*)Base::BaseObject::unwrap(args.Holder());
        uv_read_start(
            (uv_stream_t* )&wrap->handle_,
            [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
            TCPWrap* wrap = static_cast<TCPWrap*>(handle->data);
            *buf = wrap->env()->allocate_managed_buffer(suggested_size);
            },
            [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
              if (nread > 0) {
                TCPWrap* wrap = static_cast<TCPWrap*>(stream->data);
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
                wrap->MakeCallback("onread", 2, argv);
              }
            });
      }

      void TCPWrap::ReadStop(V8_ARGS) {
        V8_ISOLATE
        TCPWrap* wrap = (TCPWrap*)Base::BaseObject::unwrap(args.Holder());
        int ret = uv_read_stop((uv_stream_t*)&wrap->handle_);
        V8_RETURN(Integer::New(isolate, ret));
      }

      void Init(Isolate* isolate, Local<Object> target) {
        Local<Object> obj = Object::New(isolate);

        Local<FunctionTemplate> tcp = No::Util::NewFunctionTemplate(isolate, TCPWrap::New);
        SetProtoMethod(isolate, tcp, "listen", TCPWrap::Listen);
        SetProtoMethod(isolate, tcp, "bind", TCPWrap::Bind);
        SetProtoMethod(isolate, tcp, "bind6", TCPWrap::Bind6);
        SetProtoMethod(isolate, tcp, "connect", TCPWrap::Connect);
        SetProtoMethod(isolate, tcp, "connect6", TCPWrap::Connect6);
        SetProtoMethod(isolate, tcp, "accept", TCPWrap::Accept);
        SetProtoMethod(isolate, tcp, "readStart", TCPWrap::ReadStart);
        SetProtoMethod(isolate, tcp, "readStop", TCPWrap::ReadStop);
        SetProtoMethod(isolate, tcp, "write", TCPWrap::Write);
        tcp->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "TCP"), tcp);
        
        Local<FunctionTemplate> t = No::Util::NewDefaultFunctionTemplate(isolate);
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "TCPConnectReq"), t);

        Local<FunctionTemplate> writeReq = No::Util::NewDefaultFunctionTemplate(isolate);
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "WriteReq"), writeReq);

        ObjectSet(isolate, target, "tcp", obj);
      }
    }
}