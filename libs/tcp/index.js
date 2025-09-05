const {
    tcp,
} = No.buildin;

const { events, dns } = No.libs;

// IPv4 Segment
const v4Seg = '(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])';
const v4Str = `(?:${v4Seg}\\.){3}${v4Seg}`;
const IPv4Reg = new RegExp(`^${v4Str}$`);

// IPv6 Segment
const v6Seg = '(?:[0-9a-fA-F]{1,4})';
const IPv6Reg = new RegExp('^(?:' +
  `(?:${v6Seg}:){7}(?:${v6Seg}|:)|` +
  `(?:${v6Seg}:){6}(?:${v4Str}|:${v6Seg}|:)|` +
  `(?:${v6Seg}:){5}(?::${v4Str}|(?::${v6Seg}){1,2}|:)|` +
  `(?:${v6Seg}:){4}(?:(?::${v6Seg}){0,1}:${v4Str}|(?::${v6Seg}){1,3}|:)|` +
  `(?:${v6Seg}:){3}(?:(?::${v6Seg}){0,2}:${v4Str}|(?::${v6Seg}){1,4}|:)|` +
  `(?:${v6Seg}:){2}(?:(?::${v6Seg}){0,3}:${v4Str}|(?::${v6Seg}){1,5}|:)|` +
  `(?:${v6Seg}:){1}(?:(?::${v6Seg}){0,4}:${v4Str}|(?::${v6Seg}){1,6}|:)|` +
  `(?::(?:(?::${v6Seg}){0,5}:${v4Str}|(?::${v6Seg}){1,7}|:))` +
')(?:%[0-9a-zA-Z-.:]{1,})?$');

function isIPv4(s) {
  return IPv4Reg.test(s)
}

function isIPv6(s) {
  return IPv6Reg.test(s)
}

function isIP(s) {
  if (isIPv4(s)) return 4;
  if (isIPv6(s)) return 6;
  return 0;
}

class Server extends events {
    handle = null;
    connections = 0;
    constructor(options = {}) {
        super();
        this.options = options;
    }
    listen(options = {}) {
        this.handle = new tcp.TCP();
        let flags = 0;
        if (options.ipv6Only) {
            flags |= tcp.constant.FLAG.UV_TCP_IPV6ONLY
        }
        if (options.reusePort) {
            flags |= tcp.constant.FLAG.UV_TCP_REUSEPORT
        }
        let ret = this.handle.bind(options.host, options.port, flags);
        this.handle.onconnection = (status) => {
           if (status == 0) {
            this.connections++;
            const handle = new tcp.TCP()
            const ret = this.handle.accept(handle);
            if (ret != 0) {
                console.log(ret)
                return
            }
            const socket = new ServerSocket({ handle, pause: this.options.pause })
            this.emit('connection', socket);
           }
        }
        this.handle.listen(100)
    }
    close() {
        this.handle.close();
        this.handle = null;
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
            console.log("after write", status);
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
        this.handle = new tcp.TCP();
        this.handle.onread = (nread, data) => {
            if (nread === 0) {
                this.emit('end');
            } else if (nread > 0){
                this.emit('data', new Buffer(data));
            } else {
                this.emit('error', new Error('read socket error'));
            }
        }
    }
    connect(options) {
        const req = new tcp.TCPConnectReq();
        req.onconnection = (status) => {
            if (status === 0) {
                this.handle.readStart();
                this.emit('connection');
            } else {
                this.emit('error', new Error('failed to connect'));
            }
        };
        if (isIP(options.host) > 0) {
            this.handle.connect(req, options.host, options.port);
        } else {
            dns.lookup(options.host, (status, addresses) => {
                if (status !== 0) {
                    this.emit('error', new Error('failed to lookup host'));
                    return;
                }
                this.handle.connect(req, addresses[0], options.port);
            })
        }
    }
}

class ServerSocket extends Socket {
    constructor(options = {}) {
        super(options);
        if (options.handle) {
            this.handle = options.handle;
        } else if (options.fd){
            this.handle = new tcp.TCP();
            this.handle.open(options.fd);
        }
        
        if (!options.pause) {
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
    write(buffer) {
        if (typeof buffer === 'string') {
            buffer = Buffer.from(buffer);
        }
        const req = new tcp.WriteReq();
        req.oncomplete = (status) => {
            // console.log("after write", status);
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
    ServerSocket,
}