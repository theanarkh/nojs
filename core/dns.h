#ifndef DNS_H
#define DNS_H

#include "req.h"
#include "util.h"
#include "common.h"
#include "env.h"
#include "async.h"
#include "external_reference.h"
#include <ares.h>
#include <map>

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Async;

namespace No {
    namespace DNS {
        void Init(Isolate* isolate, Local<Object> target);
        void Lookup(V8_ARGS);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
        class LookupReq : public ReqCallback<uv_getaddrinfo_t> {
            public:
            LookupReq(No::Env::Environment *env, Local<Object> obj): ReqCallback<uv_getaddrinfo_t> (env, obj) {}
        };

        class QueryReq : public AsyncWrap {
            public:
            QueryReq(No::Env::Environment *env, Local<Object> obj): AsyncWrap(env, obj) {}
        };

        class ChannelWrap;

        struct ChannelTask {
            ChannelWrap* channel;
            ares_socket_t sock;
            uv_poll_t poll_watcher;
            static ChannelTask* Create(ChannelWrap* channel, ares_socket_t sock);
        };

        class ChannelWrap: public AsyncWrap {
            public:
            ChannelWrap(No::Env::Environment *env, Local<Object> obj, int timeout, int retries);
            ~ChannelWrap();
            static void New(V8_ARGS);
            static void Resolve(V8_ARGS);
            
            void StartTimer();
            void CloseTimer();
            uv_timer_t* timer_handle() {
                return timer_handle_;
            }
            ares_channel cares_channel() {
                return _channel;
            }
            std::map<ares_socket_t, ChannelTask *>* task_list() {
                return &_tasks;
            }
            int init();
            private:
            uv_timer_t* timer_handle_ = nullptr;
            bool _library_inited;
            ares_channel  _channel;
            int _timeout;
            int _retries;
            std::map<ares_socket_t, ChannelTask *> _tasks;
        };
        
    }
}

#endif
