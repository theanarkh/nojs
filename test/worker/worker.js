const { channel } = require('worker');
channel.postMessage("hello");
channel.on('message', console.log)
