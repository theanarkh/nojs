const cluster = require('cluster');

// cluster.createHTTPServer({
//     handler: 'test/cluster/worker.js' 
// }).listen({
//     host: '127.0.0.1',
//     port: 8888
// });

cluster.createServer({
    handler: 'test/cluster/socket.js' 
}).listen({
    host: '127.0.0.1',
    port: 8888
});