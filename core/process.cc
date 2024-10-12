#include "process.h"

namespace No {
    namespace Process {

        void Cwd(V8_ARGS) {
            V8_ISOLATE
            char * cwd = getcwd(nullptr, 0);
            V8_RETURN(NewString(isolate, cwd));
        }

        void Exit(V8_ARGS) {
            int status = 0;
            if (args.Length() > 0) {
                status = args[0].As<Integer>()->Value();
            }
            exit(status);
        }

        void Kill(V8_ARGS) {
            Environment* env = Environment::GetCurrent(args);
            Local<Context> context = env->GetContext();

            int pid;
            if (!args[0]->Int32Value(context).To(&pid)) return;
            int sig;
            if (!args[1]->Int32Value(context).To(&sig)) return;
            V8_RETURN(Number::New(env->GetIsolate(), uv_kill(pid, sig)));      
        }

        
        void Execve(V8_ARGS) {
            V8_ISOLATE
            int length = args.Length();
            char** ptr = new char*[length + 1];
            int i = 0;
            for (i = 0; i < length; i++) {
                String::Utf8Value arg(isolate, args[i]);
                ptr[i] = strdup(*arg);
            }
            ptr[i] = NULL;
            char *env[] = { (char*)"NO_WORKER=1", NULL };

            pid_t pid = fork();
            if (pid == 0) {
                execve(ptr[0], ptr, env);
                write(1, strerror(errno), sizeof(strerror(errno)));
                exit(-1);
            }
            int status = 0;
            waitpid(pid, &status, 0);
            if (ptr) {
                for (int i = 0; i < length && ptr[i]; i++) {
                    free(ptr[i]);
                }
                delete [] ptr;
            }
            V8_RETURN(Number::New(isolate, status));      
        }

        void EnvGetter(Local<Name> name, const v8::PropertyCallbackInfo<v8::Value>& args) {
            V8_ISOLATE     
            String::Utf8Value key(isolate, name);
            char * value = getenv(*key);
            if (value == nullptr) {
                return V8_RETURN(Local<Value>());
            }
            V8_RETURN(NewString(isolate, value));      
        }

        static void EnvSetter(Local<Name> key,
                            Local<Value> value,
                            const PropertyCallbackInfo<Value>& args) {                                                  
            V8_ISOLATE
            V8_CONTEXT
            Local<String> key_strng = key.As<String>();
            Local<String> value_string = value.As<String>();;
            if (key.IsEmpty() || value.IsEmpty()) {
                return;
            }

            {
                String::Utf8Value key(isolate, key_strng);
                String::Utf8Value value(isolate, value_string);
                setenv(*key, *value, 1);
            }
        }

        static void EnvEnumerator(const PropertyCallbackInfo<Array>& info) {
            // TODO
        }

        static void EnvDeleter(Local<Name> key,
                            const PropertyCallbackInfo<Boolean>& args) {
            V8_ISOLATE                        
            Local<String> keyObject = key.As<String>();
            String::Utf8Value key_stirng(isolate, keyObject);
            unsetenv(*key_stirng);
            V8_RETURN(true)
        }

        void Init(Isolate* isolate, Local<Object> target) {
            Environment * env = Environment::GetCurrent(isolate);
            Local<Context> context = env->GetContext();

            Local<Object> obj = Object::New(isolate);

            SetFunction(context, obj, NewString(isolate, "cwd"), No::Util::NewFunctionTemplate(isolate, Cwd));
            SetFunction(context, obj, NewString(isolate, "kill"), No::Util::NewFunctionTemplate(isolate, Kill));
            SetFunction(context, obj, NewString(isolate, "exit"), No::Util::NewFunctionTemplate(isolate, Exit));
            SetFunction(context, obj, NewString(isolate, "execve"), No::Util::NewFunctionTemplate(isolate, Execve));

            Local<ObjectTemplate> env_proxy_template = ObjectTemplate::New(isolate);
            NamedPropertyHandlerConfiguration cfg = NamedPropertyHandlerConfiguration(
                EnvGetter,
                EnvSetter,
                nullptr,
                EnvDeleter,
                EnvEnumerator,
                Local<Value>(),
                PropertyHandlerFlags::kHasNoSideEffect);
            env_proxy_template->SetHandler(cfg);
            ObjectSet(isolate, obj, "env", env_proxy_template->NewInstance(context).ToLocalChecked());

            int argc = env->argc();
            char ** argv = env->argv();
            Local<Array> arr = Array::New(isolate, argc);
            for (int i = 0; i < argc; i++) {
                arr->Set(context, Number::New(isolate, i), NewString(isolate, argv[i])).Check();
            }
            ObjectSet(isolate, obj, "argv", arr);
            ObjectSet(isolate, obj, "isMainThread", v8::Boolean::New(isolate, env->is_main_thread()));
            char * value = getenv("NO_WORKER");
            ObjectSet(isolate, obj, "isMainProcess", v8::Boolean::New(isolate, value == NULL));
            ObjectSet(isolate, obj, "execPath", NewString(isolate, argv[0]));
            ObjectSet(isolate, obj, "pid", Number::New(isolate, uv_os_getpid()));
            ObjectSet(isolate, obj, "ppid", Number::New(isolate, uv_os_getppid()));
            ObjectSet(isolate, target, "process", obj);
        }
    }
}