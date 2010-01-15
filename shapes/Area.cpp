#include "Area.hpp"

void Area::draw(QPainter& p, qreal scale) const
{
    QPen shapePen;
    QColor line = color();
    QColor dot = color();
    
    QPolygonF poly;
    
    const int size = _points.size();
    for (int i=0; i <_points.size(); ++i)
    {   
        QPointF p1 = _points[i] * scale;
        QPointF p2 = _points[(i+1) % size] * scale;
        
        poly.append(p1);
        
        if (i == (size-1) && !_finished)
            shapePen.setStyle(Qt::DashLine);
        else
            shapePen.setStyle(Qt::SolidLine);
        
        shapePen.setColor(line);
        shapePen.setWidth(2);
        p.setPen(shapePen);
        p.drawLine(p1, p2);
        
        int size = 2;
        p.setPen(Qt::NoPen);
        p.setBrush(dot);
        if (_selected)
        {
            size = 4;
            shapePen.setWidth(2);
            shapePen.setColor(Qt::yellow);
            p.setPen(shapePen);
        }
        
        p.drawEllipse(p1, size, size);
    }
    
    QColor fill = color();
    fill.setAlphaF(.2);
    
    p.setPen(Qt::NoPen);
    p.setBrush(fill);
    p.drawPolygon(poly);
    
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
    
    for (int i=1; i <s; ++i)
    {
        const QPointF p1 = _points[i-1];
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
    if (_sibling)
        area += _sibling->length();
    
    if (_points.size() < 3)
        return 0;
    
    const int s = _points.size();
    for (int i=0; i <s; ++i)
    {
        const QPointF& p1 = _points[i];
        const QPointF& p2 = _points[(i+1) % s];
        
        area += (p1.x()*p2.y() - p2.x()*p1.y());
    }
    
    area = qAbs(area * .5);
    
    return area;
}