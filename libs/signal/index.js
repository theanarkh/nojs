const { signal } = No.buildin;

const { events } = No.libs

const event = new events()
const eventMap = {};

function check(sig) {
    const signum = signal.constant.SIG[sig];
    if (!signum) {
        throw new Error('invalid signal');
    }
    return signum;
}

function on(sig, cb) {
    const signum = check(sig);
    event.on(sig, cb);
    if (event.listenerCount(sig) === 1) {
        eventMap[sig] = new signal.Signal();
        eventMap[sig].start(signum);
        eventMap[sig].onsignal = function(signal) {
            event.emit(sig, signal);
        };
    }
}

function off(sig, cb) {
    check(sig);
    if (typeof cb === 'function') {
        event.off(sig, cb);
    } else {
        event.removeAllListeners(sig);
    }
    if (event.listenerCount(sig) === 0 && eventMap[sig]) {
        eventMap[sig].close();
        eventMap[sig] = null;
    }
}

function kill(sig, pid = process.pid) {
    const signum = check(sig);
    signal.kill(pid, signum);
}

module.exports = {
    on,
    off,
    kill,
    constant: signal.constant,
}