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

module.exports = {
    nextTick,
    runMicroTask,
}
