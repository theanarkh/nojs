#include "buffer.h"

namespace No {
  namespace Buffer {

    void WriteUTF8(V8_ARGS) {
      V8_ISOLATE
      Environment* env = Environment::GetCurrent(args);

      Local<String> str = args[0].As<String>();
      size_t length = str->Utf8Length(isolate);

      
      std::unique_ptr<BackingStore> bs = ArrayBuffer::NewBackingStore(isolate, length);

      str->WriteUtf8(isolate,
                    static_cast<char*>(bs->Data()),
                    -1,  
                    nullptr,
                    String::NO_NULL_TERMINATION | String::REPLACE_INVALID_UTF8);

      Local<ArrayBuffer> ab = ArrayBuffer::New(isolate, std::move(bs));

      auto array = Uint8Array::New(ab, 0, length);
      args.GetReturnValue().Set(array);
    }
    
    void FromUTF8(V8_ARGS) {
      V8_ISOLATE
      Local<Uint8Array> uint8Array = args[0].As<Uint8Array>();
      Local<ArrayBuffer> arrayBuffer = uint8Array->Buffer();
      std::shared_ptr<BackingStore> backing = arrayBuffer->GetBackingStore();
      Local<String> result = String::NewFromUtf8(isolate, (const char *)backing->Data(), NewStringType::kNormal, backing->ByteLength()).ToLocalChecked();
      args.GetReturnValue().Set(result);
    }

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> obj = Object::New(isolate);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "writeUTF8"), No::Util::NewFunctionTemplate(isolate, WriteUTF8));
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "fromUTF8"), No::Util::NewFunctionTemplate(isolate, FromUTF8));
      
      ObjectSet(isolate, target, "buffer", obj);
    }
  }
}