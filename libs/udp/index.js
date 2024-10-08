const {
    udp,
} = No.buildin;

const { events } = No.libs;

class UDP extends events {
    handle = null;
    bind(options) {
        this.handle = new udp.UDP();
        let ret = this.handle.bind(options.host, options.port);
        if (ret !== 0) {
            return ret
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
        console.log(ret)
    }
    close() {
        // this.handle.close();
        // this.handle = null;
    }
}

module.exports = {
    UDP,
}