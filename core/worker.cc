#include "worker.h"
#include "core.h"
#include "allocator.h"

namespace No {
    namespace Worker {
      static std::atomic<uint32_t> next_worker_id{0};

      WorkerWrap::WorkerWrap(No::Env::Environment *env, Local<Object> obj, std::string filename, MessageEndpoint* message_endpoint): AsyncWrap(env, obj), filename_(filename), _message_endpoint(message_endpoint), _worker_id(++next_worker_id) {
        uv_sem_init(&wait_worker_init_, 0);
      }
      void WorkerWrap::New(const FunctionCallbackInfo<Value>& args) {
        Environment *env = Environment::GetCurrent(args);
        v8::String::Utf8Value filename(args.GetIsolate(), args[0]);
        Local<FunctionTemplate> ctor_templ = No::Message::GetMessageEndpointConstructorTemplate(env);
        Local<Object> message_endpoint;
        if (!ctor_templ->InstanceTemplate()->NewInstance(env->GetContext()).ToLocal(&message_endpoint)) {
          return;
        }
        new WorkerWrap(env, args.This(), std::string(*filename), new No::Message::MessageEndpoint(env, message_endpoint));
      }

      void WorkerWrap::GetMessageEndpoint(V8_ARGS){
        No::Env::Environment* env = No::Env::Environment::GetCurrent(args);
        args.GetReturnValue().Set(env->message_endpoint());
      }

      void WorkerWrap::GetWorkerId(V8_ARGS){
        No::Env::Environment* env = No::Env::Environment::GetCurrent(args);
        args.GetReturnValue().Set(env->worker_id());
      }

      void WorkerWrap::GetWorkerMessageEndpoint(V8_ARGS){
        WorkerWrap* wrap = (WorkerWrap*)Base::BaseObject::unwrap(args.Holder());
          args.GetReturnValue().Set(wrap->_message_endpoint->object()); 
      }

      void WorkerWrap::Run(void *arg) {
        WorkerWrap * wrap = (WorkerWrap *)arg;
        char* argv[] = {
          wrap->env()->argv()[0],
          (char *)wrap->filename_.c_str()
        };
        Isolate::CreateParams create_params;
        No::NoMemoryAllocator::NoArrayBufferAllocator* array_buffer_allocator = new No::NoMemoryAllocator::NoArrayBufferAllocator();
        create_params.array_buffer_allocator = array_buffer_allocator;
        Isolate* isolate = Isolate::New(create_params);
        {
          Isolate::Scope isolate_scope(isolate);
          HandleScope handle_scope(isolate);
          Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
          Local<Context> context = Context::New(isolate, nullptr, global);
          Environment * env = new Environment(context);
          env->set_argc(2);
          env->set_argv(argv);
          env->set_is_main_thread(false);
          env->set_array_buffer_allocator(array_buffer_allocator);
          env->set_worker_id(wrap->worker_id());
          Local<FunctionTemplate> ctor_templ = No::Message::GetMessageEndpointConstructorTemplate(env);
          Local<Object> message_endpoint_obj;
          if (!ctor_templ->InstanceTemplate()->NewInstance(context).ToLocal(&message_endpoint_obj)) {
            return;
          }
          env->set_message_endpoint(message_endpoint_obj);

          No::Message::MessageEndpoint* message_endpoint = new No::Message::MessageEndpoint(env, message_endpoint_obj);

          message_endpoint->SetRemote(wrap->_message_endpoint);
          wrap->_message_endpoint->SetRemote(message_endpoint);

          uv_sem_post(&wrap->wait_worker_init_);
          {
            No::MicroTask::MicroTaskScope microTaskScope(env);
            Core::Run(env);
          }

          delete env;
        }

        isolate->Dispose();
        delete create_params.array_buffer_allocator;

      }

      void WorkerWrap::Start(const FunctionCallbackInfo<Value>& args) {
        WorkerWrap* wrap = (WorkerWrap*)Base::BaseObject::unwrap(args.Holder());
        int ret = uv_thread_create(&wrap->tid, Run, wrap);
        uv_sem_wait(&wrap->wait_worker_init_);
        Isolate* isolate = wrap->env()->GetIsolate();
        wrap->object()->Set(wrap->env()->GetContext(), NewString(isolate, "workerId"),
                              Integer::New(isolate,
                                          wrap->worker_id())).Check();
                                          
        args.GetReturnValue().Set(ret);
      }

      void WorkerWrap::Stop(const FunctionCallbackInfo<Value>& args) {
        WorkerWrap* wrap = (WorkerWrap*)Base::BaseObject::unwrap(args.Holder());
        // TODO
        uv_thread_join(&wrap->tid);
      }

      void Init(Isolate* isolate, Local<Object> target) {
        Local<Object> obj = Object::New(isolate);

        Local<FunctionTemplate> tpl = No::Util::NewFunctionTemplate(isolate, WorkerWrap::New);
        SetProtoMethod(isolate, tpl, "start", WorkerWrap::Start);
        SetProtoMethod(isolate, tpl, "stop", WorkerWrap::Stop);
        SetProtoMethod(isolate, tpl, "getMessageEndpoint", WorkerWrap::GetWorkerMessageEndpoint);
        tpl->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "Worker"), tpl);

        Local<FunctionTemplate> channel_tpl = No::Util::NewFunctionTemplate(isolate, WorkerWrap::GetMessageEndpoint);
        Local<FunctionTemplate> worker_id_tpl = No::Util::NewFunctionTemplate(isolate, WorkerWrap::GetWorkerId);
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "getMessageEndpoint"), channel_tpl);
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "getWorkerId"), worker_id_tpl);
       
        ObjectSet(isolate, target, "worker", obj);
      }

      static void RegisterExternalReferences(ExternalReferenceRegistry* registry) {
        registry->Register(WorkerWrap::New);
        registry->Register(WorkerWrap::Start);
        registry->Register(WorkerWrap::Stop);
        registry->Register(WorkerWrap::GetWorkerMessageEndpoint);
        registry->Register(WorkerWrap::GetMessageEndpoint);
        registry->Register(WorkerWrap::GetWorkerId);
      }
    }
}

NODE_BINDING_EXTERNAL_REFERENCE(worker, No::Worker::RegisterExternalReferences)
