#ifndef WORKER_H
#define WORKER_H

#include "common.h"
#include "env.h"
#include "async.h"
#include <queue>
#include "message.h"
#include "micro_task.h"
#include <atomic>

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Message;
using namespace No::Async;

namespace No {
    namespace Worker {
        void Init(Isolate* isolate, Local<Object> target);
        class WorkerWrap: public AsyncWrap {
            public:
            WorkerWrap(No::Env::Environment *env, Local<Object> obj, std::string filename, MessageEndpoint* message_endpoint);
            ~WorkerWrap() {
                uv_sem_destroy(&wait_worker_init_);
            }
            static void New(V8_ARGS);
            static void Start(V8_ARGS);
            static void GetMessageEndpoint(V8_ARGS);
            static void GetWorkerId(V8_ARGS);
            static void GetWorkerMessageEndpoint(V8_ARGS);
            static void Stop(V8_ARGS);
            static void Run(void *arg);
            int worker_id() {
                return _worker_id;
            }
            private:
                MessageEndpoint* _message_endpoint;
                std::string filename_;
                uv_thread_t tid;
                uv_sem_t wait_worker_init_;
                uint32_t _worker_id;
        };
    }
}

#endif
