const {
    dns,
} = No.buildin;

function lookup(hostname, cb) {
    const queryReq = new dns.QueryReq();
    queryReq.oncomplete = function(status, list) {
        if (status !== 0) {
            const err = new Error();
            err.code = status;
            cb(err);
            return;
        }
        cb(null, list);
    };
    dns.lookup(queryReq, hostname, 4)
}

class Resolver {
    constructor(options = {}) {
        const { timeout = -1, retries = 3 } = options;
        this.handle = new dns.Channel(timeout, retries)
    }
    resolve(hostname, cb) {
        const queryReq = new dns.QueryReq();
        queryReq.oncomplete = function(status, list) {
            if (status !== 0) {
                const err = new Error();
                err.code = status;
                cb(err);
                return;
            }
            cb(null, list);
        };
        this.handle.resolve(queryReq, hostname);
    }
}
module.exports = {
    lookup,
    Resolver,
}