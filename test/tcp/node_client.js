require('net').connect({host:'127.0.0.1',port:8888}, function() {
    console.log('connection')
    setInterval(() => {
        this.write("hello\n")
    }, 1000)
})