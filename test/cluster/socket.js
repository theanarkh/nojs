const { workerId } = require('worker');

module.exports = function(socket) {
    console.log(`handle by worker${workerId}`);
}