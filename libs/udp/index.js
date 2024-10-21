const {
    udp,
} = No.buildin;

const { events } = No.libs;

const TYPE = {
    UDP4: 1,
    UDP6: 2,
};

const UNBINDED = 0;
const BINDING = 1;
const BINDED = 2

const UNCONNECTED = 0;
const CONNECTING = 1;
const CONNECTED = 2;

class UDP extends events {
    handle = null;
    constructor(options = { type: TYPE.UDP4 }) {
        super();
        this.handle = new udp.UDP();
        this.type = options.type;
        this.bindState = UNBINDED;
        this.connectState = UNCONNECTED;
    }
    
    bind(options = {}) {
        if (this.bindState !== UNBINDED) {
            process.nextTick(() => {
                this.emit('error', new Error('is not in UNBINDED state'))
            });
            return this;
        }
        let flags = 0;
        if (options.ipv6Only) {
            flags |= udp.constant.FLAG.UV_UDP_IPV6ONLY
        }
        if (options.reuseAddr) {
            flags |= udp.constant.FLAG.UV_UDP_REUSEADDR
        }
        if (options.reusePort) {
            flags |= udp.constant.FLAG.UV_UDP_REUSEPORT
        }
        const bind = this.type === TYPE.UDP4 ? 'bind' : 'bind6';
        let ret = this.handle[bind](options.host || '127.0.0.1', options.port, flags);
        if (ret !== 0) {
            process.nextTick(() => {
                const err = new Error('bind socket error'+ret);
                err.code = ret;
                this.emit('error', err);
            })
            return this
        }
        this.handle.onmessage = (nread, data) => {
            if (nread === 0) {
                this.emit('message', new Buffer(''));
            } else if (nread > 0){
                this.emit('message', new Buffer(data));
            } else {
                this.emit('error', new Error('read socket error'));
            }
        }
        ret = this.handle.readStart();
        if (ret !== 0) {
            process.nextTick(() => {
                this.emit('error', new Error('readStart error'));
            });
            return;
        }
        this.bindState = BINDED;
        process.nextTick(() => {
            this.emit('listening');
        });
        return this;
    }
   
    connect({ port, host }) {
        if (this.connectState !== UNCONNECTED) {
            process.nextTick(() => {
                this.emit('error', new Error('not in UNCONNECTED state'))
            });
            return;
        }
        this.connectState = CONNECTING;
        function doConnect() {
            this.off('listening', doConnect);
            this.off('error', error);
            const connect = this.type === TYPE.UDP4 ? 'connect' : 'connect6';
            const ret = this.handle[connect](host || '127.0.0.1', port);
            if (ret !== 0) {
                this.connectState = UNCONNECTED;
                process.nextTick(() => {
                    this.emit('error', new Error('connect error'));
                });
                return;
            }
            this.connectState = CONNECTED;
            process.nextTick(() => {
                this.emit('connect');
            });
        }
        function error() {
            this.off('listening', doConnect);
            this.off('error', error);
            this.connectState = UNCONNECTED;
        }
        // can call bind after call connect, so we should call bind before call connect
        if (this.bindState === BINDED) {
            doConnect();
        } else {
            if (this.bindState === UNBINDED) {
                this.bind({ port: 0 });
            }
            this.on('listening', doConnect);
            this.on('error', error);
        }
        return this;
    }
    disconnect() {
        if (this.connectState === CONNECTED) {
            this.handle.disconnect();
        }
        return this;
    }

    send(data, options, cb) {
        if (typeof options === 'function') {
            cb = options;
            options = {};
        }
        if (this.connectState !== UNCONNECTED && (options.port !== undefined || options.host !== undefined)) {
            process.nextTick(() => {
                this.emit('error', new Error("have already been connected"));
            })
            return this;
        }
        if (this.connectState === UNCONNECTED && (options.port === undefined || options.host === undefined)) {
            process.nextTick(() => {
                this.emit('error', new Error("socket is unconnected"));
            })
            return this;
        }
        const doSend = function() {
            this.off('listening', doSend);
            this.off('error', error);
            const sendReq = new udp.SendReq();
            sendReq.data = data;
            sendReq.oncomplete = (status) => {
                cb(status);
            }
            let ret
            if (options.host !== undefined && options.port != undefined) {
                ret = this.handle.send(sendReq, data, options.host, options.port)
            } else {
                ret = this.handle.send(sendReq, data);
            }
            if (ret !== 0) {
                process.nextTick(() => {
                    this.emit('error', new Error("send error"));
                });
            }
        }
        function error() {
            this.off('listening', doSend);
            this.off('error', error);
        }
        if (this.bindState === BINDED) {
            doSend();
        } else {
            if (this.bindState === UNBINDED) {
                this.bind({ port: 0 });
            }
            this.on('listening', doSend);
            this.on('error', error);
        }
        return this;
    }

    close() {
        this.handle.close();
        this.handle = null;
        return this;
    }
}

module.exports = {
    UDP,
    TYPE,
}