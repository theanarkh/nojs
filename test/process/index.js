console.logln(JSON.stringify(process.env)); // Need to implement EnvEnumerator
console.logln(process.env.a);
console.logln(JSON.stringify(process.execPath));
console.logln(JSON.stringify(process.argv));
console.logln(JSON.stringify(process.isMainThread));
console.logln(JSON.stringify(process.isMainProcess));
console.logln(JSON.stringify(process.cpuUsage()));
console.logln(JSON.stringify(process.memoryUsage()));
const buf =  Buffer.alloc(100 * 1024 * 1024)
console.logln(JSON.stringify(process.memoryUsage()));