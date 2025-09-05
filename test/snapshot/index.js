const snapshot = require("snapshot");

let data = { name: "a" };

if (snapshot.isBuildSnapshot()) {
    snapshot.addSerialCallback(function() {
        data = { name: "b" };
        console.log("serial", data);
    });
    snapshot.addDeSerialCallback(function() {
        console.log("deserial", data);
        data = { name: "c" };
    });
    snapshot.setDeserializeMain(function(args) {
        console.log("setDeserializeMain", data, args);
    }, {a: 1});
}
