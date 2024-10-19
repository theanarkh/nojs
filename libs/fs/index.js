const {
    fs,
    fswatcher,
} = No.buildin;

function open(path, cb) {
    const req = new fs.FSReqCallback()
    req.oncomplete = function(fd) {
        if (fd < 0) {
            const err = new Error();
            err.code = fd;
            cb(err);
            return;
        }
        cb(null, fd);
    }
    fs.open(path, fs.constant.FLAG.O_RDWR, fs.constant.MODE.S_IRUSR | fs.constant.MODE.S_IWUSR | fs.constant.MODE.S_IRGRP | fs.constant.MODE.S_IROTH, req)
}

function openSync(path) {
    return fs.openSync(path);
}

function unlink(path, cb) {
    const req = new fs.FSReqCallback()
    req.oncomplete = function(status) {
        if (status < 0) {
            const err = new Error();
            err.code = status;
            cb(err);
            return;
        }
        cb(null);
    }
    fs.unlink(path, req);
}

function unlinkSync(path) {
    return fs.unlinkSync(path);
}

function read(path, cb) {
    open(path, function(err, fd) {
        if (err) {
            cb(err);
            return;
        }
        const buffer = Buffer.alloc(100)
        const req = new fs.FSReqCallback()
        req.oncomplete = function(status) {
            if (status < 0) {
                const err = new Error();
                err.code = status;
                cb(err);
            } else {
                cb(null, buffer.toString(), status);
            }
        }
        return fs.read(fd, buffer, req)
    })
}

function write(path, data, cb) {
    open(path, function(err, fd) {
        if (err) {
            cb(err);
            return;
        }
        if (typeof data === 'stirng') {
            data = Buffer.from(data)
        }
        const req = new fs.FSReqCallback()
        req.oncomplete = function(status) {
            if (status < 0) {
                const err = new Error();
                err.code = status;
                cb(err);
            } else {
                cb(null, status);
            }
        }
        return fs.write(fd, data, req)
    })
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
    open, 
    openSync,
    readSync,
    write,
    writeSync,
    read,
    unlink,
    unlinkSync,
    watch,
}