#include "Check.hpp"

void Check::draw(QPainter& p, qreal scale) const
{
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(color());
    pen.setWidth(2);
    p.setPen(pen);

    p.setFont(QFont("sans", 7, QFont::Bold));

    const int r = 5;

    for (int i=0; i <_points.size(); ++i)
    {
        QPointF point = _points[i] * scale;

        p.drawLine(point.x()-r, point.y(), point.x(), point.y()+r);
        p.drawLine(point.x(), point.y()+r, point.x()+2*r, point.y()-2*r);
    }

    if (!_finished && !_mousePoint.isNull())
    {
        QPointF point = _mousePoint * scale;
        
        p.drawLine(point.x()-r, point.y(), point.x(), point.y()+r);
        p.drawLine(point.x(), point.y()+r, point.x()+2*r, point.y()-2*r);
    }

    if (_sibling)
        _sibling->draw(p, scale);
}

float Check::length() const
{
    return 0;
}

float Check::area() const
{
    return 0;
}
