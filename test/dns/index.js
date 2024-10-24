const dns = require('dns');

dns.lookup('localhost', (err, list) => {
    console.logln("dns.lookup: ", err, list);
});

const resolver = new dns.Resolver()
resolver.resolve('localhost',(err, list) => {
    console.logln("resolver.resolve: ", err, list);
});