const { workerId } = require('worker');

module.exports = function(socket) {
    console.logln(`handle by worker${workerId}`);
}