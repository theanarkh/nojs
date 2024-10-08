const child_process = require('child_process');

const child = child_process.fork('test/child_process/worker.js');

child.on('exit', console.log);
// child.kill()