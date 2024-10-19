#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "common.h"

using namespace v8;

namespace No {
    namespace NoMemoryAllocator {
        class NoArrayBufferAllocator: public v8::ArrayBuffer::Allocator {
            public:
            void* Allocate(size_t length) override;
            void* AllocateUninitialized(size_t length) override;
            void Free(void* data, size_t length) override;
            uint64_t allocated_size() const;
            private:
            ArrayBuffer::Allocator* _allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
            size_t _allocated_size = 0;
        };
    }
}

#endif