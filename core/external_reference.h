#ifndef EXTERNAL_REFERENCE_H
#define EXTERNAL_REFERENCE_H

#include <vector>

namespace No {
  class ExternalReferenceRegistry {
      public:
      ExternalReferenceRegistry();
    
      const std::vector<intptr_t>& external_references();
    
      bool is_empty() { return external_references_.empty(); }

      template <typename T>
      void Register(T* address) {
        external_references_.push_back(reinterpret_cast<intptr_t>(address));
      }

      private:
      
      bool is_finalized_ = false;
      std::vector<intptr_t> external_references_;
  };
} // namespace No

#define EXTERNAL_REFERENCE_BINDING_LIST(V)                                     \
  V(dns)         \
  V(fs) \
  V(process) \
  V(console) \
  V(loader) \
  V(vm) \
  V(tcp) \
  V(udp) \
  V(timer) \
  V(pipe) \
  V(http) \
  V(dns) \
  V(worker) \
  V(message) \
  V(micro_task) \
  V(immediate) \
  V(fs_watcher) \
  V(child_process) \
  V(os) \
  V(signal) \
  V(addon) \
  V(buffer) \
  V(uv) \
  V(perf)  \
  V(handle) \
  V(util) \
  V(snapshot)

#define V(modname)                                                             \
    void _register_external_reference_##modname(                                 \
        No::ExternalReferenceRegistry* registry);
  EXTERNAL_REFERENCE_BINDING_LIST(V)
#undef V

#define NODE_BINDING_EXTERNAL_REFERENCE(modname, func)                         \
    void _register_external_reference_##modname(                                 \
      No::ExternalReferenceRegistry* registry) {                                 \
      func(registry);                                                            \
    }

#endif

