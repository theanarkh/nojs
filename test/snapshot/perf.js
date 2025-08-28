if (global.sum) {
  console.log(global.sum);
} else {
  global.sum = 0
  for (let i = 0; i < 100000000; i++) {
    global.sum += 1;
  }
}
