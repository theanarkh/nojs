#include "addon.h"
#include "env.h"

using namespace No::Env;

namespace No {
    namespace Addon {
        Module::Module(const char* filename, int flags): filename_(filename), flags_(flags) {

        }
        bool Module::Open() {
            int ret = uv_dlopen(filename_.c_str(), &lib_);
            if (ret == 0) {
                handle_ = static_cast<void*>(lib_.handle);
                return true;
            }
            errmsg_ = uv_dlerror(&lib_);
            uv_dlclose(&lib_);
            return false;
        }

        void Module::Close() {
            if (handle_ == nullptr) return;
            uv_dlclose(&lib_);
            handle_ = nullptr;
        }

        void* Module::GetSymbolAddress(const char* name) {
            void* address;
            if (0 == uv_dlsym(&lib_, name, &address)) return address;
            return nullptr;
        }

        using InitHook = void (*)(Local<Object> module,
                                 Local<Object> exports,
                                 Local<Context> context);
        void Load(V8_ARGS) {
            V8_ISOLATE
            V8_CONTEXT
            Environment* env = Environment::GetCurrent(args);
            String::Utf8Value filename(isolate, args[0]);
            Module* m = new Module(*filename, 0);
            if (!m->Open()) {
                return; 
            }
            InitHook func = reinterpret_cast<InitHook>(m->GetSymbolAddress("Init"));
            if (func == nullptr) {
                m->Close();
                return;
            }
            Local<Object> module;
            Local<Object> exports;
            if (!args[1]->ToObject(context).ToLocal(&module) ||
                !args[2]->ToObject(context).ToLocal(&exports)) {
                return; 
            }
            func(module, exports, context);
            
        }

        void Init(Isolate* isolate, Local<Object> target) {
            Local<Object> obj = Object::New(isolate);
            Environment *env = Environment::GetCurrent(isolate);
            Local<FunctionTemplate> tpl = No::Util::NewFunctionTemplate(isolate, Load);
            SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "load"), tpl);

            ObjectSet(isolate, target, "addon", obj);
        }
    }
}