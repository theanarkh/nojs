const { UDP } = require('udp');

// {
//   const socket = new UDP();
//   socket.bind({
//     host: '127.0.0.1',
//     port: 9999,
//   })
//   .on('error', console.log)
//   .on('message', (data) => {
//     console.log('9999 receive data: '+ data)
//   });
// }

// {
//   const socket = new UDP();
//   socket.bind({
//     host: '127.0.0.1',
//     port: 8888,
//   })
//   .on('error', console.log)
//   .on('message', (data) => {
//     console.log('8888 receive data: '+ data)
//   });
// }


{
  const socket = new UDP();
  socket.on('error', (err) => {
    console.log('send error:' + err.message)
  });
  // socket.connect({ port: 8888, host:'127.0.0.1' })
  socket.send(Buffer.from("hello"), { port: 8888, host:'127.0.0.1' },(status) => {
    console.log('send ok: '+status)
  })
}