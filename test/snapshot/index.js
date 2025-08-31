const snapshot = require("snapshot");

let data = { name: "a" };

if (snapshot.isBuildSnapshot()) {
    snapshot.addSerialCallback(function() {
        data = { name: "b" };
        console.logln("serial", data);
    });
    snapshot.addDeSerialCallback(function() {
        console.logln("deserial", data);
        data = { name: "c" };
    });
    snapshot.setDeserializeMain(function(args) {
        console.logln("setDeserializeMain", data, args);
    }, {a: 1});
}
