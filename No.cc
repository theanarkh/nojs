
#include "libplatform/libplatform.h"
#include "v8.h"
#include "core/allocator.h"
#include "core/env.h"
#include "core/micro_task.h"
#include "core/core.h"

using namespace v8;
using namespace No::Core;
using namespace No::Env;

int main(int argc, char* argv[]) {
  setvbuf(stdout, nullptr, _IONBF, 0);
  setvbuf(stderr, nullptr, _IONBF, 0);
  
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  std::unique_ptr<Platform> platform = platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();
  Isolate::CreateParams create_params;
  No::NoMemoryAllocator::NoArrayBufferAllocator* array_buffer_allocator = new No::NoMemoryAllocator::NoArrayBufferAllocator();
  create_params.array_buffer_allocator = array_buffer_allocator;
  Isolate* isolate = Isolate::New(create_params);
  {
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
    Local<Context> context = Context::New(isolate, nullptr, global);
    Context::Scope context_scope(context);
    Environment * env = new Environment(context);
    env->set_argv(argv);
    env->set_argc(argc);
    env->set_is_main_thread(true);
    env->set_array_buffer_allocator(array_buffer_allocator);
    {
      No::MicroTask::MicroTaskScope microTaskScope(env);
      No::Core::Run(env);
    }
    delete env;
  }

  isolate->Dispose();
  delete create_params.array_buffer_allocator;
  v8::V8::Dispose();
  return 0;
}