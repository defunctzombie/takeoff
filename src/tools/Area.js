/// the AreaTool creates shapes

var ToolBase = require('./Tool');

var Area = require('../shapes/Area');

function AreaTool(drawing_params) {
    if (!(this instanceof AreaTool)) {
        return new AreaTool(drawing_params);
    }

    var self = this;
    self._drawing_params = drawing_params;

    self._shape = undefined;
    self._done = false;
};

AreaTool.class = 'area';
AreaTool.tip = 'area';

AreaTool.prototype.__proto__ = ToolBase.prototype;

// add a point for tool tracking
// point is in canvas coordinate space
AreaTool.prototype.add_point = function(point) {
    var self = this;

    if (self._done) {
        return;
    }

    if (!self._shape) {
        self._shape = new Area();
        self.emit('shape', self._shape);
    }

    self._shape.add_point(point);

    // done on double click? we can't really do that here
    // emit done
};

AreaTool.prototype.done = function() {
    var self = this;
    self._done = true;
};

module.exports = AreaTool;
