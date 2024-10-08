const {  ClientSocket } = require('tcp');

new ClientSocket().connect({
    host: '127.0.0.1', port: 8888
})