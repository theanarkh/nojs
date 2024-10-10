const cluster = require('cluster');

cluster.createServer({
    handler: 'test/cluster/worker.js' 
}).listen({
    host: '127.0.0.1',
    port: 8888
});