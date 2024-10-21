#ifndef ADDON_H
#define ADDON_H

#include "common.h"
#include "util.h"

using namespace v8;
using namespace No::Util;

namespace No {
    namespace Addon {
        void Init(Isolate* isolate, Local<Object> target);
        
        class Module {
            public:
            Module(const char* filename, int flags);
            Module(const Module&) = delete;
            Module& operator=(const Module&) = delete;

            bool Open();
            void Close();
            void* GetSymbolAddress(const char* name);
            
            const std::string filename_;
            const int flags_;
            std::string errmsg_;
            void* handle_;
            uv_lib_t lib_;
            bool has_entry_in_global_handle_map_ = false;

        };
        static void Load(V8_ARGS);
    }
}

#endif