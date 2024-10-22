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

const contents = [];
for (const [file, content] of Object.entries(map)) {
    contents.push(`{"${file}", R"(${content})"}`)
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