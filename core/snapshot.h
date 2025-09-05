#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <vector>
#include "common.h"
#include "external_reference.h"

using namespace v8;

namespace No {
  namespace Snapshot {
    struct PropInfo {
      std::string name;
      size_t index;
      uint32_t id;
    };
    struct EnvInfo {
      std::vector<PropInfo> props;
    };
    struct SnapshotData {
      EnvInfo env_info;
      v8::StartupData blob;
    };
    
    void Init(Isolate* isolate, Local<Object> target);
    static void SetSerializeCallback(V8_ARGS);
    static void SetDeSerializeCallback(V8_ARGS);
    static void IsBuildSnapshot(V8_ARGS);
    static void HasStartupSnapshot(V8_ARGS);
    static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
  }
} // namespace No
#endif

