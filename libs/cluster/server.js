const { channel } = require('worker');
const tcp = require('tcp');
const http = require('http');

channel.on('message', (msg) => {
    if (msg.type !== 'SEND_FD') {
        return;
    }
    channel.postMessage({
        type: 'ACK_FD',
        fd: msg.fd,
    });
    const socket = new tcp.ServerSocket({fd: +msg.fd});
    const server = http.createServer();
    server.on('request', (req, res) => {
        require(msg.handler)(req, res);
    });
    new http.HTTPRequest({socket, server});
});
