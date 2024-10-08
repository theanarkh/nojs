const fs = require("fs");

fs.watch('test/fs/1.txt').on('change', console.log);