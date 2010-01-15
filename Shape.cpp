#include "Shape.hpp"

#include <QLineF>

Shape::Shape() :
    _sibling(0), _finished(false)
{
    _color = Qt::red;
}

void Shape::linkShape(Shape* s)
{
    if (_sibling)
        delete _sibling;
    
    _sibling = s;
    color(_color);
}

void Shape::color(QColor& color)
{
    _color = color;
    if (_sibling)
        _sibling->color(color);
}

void Shape::addPoint(QPointF p)
{
    _points.append(p);
}

void Shape::setSelected(bool s)
{
    _selected = s;
    
    if (_sibling)
        _sibling->setSelected(_selected);
}

QPointF* Shape::selected(QPoint p, int dpi)
{
    const int size = _points.size();
    for (int i=0; i < _points.size(); ++i)
    {
        QPointF& p1 = _points[i];
        QPointF screen = p1 * dpi;
        QPointF delta = screen - p;
    
        const float dist = delta.x() * delta.x() + delta.y() * delta.y();
    
        //see if we are near-enough
        if (dist < 30)
            return &p1;
    }
    
    if (_sibling)
        return _sibling->selected(p, dpi);
    
    return 0;
}
