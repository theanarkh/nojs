#include "external_reference.h"
namespace No {  
  ExternalReferenceRegistry::ExternalReferenceRegistry() {
    #define V(modname) _register_external_reference_##modname(this);
      EXTERNAL_REFERENCE_BINDING_LIST(V)
    #undef V
  }

  const std::vector<intptr_t>& ExternalReferenceRegistry::external_references() {
    if (!is_finalized_) {
      external_references_.push_back(reinterpret_cast<intptr_t>(nullptr));
      is_finalized_ = true;
    }
    return external_references_;
  }
  
} // namespace No