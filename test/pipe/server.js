const { createServer } = require('pipe');

const server = createServer()
server.listen({
    path: "/tmp/1.sock"
})
server.on('connection', (socket) => {
    console.log('connection')
    socket.on('data', (data) => {
        console.log(data.toString())
    })
})