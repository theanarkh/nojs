#include "external_reference.h"

namespace No {  

  const std::vector<intptr_t>& ExternalReferenceRegistry::external_references() {
    if (!is_finalized_) {
      external_references_.push_back(reinterpret_cast<intptr_t>(nullptr));
      is_finalized_ = true;
    }
    return external_references_;
  }
  
  ExternalReferenceRegistry::ExternalReferenceRegistry() {
  
  }
} // namespace No