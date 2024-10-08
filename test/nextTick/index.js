process.nextTick(function(a,b) {
    console.log(a, b);
}, 1,2);

Promise.resolve().then(function() {
    console.log('promise')
});