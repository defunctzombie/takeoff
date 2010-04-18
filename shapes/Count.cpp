#include "Count.hpp"

void Count::draw(QPainter& p, qreal scale) const
{   
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(color());
    pen.setWidth(2);
    p.setPen(pen);
    
    p.setFont(QFont("sans", 7, QFont::Bold));
    
    const int diameter = 20;
    const int r = diameter/2;
    
    for (int i=0; i <_points.size(); ++i)
    {
        QPointF point = _points[i] * scale;
        
        QRect textRect(point.x()-r, point.y()-r, diameter, diameter);
        p.drawText(textRect, Qt::AlignVCenter | Qt::AlignHCenter, 
                   QString::number(i+1));
        p.drawEllipse(point, r, r);
    }
    
    if (!_finished && !_mousePoint.isNull())
    {
        QPointF point = _mousePoint * scale;
        
        QRect textRect(point.x()-r, point.y()-r, diameter, diameter);
        p.drawText(textRect, Qt::AlignVCenter | Qt::AlignHCenter, 
                   QString::number(_points.size()+1));
        p.drawEllipse(point, r, r);
    }
    
    #if 0
    QPen shapePen;
    shapePen.setStyle(Qt::SolidLine);
    shapePen.setColor(color());
    shapePen.setWidth(2);
    p.setPen(shapePen);
    
    p.drawPolyline(points.data(), points.size());
    
    shapePen.setWidth(4);
    p.setPen(shapePen);
    
    p.drawPoints(points.data(), points.size());
    
    if (_selected)
    {
        QPen selectedPen;
        selectedPen.setStyle(Qt::SolidLine);
        selectedPen.setColor(Qt::green);
        selectedPen.setWidth(6);
        p.setPen(selectedPen);
        
        p.drawPoints(points.data(), points.size());
    }
    #endif
        
    if (_sibling)
        _sibling->draw(p, scale);
}

float Count::length() const
{
    if (_sibling)
        return _sibling->length();
    
    return 0;
}

float Count::area() const
{
    if (_sibling)
        return _sibling->area();
    
    return 0;
}
