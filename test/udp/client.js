const dgram = require('dgram');
const socket = dgram.createSocket('udp4');

socket.send("hello", 9999, '127.0.0.1')