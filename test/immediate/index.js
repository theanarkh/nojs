const timer = require('timer');
process.setImmediate(function(a,b) {
    console.log(a);
    console.log(b);
}, 1,2);

// check 不会影响 Poll IO 阶段的阻塞
timer.setTimeout(() => {}, 1000)