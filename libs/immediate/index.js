const {
    immediate,
} = No.buildin;

let queue = [];
let flag = false;

function setImmediate(fn, ...args) {
    queue.push({ fn, args });
    if (!flag) {
        flag = true;
        immediate.start();
    }
}

immediate.setImmediateCallback(function() {
    flag = false;
    immediate.stop();
    const _queue = queue;
    queue = [];
    let task;
    while(task = _queue.shift()) {
        task.fn(...task.args);
        No.libs.microtask.runMicroTask();
    }
});

module.exports = {
    setImmediate,
}
