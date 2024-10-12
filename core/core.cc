#include "core.h"

void No::Core::RegisterBuiltins(Isolate * isolate, Local<Object> No) {
    Local<Object> target = Object::New(isolate);
    Process::Init(isolate, target); 
    Console::Init(isolate, target);
    Loader::Init(isolate, target);
    FS::Init(isolate, target);
    Util::Init(isolate, target);
    VM::Init(isolate, target);
    TCP::Init(isolate, target);
    Pipe::Init(isolate, target);
    UDP::Init(isolate, target);
    Timer::Init(isolate, target);
    HTTP::Init(isolate, target);
    DNS::Init(isolate, target);
    Worker::Init(isolate, target);
    Message::Init(isolate, target);
    MicroTask::Init(isolate, target);
    Immediate::Init(isolate, target);
    FSWatcher::Init(isolate, target);
    ChildProcess::Init(isolate, target);
    OS::Init(isolate, target);
    Signal::Init(isolate, target);
    ObjectSet(isolate, No, "buildin", target);
}

void No::Core::Run(Environment * env) {
    Isolate * isolate = env->GetIsolate();
    Local<Context> context = env->GetContext();
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    Context::Scope context_scope(context);
    Local<Object> No = Object::New(isolate);
    RegisterBuiltins(isolate, No);
    Local<Object> global = context->Global();
    global->Set(context, NewString(isolate, "global"), global).Check();
    int fd = open("No.js", 0 , O_RDONLY);
    if (fd == -1) {
        perror("failed to open No.js");
        return;
    }
    std::string content;
    char buffer[4096];
    while (1)
    {
        memset(buffer, 0, 4096);
        int ret = read(fd, buffer, 4096);
        if (ret == -1) {
            perror("failed to read No.js");
            return;
        }
        if (ret == 0) {
            break;
        }
        content.append(buffer, ret);
    }
    close(fd);
    ScriptCompiler::Source script_source(NewString(isolate, content.c_str()));
    Local<String> params[] = {
        NewString(isolate, "No"),
    };
    Local<Value> argv[] = {
        No.As<Value>()
    };
    Local<Function> func = ScriptCompiler::CompileFunction(context, &script_source, 1, params, 0, nullptr).ToLocalChecked();
    func->Call(context, context->Global(), 1, argv).ToLocalChecked();  
    uv_run(env->loop(), UV_RUN_DEFAULT);
}