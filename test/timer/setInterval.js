const timer = require('timer');
const id1 = timer.setInterval(() => {
    console.log("timeout 3s");
},1000);

id1.stop()