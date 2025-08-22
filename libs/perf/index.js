const { perf } = No.buildin;

function hrtime() {
    return perf.hrtime();
}

module.exports = {
    hrtime
}