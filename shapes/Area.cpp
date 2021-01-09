#include "Area.hpp"

#include <QPainterPath>

void Area::draw(QPainter &p, qreal scale) const
{
    if (_points.size() == 0)
        return;

    QColor fill = color();
    fill.setAlphaF(.2);

    QPen shapePen;
    shapePen.setColor(fill);
    shapePen.setWidth(1);
    p.setPen(shapePen);

    QPolygonF poly;

    for (int i = 0; i < _points.size(); ++i)
    {
        poly.append(_points[i] * scale);
    }

    if (!_finished && !_mousePoint.isNull())
        poly.append(_mousePoint * scale);

    poly.append(_points[0] * scale);

    QPainterPath pp;
    pp.addPolygon(poly);

    p.fillPath(pp, fill);
    p.drawPath(pp);

    if (_selected)
    {
        shapePen.setColor(Qt::green);
        shapePen.setWidth(6);
    }
    else
    {
        shapePen.setWidth(4);
    }

    p.setPen(shapePen);
    p.drawPoints(poly.data(), poly.size());

    if (_sibling)
        _sibling->draw(p, scale);
}

float Area::length() const
{
    const int s = _points.size();
    float length = 0;

    //add the length from first to last for area
    //closes the area
    const QPointF p1 = _points.first();
    const QPointF p2 = _points.last();

    length += QLineF(p1, p2).length();

    for (int i = 1; i < s; ++i)
    {
        const QPointF p1 = _points[i - 1];
        const QPointF p2 = _points[i];

        length += QLineF(p1, p2).length();
    }

    if (_sibling)
        length += _sibling->length();

    return length;
}

float Area::area() const
{
    ///http://local.wasp.uwa.edu.au/~pbourke/geometry/polyarea/

    float area = 0;

    if (_points.size() < 3)
        return 0;

    const int s = _points.size();
    for (int i = 0; i < s; ++i)
    {
        const QPointF &p1 = _points[i];
        const QPointF &p2 = _points[(i + 1) % s];

        area += (p1.x() * p2.y() - p2.x() * p1.y());
    }

    area = qAbs(area) * .5;
    if (_sibling)
        area += _sibling->area();

    return area;
}
