const {
    pipe,
} = No.buildin;

const { events } = No.libs;

class Server extends events {
    handle = null;
    connections = 0;
    constructor(options = {}) {
        super();
    }
    listen(options) {
        this.handle = new pipe.Pipe(0);
        let ret = this.handle.bind(options.path);
        if (ret !== 0) {
            const err = new Error();
            err.code = ret;
            this.emit("error", err);
            return;
        }
        this.handle.onconnection = (status) => {
           if (status == 0) {
            this.connections++;
            const handle = new pipe.Pipe(0)
            const ret = this.handle.accept(handle);
            if (ret != 0) {
                console.log(ret)
                return
            }
            const socket = new ServerSocket({ handle })
            this.emit('connection', socket);
           }
        }
        this.handle.listen(100)
    }
    close() {
        // this.handle.close();
        // this.handle = null;
    }
}

class Socket extends events {
    handle
    write(buffer) {
        if (typeof buffer === 'string') {
            buffer = Buffer.from(buffer);
        }
        const req = new tcp.WriteReq();
        req.buffer = buffer;
        req.oncomplete = (status) => {
            console.logln("after write", status);
        };
        this.handle.write(req, buffer);
    }
    close() {
        this.handle.close();
        this.handle = null;
    }
}

class ClientSocket extends Socket {
    constructor(options = {}) {
        super(options);
        this.handle = new pipe.Pipe(0);
    }
    connect(options) {
        const req = new pipe.PipeConenctReq();
        req.onconnection = (status) => {
            if (status === 0) {
                this.emit('connection');
                // call readStart to keep loop alive
                this.handle.readStart();
            } else {
                this.emit('error');
            }
        };
        return this.handle.connect(req, options.path)
    }
}

class ServerSocket extends Socket {
    constructor(options = {}) {
        super(options);
        this.handle = options.handle;
        this.handle.onread = (nread, data) => {
            if (nread === 0) {
                this.emit('end');
            } else if (nread > 0){
                this.emit('data', new Buffer(data));
            } else {
                this.emit('error', new Error('read socket error'));
            }
        }
        this.handle.readStart();
    }
}

function createServer(...arg) {
    return new Server(...arg);
}

module.exports = {
    createServer,
    Server,
    ClientSocket,
}