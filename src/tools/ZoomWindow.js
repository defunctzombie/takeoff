function ZoomWindowTool(drawing_params) {
    if (!(this instanceof ZoomWindowTool)) {
        return new ZoomWindowTool(drawing_params);
    }

    var self = this;
    self._drawing_params = drawing_params;
    self._p1 = undefined;
    self._p2 = undefined;
};

ZoomWindowTool.class = 'zoom-window';
ZoomWindowTool.tip = 'zoom window';

// add a point for tool tracking
// point is in canvas coordinate space
ZoomWindowTool.prototype.add_point = function(point) {
    var self = this;

    if (!self._p1) {
        self._p1 = point;
        return;
    }

    self._p2 = point;
    // emit done
};

// ask tool to render itself
ZoomWindowTool.prototype.render = function(ctx) {
    var self = this;
    if (!self._p1 || !self._p2) {
        return;
    }

    var p1 = self._p1;
    var p2 = self._p2;

    var x1 = Math.min(p1.x, p2.x);
    var x2 = Math.max(p1.x, p2.x);

    var y1 = Math.min(p1.y, p2.y);
    var y2 = Math.max(p1.y, p2.y);


    ctx.strokeStyle = 'rgba(0,153,255,0.9)';
    ctx.fillStyle = 'rgba(0,153,255,0.4)';
    ctx.strokeWidth = 5;

    // x,y,w,h
    ctx.rect(x1, y1, x2 - x1, y2 - y1);
    ctx.stroke();
};

module.exports = ZoomWindowTool;
