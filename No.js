const {
    loader,
    process,
    console,
} = No.buildin;

function loaderNativeModule() {
    const modules = [
        {
            module: 'libs/os/index.js',
            name: 'os',
        },
        {
            module: 'libs/console/index.js',
            name: 'console',
            after: (exports) => {
                global.console = exports;
            }
        },
        {
            module: 'libs/vm/index.js',
            name: 'vm',
        },
        {
            module: 'libs/module/index.js',
            name: 'module'
        },
        {
            module: 'libs/events/index.js',
            name: 'events'
        },
        {
            module: 'libs/timer/index.js',
            name: 'timer',
            after: (exports) => {
                global.setTimeout = exports.setTimeout;
                global.setInterval = exports.setInterval;
            }
        },
        {
            module: 'libs/process/index.js',
            name: 'process',
            after: (exports) => {
                global.process = exports;
            }
        },
        {
            module: 'libs/buffer/index.js',
            name: 'buffer',
            after: (exports) => {
                global.Buffer = exports;
            }
        },
        {
            module: 'libs/nextTick/index.js',
            name: 'microtask',
            after: (exports) => {
                global.process.nextTick = exports.nextTick;
            }
        },
        {
            module: 'libs/immediate/index.js',
            name: 'immediate',
            after: (exports) => {
                global.process.setImmediate = exports.setImmediate;
            }
        },
        {
            module: 'libs/dns/index.js',
            name: 'dns'
        },
        {
            module: 'libs/pipe/index.js',
            name: 'pipe'
        },
        {
            module: 'libs/udp/index.js',
            name: 'udp'
        },
        {
            module: 'libs/fs/index.js',
            name: 'fs'
        },
        {
            module: 'libs/tcp/index.js',
            name: 'tcp'
        },
        {
            module: 'libs/http/index.js',
            name: 'http'
        },
        {
            module: 'libs/worker/index.js',
            name: 'worker'
        },
        {
            module: 'libs/child_process/index.js',
            name: 'child_process',
        },
        {
            module: 'libs/cluster/index.js',
            name: 'cluster',
        },
        {
            module: 'libs/signal/index.js',
            name: 'signal',
        },
    ];
    No.libs = {};
    for (let i = 0; i < modules.length; i++) {
        const module = {
            exports: {},
        };
        loader.compileNative(modules[i].module).call(null, loader.compile, module.exports, module, No);
        No.libs[modules[i].name] = module.exports;
        typeof modules[i].after === 'function' && modules[i].after(module.exports);
    }
}

loaderNativeModule();

if (process.isMainThread) {
    No.libs.module.load(process.argv[1]);
} else {
    No.libs.module.load("libs/worker/main.js");
    No.libs.module.load(process.argv[1]);
}
