const {
    child_process,
} = No.buildin;

class ChildProcess extends No.libs.events {
    constructor() {
        super();
        this.handle = new child_process.ChildProcess();
        this.handle.onexit = (exitCode, exitSignal) => {
            if (exitCode === 0) {
                this.emit('exit', exitCode, exitSignal);
            } else {
                const error = new Error();
                error.code = exitCode;
                error.signal = exitSignal;
                this.emit('error', error);
            }
            // this.handle.close();
            this.handle = null;
        }
    }

    get pid() {
        return this.handle.pid;
    }

    spawn(options) {
        return this.handle.spawn(options);
    }
    kill() {
        // SIGTERM: 15;
        return this.handle.kill(15);
    }
}

function fork(file) {
    const child = new ChildProcess();
    const ret = child.spawn({
        file: process.execPath,
        args: [process.execPath, file],
        env: ["WORKER=1"]
    });
    if (ret !== 0) {
        const err = new Error();
        err.code = ret;
        //err.message = uvErrorMap(ret);
        process.nextTick(() => {
            child.emit('error', err);
        })
    }
    return child;
}

function forkSync(file, options = {}) {
    return child_process.spawnSync({
        ...options,
        file: process.execPath,
        args: [process.execPath, file],
        env: ["WORKER=1"],
    });
}

module.exports = {
    fork,
    forkSync,
}