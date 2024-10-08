const { UDP } = require('udp');

const socket = new UDP();

socket.on('message', console.log);

socket.bind({
  host: '127.0.0.1',
  port: 9999
});