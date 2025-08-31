
#include "libplatform/libplatform.h"
#include "v8.h"
#include "core/allocator.h"
#include "core/env.h"
#include "core/micro_task.h"
#include "core/core.h"
#include "core/util.h"
#include "core/snapshot.h"
#include "core/external_reference.h"

using namespace v8;
using namespace No::Core;
using namespace No::Env;

void BuildSnapshot(int argc, char* argv[]) {
  {
    No::Snapshot::SnapshotData snapshot_data;
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
      env->run_snapshot_serial_callback();
      creator.SetDefaultContext(context, v8::SerializeInternalFieldsCallback());
      env->serialize(&creator, &snapshot_data);
      delete env;
    }
    snapshot_data.blob = creator.CreateBlob(v8::SnapshotCreator::FunctionCodeHandling::kKeep);
    std::ofstream out("snapshot.blob", std::ios::out | std::ios::binary);
    for (size_t i = 0; i < snapshot_data.env_info.props.size(); i++) {
      std::string name = snapshot_data.env_info.props[i].name;
      std::string id = std::to_string(snapshot_data.env_info.props[i].id);
      std::string index = std::to_string(snapshot_data.env_info.props[i].index);
      out.write(name.data(), name.size());
      out.write(":", 1);
      out.write(id.data(), id.size());
      out.write(":", 1);
      out.write(index.data(), index.size());
      if (i != (snapshot_data.env_info.props.size() - 1)) {
        out.write("|", 1);
      }
    }
    out.write("\n", 1);
    out.write(snapshot_data.blob.data, snapshot_data.blob.raw_size);
    out.close();
  }
}

void Start(int argc, char* argv[]) {
  Isolate::CreateParams create_params;
  No::Snapshot::SnapshotData snapshot_data;
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
    std::string s(buffer.data(), buffer.size());
    int end = s.find("\n"); 
    if (end == -1) {
      std::cerr << "读取文件失败" << std::endl;
      return;
    }
    std::string prop_data = s.substr(0, end);
    std::vector<std::string> props = No::Util::Split(prop_data, '|');
    for (auto& prop : props) {
      std::vector<std::string> prop_fields = No::Util::Split(prop, ':');
      snapshot_data.env_info.props.push_back({prop_fields[0],static_cast<size_t>(std::stoi(prop_fields[1])), static_cast<uint32_t>(std::stoi(prop_fields[2]))});
    }
    s = s.substr(end + 1);
    snapshot_data.blob = {s.data(), static_cast<int>(s.size())};
    create_params.snapshot_blob = &snapshot_data.blob;
    No::ExternalReferenceRegistry external_reference_registry;
    const std::vector<intptr_t>& external_references = external_reference_registry.external_references();
    create_params.external_references = external_references.data();
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
    env->deserialize(&snapshot_data);
    env->run_snapshot_deserial_callback();
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