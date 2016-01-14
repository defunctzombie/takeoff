function ZoomOutTool(drawing_params) {
    if (!(this instanceof ZoomOutTool)) {
        return new ZoomOutTool(drawing_params);
    }

    var self = this;
    self._drawing_params = drawing_params;
};

ZoomOutTool.class = 'zoom-out';
ZoomOutTool.tip = 'zoom out';

// add a point for tool tracking
// point is in canvas coordinate space
ZoomOutTool.prototype.add_point = function(point) {
    var self = this;

    // zoom out is a one shot tool
    self._drawing_params.zoom_factor *= 0.80;
    var zoom_factor = self._drawing_params.zoom_factor;
    var off = self._drawing_params.off;

    var x = point.x;
    var y = point.y;

    //console.log(x - x * 1.25);
    off.x += x - x * zoom_factor;
    off.y += y - y * zoom_factor;

    // emit done
};

module.exports = ZoomOutTool;
