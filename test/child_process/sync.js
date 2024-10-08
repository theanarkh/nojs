const child_process = require('child_process');

const result = child_process.forkSync('test/child_process/worker.js');

console.log(JSON.stringify(result));
// child.kill()