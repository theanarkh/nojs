const { Worker } = require('worker');

const worker = new Worker("test/worker/worker.js");
worker.on('message', console.log);
worker.postMessage("world");

// Make loop alive, TODO
// setInterval(() => {}, 1000000);