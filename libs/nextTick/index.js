const {
    microTask,
} = No.buildin;

let queue = [];
let flag = false;

function nextTick(fn, ...args) {
    queue.push({ fn, args });
    if (!flag) {
        flag = true;
        microTask.setMicroTaskFlag(flag);
    }
}

function runMicroTask() {
    let task;
    while(task = queue.shift()) {
        task.fn(...task.args);
    }
    microTask.runMicrotasks();
    flag = false;
    microTask.setMicroTaskFlag(flag);
}

microTask.setMicroTaskCallback(runMicroTask);

function enqueueMicrotask(fn) {
    if (typeof fn !== "function") {
        throw new Error("fn is not a function");
    }
    microTask.enqueueMicrotask(fn);
}

module.exports = {
    nextTick,
    runMicroTask,
    enqueueMicrotask
}

global.process.nextTick = exports.nextTick;
global.process.enqueueMicrotask = exports.enqueueMicrotask;