const timer = require('timer');
process.setImmediate(function(a,b) {
    console.log(a);
    process.nextTick(() => console.log('tick'));
}, 1);

process.setImmediate(function(a) {
    console.log(a);
}, 2);


// check 不会影响 Poll IO 阶段的阻塞
timer.setTimeout(() => {}, 1000)