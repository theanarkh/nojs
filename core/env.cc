
#include "env.h"

namespace No {
    namespace Env {
        #define V(PropertyName, TypeName)                                              \
        v8::Local<TypeName> Environment::PropertyName() const {               \
            return PropertyName##_.Get(_isolate);                                      \
        }                                                                            \
        void Environment::set_##PropertyName(v8::Local<TypeName> value) {     \
            PropertyName##_.Set(_isolate, value);                                      \
        }
        PER_ISOLATE_TEMPLATE_PROPERTIES(V)
        #undef V

        Environment::Environment(Local<Context> context) {
            Isolate* isolate = context->GetIsolate();
            _context.Reset(isolate, context);
            _isolate = isolate;
            uv_loop_init(&_loop);
            context->SetAlignedPointerInEmbedderData(CONTEXT_INDEX, this);
            uv_check_init(&_loop, &_immediate);
            _immediate.data = this;
        }

        Environment::~Environment() {
            for (size_t i = 0; i < _addons.size(); i++) {
                _addons[i]->Close();
            }
            _addons.clear();
        }

        Environment* Environment::GetCurrent(Local<Context> context) {
            return (Environment*)context->GetAlignedPointerFromEmbedderData(CONTEXT_INDEX);
        }

        Environment* Environment::GetCurrent(Isolate* isolate) {
            return GetCurrent(isolate->GetCurrentContext());
        }

        Environment* Environment::GetCurrent(const FunctionCallbackInfo<Value>& args) {
            return GetCurrent(args.GetIsolate());
        }

        Isolate * Environment::GetIsolate() const {
            return _isolate;
        }

        Local<Context> Environment::GetContext() const {
            return PersistentToLocal::Strong(_context);
        }

        uv_loop_t* Environment::loop() {
            return &_loop;
        }
        void Environment::set_argv(char **argv) {
            _argv = argv;
        }
        char ** Environment::argv() {
            return _argv;
        }
        void Environment::set_argc(int argc) {
            _argc = argc;
        }

        int Environment::argc() {
            return _argc;
        }

        void Environment::set_worker_id(uint32_t worker_id) {
            _worker_id = worker_id;
        }

        uint32_t Environment::worker_id() {
            return _worker_id;
        }

        bool Environment::is_main_thread() {
            return _is_main_thread;
        }

        void Environment::set_is_main_thread(bool is_main_thread) {
            _is_main_thread = is_main_thread;
        }

        bool Environment::micro_task_flag() {
            return _micro_task_flag;
        }

        void Environment::set_micro_task_flag(bool micro_task_flag) {
            _micro_task_flag = micro_task_flag;
        }

        void Environment::start_immediate_task() {
            uv_check_start(&_immediate, [](uv_check_t *handle) {
                Environment *env = (Environment *)handle->data;
                env->run_immediate_task();
            });
        }

        void Environment::stop_immediate_task() {
            uv_check_stop(&_immediate);
        }

        void Environment::run_immediate_task() {
            immediate_cb()->Call(GetContext(), GetContext()->Global(), 0, nullptr).ToLocalChecked();
        }
        
        // Copy from Node.js env.h
        uv_buf_t Environment::allocate_managed_buffer(const size_t suggested_size) {
            std::unique_ptr<v8::BackingStore> bs =
                v8::ArrayBuffer::NewBackingStore(_isolate, suggested_size);
            uv_buf_t buf = uv_buf_init(static_cast<char*>(bs->Data()), bs->ByteLength());
            released_allocated_buffers_.emplace(buf.base, std::move(bs));
            return buf;
        }

        std::unique_ptr<v8::BackingStore> Environment::release_managed_buffer(
            const uv_buf_t* buf) {
            std::unique_ptr<v8::BackingStore> bs;
            if (buf->base != nullptr) {
                auto it = released_allocated_buffers_.find(buf->base);
                bs = std::move(it->second);
                released_allocated_buffers_.erase(it);
            }
            return bs;
        }
        No::NoMemoryAllocator::NoArrayBufferAllocator* Environment::array_buffer_allocator() {
            return _array_buffer_allocator;
        }
        
        void Environment::set_array_buffer_allocator(No::NoMemoryAllocator::NoArrayBufferAllocator* allocator) {
            _array_buffer_allocator = allocator;
        }

        void Environment::register_addon(No::Addon::Module* module) {
            _addons.push_back(std::unique_ptr<No::Addon::Module>(module));
        }
    }
}