#ifndef REQ_H
#define REQ_H

#include "common.h"
#include "env.h"
#include "async.h"

using namespace No::Async;

namespace No {
    template <typename T>
    class ReqCallback: public AsyncWrap {
        public:
        ReqCallback(No::Env::Environment *env, Local<Object> obj):AsyncWrap(env, obj) {
            _req.data = this;
            MakeWeak();
        };
        T *req() {
            return &_req;
        }
        static ReqCallback* from_req(T* req) {
            return (ReqCallback*)req->data;
        }
        protected:
        T _req;
    };
}

#endif
