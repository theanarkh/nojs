const {
    loader,
    process,
    snapshot,
} = No.buildin;

class NativeModule {
    exports
    constructor(filename) {
        this.filename = filename;
        this.exports = {};
    }
    load() {
        loader[process.env.Local ? 'compile' : 'compileNative'](this.filename).call(null, loader.compile, this.exports, this, No);
    }
}

function require(filename) {
    const module = new NativeModule(filename);
    module.load();
    return module.exports;
}

function loaderNativeModules() {
    const modules = [
        {
            filename: 'libs/uv/index.js',
            name: 'uv',
        },
        {
            filename: 'libs/os/index.js',
            name: 'os',
        },
        {
            filename: 'libs/console/index.js',
            name: 'console',
        },
        {
            filename: 'libs/vm/index.js',
            name: 'vm',
        },
        {
            filename: 'libs/events/index.js',
            name: 'events'
        },
        {
            filename: 'libs/timer/index.js',
            name: 'timer',
        },
        {
            filename: 'libs/process/index.js',
            name: 'process',
        },
        {
            filename: 'libs/buffer/index.js',
            name: 'buffer',
        },
        {
            filename: 'libs/nextTick/index.js',
            name: 'microtask'
        },
        {
            filename: 'libs/immediate/index.js',
            name: 'immediate',
        },
        {
            filename: 'libs/dns/index.js',
            name: 'dns'
        },
        {
            filename: 'libs/pipe/index.js',
            name: 'pipe'
        },
        {
            filename: 'libs/udp/index.js',
            name: 'udp'
        },
        {
            filename: 'libs/fs/index.js',
            name: 'fs'
        },
        {
            filename: 'libs/tcp/index.js',
            name: 'tcp'
        },
        {
            filename: 'libs/http/index.js',
            name: 'http'
        },
        {
            filename: 'libs/worker/index.js',
            name: 'worker'
        },
        {
            filename: 'libs/child_process/index.js',
            name: 'child_process',
        },
        {
            filename: 'libs/cluster/index.js',
            name: 'cluster',
        },
        {
            filename: 'libs/signal/index.js',
            name: 'signal',
        },
        {
            filename: 'libs/perf/index.js',
            name: 'perf',
        },
        {
            filename: 'libs/snapshot/index.js',
            name: 'snapshot',
        },
    ];
    No.libs = {};
    for (let i = 0; i < modules.length; i++) {
        const { name, filename } = modules[i];
        No.libs[name] = require(filename);
    }
}

if (!snapshot.hasStartupSnapshot()) {
    loaderNativeModules();
}

function runMain() {
    const module = require("libs/module/index.js");
    let entry;
    for (let i = 0; i < process.argv.length; i++) {
        if (process.argv[i].endsWith('.js')) {
            entry = process.argv[i];
        }
    }
    if (process.isMainThread) {
        module.load(entry);
    } else {
        module.load("libs/worker/main.js");
        module.load(entry);
    }
}

runMain();