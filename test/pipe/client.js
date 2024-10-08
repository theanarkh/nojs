const {  ClientSocket } = require('pipe');

const socket = new ClientSocket()

socket.on('connection', () => {
    console.log('done');
});

socket.connect({
    path: '/tmp/1.sock'
});

