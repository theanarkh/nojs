#ifndef ENV_H
#define ENV_H

#include <fcntl.h>
#include <unistd.h>
#include "common.h"
#include "util.h"
#include "allocator.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "addon.h"
#include "external_reference.h"
#include "snapshot.h"

using namespace v8;
using namespace std;
using namespace No::Util;

namespace No {
    namespace Env {
        #define PER_ISOLATE_TEMPLATE_PROPERTIES(V)                                     \
        V(message_endpoint_ctor_template, v8::FunctionTemplate)       \
        V(handle_wrap_ctor_template, v8::FunctionTemplate)        

        #define PER_ISOLATE_OBJECT_PROPERTIES(V)                                     \
        V(message_endpoint, v8::Object)      

        #define PER_ISOLATE_FUNCTION_PROPERTIES(V)                                     \
        V(micro_task_cb, v8::Function)   \
        V(immediate_cb, v8::Function)   \
        V(snapshot_serialize_cb, v8::Function)   \
        V(snapshot_deserialize_cb, v8::Function)  \
        V(snapshot_deserialize_main, v8::Function)   
    
        enum {
            CONTEXT_INDEX
        } ENV_INDEX;
        
        class Environment {
            public:
                Environment(Local<Context> context);
                ~Environment();
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
                void set_worker_id(uint32_t worker_id);
                uint32_t worker_id();
                bool is_main_thread();
                void set_is_main_thread(bool is_main_thread);
                bool micro_task_flag();
                void set_micro_task_flag(bool micro_task_flag);
                void start_immediate_task();
                void stop_immediate_task();
                void run_immediate_task();
                void register_addon(No::Addon::Module* module);
                uv_buf_t allocate_managed_buffer(const size_t suggested_size);
                std::unique_ptr<v8::BackingStore> release_managed_buffer(const uv_buf_t* buf);
                No::NoMemoryAllocator::NoArrayBufferAllocator* array_buffer_allocator();
                void set_array_buffer_allocator(No::NoMemoryAllocator::NoArrayBufferAllocator* allocator);
                void serialize(v8::SnapshotCreator* creator, No::Snapshot::SnapshotData* snapshot_data);
                void deserialize(No::Snapshot::SnapshotData* snapshot_data);
                void run_snapshot_serial_callback();
                void run_snapshot_deserial_callback();
                bool is_build_snapshot();
                void run_snapshot_deserialize_main();
                #define V(PropertyName, TypeName)                                              \
                v8::Local<TypeName> PropertyName() const;                             \
                void set_##PropertyName(v8::Local<TypeName> value);              
                PER_ISOLATE_TEMPLATE_PROPERTIES(V)
                PER_ISOLATE_OBJECT_PROPERTIES(V)
                PER_ISOLATE_FUNCTION_PROPERTIES(V)
                #undef V
            private:
                Global<Context> _context;
                Isolate * _isolate;
                uv_loop_t _loop;
                int _argc;
                uint32_t _worker_id = 0;
                char **_argv;
                bool _is_main_thread = false;
                bool _micro_task_flag;
                uv_check_t _immediate;
                uv_idle_t _immediate_idle;
                std::vector<std::unique_ptr<No::Addon::Module>> _addons;
                No::NoMemoryAllocator::NoArrayBufferAllocator* _array_buffer_allocator = nullptr;
                std::unordered_map<char*, std::unique_ptr<v8::BackingStore>>released_allocated_buffers_;
                #define V(PropertyName, TypeName)   \
                v8::Eternal<TypeName> PropertyName ## _;
                PER_ISOLATE_TEMPLATE_PROPERTIES(V)
                PER_ISOLATE_OBJECT_PROPERTIES(V)
                PER_ISOLATE_FUNCTION_PROPERTIES(V)
                #undef V
        };
    }
}

#endif

