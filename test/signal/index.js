const signal = require('signal');

function handler1() {
    console.logln(111111);
}

function handler2() {
    console.logln(22222);
}

signal.on('SIGUSR1', handler1);

signal.on('SIGUSR1', handler2);

signal.kill("SIGUSR1");

console.logln(`pid: ${process.pid}`)

setTimeout(() => {
    signal.off("SIGUSR1", handler1);
    signal.off("SIGUSR1");
}, 1000)