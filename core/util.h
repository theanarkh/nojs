#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include <csignal>

using namespace v8;

namespace No {
    namespace Util {
        void Log(const char * str);
        Local<String> NewString(Isolate * isolate, const char * str, NewStringType type = NewStringType::kNormal);
        void setMethod(Isolate *isolate, Local<ObjectTemplate> recv, const char *name, FunctionCallback callback);
        void setObjectTemplateValue(Isolate *isolate, Local<ObjectTemplate> recv, const char *name, Local<Data> value);
        void ObjectSet(Isolate *isolate, Local<Object> recv, const char *name, Local<Value> value);
        Local<v8::FunctionTemplate> NewDefaultFunctionTemplate(v8::Isolate* isolate);
        Local<v8::FunctionTemplate> NewFunctionTemplate(v8::Isolate* isolate, v8::FunctionCallback callback);
       void SetProtoMethod(v8::Isolate* isolate,
                    Local<v8::FunctionTemplate> that,
                    const char* name,
                    v8::FunctionCallback callback);
        void SetFunction(Local<Context> context,
                            Local<Object> that,
                            Local<String> name,
                            Local<FunctionTemplate> tmpl) ;
        class PersistentToLocal {
            public:
                template <typename TypeName>
                static Local<TypeName> Strong(const PersistentBase<TypeName>& persistent) {
                    return *reinterpret_cast<Local<TypeName>*>(
                        const_cast<PersistentBase<TypeName>*>(&persistent)
                    );
                };
        };
        template <typename T>
        struct KeyCompare {
            bool operator()(T* x, T* k) const {
                return x->id >= k->id;
            }
        };

        class IdGenerator {
            public:
                IdGenerator(uint64_t _id = 0): id(_id) {}
                uint64_t getNextId() {
                    return ++id;
                }
            private:
                uint64_t id;
        };
        void GetError(V8_ARGS);
        void Init(Isolate* isolate, Local<Object> target);
        const char* SignoTostring(int signo);
        int SockaddrForfamily(int address_family,
                              const char* address,
                              const unsigned short port,
                              struct sockaddr_storage* addr);
    }
}

#endif 
