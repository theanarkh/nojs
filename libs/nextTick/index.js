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
    flag = false;
    microTask.setMicroTaskFlag(flag);
    const _queue = queue;
    queue = [];
    let task;
    while(task = _queue.shift()) {
        task.fn(...task.args);
    }
    microTask.runMicrotasks();
}

microTask.setMicroTaskCallback(runMicroTask);

module.exports = {
    nextTick,
    runMicroTask,
}
