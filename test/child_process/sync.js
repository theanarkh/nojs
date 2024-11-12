const child_process = require('child_process');

const result = child_process.forkSync('test/child_process/worker.js', {timeout: 1000});

console.log(JSON.stringify(result));
// child.kill()