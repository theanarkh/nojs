const os = require('os');

console.log(os.getConstrainedMemory());
console.log(os.getAvailableMemory());
console.log(os.getAvailableParallelism());
console.log(os.getTotalMemory());
console.log(os.getFreeMemory());