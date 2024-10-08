const {
    dns,
} = No.buildin;

function lookup(hostname, cb) {
    const lookupReq = new dns.LookupReq();
    lookupReq.oncomplete = cb;
    dns.lookup(lookupReq, hostname, 4)
}

module.exports = {
    lookup,
}