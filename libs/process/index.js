
const { process, addon } = No.buildin;

function cpuUsage() {
    const data = new Float64Array(2);
    process.cpuUsage(data);
    return {
        user: data[0],
        system: data[1],
    };
}

function memoryUsage() {
    const data = new Float64Array(5);
    process.memoryUsage(data);
    return {
        rss: data[0],
        totalHeapSize: data[1],
        usedHeapSize: data[2],
        externalMemorySize: data[3],
        arrayBufferSize: data[4],
    };
}

function load(filename) {
    const module = { exports: {} };
    addon.load(filename, module, module.exports);
    return module.exports;
}

module.exports = {
    ...process,
    cpuUsage,
    memoryUsage,
    load,
};