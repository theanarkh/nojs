const {
    dns,
} = No.buildin;

function lookup(hostname, cb) {
    const lookupReq = new dns.LookupReq();
    lookupReq.oncomplete = function(status, list) {
        if (status !== 0) {
            const err = new Error();
            err.code = status;
            cb(err);
            return;
        }
        cb(null, list);
    };
    dns.lookup(lookupReq, hostname, 4)
}

module.exports = {
    lookup,
}