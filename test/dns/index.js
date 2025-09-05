const dns = require('dns');

dns.lookup('localhost', (err, list) => {
    console.log("dns.lookup: ", err, list);
});

const resolver = new dns.Resolver()
resolver.resolve('localhost',(err, list) => {
    console.log("resolver.resolve: ", err, list);
});