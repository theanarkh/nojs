#ifndef CORE_H
#define CORE_H

#include "common.h"
#include "env.h"
#include "fs.h" 
#include "process.h"
#include "console.h"
#include "loader.h"
#include "util.h"
#include "vm.h"
#include "tcp.h"
#include "udp.h"
#include "timer.h"
#include "pipe.h"
#include "http.h"
#include "dns.h"
#include "worker.h"
#include "message.h"
#include "micro_task.h"
#include "immediate.h"
#include "fs_watcher.h"
#include "child_process.h"
#include "os.h"
#include "signal.h"
#include "addon.h"
#include "buffer.h"

using namespace v8;
using namespace No::Env;
using namespace No::Util;

namespace No {
    namespace Core {
        void RegisterBuiltins(Isolate * isolate, Local<Object> target);
        void Run(Environment * env);
    }
}

#endif