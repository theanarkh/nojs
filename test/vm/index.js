const vm = require('vm');

{
    const ret = vm.run("function sum() { let sum = 0; for (let i = 0; i < 100; i++) { sum += i; } return sum; }; sum();", {mode: vm.MODE.CODE});
    console.logln(ret);
}

{
    const ret = vm.run('test/vm/code.js', { mode: vm.MODE.FILE });
    console.logln(ret);
}

{
    const fun = vm.compileFunction('return 1 + 1', { mode: vm.MODE.CODE });
    console.logln(fun());
}

{
    const fun = vm.compileFunction('test/vm/compileCode.js', { mode: vm.MODE.FILE });
    console.logln(fun());
}

{
    const ret = vm.compileFunction('return arguments[0]', { mode: vm.MODE.CODE, call: true, paramValues: ["hello"]});
    console.logln(ret);
}

{
    const fun = vm.compileFunction('return a + b', { mode: vm.MODE.CODE, paramNames: ["a", "b"]});
    console.logln(fun(1,2));
}

{
    const ret = vm.compileFunction('return a + b', { mode: vm.MODE.CODE, call: true, paramNames: ["a", "b"], paramValues: [1, 2] });
    console.logln(ret);
}

{
    const func = vm.compileFunction('return a + b', { mode: vm.MODE.CODE, paramNames: ["a", "b"], paramValues: [1, 2] });
    console.logln(func(1,2));
}

{
    const func = vm.compileFunction('return a + b', { mode: vm.MODE.CODE, extensions: [{a: 1, b: 2}] });
    console.logln(func(1,2));
}

{
    const func = vm.compileFunction('return a + b', { mode: vm.MODE.CODE, extensions: [{a: 1}, {b: 2}] });
    console.logln(func(1,2));
}

{
    const ret = vm.compileFunction('return a + b', { mode: vm.MODE.CODE, call: true, extensions: [{a: 1}, {b: 2}] });
    console.logln(ret);
}