var Shape = require('./Shape');

function Line() {
    if (!(this instanceof Line)) {
        return new Line();
    }

    Shape.call(this);
    var self = this;
}

Line.prototype.__proto__ = Shape.prototype;

// maybe?
Line.prototype.set_color = function(color) {
};

Line.prototype.link = function(other) {
};

Line.prototype.add_point = function(point) {
    var self = this;
    self._points.push(point);
};

// render the area
Line.prototype.render = function(ctx, scaling_factor) {
    var self = this;

    scaling_factor = scaling_factor || 0;
    var points = self._points;

    if (points.length < 2) {
        return;
    }

    ctx.strokeStyle = 'rgba(0,153,255,0.9)';
    ctx.strokeWidth = 5;

    ctx.beginPath();

    var point = points[0];
    ctx.moveTo(point.x * scaling_factor, point.y * scaling_factor);

    for (var i=1 ; i<points.length ; ++i) {
        var point = points[i];
        ctx.lineTo(point.x * scaling_factor, point.y * scaling_factor);
    }

    ctx.stroke();

    ctx.fillStyle = 'rgba(0, 153, 255, 0.4)';

    for (var i=0 ; i<points.length ; ++i) {
        var point = points[i];
        ctx.beginPath();
        ctx.arc(point.x * scaling_factor, point.y * scaling_factor, 3, 0, 2 * Math.PI);
        ctx.fill();
    }
};

module.exports = Line;
