#ifndef ENV_H
#define ENV_H

#include <fcntl.h>
#include <unistd.h>
#include "common.h"
#include "util.h"
#include <unordered_map>
#include <unordered_set>

using namespace v8;
using namespace std;
using namespace No::Util;

namespace No {
    namespace Env {
        #define PER_ISOLATE_TEMPLATE_PROPERTIES(V)                                     \
        V(message_endpoint_ctor_template, v8::FunctionTemplate)       \
        V(message_endpoint, v8::Object)       \
        V(micro_task_cb, v8::Function)   \
        V(immediate_cb, v8::Function)   

        enum {
            CONTEXT_INDEX
        } ENV_INDEX;
        
        class Environment {
            public:
                Environment(Local<Context> context);
                static Environment * GetCurrent(Local<Context> context);
                static Environment * GetCurrent(Isolate* isolate);
                static Environment * GetCurrent(const FunctionCallbackInfo<Value>& args);
                Isolate * GetIsolate() const ;
                Local<Context> GetContext() const ;
                uv_loop_t* loop() ;
                void set_argv(char **argv) ;
                char ** argv() ;
                void set_argc(int argc);
                int argc();
                bool is_main_thread();
                void set_is_main_thread(bool is_main_thread);
                bool micro_task_flag();
                void set_micro_task_flag(bool micro_task_flag);
                void start_immediate_task();
                void stop_immediate_task();
                void run_immediate_task();
                uv_buf_t allocate_managed_buffer(const size_t suggested_size);
                std::unique_ptr<v8::BackingStore> release_managed_buffer(const uv_buf_t* buf);
                #define V(PropertyName, TypeName)                                              \
                v8::Local<TypeName> PropertyName() const;                             \
                void set_##PropertyName(v8::Local<TypeName> value);              
                PER_ISOLATE_TEMPLATE_PROPERTIES(V)
                #undef V
            private:
                Global<Context> _context;
                Isolate * _isolate;
                uv_loop_t _loop;
                int _argc;
                char **_argv;
                bool _is_main_thread = false;
                bool _micro_task_flag;
                uv_check_t _immediate;
                std::unordered_map<char*, std::unique_ptr<v8::BackingStore>>released_allocated_buffers_;
                #define V(PropertyName, TypeName)   \                                          
                v8::Eternal<TypeName> PropertyName ## _;
                PER_ISOLATE_TEMPLATE_PROPERTIES(V)
                #undef V
        };
    }
}

#endif

