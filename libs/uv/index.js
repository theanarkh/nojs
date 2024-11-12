const {
    uv,
} = No.buildin;

const errMap = uv.getErrMap();

function uvError(errno) {
    const [code, msg] = errMap.get(errno) || ['UNKNOWN', 'unknown error'];
    return { code, msg };
}

module.exports = {
    uvError,
};