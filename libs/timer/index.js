const {
    timer,
} = No.buildin;

function setTimeoutWithRepeat(cb, ms, repeat) {
    return new Timer(cb, ms, repeat);
}

function setTimeout(cb, ms) {
    return new Timer(cb, ms, 0);
}

function setInterval(cb, ms) {
    return new Timer(cb, ms, ms);
}

function clearTimeout(timer) {
    timer.stop();
}

function clearInterval(timer) {
    timer.stop();
}

function clearTimeoutWithRepeat(timer) {
    timer.stop();
}

class Timer  {
    timer
    constructor(cb, ms, repeat) {
        this.timer = new timer.Timer();
        this.timer.ontimeout = cb;
        this.timer.start(ms, repeat);
    }
    stop() {
        this.timer.stop();
    }
}

module.exports = {
    setTimeout,
    setInterval,
    clearTimeout,
    clearInterval,
    setTimeoutWithRepeat,
    clearTimeoutWithRepeat,
}

global.setTimeout = setTimeout;
global.setInterval = setInterval;
global.clearTimeout = clearTimeout;
global.clearInterval = clearInterval;
global.setTimeoutWithRepeat = setTimeoutWithRepeat;
global.clearTimeoutWithRepeat = clearTimeoutWithRepeat;