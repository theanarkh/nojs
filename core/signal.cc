
#include "signal.h"

namespace No {
    namespace Signal {
        void SignalWrap::New(V8_ARGS) {
            Environment* env = Environment::GetCurrent(args);
            new SignalWrap(env, args.This());
        } 

        void Kill(V8_ARGS) {
            V8_ISOLATE
            int pid = args[0].As<Integer>()->Value(); 
            int sig = args[1].As<Integer>()->Value(); 
            V8_RETURN(Integer::New(isolate, kill(pid, sig)))
        }

        void SignalWrap::OnSignal(uv_signal_t* handle, int signum) {
            SignalWrap* wrap = (SignalWrap*)handle->data;
            Environment* env = wrap->env();
            HandleScope handle_scope(env->GetIsolate());
            Context::Scope context_scope(env->GetContext());
            Local<Value> arg = Integer::New(env->GetIsolate(), signum);
            wrap->MakeCallback("onsignal", 1, &arg);
        }

        void SignalWrap::Start(V8_ARGS) {
            SignalWrap* wrap = (SignalWrap*)Base::BaseObject::unwrap(args.Holder()) ;
            Environment* env = wrap->env();
            int signum;
            if (!args[0]->Int32Value(env->GetContext()).To(&signum)) return;
            int err = uv_signal_start(&wrap->handle_, SignalWrap::OnSignal, signum);
            args.GetReturnValue().Set(err);
        }

        void SignalWrap::Stop(V8_ARGS) {
            SignalWrap* wrap = (SignalWrap*)Base::BaseObject::unwrap(args.Holder());
            Environment* env = wrap->env();
            int err = uv_signal_stop(&wrap->handle_);
            args.GetReturnValue().Set(err);
        }
        
        void Init(Isolate* isolate, Local<Object> target) {
            Local<Object> object = Object::New(isolate);
            Environment *env = Environment::GetCurrent(isolate);
            Local<FunctionTemplate> signal = No::Util::NewFunctionTemplate(isolate, SignalWrap::New);
            SetProtoMethod(isolate, signal, "start", SignalWrap::Start);
            SetProtoMethod(isolate, signal, "stop", SignalWrap::Stop);
            signal->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
            signal->Inherit(HandleWrap::GetConstructorTemplate(env));
            SetFunction(isolate->GetCurrentContext(), object, NewString(isolate, "Signal"), signal);
            ObjectSet(isolate, target, "signal", object);

            SetFunction(isolate->GetCurrentContext(), object, NewString(isolate, "kill"), No::Util::NewFunctionTemplate(isolate, Kill));
      
            Local<Object> constant = Object::New(isolate);
            Local<Object> sigs = Object::New(isolate);
            #define SIG_LIST(Set) \
                Set(SIGHUP) \
                Set(SIGINT) \
                Set(SIGQUIT) \
                Set(SIGILL) \
                Set(SIGTRAP) \
                Set(SIGABRT) \
                Set(SIGBUS) \
                Set(SIGFPE) \
                Set(SIGKILL) \
                Set(SIGUSR1) \
                Set(SIGSEGV) \
                Set(SIGUSR2) \
                Set(SIGPIPE) \
                Set(SIGALRM) \
                Set(SIGTERM) \
                Set(SIGCHLD) \
                Set(SIGCONT) \
                Set(SIGSTOP) \
                Set(SIGTSTP) \
                Set(SIGTTIN) \
                Set(SIGTTOU) \
                Set(SIGURG) \
                Set(SIGXCPU) \
                Set(SIGXFSZ) \
                Set(SIGVTALRM) \
                Set(SIGPROF) \
                Set(SIGWINCH) \
                Set(SIGIO) \
                Set(SIGSYS)
                #define Set(val) ObjectSet(isolate, sigs, #val, Number::New(isolate, val));
                    SIG_LIST(Set)
                #undef Set
            #undef SIG_LIST

            ObjectSet(isolate, constant, "SIG", sigs);
            ObjectSet(isolate, object, "constant", constant);
            ObjectSet(isolate, target, "signal", object);
        }

        static void RegisterExternalReferences(ExternalReferenceRegistry* registry) {
            registry->Register(SignalWrap::New);
            registry->Register(SignalWrap::Start);
            registry->Register(SignalWrap::Stop);
            registry->Register(Kill);
        }
    }
}

NODE_BINDING_EXTERNAL_REFERENCE(signal, No::Signal::RegisterExternalReferences)
