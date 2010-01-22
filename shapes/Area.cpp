#include "Area.hpp"

void Area::draw(QPainter& p, qreal scale) const
{
    QPolygonF poly;
    
    QColor fill = color();
    fill.setAlphaF(.2);
    
	QPen shapePen;
	shapePen.setColor(fill);
	shapePen.setWidth(1);
	p.setPen(shapePen);
	
	const int size = _points.size();
    for (int i=0; i < size; ++i)
    {   
        QPointF p1 = _points[i] * scale;
        QPointF p2 = _points[(i+1) % size] * scale;
        
        poly.append(p1);
        
        if (i == (size-1) && !_finished)
            shapePen.setStyle(Qt::DashLine);
        else
            shapePen.setStyle(Qt::SolidLine);
        
		p.setPen(shapePen);
        p.drawLine(p1, p2);
    }
	
	shapePen.setWidth(4);
	p.setPen(shapePen);
	
	p.drawPoints(poly.data(), poly.size());
	
	if (_selected)
	{
		shapePen.setColor(Qt::green);
		shapePen.setWidth(6);
		p.setPen(shapePen);
		p.drawPoints(poly.data(), poly.size());
	}
	
	QPainterPath pp;
	pp.addPolygon(poly);
    p.fillPath(pp, fill);
    
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
        area += _sibling->area();
    
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