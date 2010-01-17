#include "Line.hpp"

void Line::draw(QPainter& p, qreal scale) const
{	
	QVector<QPointF> points = _points;
    for (int i=0; i <_points.size(); ++i)
    {
        points[i] *= scale;
    }
	
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
	if (_sibling)
        return _sibling->area();
	
    return 0;
}
