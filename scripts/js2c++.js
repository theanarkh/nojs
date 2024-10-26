const fs = require('fs');
const path = require('path');

const libsPath = path.join(__dirname, '../libs');
const dirs = fs.readdirSync(libsPath, { recursive: true });

let file;
const map = {};
while(file = dirs.shift()) {
    const fliePath = path.join(libsPath, file);
    const stat = fs.statSync(fliePath);
    if (stat.isFile()) {
        const content = fs.readFileSync(fliePath, 'utf-8');
        map[`libs/${file}`] = content;
    }
}

function makeItem(file, content) {
    return `{"${file}", R"(${content})"}`;
}

const contents = [makeItem('No.js', fs.readFileSync('No.js', "utf-8"))];
for (const [file, content] of Object.entries(map)) {
    contents.push(makeItem(file, content));
}
const content = `
#ifndef JS_CODE_H
#define JS_CODE_H

#include <map>

std::map<std::string, std::string> js_code = {
    ${contents.join(',')}
};

#endif
`;
fs.writeFileSync(path.join(__dirname, '../core/js.h'), content);