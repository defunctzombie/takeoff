#include "Line.hpp"

void Line::draw(QPainter& p, qreal scale) const
{
    QPen shapePen;
    QColor line = color();
    QColor dot = color();
    
    const int size = _points.size();
    for (int i=0; i <_points.size(); ++i)
    {
        QPointF p1 = _points[i] * scale;
        
        if (i != _points.size()-1)
        {
            QPointF p2 = _points[(i+1) % size] * scale;
            
            shapePen.setStyle(Qt::SolidLine);
            
            shapePen.setColor(line);
            shapePen.setWidth(2);
            p.setPen(shapePen);
            p.drawLine(p1, p2);
        }
        
        int size = 2;
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
    
    if (_sibling)
        _sibling->draw(p, scale);
}

float Line::length() const
{
    const int s = _points.size();
    float length = 0;
    
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

float Line::area() const
{
    return 0;
}
