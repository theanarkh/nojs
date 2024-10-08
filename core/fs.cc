#include "fs.h"

namespace No {
  namespace FS {
    void OpenSync(V8_ARGS) {
      V8_ISOLATE
      String::Utf8Value filename(isolate, args[0]);
      int argc = args.Length();
      int flags = O_RDONLY;
      if (argc > 1) flags = Local<Integer>::Cast(args[1])->Value();
      int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
      if (argc > 2) mode = Local<Integer>::Cast(args[2])->Value();
      uv_fs_t req;
      Local<Integer> ret = Integer::New(isolate, uv_fs_open(Env::Environment::GetCurrent(isolate)->loop(),&req, *filename, flags, mode, NULL));
      V8_RETURN(ret);
    }

    void ReadSync(V8_ARGS) {
        V8_ISOLATE
        int fd = args[0].As<Uint32>()->Value();
        Local<Uint8Array> uint8Array = args[1].As<Uint8Array>();
        Local<ArrayBuffer> arrayBuffer = uint8Array->Buffer();
        std::shared_ptr<BackingStore> backing = arrayBuffer->GetBackingStore();
        uv_fs_t req;
        const uv_buf_t bufs[] = {
          {
            (char*)backing->Data(),
            backing->ByteLength()
          }
        };
        Local<Integer> ret = Integer::New(isolate, uv_fs_read(Env::Environment::GetCurrent(isolate)->loop(), &req, fd, bufs, 1, 0, NULL));
        V8_RETURN(ret);
    }

    void AfterRead(uv_fs_t* req) {
      FSReqCallback* ctx = (FSReqCallback*)FSReqCallback::from_req(req);
      Environment* env = ctx->env();
      HandleScope handle_scope(env->GetIsolate());
      Context::Scope context_scope(env->GetContext());
      Local<Value> argv[] = {
          Integer::New(ctx->env()->GetIsolate(), req->result)
      };
      ctx->MakeCallback("oncomplete", 1, argv);
    }

    void Read(V8_ARGS) {
        V8_ISOLATE
        int fd = args[0].As<Uint32>()->Value();
        Local<Uint8Array> uint8Array = args[1].As<Uint8Array>();
        Local<ArrayBuffer> arrayBuffer = uint8Array->Buffer();
        std::shared_ptr<BackingStore> backing = arrayBuffer->GetBackingStore();
        Local<Object> req_wrap_obj = args[2].As<Object>();
        FSReqCallback *ctx = new FSReqCallback(No::Env::Environment::GetCurrent(args), req_wrap_obj);
        const uv_buf_t bufs[] = {
          {
            (char*)backing->Data(),
            backing->ByteLength()
          }
        };
        Local<Integer> ret = Integer::New(isolate, uv_fs_read(Env::Environment::GetCurrent(isolate)->loop(), ctx->req(), fd, bufs, 1, 0, AfterRead));
        V8_RETURN(ret);
    }

    void WriteSync(V8_ARGS) {
        V8_ISOLATE
        int fd = args[0].As<Uint32>()->Value();
        Local<Uint8Array> uint8Array = args[1].As<Uint8Array>();
        Local<ArrayBuffer> arrayBuffer = uint8Array->Buffer();
        std::shared_ptr<BackingStore> backing = arrayBuffer->GetBackingStore();
        uv_fs_t req;
        const uv_buf_t bufs[] = {
          {
            (char*)backing->Data(),
            backing->ByteLength()
          }
        };
        Local<Integer> ret = Integer::New(isolate, uv_fs_write(Env::Environment::GetCurrent(isolate)->loop(), &req, fd, bufs, 1, 0, NULL));
        V8_RETURN(ret);
    }

    static void InitConstant(Isolate* isolate, Local<Object> target) {
      
      Local<Object> constant = Object::New(isolate);
      Local<Object> mode = Object::New(isolate);
      Local<Object> flag = Object::New(isolate);

      #define MODE_LIST(Set) \
        Set(S_ISUID) \
        Set(S_ISGID) \
        Set(S_ISVTX) \
        Set(S_IRUSR) \
        Set(S_IWUSR) \
        Set(S_IXUSR) \
        Set(S_IRGRP) \
        Set(S_IWGRP) \
        Set(S_IXGRP) \
        Set(S_IROTH) \
        Set(S_IWOTH) \
        Set(S_IXOTH) 
        #define Set(val) ObjectSet(isolate, mode, #val, Number::New(isolate, val));
            MODE_LIST(Set)
        #undef Set
      #undef MODE_LIST

      #define FLAG_LIST(Set) \
        Set(O_ACCMODE) \
        Set(O_RDONLY) \
        Set(O_WRONLY) \
        Set(O_RDWR) \
        Set(O_APPEND) \
        Set(O_ASYNC) \
        Set(O_CLOEXEC) \
        Set(O_CREAT) \
        Set(O_DIRECTORY) \
        Set(O_DSYNC) \
        Set(O_EXCL) \
        Set(O_NOCTTY) \
        Set(O_NOFOLLOW) \
        Set(O_NONBLOCK) \
        Set(O_NDELAY)  \
        Set(O_SYNC) \
        Set(O_TRUNC) 
        #define Set(val) ObjectSet(isolate, flag, #val, Number::New(isolate, val));
            FLAG_LIST(Set)
        #undef Set
      #undef FLAG_LIST

      ObjectSet(isolate, constant, "MODE", mode);
      ObjectSet(isolate, constant, "FLAG", flag);
      ObjectSet(isolate, target, "constant", constant);
    }

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> obj = Object::New(isolate);

      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "openSync"), No::Util::NewFunctionTemplate(isolate, OpenSync));
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "readSync"), No::Util::NewFunctionTemplate(isolate, ReadSync));
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "read"), No::Util::NewFunctionTemplate(isolate, Read));
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "writeSync"), No::Util::NewFunctionTemplate(isolate, WriteSync));
      
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "FSReqCallback"), No::Util::NewDefaultFunctionTemplate(isolate));

      InitConstant(isolate, obj);
      
      ObjectSet(isolate, target, "fs", obj);
    }
  }
}

