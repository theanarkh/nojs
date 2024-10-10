const http = require('http');

const client = http.request({host: '127.0.0.1', port: 8888, method: 'GET', url: '/', headers: {"content-length": 0, "hello": "world"}});
client.on('error', (data) => {
    console.log(JSON.stringify(data))
});
client.on('response', (data) => {
    console.log(JSON.stringify(data))
});