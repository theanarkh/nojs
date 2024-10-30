const timer = require('timer');

process.setImmediate(function(a) {
    console.log(a);
}, 2);

process.setImmediate(function(a) {
    console.log(a);
}, 3);

// check 不会影响 Poll IO 阶段的阻塞
// timer.setTimeout(() => {
//     console.log('xx')
// }, 10000)