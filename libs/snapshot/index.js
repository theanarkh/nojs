const {
    snapshot,
} = No.buildin;

const serialCallbackQueue = [];
const deserialCallbackQueue = [];

class SnapshotTask {
    #callback
    #data
    constructor(callback, data) {
        this.#callback = callback;
        this.#data = data;
    }

    callback() {
        return this.#callback
    }

    data() {
        return this.#data
    }
}

function addSerialCallback(callback, data) {
    serialCallbackQueue.push(new SnapshotTask(callback, data));
    if (serialCallbackQueue.length === 1) {
        snapshot.addSerialCallback(runSerialCallback)
    }
}

function addDeSerialCallback(callback, data) {
    deserialCallbackQueue.push(new SnapshotTask(callback, data));
    if (deserialCallbackQueue.length === 1) {
        snapshot.addDeSerialCallback(runDeSerialCallback)
    }
}

function runSerialCallback() {
    serialCallbackQueue.forEach(task => {
        task.callback()(task.data());
    })
}

function runDeSerialCallback() {
    deserialCallbackQueue.forEach(task => {
        task.callback()(task.data());
    })
}

function isBuildSnapshot() {
    return snapshot.isBuildSnapshot();
}

function setDeserializeMain(fn, data) {
    snapshot.setDeserializeMain(function() {
        fn(data);
    });
}

module.exports = {
    addSerialCallback,
    addDeSerialCallback,
    isBuildSnapshot,
    setDeserializeMain,
}