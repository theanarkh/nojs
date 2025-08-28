
#include "libplatform/libplatform.h"
#include "v8.h"
#include "core/allocator.h"
#include "core/env.h"
#include "core/micro_task.h"
#include "core/core.h"
#include "core/util.h"
#include "core/external_reference.h"

using namespace v8;
using namespace No::Core;
using namespace No::Env;

void BuildSnapshot(int argc, char* argv[]) {
  {
    No::ExternalReferenceRegistry external_reference_registry;
    const std::vector<intptr_t>& external_references = external_reference_registry.external_references();
    v8::SnapshotCreator creator(external_references.data());
    Isolate* isolate = creator.GetIsolate();
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
      {
        No::MicroTask::MicroTaskScope microTaskScope(env);
        No::Core::Run(env);
      }
      creator.SetDefaultContext(context, v8::SerializeInternalFieldsCallback());
      env->serialize(&creator);
      delete env;
    }
    v8::StartupData blob = creator.CreateBlob(v8::SnapshotCreator::FunctionCodeHandling::kKeep);
    std::ofstream out("snapshot.blob", std::ios::out | std::ios::binary);
    out.write(blob.data, blob.raw_size);
    out.close();
  }
}

void Start(int argc, char* argv[]) {
  Isolate::CreateParams create_params;
  if (strcmp(argv[1], "--snapshot_blob") == 0) {
    std::ifstream file("snapshot.blob", std::ios::in | std::ios::binary);
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
      std::cerr << "读取文件失败" << std::endl;
      return;
    }
    file.close();
    v8::StartupData blob = {buffer.data(), static_cast<int>(buffer.size())};
    create_params.snapshot_blob = &blob;
  }

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
}

int main(int argc, char* argv[]) {
  No::Util::Timer timer;
  setvbuf(stdout, nullptr, _IONBF, 0);
  setvbuf(stderr, nullptr, _IONBF, 0);

  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  std::unique_ptr<Platform> platform = platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();
 
  if (argc > 1) {
    if (strcmp(argv[1], "--build_snapshot") == 0) {
      BuildSnapshot(argc, argv);
    } else {
      Start(argc, argv);
    }
  }

  v8::V8::Dispose();
  v8::V8::DisposePlatform();
  return 0;
}