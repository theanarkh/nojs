#include "async.h"

namespace No {
    namespace Async {
        void AsyncWrap::MakeCallback(const char* event, int argc, Local<Value> *argv) {
            No::MicroTask::MicroTaskScope microTaskScope(env());
            Local<Value> cb;
            Local<Object> obj = object();
            Local<Context> context = env()->GetContext();
            Local<String> event_name = NewString(env()->GetIsolate(), event);       
            (void)obj->Get(context, event_name).ToLocal(&cb);
            if (cb->IsFunction()) {
                cb.As<v8::Function>()->Call(context, obj, argc, argv).ToLocalChecked();
            }
        }
    }
}