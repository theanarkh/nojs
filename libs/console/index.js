const { console } = No.buildin;

const log = (...args) => {
    for (let i = 0; i < args.length; i++) {
        console.log(args[i]);
    }
}

const logln = (...args) => {
    args.push('\n');
    log(...args);
}
module.exports = {
    log,
    logln,
}