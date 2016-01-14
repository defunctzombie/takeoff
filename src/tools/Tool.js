var EventEmitter = require('events').EventEmitter;

function Tool() {
    if (!(this instanceof Tool)) {
        return new Tool();
    }
};

Tool.prototype.__proto__ = EventEmitter.prototype;

module.exports = Tool;
