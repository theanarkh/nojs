const {
    vm,
} = No.buildin;

class Script {
    constructor(code, { filename = 'dummy', cache = null } = {}) {
        this.script = new vm.Script(code, filename, cache);
    }
    run() {
        return this.script.run();
    }
    createCodeCache() {
        return this.script.createCodeCache();
    }
}

module.exports = {
    run: vm.run,
    compileFunction: vm.compileFunction,
    MODE: vm.MODE,
    Script,
}