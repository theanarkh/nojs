const vm = require('vm');
const fs = require('fs');
const perf = require('perf');

const code = fs.readSync("test/vm/demo.js");

var cache;
{
    const start = perf.hrtime();
    const script = new vm.Script(code, { filename: "script1.js" });
    console.log("first compile: ", (perf.hrtime() - start) / 1000);
    cache = script.createCodeCache();
    script.run();
}
{   
    const start = perf.hrtime();
    const script = new vm.Script(code, { filename: "script2.js", cache });
    console.log("second compile: ", (perf.hrtime() - start) / 1000);
    script.run();
}