function ZoomInTool(drawing_params) {
    if (!(this instanceof ZoomInTool)) {
        return new ZoomInTool(drawing_params);
    }

    var self = this;
    self._drawing_params = drawing_params;
};

ZoomInTool.class = 'zoom-in';
ZoomInTool.tip = 'zoom in';

// add a point for tool tracking
// point is in canvas coordinate space
ZoomInTool.prototype.add_point = function(point) {
    var self = this;

    // zoom out is a one shot tool
    self._drawing_params.zoom_factor *= 1.25;
    var zoom_factor = self._drawing_params.zoom_factor;
    var off = self._drawing_params.off;

    var x = point.x;
    var y = point.y;

    //console.log(x - x * 1.25);
    off.x += x - x * zoom_factor;
    off.y += y - y * zoom_factor;

    console.log(off);

    // emit done
};

module.exports = ZoomInTool;
