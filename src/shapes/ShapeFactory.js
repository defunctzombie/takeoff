var Area = require('./Area');
var Line = require('./Line');
var Count = require('./Count');

module.exports.create = function(type, params) {

    var Ctor = undefined;
    switch (type) {
    case 'Area':
        Ctor = Area;
        break;
    case 'Line':
        Ctor = Line;
        break;
    case 'Count':
    case 'Check':
        Ctor = Count;
        break;
    }

    if (!Ctor) {
        throw new Error('unknown shape: ' + type);
    }

    var shape = Ctor();

    // TODO shape.color(params.color);

    params.points.forEach(function(point) {
        shape.add_point(point);
    });

    return shape;
};
