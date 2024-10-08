const {
    worker,
} = No.buildin;

const { events } = No.libs;

class Worker extends events {
    handle = null;
    constructor(filename, options) {
        super();
        this.handle = new worker.Worker(filename, options);
        this.handle.start();
        this.channel = new Channel(this.handle.getMessageEndpoint())
        this.channel.on('message', (msg) => {
            this.emit('message', msg);
        })
    }
    stop() {
        this.handle.stop();
    }
    postMessage(msg) {
        this.channel.postMessage(msg);
    }
}

class Channel extends events {
    constructor(handle) {
        super()
        this.handle = handle;
        this.handle.onmessage = (msg) => {
            this.emit('message', msg);
        }
    }
    postMessage(msg) {
        this.handle.postMessage(msg);
    }
}

module.exports = {
    Worker,
    channel: process.isMainThread ? null : new Channel(worker.getMessageEndpoint())
}