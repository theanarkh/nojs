const {
    timer,
} = No.buildin;

function setTimeout(cb, ms) {
    return new Timer(cb, ms, false);
}

function setInterval(cb, ms) {
    return new Timer(cb, ms, true);
}

function clearTimeout(timer) {
    timer.stop();
}

function clearInterval(timer) {
    timer.stop();
}

class Timer  {
    timer
    constructor(cb, ms, repeat) {
        this.timer = new timer.Timer();
        this.timer.ontimeout = cb;
        this.timer.start(ms, repeat ? ms : 0);
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
}