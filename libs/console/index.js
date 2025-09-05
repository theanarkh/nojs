const { console } = No.buildin;

const log = (...args) => {
    args.push('\n');
    for (let i = 0; i < args.length; i++) {
        console.log(typeof args[i] === 'object' ? JSON.stringify(args[i]) : args[i]);
    }
}


module.exports = {
    log,
}

global.console = {
    log,
}