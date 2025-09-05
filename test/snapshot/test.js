const snapshot = require('snapshot')

if (snapshot.isBuildSnapshot()) {
  snapshot.hello="world"
} else {
  console.log(snapshot.hello)
}