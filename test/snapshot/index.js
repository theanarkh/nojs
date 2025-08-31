const snapshot = require("snapshot");

let data = {name: "a"};

if (snapshot.isBuildSnapshot()) {
    snapshot.addSerialCallback(function() {
        console.log("serial", data);
    });
    snapshot.addDeSerialCallback(function() {
        console.log("deserial", data);
    });
}
