const {
    loader,
    process,
    console,
} = No.buildin;

function loaderNativeModule() {
    const modules = [
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
            module: 'libs/fs/index.js',
            name: 'fs'
        },
        {
            module: 'libs/tcp/index.js',
            name: 'tcp'
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
            module: 'libs/pipe/index.js',
            name: 'pipe'
        },
        {
            module: 'libs/udp/index.js',
            name: 'udp'
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
            module: 'libs/tcp/index.js',
            name: 'tcp'
        },
        {
            module: 'libs/dns/index.js',
            name: 'dns'
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
            module: 'libs/nextTick/index.js',
            name: 'task',
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
            module: 'libs/child_process/index.js',
            name: 'child_process',
        },
    ];
    No.libs = {};
    for (let i = 0; i < modules.length; i++) {
        const module = {
            exports: {},
        };
        loader.compile(modules[i].module).call(null, loader.compile, module.exports, module, No);
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
