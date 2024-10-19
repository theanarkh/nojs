const fs = require("fs");

fs.write('./test/fs/1.txt', Buffer.from('AB'), console.log);
