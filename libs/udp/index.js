const {
    udp,
} = No.buildin;

const { events } = No.libs;

class UDP extends events {
    handle = null;
    bind(options = {}) {
        this.handle = new udp.UDP();
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
        let ret = this.handle.bind(options.host, options.port, flags);
        if (ret !== 0) {
            process.nextTick(() => {
                const err = new Error('bind socket error');
                err.code = ret;
                this.emit('error', err);
            })
            return this
        }
        this.handle.onmessage = (nread, data) => {
            if (nread === 0) {
                this.emit('end');
            } else if (nread > 0){
                this.emit('message', new Buffer(data));
            } else {
                this.emit('error', new Error('read socket error'));
            }
        }
        ret = this.handle.readStart();
        return this;
    }
    close() {
        // this.handle.close();
        // this.handle = null;
        return this;
    }
}

module.exports = {
    UDP,
}