const fs = require("fs");

const filename = './test/fs/1.txt';
{
    const arr = Buffer.alloc(100);
    const data = fs.readSync(filename, arr);
    console.logln(data);
}

{
    const data = Buffer.from('AB');
    const nbytes = fs.writeSync(filename, data);
    console.logln(nbytes);
}

fs.read(filename, console.logln);
