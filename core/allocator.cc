
#include "allocator.h"

namespace No {
    namespace NoMemoryAllocator {
        
        void* NoArrayBufferAllocator::Allocate(size_t size) {
            void* ret = _allocator->Allocate(size);
            if (ret != nullptr) {
                _allocated_size += size;
            }
            return ret;
        };

        void* NoArrayBufferAllocator::AllocateUninitialized(size_t size) {
            void* ret = _allocator->AllocateUninitialized(size);
            if (ret != nullptr) {
                _allocated_size += size;
            }
            return ret;
        };

        void NoArrayBufferAllocator::Free(void* data, size_t size) {
            _allocated_size -= size;
            _allocator->Free(data, size);
        };

        uint64_t NoArrayBufferAllocator::allocated_size() const {
            return static_cast<uint64_t>(_allocated_size);
        };
    }
}