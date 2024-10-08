const {
    fs,
    fswatcher,
} = No.buildin;

function read(path, cb) {
    const fd = fs.openSync(path)
    const buffer = Buffer.alloc(100)
    const req = new fs.FSReqCallback()
    req.oncomplete = function(status) {
        cb(status, buffer.toString());
    }
    const ret = fs.read(fd, buffer, req)
}

function readSync(path) {
    const fd = fs.openSync(path)
    const buffer = Buffer.alloc(100)
    const ret = fs.readSync(fd, buffer)
    return buffer.toString()
}

function writeSync(path, data) {
    const fd = fs.openSync(path, fs.constant.FLAG.O_RDWR)
    if (typeof data === 'stirng') {
        data = Buffer.from(data)
    }
    const ret = fs.writeSync(fd, data)
    return ret
}

class Watcher extends No.libs.events {
    constructor(path, options) {
        super();
        this.handle = new fswatcher.Watcher();
        this.handle.start(path, options.recursive);
        this.handle.onchange = (status, events, filename) => {
            if (status < 0) {
                if (this.handle !== null) {
                    this.handle.close();
                    this.handle = null;
                }
                const error = new Error();
                error.code = status;
                error.filename = filename;
                this.emit('error', error);
            } else {
                this.emit('change', events, filename);
            }
        }
    }
    close() {
        if (this.handle) {
            this.handle.close();
            this.handle = null;
        }
    }
}

function watch(path, options = {}) {
    return new Watcher(path, options);
}

module.exports = {
    readSync,
    writeSync,
    read,
    watch,
}