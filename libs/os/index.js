const {
    os,
} = No.buildin;

function getConstrainedMemory() {
    return os.getConstrainedMemory();
}

function getAvailableMemory() {
    return os.getAvailableMemory(); 
}

function getAvailableParallelism() {
    return os.getAvailableParallelism();
}

function getTotalMemory() {
    return os.getTotalMemory();
}

function getFreeMemory() {
    return os.getFreeMemory();
}

module.exports = {
    getConstrainedMemory,
    getAvailableMemory,
    getAvailableParallelism,
    getTotalMemory,
    getFreeMemory,
};