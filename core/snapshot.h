#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <vector>

namespace No {
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
} // namespace No
#endif

