#ifndef HTTP_H
#define HTTP_H

#include "http_parser.h"
#include "util.h"
#include "common.h"
#include "env.h"
#include "async.h"
#include "llhttp.h"
#include "external_reference.h"

using namespace v8;
using namespace No::Util;
using namespace No::Env;
using namespace No::Base;
using namespace No::HTTP;
using namespace No::Async;

namespace No {
    namespace HTTP {
        void Init(Isolate* isolate, Local<Object> target);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
        class Parser : public AsyncWrap {
            public:
                Parser(Environment* env, Local<Object> object, llhttp_type type);
                void test() {
                    printf("hello");
                }
                static void Parse(const FunctionCallbackInfo<Value>& args); 
                static void New(const FunctionCallbackInfo<Value>& args); 
            private:
                std::unique_ptr<HTTPParser> httpparser;
        };
        
    }
}

#endif