const { workerId } = require('worker');

module.exports = function(req, res) {
    console.log(JSON.stringify(req.headers), `handle by worker${workerId}`);
    req.on('data', (buffer) => {
        console.log(buffer.toString());
    });
    req.on('end', () => {
        const body  = `<html>
            <head></head>
            <body>
                你好!
            </body>
            </html>`;
        res.setHeaders({
            "Content-Type": "text/html; charset=UTF-8"
        });
        res.end(body);
    });
}