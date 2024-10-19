const os = require('os');

console.logln(os.getConstrainedMemory());
console.logln(os.getAvailableMemory());
console.logln(os.getAvailableParallelism());
console.logln(os.getTotalMemory());
console.logln(os.getFreeMemory());