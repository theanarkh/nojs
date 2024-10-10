const {
    os,
} = No.buildin;

const { tcp, worker, events } = No.libs;

class Server extends events {
    workers = []
    server
    options
    fds = {};
    next = 0
    constructor(options = {}) {
        super();
        this.options = options;
    }
    listen(options) {
        const server = tcp.createServer({pause: true})
        
        server.on('connection',(connection) => {
            const fd = os.dup(connection.handle.fd);
            if (fd < 0) {
                connection.close();
                return;
            }
            connection.close();
            this.fds[fd] = true;
            const { next } = this;
            this.next = (next + 1) % this.workers.length;
            this.workers[next].postMessage({
                type: "SEND_FD",
                fd,
                handler: this.options.handler,
            });
            this.workers[next].on('message', (msg) => {
                if (msg.type === "ACK_FD") {
                    // TODO: send to next worker if the worker can not handle the fd
                    delete this.fds[msg.fd];
                }
            });
        });
        server.listen(options);
        for (let i = 0; i < (this.options.worker || 2); i++) {
            const w = new worker.Worker("libs/cluster/server.js");
            this.workers.push(w);
        }
    }
}

function createServer(options) {
    return new Server(options);
}

module.exports = {
    createServer,
}