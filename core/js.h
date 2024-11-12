
#ifndef JS_CODE_H
#define JS_CODE_H

#include <map>

std::map<std::string, std::string> js_code = {
    {"No.js", R"(const {
    loader,
    process,
    console,
} = No.buildin;

function loaderNativeModule() {
    const modules = [
        {
            module: 'libs/uv/index.js',
            name: 'uv',
        },
        {
            module: 'libs/os/index.js',
            name: 'os',
        },
        {
            module: 'libs/console/index.js',
            name: 'console',
            after: (exports) => {
                global.console = exports;
            }
        },
        {
            module: 'libs/vm/index.js',
            name: 'vm',
        },
        {
            module: 'libs/module/index.js',
            name: 'module'
        },
        {
            module: 'libs/events/index.js',
            name: 'events'
        },
        {
            module: 'libs/timer/index.js',
            name: 'timer',
            after: (exports) => {
                global.setTimeout = exports.setTimeout;
                global.setInterval = exports.setInterval;
            }
        },
        {
            module: 'libs/process/index.js',
            name: 'process',
            after: (exports) => {
                global.process = exports;
            }
        },
        {
            module: 'libs/buffer/index.js',
            name: 'buffer',
            after: (exports) => {
                global.Buffer = exports;
            }
        },
        {
            module: 'libs/nextTick/index.js',
            name: 'microtask',
            after: (exports) => {
                global.process.nextTick = exports.nextTick;
                global.process.enqueueMicrotask = exports.enqueueMicrotask;
            }
        },
        {
            module: 'libs/immediate/index.js',
            name: 'immediate',
            after: (exports) => {
                global.process.setImmediate = exports.setImmediate;
            }
        },
        {
            module: 'libs/dns/index.js',
            name: 'dns'
        },
        {
            module: 'libs/pipe/index.js',
            name: 'pipe'
        },
        {
            module: 'libs/udp/index.js',
            name: 'udp'
        },
        {
            module: 'libs/fs/index.js',
            name: 'fs'
        },
        {
            module: 'libs/tcp/index.js',
            name: 'tcp'
        },
        {
            module: 'libs/http/index.js',
            name: 'http'
        },
        {
            module: 'libs/worker/index.js',
            name: 'worker'
        },
        {
            module: 'libs/child_process/index.js',
            name: 'child_process',
        },
        {
            module: 'libs/cluster/index.js',
            name: 'cluster',
        },
        {
            module: 'libs/signal/index.js',
            name: 'signal',
        },
    ];
    No.libs = {};
    for (let i = 0; i < modules.length; i++) {
        const module = {
            exports: {},
        };
        loader[process.env.Local ? 'compile' : 'compileNative'](modules[i].module).call(null, loader.compile, module.exports, module, No);
        No.libs[modules[i].name] = module.exports;
        typeof modules[i].after === 'function' && modules[i].after(module.exports);
    }
}

loaderNativeModule();

if (process.isMainThread) {
    No.libs.module.load(process.argv[1]);
} else {
    No.libs.module.load("libs/worker/main.js");
    No.libs.module.load(process.argv[1]);
}
)"},{"libs/buffer/index.js", R"(const {
    buffer,
} = No.buildin;

function toUTF8(str) {
    const chars = [...str];
    const bytes = [];
    for (let i = 0; i < chars.length; i++) {
        const char = chars[i];
        const code = char.codePointAt(0);
        if (code > 0 && code < 0x7F) {
            bytes.push(code)
        } else if (code > 0x80 && code < 0x7FF) {
            bytes.push((code >> 6) & 0x1f | 0xC0);
            bytes.push(code & 0x3f | 0x80);  
        } else if ((code > 0x800 && code < 0xFFFF) || (code > 0xE000 && code < 0xFFFF)) {
            bytes.push((code >> 12) & 0x0f | 0xE0);
            bytes.push((code >> 6) & 0x3f | 0x80);
            bytes.push(code & 0x3f | 0x80); 
        } else if (code > 0x10000 && code < 0x10FFFF) {
            bytes.push((code >> 18) & 0x07 | 0xF0);
            bytes.push((code >> 12) & 0x3f | 0x80);
            bytes.push((code >> 6) & 0x3f | 0x80);
            bytes.push(code & 0x3f | 0x80); 
        } else if (code > 0x200000 && code < 0x3FFFFFF) {
            bytes.push((code >> 24) & 0x03 | 0xF8);
            bytes.push((code >> 18) & 0x3f | 0x80);
            bytes.push((code >> 12) & 0x3f | 0x80);
            bytes.push((code >> 6) & 0x3f | 0x80);
            bytes.push(code & 0x3f | 0x80); 
        } else {
            bytes.push((code >> 30) & 0x01 | 0xFC);
            bytes.push((code >> 24) & 0x3f | 0x80);
            bytes.push((code >> 18) & 0x3f | 0x80);
            bytes.push((code >> 12) & 0x3f | 0x80);
            bytes.push((code >> 6) & 0x3f | 0x80);
            bytes.push(code & 0x3f | 0x80); 
        }
    }
    return bytes;
}

function countByte(byte) {
    let bytelen = 0;
    while(byte & 0x80) {
        bytelen++;
        byte = (byte << 1) & 0xFF;
    }
    return bytelen || 1;
}

function fromUTF8(bytes) {
    let i = 0;
    const chars = [];
    while(i < bytes.length) {
        const byteLen = countByte(bytes[i]);
        switch(byteLen) {
            case 1:
                chars.push(String.fromCodePoint(bytes[i]));
                i += 1;
                break;
            case 2:
                chars.push(String.fromCodePoint( (bytes[i] & 0x1F) << 6 | (bytes[i + 1] & 0x3F) ));
                i += 2;
                break;
            case 3:
                chars.push(String.fromCodePoint( (bytes[i] & 0x0F) << 12 | (bytes[i + 1] & 0x3F) << 6| (bytes[i + 2] & 0x3F) ));
                i += 3;
                break;
            case 4:
                chars.push(String.fromCodePoint( (bytes[i] & 0x07) << 18 | (bytes[i + 1] & 0x3F) << 12 | (bytes[i + 2] & 0x3F) << 6 | (bytes[i + 3] & 0x3F) ));
                i += 4;
                break;
            case 5:
                chars.push(String.fromCodePoint( (bytes[i] & 0x03) << 24 | (bytes[i + 1] & 0x3F) << 18 | (bytes[i + 2] & 0x3F) << 12 | (bytes[i + 3] & 0x3F) << 6 | (bytes[i + 4] & 0x3F) ));
                i += 5;
                break;
            case 6:
                chars.push(String.fromCodePoint( (bytes[i] & 0x01) << 30 | (bytes[i + 1] & 0x3F) << 24 | (bytes[i + 2] & 0x3F) << 18 | (bytes[i + 3] & 0x3F) << 12 | (bytes[i + 4] & 0x3F) << 6 | (bytes[i + 5] & 0x3F) ));
                i += 6;
                break;
            default:
                throw new Error('invalid byte');
        }
    }
    return chars.join('');
}

class Buffer extends Uint8Array {
    constructor(...args) {
        super(...args);
    }

    toString(encoding = 'UTF-8') {
        return fromUTF8(this);
    }
    toStringNative(encoding = 'UTF-8') {
        return buffer.fromUTF8(this);
    }
    static strlen(str) {
        return toUTF8(str).length;
    }

    static from(str) {
        const chars = toUTF8(str);
        const buffer = new Buffer(chars.length);
        for (let i = 0; i < buffer.byteLength; i++) {
            buffer[i] = chars[i];
        }
        return buffer;
    }

    static toString(bytes) {
        return fromUTF8(bytes);
    }

    static concat(arr) {
        let len = 0;
        for (let i = 0; i < arr.length; i++) {
            len += arr[i].byteLength;
        }
        const newBuffer = Buffer.alloc(len);
        let index = 0;
        for (let i = 0; i < arr.length; i++) {
            const buffer = arr[i];
            for (let j = 0; j < buffer.byteLength; j++) {
                newBuffer[index++] = buffer[j];
            }
        }
        return newBuffer;
    }

    static alloc(length) {
        return new Buffer(length);
    }
    static fromNative(str) {
        return buffer.writeUTF8(str)
    }
}

module.exports = Buffer;
)"},{"libs/child_process/index.js", R"(const {
    child_process,
} = No.buildin;

class ChildProcess extends No.libs.events {
    constructor() {
        super();
        this.handle = new child_process.ChildProcess();
        this.handle.onexit = (exitCode, exitSignal) => {
            if (exitCode === 0) {
                this.emit('exit', exitCode, exitSignal);
            } else {
                const error = new Error();
                error.code = exitCode;
                error.signal = exitSignal;
                this.emit('error', error);
            }
            // this.handle.close();
            this.handle = null;
        }
    }

    get pid() {
        return this.handle.pid;
    }

    spawn(options) {
        return this.handle.spawn(options);
    }
    kill() {
        // SIGTERM: 15;
        return this.handle.kill(15);
    }
}

function fork(file) {
    const child = new ChildProcess();
    const ret = child.spawn({
        file: process.execPath,
        args: [process.execPath, file],
        env: ["WORKER=1"]
    });
    if (ret !== 0) {
        const err = new Error();
        err.code = ret;
        //err.message = uvErrorMap(ret);
        process.nextTick(() => {
            child.emit('error', err);
        })
    }
    return child;
}

function forkSync(file, options = {}) {
    return child_process.spawnSync({
        ...options,
        file: process.execPath,
        args: [process.execPath, file],
        env: ["WORKER=1"],
    });
}

module.exports = {
    fork,
    forkSync,
})"},{"libs/cluster/http_server.js", R"(const { channel } = require('worker');
const tcp = require('tcp');
const http = require('http');

channel.on('message', (msg) => {
    if (msg.type !== 'SEND_FD') {
        return;
    }
    channel.postMessage({
        type: 'ACK_FD',
        fd: msg.fd,
    });
    const socket = new tcp.ServerSocket({fd: +msg.fd});
    const server = http.createServer();
    server.on('request', (req, res) => {
        require(msg.handler)(req, res);
    });
    new http.HTTPRequest({socket, server});
});)"},{"libs/cluster/index.js", R"(const {
    os,
} = No.buildin;

const { tcp, worker, events } = No.libs;

class Server extends events {
    workers = []
    server
    options
    fds = {};
    next = 0
    constructor(options = {}) {
        super();
        this.options = { ...options, type: "HTTP" };
    }
    listen(options) {
        const server = tcp.createServer({pause: true})
        
        server.on('connection',(connection) => {
            const fd = os.dup(connection.handle.fd);
            if (fd < 0) {
                connection.close();
                return;
            }
            connection.close();
            this.fds[fd] = true;
            const { next } = this;
            this.next = (next + 1) % this.workers.length;
            this.workers[next].postMessage({
                type: "SEND_FD",
                fd,
                handler: this.options.handler,
            });
            this.workers[next].on('message', (msg) => {
                if (msg.type === "ACK_FD") {
                    // TODO: send to next worker if the worker can not handle the fd
                    delete this.fds[msg.fd];
                }
            });
        });
        server.listen(options);
        for (let i = 0; i < (this.options.worker || 2); i++) {
            const w = new worker.Worker(this.file);
            this.workers.push(w);
        }
    }
    get file() {
        return "libs/cluster/server.js"
    }
}

class HTTPServer extends Server {
    get file() {
        return "libs/cluster/http_server.js"
    }
}

function createServer(options) {
    return new Server(options);
}

function createHTTPServer(options) {
    return new HTTPServer(options);
}

module.exports = {
    createServer,
    createHTTPServer,
})"},{"libs/cluster/server.js", R"(const { channel } = require('worker');
const tcp = require('tcp');

channel.on('message', (msg) => {
    if (msg.type !== 'SEND_FD') {
        return;
    }
    channel.postMessage({
        type: 'ACK_FD',
        fd: msg.fd,
    });
    const socket = new tcp.ServerSocket({fd: +msg.fd});
    require(msg.handler)(socket);
});
)"},{"libs/console/index.js", R"(const { console } = No.buildin;

const log = (...args) => {
    for (let i = 0; i < args.length; i++) {
        console.log(typeof args[i] === 'object' ? JSON.stringify(args[i]) : args[i]);
    }
}

const logln = (...args) => {
    args.push('\n');
    log(...args);
}
module.exports = {
    log,
    logln,
})"},{"libs/dns/index.js", R"(const {
    dns,
} = No.buildin;

function lookup(hostname, cb) {
    const queryReq = new dns.QueryReq();
    queryReq.oncomplete = function(status, list) {
        if (status !== 0) {
            const err = new Error();
            err.code = status;
            cb(err);
            return;
        }
        cb(null, list);
    };
    dns.lookup(queryReq, hostname, 4)
}

class Resolver {
    constructor(options = {}) {
        const { timeout = -1, retries = 3 } = options;
        this.handle = new dns.Channel(timeout, retries)
    }
    resolve(hostname, cb) {
        const queryReq = new dns.QueryReq();
        queryReq.oncomplete = function(status, list) {
            if (status !== 0) {
                const err = new Error();
                err.code = status;
                cb(err);
                return;
            }
            cb(null, list);
        };
        this.handle.resolve(queryReq, hostname);
    }
}
module.exports = {
    lookup,
    Resolver,
})"},{"libs/events/index.js", R"(// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

'use strict';

var R = typeof Reflect === 'object' ? Reflect : null
var ReflectApply = R && typeof R.apply === 'function'
  ? R.apply
  : function ReflectApply(target, receiver, args) {
    return Function.prototype.apply.call(target, receiver, args);
  }

var ReflectOwnKeys
if (R && typeof R.ownKeys === 'function') {
  ReflectOwnKeys = R.ownKeys
} else if (Object.getOwnPropertySymbols) {
  ReflectOwnKeys = function ReflectOwnKeys(target) {
    return Object.getOwnPropertyNames(target)
      .concat(Object.getOwnPropertySymbols(target));
  };
} else {
  ReflectOwnKeys = function ReflectOwnKeys(target) {
    return Object.getOwnPropertyNames(target);
  };
}

function ProcessEmitWarning(warning) {
  if (console && console.warn) console.warn(warning);
}

var NumberIsNaN = Number.isNaN || function NumberIsNaN(value) {
  return value !== value;
}

function EventEmitter() {
  EventEmitter.init.call(this);
}
module.exports = EventEmitter;
module.exports.once = once;

// Backwards-compat with node 0.10.x
EventEmitter.EventEmitter = EventEmitter;

EventEmitter.prototype._events = undefined;
EventEmitter.prototype._eventsCount = 0;
EventEmitter.prototype._maxListeners = undefined;

// By default EventEmitters will print a warning if more than 10 listeners are
// added to it. This is a useful default which helps finding memory leaks.
var defaultMaxListeners = 10;

function checkListener(listener) {
  if (typeof listener !== 'function') {
    throw new TypeError('The "listener" argument must be of type Function. Received type ' + typeof listener);
  }
}

Object.defineProperty(EventEmitter, 'defaultMaxListeners', {
  enumerable: true,
  get: function() {
    return defaultMaxListeners;
  },
  set: function(arg) {
    if (typeof arg !== 'number' || arg < 0 || NumberIsNaN(arg)) {
      throw new RangeError('The value of "defaultMaxListeners" is out of range. It must be a non-negative number. Received ' + arg + '.');
    }
    defaultMaxListeners = arg;
  }
});

EventEmitter.init = function() {

  if (this._events === undefined ||
      this._events === Object.getPrototypeOf(this)._events) {
    this._events = Object.create(null);
    this._eventsCount = 0;
  }

  this._maxListeners = this._maxListeners || undefined;
};

// Obviously not all Emitters should be limited to 10. This function allows
// that to be increased. Set to zero for unlimited.
EventEmitter.prototype.setMaxListeners = function setMaxListeners(n) {
  if (typeof n !== 'number' || n < 0 || NumberIsNaN(n)) {
    throw new RangeError('The value of "n" is out of range. It must be a non-negative number. Received ' + n + '.');
  }
  this._maxListeners = n;
  return this;
};

function _getMaxListeners(that) {
  if (that._maxListeners === undefined)
    return EventEmitter.defaultMaxListeners;
  return that._maxListeners;
}

EventEmitter.prototype.getMaxListeners = function getMaxListeners() {
  return _getMaxListeners(this);
};

EventEmitter.prototype.emit = function emit(type) {
  var args = [];
  for (var i = 1; i < arguments.length; i++) args.push(arguments[i]);
  var doError = (type === 'error');

  var events = this._events;
  if (events !== undefined)
    doError = (doError && events.error === undefined);
  else if (!doError)
    return false;

  // If there is no 'error' event listener then throw.
  if (doError) {
    var er;
    if (args.length > 0)
      er = args[0];
    if (er instanceof Error) {
      // Note: The comments on the `throw` lines are intentional, they show
      // up in Node's output if this results in an unhandled exception.
      throw er; // Unhandled 'error' event
    }
    // At least give some kind of context to the user
    var err = new Error('Unhandled error.' + (er ? ' (' + er.message + ')' : ''));
    err.context = er;
    throw err; // Unhandled 'error' event
  }

  var handler = events[type];

  if (handler === undefined)
    return false;

  if (typeof handler === 'function') {
    ReflectApply(handler, this, args);
  } else {
    var len = handler.length;
    var listeners = arrayClone(handler, len);
    for (var i = 0; i < len; ++i)
      ReflectApply(listeners[i], this, args);
  }

  return true;
};

function _addListener(target, type, listener, prepend) {
  var m;
  var events;
  var existing;

  checkListener(listener);

  events = target._events;
  if (events === undefined) {
    events = target._events = Object.create(null);
    target._eventsCount = 0;
  } else {
    // To avoid recursion in the case that type === "newListener"! Before
    // adding it to the listeners, first emit "newListener".
    if (events.newListener !== undefined) {
      target.emit('newListener', type,
                  listener.listener ? listener.listener : listener);

      // Re-assign `events` because a newListener handler could have caused the
      // this._events to be assigned to a new object
      events = target._events;
    }
    existing = events[type];
  }

  if (existing === undefined) {
    // Optimize the case of one listener. Don't need the extra array object.
    existing = events[type] = listener;
    ++target._eventsCount;
  } else {
    if (typeof existing === 'function') {
      // Adding the second element, need to change to array.
      existing = events[type] =
        prepend ? [listener, existing] : [existing, listener];
      // If we've already got an array, just append.
    } else if (prepend) {
      existing.unshift(listener);
    } else {
      existing.push(listener);
    }

    // Check for listener leak
    m = _getMaxListeners(target);
    if (m > 0 && existing.length > m && !existing.warned) {
      existing.warned = true;
      // No error code for this since it is a Warning
      // eslint-disable-next-line no-restricted-syntax
      var w = new Error('Possible EventEmitter memory leak detected. ' +
                          existing.length + ' ' + String(type) + ' listeners ' +
                          'added. Use emitter.setMaxListeners() to ' +
                          'increase limit');
      w.name = 'MaxListenersExceededWarning';
      w.emitter = target;
      w.type = type;
      w.count = existing.length;
      ProcessEmitWarning(w);
    }
  }

  return target;
}

EventEmitter.prototype.addListener = function addListener(type, listener) {
  return _addListener(this, type, listener, false);
};

EventEmitter.prototype.on = EventEmitter.prototype.addListener;

EventEmitter.prototype.prependListener =
    function prependListener(type, listener) {
      return _addListener(this, type, listener, true);
    };

function onceWrapper() {
  if (!this.fired) {
    this.target.removeListener(this.type, this.wrapFn);
    this.fired = true;
    if (arguments.length === 0)
      return this.listener.call(this.target);
    return this.listener.apply(this.target, arguments);
  }
}

function _onceWrap(target, type, listener) {
  var state = { fired: false, wrapFn: undefined, target: target, type: type, listener: listener };
  var wrapped = onceWrapper.bind(state);
  wrapped.listener = listener;
  state.wrapFn = wrapped;
  return wrapped;
}

EventEmitter.prototype.once = function once(type, listener) {
  checkListener(listener);
  this.on(type, _onceWrap(this, type, listener));
  return this;
};

EventEmitter.prototype.prependOnceListener =
    function prependOnceListener(type, listener) {
      checkListener(listener);
      this.prependListener(type, _onceWrap(this, type, listener));
      return this;
    };

// Emits a 'removeListener' event if and only if the listener was removed.
EventEmitter.prototype.removeListener =
    function removeListener(type, listener) {
      var list, events, position, i, originalListener;

      checkListener(listener);

      events = this._events;
      if (events === undefined)
        return this;

      list = events[type];
      if (list === undefined)
        return this;

      if (list === listener || list.listener === listener) {
        if (--this._eventsCount === 0)
          this._events = Object.create(null);
        else {
          delete events[type];
          if (events.removeListener)
            this.emit('removeListener', type, list.listener || listener);
        }
      } else if (typeof list !== 'function') {
        position = -1;

        for (i = list.length - 1; i >= 0; i--) {
          if (list[i] === listener || list[i].listener === listener) {
            originalListener = list[i].listener;
            position = i;
            break;
          }
        }

        if (position < 0)
          return this;

        if (position === 0)
          list.shift();
        else {
          spliceOne(list, position);
        }

        if (list.length === 1)
          events[type] = list[0];

        if (events.removeListener !== undefined)
          this.emit('removeListener', type, originalListener || listener);
      }

      return this;
    };

EventEmitter.prototype.off = EventEmitter.prototype.removeListener;

EventEmitter.prototype.removeAllListeners =
    function removeAllListeners(type) {
      var listeners, events, i;

      events = this._events;
      if (events === undefined)
        return this;

      // not listening for removeListener, no need to emit
      if (events.removeListener === undefined) {
        if (arguments.length === 0) {
          this._events = Object.create(null);
          this._eventsCount = 0;
        } else if (events[type] !== undefined) {
          if (--this._eventsCount === 0)
            this._events = Object.create(null);
          else
            delete events[type];
        }
        return this;
      }

      // emit removeListener for all listeners on all events
      if (arguments.length === 0) {
        var keys = Object.keys(events);
        var key;
        for (i = 0; i < keys.length; ++i) {
          key = keys[i];
          if (key === 'removeListener') continue;
          this.removeAllListeners(key);
        }
        this.removeAllListeners('removeListener');
        this._events = Object.create(null);
        this._eventsCount = 0;
        return this;
      }

      listeners = events[type];

      if (typeof listeners === 'function') {
        this.removeListener(type, listeners);
      } else if (listeners !== undefined) {
        // LIFO order
        for (i = listeners.length - 1; i >= 0; i--) {
          this.removeListener(type, listeners[i]);
        }
      }

      return this;
    };

function _listeners(target, type, unwrap) {
  var events = target._events;

  if (events === undefined)
    return [];

  var evlistener = events[type];
  if (evlistener === undefined)
    return [];

  if (typeof evlistener === 'function')
    return unwrap ? [evlistener.listener || evlistener] : [evlistener];

  return unwrap ?
    unwrapListeners(evlistener) : arrayClone(evlistener, evlistener.length);
}

EventEmitter.prototype.listeners = function listeners(type) {
  return _listeners(this, type, true);
};

EventEmitter.prototype.rawListeners = function rawListeners(type) {
  return _listeners(this, type, false);
};

EventEmitter.listenerCount = function(emitter, type) {
  if (typeof emitter.listenerCount === 'function') {
    return emitter.listenerCount(type);
  } else {
    return listenerCount.call(emitter, type);
  }
};

EventEmitter.prototype.listenerCount = listenerCount;
function listenerCount(type) {
  var events = this._events;

  if (events !== undefined) {
    var evlistener = events[type];

    if (typeof evlistener === 'function') {
      return 1;
    } else if (evlistener !== undefined) {
      return evlistener.length;
    }
  }

  return 0;
}

EventEmitter.prototype.eventNames = function eventNames() {
  return this._eventsCount > 0 ? ReflectOwnKeys(this._events) : [];
};

function arrayClone(arr, n) {
  var copy = new Array(n);
  for (var i = 0; i < n; ++i)
    copy[i] = arr[i];
  return copy;
}

function spliceOne(list, index) {
  for (; index + 1 < list.length; index++)
    list[index] = list[index + 1];
  list.pop();
}

function unwrapListeners(arr) {
  var ret = new Array(arr.length);
  for (var i = 0; i < ret.length; ++i) {
    ret[i] = arr[i].listener || arr[i];
  }
  return ret;
}

function once(emitter, name) {
  return new Promise(function (resolve, reject) {
    function errorListener(err) {
      emitter.removeListener(name, resolver);
      reject(err);
    }

    function resolver() {
      if (typeof emitter.removeListener === 'function') {
        emitter.removeListener('error', errorListener);
      }
      resolve([].slice.call(arguments));
    };

    eventTargetAgnosticAddListener(emitter, name, resolver, { once: true });
    if (name !== 'error') {
      addErrorHandlerIfEventEmitter(emitter, errorListener, { once: true });
    }
  });
}

function addErrorHandlerIfEventEmitter(emitter, handler, flags) {
  if (typeof emitter.on === 'function') {
    eventTargetAgnosticAddListener(emitter, 'error', handler, flags);
  }
}

function eventTargetAgnosticAddListener(emitter, name, listener, flags) {
  if (typeof emitter.on === 'function') {
    if (flags.once) {
      emitter.once(name, listener);
    } else {
      emitter.on(name, listener);
    }
  } else if (typeof emitter.addEventListener === 'function') {
    // EventTarget does not have `error` event semantics like Node
    // EventEmitters, we do not listen for `error` events here.
    emitter.addEventListener(name, function wrapListener(arg) {
      // IE does not have builtin `{ once: true }` support so we
      // have to do it manually.
      if (flags.once) {
        emitter.removeEventListener(name, wrapListener);
      }
      listener(arg);
    });
  } else {
    throw new TypeError('The "emitter" argument must be of type EventEmitter. Received type ' + typeof emitter);
  }
})"},{"libs/fs/index.js", R"(const {
    fs,
    fswatcher,
} = No.buildin;

function open(path, cb) {
    const req = new fs.FSReqCallback()
    req.oncomplete = function(fd) {
        if (fd < 0) {
            const err = new Error();
            err.code = fd;
            cb(err);
            return;
        }
        cb(null, fd);
    }
    fs.open(path, fs.constant.FLAG.O_RDWR, fs.constant.MODE.S_IRUSR | fs.constant.MODE.S_IWUSR | fs.constant.MODE.S_IRGRP | fs.constant.MODE.S_IROTH, req)
}

function openSync(path) {
    return fs.openSync(path);
}

function unlink(path, cb) {
    const req = new fs.FSReqCallback()
    req.oncomplete = function(status) {
        if (status < 0) {
            const err = new Error();
            err.code = status;
            cb(err);
            return;
        }
        cb(null);
    }
    fs.unlink(path, req);
}

function unlinkSync(path) {
    return fs.unlinkSync(path);
}

function read(path, cb) {
    open(path, function(err, fd) {
        if (err) {
            cb(err);
            return;
        }
        const buffer = Buffer.alloc(100)
        const req = new fs.FSReqCallback()
        req.oncomplete = function(status) {
            if (status < 0) {
                const err = new Error();
                err.code = status;
                cb(err);
            } else {
                cb(null, buffer.toString(), status);
            }
        }
        return fs.read(fd, buffer, req)
    })
}

function write(path, data, cb) {
    open(path, function(err, fd) {
        if (err) {
            cb(err);
            return;
        }
        if (typeof data === 'stirng') {
            data = Buffer.from(data)
        }
        const req = new fs.FSReqCallback()
        req.oncomplete = function(status) {
            if (status < 0) {
                const err = new Error();
                err.code = status;
                cb(err);
            } else {
                cb(null, status);
            }
        }
        return fs.write(fd, data, req)
    })
}

function readSync(path) {
    const fd = fs.openSync(path)
    const buffer = Buffer.alloc(100)
    const ret = fs.readSync(fd, buffer)
    return buffer.toString()
}

function writeSync(path, data) {
    const fd = fs.openSync(path, fs.constant.FLAG.O_RDWR)
    if (typeof data === 'stirng') {
        data = Buffer.from(data)
    }
    const ret = fs.writeSync(fd, data)
    return ret
}

class Watcher extends No.libs.events {
    constructor(path, options) {
        super();
        this.handle = new fswatcher.Watcher();
        this.handle.start(path, options.recursive);
        this.handle.onchange = (status, events, filename) => {
            if (status < 0) {
                if (this.handle !== null) {
                    this.handle.close();
                    this.handle = null;
                }
                const error = new Error();
                error.code = status;
                error.filename = filename;
                this.emit('error', error);
            } else {
                this.emit('change', events, filename);
            }
        }
    }
    close() {
        if (this.handle) {
            this.handle.close();
            this.handle = null;
        }
    }
}

function watch(path, options = {}) {
    return new Watcher(path, options);
}

module.exports = {
    open, 
    openSync,
    readSync,
    write,
    writeSync,
    read,
    unlink,
    unlinkSync,
    watch,
})"},{"libs/http/index.js", R"(const {
    HTTPParser,
} = No.buildin;

const { tcp } = No.libs;

class Server extends tcp.Server {
    constructor(options = {}, cb) {
        super(options);
        this.options = options;
        if (typeof cb === 'function') {
            this.on('request', cb);
        }
        this.on('connection', (socket) => {
            new HTTPRequest({socket, server: this});
        });
    }
}

class HTTPRequest extends No.libs.events {
    socket = null;
    httpparser = null;
    constructor({socket, server}) {
        super();
        this.server = server;
        this.socket = socket;
        this.httpparser = new HTTPParser(1);
        this.httpparser.onHeaderComplete = (data) => {
            this.method = data.method;
            this.major = data.major;
            this.minor = data.minor;
            this.keepalive = data.keepalive;
            this.upgrade = data.upgrade;
            this.headers = data.headers;
            const response = new HTTPResponse({socket, server});
            this.server.emit('request', this, response);
        }
        this.httpparser.onBody = (data) => {
            this.emit('data', data);
        }
        this.httpparser.onBodyComplete = () => {
            this.emit('end');
        }
        socket.on('data', (buffer) => {
            this.httpparser.parse(buffer);
        });
        socket.on('end', () => {
            this.emit('end');
        });
    }
}

class HTTPResponse extends No.libs.events {
    statusCode = 200;
    headers = {
        "Server": "No.js"
    };
    socket = null;
    constructor({socket, server}) {
        super();
        this.server = server;
        this.socket = socket;
    }
    buildHeaders(headers) {
        const arr = [];
        for (const [k, v] of Object.entries(headers)) {
            arr.push(`${k}:${v}\r\n`);
        }
        return arr.join('');
    }
    end(data) {
        const responseLine = `HTTP/1.1 ${this.statusCode} OK`;
        const headers = { ...this.headers, "Content-length": Buffer.strlen(data)};
        const responseHeaders = this.buildHeaders(headers);
        this.socket.write(`${responseLine}\r\n${responseHeaders}\r\n${data}\r\n\r\n`);
    }
    setHeaders(headers) {
        for (const [k, v] of Object.entries(headers)) {
            this.setHeader(k, v);
        }
    }
    setHeader(k, v) {
        this.headers[k] = v;
    }
}

class HTTPClientResponse extends No.libs.events {
    constructor(options) {
        super();
        this.status = options.status;
        this.statusMessage = options.statusMessage;
        this.major = options.major;
        this.minor = options.minor;
        this.keepalive = options.keepalive;
        this.upgrade = options.upgrade;
        this.headers = options.headers;
    }
}

class HTTPClient extends No.libs.events {
    socket = null;
    httpparser = null;
    constructor(options) {
        super();
        let response
        this.socket = new tcp.ClientSocket();
        this.socket.connect(options);
        this.socket.on('error', () => {
            const error = new Error('socket close before recieve response');
            if (response) {
                response.emit('error', error)
            } else {
                this.emit('error', error);
            }
        });
        this.socket.on('connection', () => {
            this.socket.write(`${options.method || 'GET'} ${options.url || '/'} HTTP/1.1\r\n`)
            if (options.headers) {
                const arr = [];
                for (const [k, v] of Object.entries(options.headers)) {
                    arr.push(`${k}:${v}\r\n`);
                }
                this.socket.write(arr.join('') + '\r\n')
            }
            if (options.body) {
                this.socket.write(options.body)
            }
            this.socket.write('\r\n\r\n');
            
            this.httpparser = new HTTPParser(2);
            this.httpparser.onHeaderComplete = (data) => {
                response = new HTTPClientResponse(data);
                this.emit('response', response);
            }
            this.httpparser.onBody = (data) => {
                response.emit('data', data);
            }
            this.httpparser.onBodyComplete = (data) => {
                console.log(data);
            }
            this.socket.on('data', (buffer) => {
                this.httpparser.parse(buffer);
            });
            this.socket.on('end', () => {
                if (!response) {
                    this.emit('error', new Error('socket close before recieve response'))
                } else {
                    response.emit('end');
                }
            });
        })
    }
}

function createServer(...arg) {
    return new Server(...arg);
}

function request(options) {
    return new HTTPClient(options);
}
module.exports = {
    createServer,
    request,
    HTTPRequest,
})"},{"libs/immediate/index.js", R"(const {
    immediate,
} = No.buildin;

let queue = [];
let flag = false;

function setImmediate(fn, ...args) {
    queue.push({ fn, args });
    if (!flag) {
        flag = true;
        immediate.start();
    }
}

immediate.setImmediateCallback(function() {
    flag = false;
    immediate.stop();
    const _queue = queue;
    queue = [];
    let task;
    while(task = _queue.shift()) {
        task.fn(...task.args);
        No.libs.microtask.runMicroTask();
    }
});

module.exports = {
    setImmediate,
}
)"},{"libs/module/index.js", R"(const {
    loader,
} = No.buildin;

const map = {};
class Module {
    constructor(filename) {
        this.filename = filename;
        this.exports = {};
    }
    load() {
        const result = loader.compile(this.filename);
        result.call(this, Module.load, this.exports, this);
        return this.exports;
    }
    static load(filename, ...args) {
        if (No.libs[filename]) {
            return No.libs[filename]
        }
        if (!/\.js$/.test(filename)) {
            filename = `${filename}.js`;
        }
        if (map[filename]) {
            return map[filename];
        }
        const module = new Module(filename, ...args);
        return (map[filename] = module.load());
    }
};

module.exports = Module;)"},{"libs/nextTick/index.js", R"(const {
    microTask,
} = No.buildin;

let queue = [];
let flag = false;

function nextTick(fn, ...args) {
    queue.push({ fn, args });
    if (!flag) {
        flag = true;
        microTask.setMicroTaskFlag(flag);
    }
}

function runMicroTask() {
    let task;
    while(task = queue.shift()) {
        task.fn(...task.args);
    }
    microTask.runMicrotasks();
    flag = false;
    microTask.setMicroTaskFlag(flag);
}

microTask.setMicroTaskCallback(runMicroTask);

function enqueueMicrotask(fn) {
    if (typeof fn !== "function") {
        throw new Error("fn is not a function");
    }
    microTask.enqueueMicrotask(fn);
}

module.exports = {
    nextTick,
    runMicroTask,
    enqueueMicrotask
}
)"},{"libs/os/index.js", R"(const {
    os,
} = No.buildin;

function getConstrainedMemory() {
    return os.getConstrainedMemory();
}

function getAvailableMemory() {
    return os.getAvailableMemory(); 
}

function getAvailableParallelism() {
    return os.getAvailableParallelism();
}

function getTotalMemory() {
    return os.getTotalMemory();
}

function getFreeMemory() {
    return os.getFreeMemory();
}

module.exports = {
    getConstrainedMemory,
    getAvailableMemory,
    getAvailableParallelism,
    getTotalMemory,
    getFreeMemory,
};)"},{"libs/pipe/index.js", R"(const {
    pipe,
} = No.buildin;

const { events } = No.libs;

class Server extends events {
    handle = null;
    connections = 0;
    constructor(options = {}) {
        super();
    }
    listen(options) {
        this.handle = new pipe.Pipe(0);
        let ret = this.handle.bind(options.path);
        if (ret !== 0) {
            const err = new Error();
            err.code = ret;
            this.emit("error", err);
            return;
        }
        this.handle.onconnection = (status) => {
           if (status == 0) {
            this.connections++;
            const handle = new pipe.Pipe(0)
            const ret = this.handle.accept(handle);
            if (ret != 0) {
                console.log(ret)
                return
            }
            const socket = new ServerSocket({ handle })
            this.emit('connection', socket);
           }
        }
        this.handle.listen(100)
    }
    close() {
        // this.handle.close();
        // this.handle = null;
    }
}

class Socket extends events {
    handle
    write(buffer) {
        if (typeof buffer === 'string') {
            buffer = Buffer.from(buffer);
        }
        const req = new tcp.WriteReq();
        req.buffer = buffer;
        req.oncomplete = (status) => {
            console.logln("after write", status);
        };
        this.handle.write(req, buffer);
    }
    close() {
        this.handle.close();
        this.handle = null;
    }
}

class ClientSocket extends Socket {
    constructor(options = {}) {
        super(options);
        this.handle = new pipe.Pipe(0);
    }
    connect(options) {
        const req = new pipe.PipeConenctReq();
        req.onconnection = (status) => {
            if (status === 0) {
                this.emit('connection');
                // call readStart to keep loop alive
                this.handle.readStart();
            } else {
                this.emit('error');
            }
        };
        return this.handle.connect(req, options.path)
    }
}

class ServerSocket extends Socket {
    constructor(options = {}) {
        super(options);
        this.handle = options.handle;
        this.handle.onread = (nread, data) => {
            if (nread === 0) {
                this.emit('end');
            } else if (nread > 0){
                this.emit('data', new Buffer(data));
            } else {
                this.emit('error', new Error('read socket error'));
            }
        }
        this.handle.readStart();
    }
}

function createServer(...arg) {
    return new Server(...arg);
}

module.exports = {
    createServer,
    Server,
    ClientSocket,
})"},{"libs/process/index.js", R"(
const { process, addon } = No.buildin;

function cpuUsage() {
    const data = new Float64Array(2);
    process.cpuUsage(data);
    return {
        user: data[0],
        system: data[1],
    };
}

function memoryUsage() {
    const data = new Float64Array(5);
    process.memoryUsage(data);
    return {
        rss: data[0],
        totalHeapSize: data[1],
        usedHeapSize: data[2],
        externalMemorySize: data[3],
        arrayBufferSize: data[4],
    };
}

function load(filename) {
    const module = { exports: {} };
    addon.load(filename, module, module.exports);
    return module.exports;
}

module.exports = {
    ...process,
    cpuUsage,
    memoryUsage,
    load,
};)"},{"libs/signal/index.js", R"(const { signal } = No.buildin;

const { events } = No.libs

const event = new events()
const eventMap = {};

function check(sig) {
    const signum = signal.constant.SIG[sig];
    if (!signum) {
        throw new Error('invalid signal');
    }
    return signum;
}

function on(sig, cb) {
    const signum = check(sig);
    event.on(sig, cb);
    if (event.listenerCount(sig) === 1) {
        eventMap[sig] = new signal.Signal();
        eventMap[sig].start(signum);
        eventMap[sig].onsignal = function(signal) {
            event.emit(sig, signal);
        };
    }
}

function off(sig, cb) {
    check(sig);
    if (typeof cb === 'function') {
        event.off(sig, cb);
    } else {
        event.removeAllListeners(sig);
    }
    if (event.listenerCount(sig) === 0 && eventMap[sig]) {
        eventMap[sig].close();
        eventMap[sig] = null;
    }
}

function kill(sig, pid = process.pid) {
    const signum = check(sig);
    signal.kill(pid, signum);
}

module.exports = {
    on,
    off,
    kill,
    constant: signal.constant,
})"},{"libs/tcp/index.js", R"(const {
    tcp,
} = No.buildin;

const { events, dns } = No.libs;

// IPv4 Segment
const v4Seg = '(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])';
const v4Str = `(?:${v4Seg}\\.){3}${v4Seg}`;
const IPv4Reg = new RegExp(`^${v4Str}$`);

// IPv6 Segment
const v6Seg = '(?:[0-9a-fA-F]{1,4})';
const IPv6Reg = new RegExp('^(?:' +
  `(?:${v6Seg}:){7}(?:${v6Seg}|:)|` +
  `(?:${v6Seg}:){6}(?:${v4Str}|:${v6Seg}|:)|` +
  `(?:${v6Seg}:){5}(?::${v4Str}|(?::${v6Seg}){1,2}|:)|` +
  `(?:${v6Seg}:){4}(?:(?::${v6Seg}){0,1}:${v4Str}|(?::${v6Seg}){1,3}|:)|` +
  `(?:${v6Seg}:){3}(?:(?::${v6Seg}){0,2}:${v4Str}|(?::${v6Seg}){1,4}|:)|` +
  `(?:${v6Seg}:){2}(?:(?::${v6Seg}){0,3}:${v4Str}|(?::${v6Seg}){1,5}|:)|` +
  `(?:${v6Seg}:){1}(?:(?::${v6Seg}){0,4}:${v4Str}|(?::${v6Seg}){1,6}|:)|` +
  `(?::(?:(?::${v6Seg}){0,5}:${v4Str}|(?::${v6Seg}){1,7}|:))` +
')(?:%[0-9a-zA-Z-.:]{1,})?$');

function isIPv4(s) {
  return IPv4Reg.test(s)
}

function isIPv6(s) {
  return IPv6Reg.test(s)
}

function isIP(s) {
  if (isIPv4(s)) return 4;
  if (isIPv6(s)) return 6;
  return 0;
}

class Server extends events {
    handle = null;
    connections = 0;
    constructor(options = {}) {
        super();
        this.options = options;
    }
    listen(options = {}) {
        this.handle = new tcp.TCP();
        let flags = 0;
        if (options.ipv6Only) {
            flags |= tcp.constant.FLAG.UV_TCP_IPV6ONLY
        }
        if (options.reusePort) {
            flags |= tcp.constant.FLAG.UV_TCP_REUSEPORT
        }
        let ret = this.handle.bind(options.host, options.port, flags);
        this.handle.onconnection = (status) => {
           if (status == 0) {
            this.connections++;
            const handle = new tcp.TCP()
            const ret = this.handle.accept(handle);
            if (ret != 0) {
                console.log(ret)
                return
            }
            const socket = new ServerSocket({ handle, pause: this.options.pause })
            this.emit('connection', socket);
           }
        }
        this.handle.listen(100)
    }
    close() {
        this.handle.close();
        this.handle = null;
    }
}

class Socket extends events {
    handle
    write(buffer) {
        if (typeof buffer === 'string') {
            buffer = Buffer.from(buffer);
        }
        const req = new tcp.WriteReq();
        req.buffer = buffer;
        req.oncomplete = (status) => {
            console.logln("after write", status);
        };
        this.handle.write(req, buffer);
    }
    close() {
        this.handle.close();
        this.handle = null;
    }
}

class ClientSocket extends Socket {
    constructor(options = {}) {
        super(options);
        this.handle = new tcp.TCP();
        this.handle.onread = (nread, data) => {
            if (nread === 0) {
                this.emit('end');
            } else if (nread > 0){
                this.emit('data', new Buffer(data));
            } else {
                this.emit('error', new Error('read socket error'));
            }
        }
    }
    connect(options) {
        const req = new tcp.TCPConnectReq();
        req.onconnection = (status) => {
            if (status === 0) {
                this.handle.readStart();
                this.emit('connection');
            } else {
                this.emit('error', new Error('failed to connect'));
            }
        };
        if (isIP(options.host) > 0) {
            this.handle.connect(req, options.host, options.port);
        } else {
            dns.lookup(options.host, (status, addresses) => {
                if (status !== 0) {
                    this.emit('error', new Error('failed to lookup host'));
                    return;
                }
                this.handle.connect(req, addresses[0], options.port);
            })
        }
    }
}

class ServerSocket extends Socket {
    constructor(options = {}) {
        super(options);
        if (options.handle) {
            this.handle = options.handle;
        } else if (options.fd){
            this.handle = new tcp.TCP();
            this.handle.open(options.fd);
        }
        
        if (!options.pause) {
            this.handle.onread = (nread, data) => {
                if (nread === 0) {
                    this.emit('end');
                } else if (nread > 0){
                    this.emit('data', new Buffer(data));
                } else {
                    this.emit('error', new Error('read socket error'));
                }
            }
            this.handle.readStart();
        }
    }
    write(buffer) {
        if (typeof buffer === 'string') {
            buffer = Buffer.from(buffer);
        }
        const req = new tcp.WriteReq();
        req.oncomplete = (status) => {
            // console.log("after write", status);
        };
        this.handle.write(req, buffer);
    }
}

function createServer(...arg) {
    return new Server(...arg);
}

module.exports = {
    createServer,
    Server,
    ClientSocket,
    ServerSocket,
})"},{"libs/timer/index.js", R"(const {
    timer,
} = No.buildin;

function setTimeoutWithRepeat(cb, ms, repeat) {
    return new Timer(cb, ms, repeat);
}

function setTimeout(cb, ms) {
    return new Timer(cb, ms, 0);
}

function setInterval(cb, ms) {
    return new Timer(cb, ms, ms);
}

function clearTimeout(timer) {
    timer.stop();
}

function clearInterval(timer) {
    timer.stop();
}

function clearTimeoutWithRepeat(timer) {
    timer.stop();
}

class Timer  {
    timer
    constructor(cb, ms, repeat) {
        this.timer = new timer.Timer();
        this.timer.ontimeout = cb;
        this.timer.start(ms, repeat);
    }
    stop() {
        this.timer.stop();
    }
}

module.exports = {
    setTimeout,
    setInterval,
    clearTimeout,
    clearInterval,
    setTimeoutWithRepeat,
    clearTimeoutWithRepeat,
})"},{"libs/udp/index.js", R"(const {
    udp,
} = No.buildin;

const { events } = No.libs;

const TYPE = {
    UDP4: 1,
    UDP6: 2,
};

const UNBINDED = 0;
const BINDING = 1;
const BINDED = 2

const UNCONNECTED = 0;
const CONNECTING = 1;
const CONNECTED = 2;

class UDP extends events {
    handle = null;
    constructor(options = { type: TYPE.UDP4 }) {
        super();
        this.handle = new udp.UDP();
        this.type = options.type;
        this.bindState = UNBINDED;
        this.connectState = UNCONNECTED;
    }
    
    bind(options = {}) {
        if (this.bindState !== UNBINDED) {
            process.nextTick(() => {
                this.emit('error', new Error('is not in UNBINDED state'))
            });
            return this;
        }
        let flags = 0;
        if (options.ipv6Only) {
            flags |= udp.constant.FLAG.UV_UDP_IPV6ONLY
        }
        if (options.reuseAddr) {
            flags |= udp.constant.FLAG.UV_UDP_REUSEADDR
        }
        if (options.reusePort) {
            flags |= udp.constant.FLAG.UV_UDP_REUSEPORT
        }
        const bind = this.type === TYPE.UDP4 ? 'bind' : 'bind6';
        let ret = this.handle[bind](options.host || '127.0.0.1', options.port, flags);
        if (ret !== 0) {
            process.nextTick(() => {
                const err = new Error('bind socket error'+ret);
                err.code = ret;
                this.emit('error', err);
            })
            return this
        }
        this.handle.onmessage = (nread, data) => {
            if (nread === 0) {
                this.emit('message', new Buffer(''));
            } else if (nread > 0){
                this.emit('message', new Buffer(data));
            } else {
                this.emit('error', new Error('read socket error'));
            }
        }
        ret = this.handle.readStart();
        if (ret !== 0) {
            process.nextTick(() => {
                this.emit('error', new Error('readStart error'));
            });
            return;
        }
        this.bindState = BINDED;
        process.nextTick(() => {
            this.emit('listening');
        });
        return this;
    }
   
    connect({ port, host }) {
        if (this.connectState !== UNCONNECTED) {
            process.nextTick(() => {
                this.emit('error', new Error('not in UNCONNECTED state'))
            });
            return;
        }
        this.connectState = CONNECTING;
        function doConnect() {
            this.off('listening', doConnect);
            this.off('error', error);
            const connect = this.type === TYPE.UDP4 ? 'connect' : 'connect6';
            const ret = this.handle[connect](host || '127.0.0.1', port);
            if (ret !== 0) {
                this.connectState = UNCONNECTED;
                process.nextTick(() => {
                    this.emit('error', new Error('connect error'));
                });
                return;
            }
            this.connectState = CONNECTED;
            process.nextTick(() => {
                this.emit('connect');
            });
        }
        function error() {
            this.off('listening', doConnect);
            this.off('error', error);
            this.connectState = UNCONNECTED;
        }
        // can call bind after call connect, so we should call bind before call connect
        if (this.bindState === BINDED) {
            doConnect();
        } else {
            if (this.bindState === UNBINDED) {
                this.bind({ port: 0 });
            }
            this.on('listening', doConnect);
            this.on('error', error);
        }
        return this;
    }
    disconnect() {
        if (this.connectState === CONNECTED) {
            this.handle.disconnect();
        }
        return this;
    }

    send(data, options, cb) {
        if (typeof options === 'function') {
            cb = options;
            options = {};
        }
        if (this.connectState !== UNCONNECTED && (options.port !== undefined || options.host !== undefined)) {
            process.nextTick(() => {
                this.emit('error', new Error("have already been connected"));
            })
            return this;
        }
        if (this.connectState === UNCONNECTED && (options.port === undefined || options.host === undefined)) {
            process.nextTick(() => {
                this.emit('error', new Error("socket is unconnected"));
            })
            return this;
        }
        const doSend = function() {
            this.off('listening', doSend);
            this.off('error', error);
            const sendReq = new udp.SendReq();
            sendReq.data = data;
            sendReq.oncomplete = (status) => {
                cb(status);
            }
            let ret
            if (options.host !== undefined && options.port != undefined) {
                ret = this.handle.send(sendReq, data, options.host, options.port)
            } else {
                ret = this.handle.send(sendReq, data);
            }
            if (ret !== 0) {
                process.nextTick(() => {
                    this.emit('error', new Error("send error"));
                });
            }
        }
        function error() {
            this.off('listening', doSend);
            this.off('error', error);
        }
        if (this.bindState === BINDED) {
            doSend();
        } else {
            if (this.bindState === UNBINDED) {
                this.bind({ port: 0 });
            }
            this.on('listening', doSend);
            this.on('error', error);
        }
        return this;
    }

    close() {
        this.handle.close();
        this.handle = null;
        return this;
    }
}

module.exports = {
    UDP,
    TYPE,
})"},{"libs/uv/index.js", R"(const {
    uv,
} = No.buildin;

const errMap = uv.getErrMap();

function uvError(errno) {
    const [code, msg] = errMap.get(errno) || ['UNKNOWN', 'unknown error'];
    return { code, msg };
}

module.exports = {
    uvError,
};)"},{"libs/vm/index.js", R"(const {
    vm,
} = No.buildin;

module.exports = {
    run: vm.run,
    compileFunction: vm.compileFunction,
    MODE: vm.MODE,
})"},{"libs/worker/index.js", R"(const {
    worker,
} = No.buildin;

const { events } = No.libs;

class Worker extends events {
    handle = null;
    constructor(filename, options) {
        super();
        this.handle = new worker.Worker(filename, options);
        this.handle.start();
        this.channel = new Channel(this.handle.getMessageEndpoint())
        this.channel.on('message', (msg) => {
            this.emit('message', msg);
        })
    }
    stop() {
        this.handle.stop();
    }
    postMessage(msg) {
        this.channel.postMessage(msg);
    }
    get id() {
        return this.handle.workerId;
    }
}

class Channel extends events {
    constructor(handle) {
        super()
        this.handle = handle;
        this.handle.onmessage = (msg) => {
            this.emit('message', JSON.parse(msg));
        }
    }
    postMessage(msg) {
        this.handle.postMessage(JSON.stringify(msg));
    }
}

module.exports = {
    Worker,
    channel: process.isMainThread ? null : new Channel(worker.getMessageEndpoint()),
    get workerId() {
        return worker.getWorkerId();
    }
})"},{"libs/worker/main.js", R"()"}
};

#endif
