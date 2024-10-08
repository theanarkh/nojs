#ifndef MESSAGE_CHANNEL_H
#define MESSAGE_CHANNEL_H

#include "common.h"
#include "env.h"
#include "async.h"
#include <queue>
#include <mutex>

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Async;

namespace No {
    namespace Message {
        void Init(Isolate* isolate, Local<Object> target);
        Local<FunctionTemplate> GetMessageEndpointConstructorTemplate(Environment* env);
        class MessageEndpoint: public AsyncWrap {
            public:
            MessageEndpoint(No::Env::Environment *env, Local<Object> obj): 
                AsyncWrap(env, obj) {
                uv_async_init(env->loop(), &async_, [](uv_async_t* handle) {
                    MessageEndpoint* endpoint = (MessageEndpoint*)handle->data;
                    lock_guard<std::mutex> guard(endpoint->mutex_);
                    while (!endpoint->message_queue.empty()) {
                        Local<Value> argv[] = {
                            NewString(endpoint->env()->GetIsolate(), endpoint->message_queue.front().c_str()).As<Value>()
                        };
                        endpoint->MakeCallback("onmessage", 1, argv);
                        endpoint->message_queue.pop();
                    }
                });
                async_.data = this;
            }

            static void New(const FunctionCallbackInfo<Value>& args);
            static void PostMessage(const FunctionCallbackInfo<Value>& args);
            void SetRemote(MessageEndpoint* endpoint) {
                remote = endpoint;
            }
            private:
            void ReceiveMessage(string msg);
            std::mutex mutex_;
            uv_async_t async_;
            std::queue<std::string> message_queue;
            MessageEndpoint* remote;
        };
    }
}

#endif
