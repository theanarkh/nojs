#ifndef EXTERNAL_REFERENCE_H
#define EXTERNAL_REFERENCE_H

#include <vector>

namespace No {
  class ExternalReferenceRegistry {
      public:
      ExternalReferenceRegistry();
    
      const std::vector<intptr_t>& external_references();
    
      bool is_empty() { return external_references_.empty(); }
    
      private:
      template <typename T>
      void RegisterT(T* address) {
        external_references_.push_back(reinterpret_cast<intptr_t>(address));
      }
      bool is_finalized_ = false;
      std::vector<intptr_t> external_references_;
  };
} // namespace No
#endif