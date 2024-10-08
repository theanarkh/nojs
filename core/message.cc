#include "message.h"

namespace No {
    namespace Message {

        void Init(Isolate* isolate, Local<Object> target) {
           
        }

        Local<FunctionTemplate> GetMessageEndpointConstructorTemplate(Environment* env) {
            Isolate* isolate = env->GetIsolate();
            Local<FunctionTemplate> tpl = env->message_endpoint_ctor_template();
            if (!tpl.IsEmpty()) {
                return tpl;
            }
            tpl = No::Util::NewFunctionTemplate(isolate, nullptr);
            SetProtoMethod(isolate, tpl, "postMessage", MessageEndpoint::PostMessage);
            tpl->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
            tpl->SetClassName(NewString(isolate, "MessageEndpoint"));
            env->set_message_endpoint_ctor_template(tpl);
            return tpl;
        }

        void MessageEndpoint::New(const FunctionCallbackInfo<Value>& args) {
            new MessageEndpoint(Environment::GetCurrent(args), args.This());
        }

        void MessageEndpoint::PostMessage(const FunctionCallbackInfo<Value>& args) {
            MessageEndpoint* wrap = (MessageEndpoint*)Base::BaseObject::unwrap(args.Holder());
            v8::String::Utf8Value message(args.GetIsolate(), args[0]);
            wrap->remote->ReceiveMessage(*message);
        }

        void MessageEndpoint::ReceiveMessage(string msg) {
            {
                lock_guard<std::mutex> guard(mutex_);
                message_queue.push(msg);
            }
            uv_async_send(&async_);
        }
    }
}
