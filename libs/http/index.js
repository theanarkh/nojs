const {
    HTTPParser,
} = No.buildin;

const { tcp } = No.libs;

class Server extends tcp.Server {
    constructor(options = {}, cb) {
        super(options);
        this.options = options;
        if (typeof cb === 'function') {
            this.on('request', cb);
        }
        this.on('connection', (socket) => {
            new HTTPRequest({socket, server: this});
        });
    }
}

class HTTPRequest extends No.libs.events {
    socket = null;
    httpparser = null;
    constructor({socket, server}) {
        super();
        this.server = server;
        this.socket = socket;
        this.httpparser = new HTTPParser(1);
        this.httpparser.onHeaderComplete = (data) => {
            this.method = data.method;
            this.major = data.major;
            this.minor = data.minor;
            this.keepalive = data.keepalive;
            this.upgrade = data.upgrade;
            this.headers = data.headers;
            const response = new HTTPResponse({socket, server});
            this.server.emit('request', this, response);
        }
        this.httpparser.onBody = (data) => {
            this.emit('data', data);
        }
        this.httpparser.onBodyComplete = () => {
            this.emit('end');
        }
        socket.on('data', (buffer) => {
            this.httpparser.parse(buffer);
        });
        socket.on('end', () => {
            this.emit('end');
        });
    }
}

class HTTPResponse extends No.libs.events {
    statusCode = 200;
    headers = {
        "Server": "No.js"
    };
    socket = null;
    constructor({socket, server}) {
        super();
        this.server = server;
        this.socket = socket;
    }
    buildHeaders(headers) {
        const arr = [];
        for (const [k, v] of Object.entries(headers)) {
            arr.push(`${k}:${v}\r\n`);
        }
        return arr.join('');
    }
    end(data) {
        const responseLine = `HTTP/1.1 ${this.statusCode} OK`;
        const headers = { ...this.headers, "Content-length": Buffer.strlen(data)};
        const responseHeaders = this.buildHeaders(headers);
        this.socket.write(`${responseLine}\r\n${responseHeaders}\r\n${data}\r\n\r\n`);
    }
    setHeaders(headers) {
        for (const [k, v] of Object.entries(headers)) {
            this.setHeader(k, v);
        }
    }
    setHeader(k, v) {
        this.headers[k] = v;
    }
}

class HTTPClientResponse extends No.libs.events {
    constructor(options) {
        super();
        this.status = options.status;
        this.statusMessage = options.statusMessage;
        this.major = options.major;
        this.minor = options.minor;
        this.keepalive = options.keepalive;
        this.upgrade = options.upgrade;
        this.headers = options.headers;
    }
}

class HTTPClient extends No.libs.events {
    socket = null;
    httpparser = null;
    constructor(options) {
        super();
        let response
        this.socket = new tcp.ClientSocket();
        this.socket.connect(options);
        this.socket.on('error', () => {
            const error = new Error('socket close before recieve response');
            if (response) {
                response.emit('error', error)
            } else {
                this.emit('error', error);
            }
        });
        this.socket.on('connection', () => {
            this.socket.write(`${options.method || 'GET'} ${options.url || '/'} HTTP/1.1\r\n`)
            if (options.headers) {
                const arr = [];
                for (const [k, v] of Object.entries(options.headers)) {
                    arr.push(`${k}:${v}\r\n`);
                }
                this.socket.write(arr.join('') + '\r\n')
            }
            if (options.body) {
                this.socket.write(options.body)
            }
            this.socket.write('\r\n\r\n');
            
            this.httpparser = new HTTPParser(2);
            this.httpparser.onHeaderComplete = (data) => {
                response = new HTTPClientResponse(data);
                this.emit('response', response);
            }
            this.httpparser.onBody = (data) => {
                response.emit('data', data);
            }
            this.httpparser.onBodyComplete = (data) => {
                console.log(data);
            }
            this.socket.on('data', (buffer) => {
                this.httpparser.parse(buffer);
            });
            this.socket.on('end', () => {
                if (!response) {
                    this.emit('error', new Error('socket close before recieve response'))
                } else {
                    response.emit('end');
                }
            });
        })
    }
}

function createServer(...arg) {
    return new Server(...arg);
}

function request(options) {
    return new HTTPClient(options);
}
module.exports = {
    createServer,
    request,
    HTTPRequest,
}