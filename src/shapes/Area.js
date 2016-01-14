var Shape = require('./Shape');

function Area() {
    if (!(this instanceof Area)) {
        return new Area();
    }

    Shape.call(this);
    var self = this;
}

Area.prototype.__proto__ = Shape.prototype;

// maybe?
Area.prototype.set_color = function(color) {
};

Area.prototype.link = function(other) {
};

Area.prototype.add_point = function(point) {
    var self = this;
    self._points.push(point);
};

// render the area
Area.prototype.render = function(ctx, scaling_factor) {
    var self = this;

    scaling_factor = scaling_factor || 0;
    var points = self._points;

    if (points.length < 2) {
        return;
    }

    ctx.strokeStyle = 'rgba(0,153,255,0.9)';
    ctx.fillStyle = 'rgba(0,153,255,0.4)';
    ctx.strokeWidth = 5;

    ctx.beginPath();

    var point = points[0];
    ctx.moveTo(point.x * scaling_factor, point.y * scaling_factor);

    for (var i=1 ; i<points.length ; ++i) {
        var point = points[i];
        ctx.lineTo(point.x * scaling_factor, point.y * scaling_factor);
    }

    ctx.stroke();
    ctx.fill();

    ctx.fillStyle = 'rgba(0, 153, 255, 0.4)';

    for (var i=0 ; i<points.length ; ++i) {
        var point = points[i];
        ctx.beginPath();
        ctx.arc(point.x * scaling_factor, point.y * scaling_factor, 3, 0, 2 * Math.PI);
        ctx.fill();
    }
};

module.exports = Area;
