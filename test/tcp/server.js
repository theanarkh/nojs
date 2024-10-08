const { createServer, ClientSocket } = require('tcp');

const server = createServer()
server.listen({
    host: '127.0.0.1', port: 8888
})
server.on('connection', (socket) => {
    socket.on('data', (data) => {
        console.log(data.toString())
    })
   
})