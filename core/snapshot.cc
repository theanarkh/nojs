#include "snapshot.h"
#include "env.h"

using namespace No::Env;

namespace No {
    namespace Snapshot {
        void SetSerializeCallback(V8_ARGS) {
            Environment *env = Environment::GetCurrent(args.GetIsolate());
            env->set_snapshot_serialize_cb(args[0].As<Function>());
        }

        void SetDeSerializeCallback(V8_ARGS) {
            Environment *env = Environment::GetCurrent(args.GetIsolate());
            env->set_snapshot_deserialize_cb(args[0].As<Function>());
        }
        
        void SetDeserializeMain(V8_ARGS) {
            Environment *env = Environment::GetCurrent(args.GetIsolate());
            env->set_snapshot_deserialize_main(args[0].As<Function>());
        }

        void IsBuildSnapshot(V8_ARGS) {
            Environment *env = Environment::GetCurrent(args.GetIsolate());
            V8_RETURN(Boolean::New(args.GetIsolate(), env->is_build_snapshot()))
        }

        void HasStartupSnapshot(V8_ARGS) {
            Environment *env = Environment::GetCurrent(args.GetIsolate());
            V8_RETURN(Boolean::New(args.GetIsolate(), env->has_startup_snapshot()))
        }
      
        void Init(Isolate* isolate, Local<Object> target) {
            Local<Object> obj = Object::New(isolate);
            SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "addSerialCallback"), No::Util::NewFunctionTemplate(isolate, SetSerializeCallback));
            SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "addDeSerialCallback"), No::Util::NewFunctionTemplate(isolate, SetDeSerializeCallback));
            SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "setDeserializeMain"), No::Util::NewFunctionTemplate(isolate, SetDeserializeMain));
            SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "isBuildSnapshot"), No::Util::NewFunctionTemplate(isolate, IsBuildSnapshot));
            SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "hasStartupSnapshot"), No::Util::NewFunctionTemplate(isolate, HasStartupSnapshot));
            ObjectSet(isolate, target, "snapshot", obj);
        }

        static void RegisterExternalReferences(ExternalReferenceRegistry* registry) {
            registry->Register(SetSerializeCallback);
            registry->Register(SetDeSerializeCallback);
            registry->Register(IsBuildSnapshot);
            registry->Register(SetDeserializeMain);
            registry->Register(HasStartupSnapshot);
        }
    }
}

NODE_BINDING_EXTERNAL_REFERENCE(snapshot, No::Snapshot::RegisterExternalReferences)
