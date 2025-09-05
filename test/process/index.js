console.log(JSON.stringify(process.env)); // Need to implement EnvEnumerator
console.log(process.env.a);
console.log(JSON.stringify(process.execPath));
console.log(JSON.stringify(process.argv));
console.log(JSON.stringify(process.isMainThread));
console.log(JSON.stringify(process.isMainProcess));
console.log(JSON.stringify(process.cpuUsage()));
console.log(JSON.stringify(process.memoryUsage()));
const buf =  Buffer.alloc(100 * 1024 * 1024)
console.log(JSON.stringify(process.memoryUsage()));