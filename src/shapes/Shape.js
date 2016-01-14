function Shape() {
    if (!(this instanceof Shape)) {
        return new Shape();
    }

    var self = this;
    self._points = [];
};

module.exports = Shape;
