const {
    tcp,
} = No.buildin;

const { events } = No.libs;

class Server extends events {
    handle = null;
    connections = 0;
    constructor(options = {}) {
        super();
    }
    listen(options) {
        this.handle = new tcp.TCP();
        let ret = this.handle.bind(options.host, options.port);
        this.handle.onconnection = (status) => {
           if (status == 0) {
            this.connections++;
            const handle = new tcp.TCP()
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
    fd = -1;
    write(buffer) {
        if (typeof buffer === 'string') {
            buffer = Buffer.from(buffer);
        }
        tcp.write(this.fd, buffer);
    }
}

class ClientSocket extends Socket {
    constructor(options = {}) {
        super(options);
        this.handle = new tcp.TCP();
    }
    connect(options) {
        const req = new tcp.TCPConnectReq();
        req.onconnection = (status) => {
            this.emit('connection');
        };
        return this.handle.connect(req, options.host, options.port)
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
    write(buffer) {
        if (typeof buffer === 'string') {
            buffer = Buffer.from(buffer);
        }
        const req = new tcp.WriteReq();
        req.oncomplete = (status) => {
            console.log("after write", status);
        };
        this.handle.write(req, buffer);
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