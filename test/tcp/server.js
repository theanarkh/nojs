const { createServer } = require('tcp');

const server = createServer();
server.listen({
    host: '127.0.0.1',
    port: 8888,
})