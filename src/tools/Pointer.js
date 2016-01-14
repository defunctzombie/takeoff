var Tool = require('./Tool');

function Pointer(drawing_params) {
    if (!(this instanceof Pointer)) {
        return new Pointer(drawing_params);
    }

    var self = this;
    self._drawing_params = drawing_params;
};

Pointer.prototype.__proto__ = Tool.prototype;

Pointer.class = 'pointer';
Pointer.tip = 'select';

Pointer.prototype.add_point = function(point) {
    var self = this;
};

module.exports = Pointer;
