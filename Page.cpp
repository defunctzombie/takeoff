#include "Page.hpp"

#include <poppler-qt4.h>

#include "Shape.hpp"
#include "shapes/Area.hpp"
#include "shapes/Line.hpp"

Page::Page(Poppler::Page* page) :
    ppage(page)
{
    dpi = 1;
    scale = 1.0;
}

void Page::write(QDomElement& parent) const
{
    QDomElement self = parent.ownerDocument().createElement("page");
    parent.appendChild(self);
    
    self.setAttribute("scale", scale);
    
    Q_FOREACH(Shape* s, shapes)
    {
        Shape* sib = s;
        while (sib)
        {
            bool sibling = sib->sibling();
            
            QDomElement shape = parent.ownerDocument().createElement("shape");
            self.appendChild(shape);
            
            shape.setAttribute("type", sib->name());
            shape.setAttribute("color", QString::number(sib->color().rgb(), 16));
            shape.setAttribute("sibling", sibling);
            
            Q_FOREACH(const QPointF& p, sib->points())
            {
                QDomElement point = parent.ownerDocument().createElement("point");
                shape.appendChild(point);
                point.setAttribute("x", p.x());
                point.setAttribute("y", p.y());
            }
            
            sib = sib->sibling();
        }
    }
}
#include <QDebug>
void Page::read(const QDomElement& self)
{
    scale = self.attribute("scale", "1.0").toFloat();
    
    QDomNodeList shapesList = self.elementsByTagName("shape");
    
    Shape* prev = 0;
    for (int i=0 ; i<shapesList.count() ; ++i)
    {
        QDomElement shapeE = shapesList.item(i).toElement();
        
        QString type = shapeE.attribute("type", "Line");
        QString color = shapeE.attribute("color", "ff000000");
        QString sibling = shapeE.attribute("sibling", "false");
        
        Shape* shape = 0;
        if (type == "Area")
            shape = new Area();
        else
            shape = new Line();
        
        if (prev)
        {
            prev->linkShape(shape);
            prev = 0;
        }
        else
        {
            shapes.append(shape);
        }
        
        bool ok = false;;
        shape->setFinished(true);
        shape->color(QColor::fromRgb(color.toUInt(&ok, 16)));
        
        QDomNodeList points = shapeE.elementsByTagName("point");
        
        for (int j=0 ; j<points.count() ; ++j)
        {
            QDomElement point = points.item(j).toElement();
            const float x = point.attribute("x", "0").toFloat();
            const float y = point.attribute("y", "0").toFloat();
            
            shape->addPoint(QPointF(x, y));
        }
        
        if (sibling.toInt())
            prev = shape;
    }
}

QDataStream& operator << (QDataStream& out, const Page& p)
{
    out << p.dpi;
    out << p.scale;
    
    out << p.shapes.size();
    
    Q_FOREACH(Shape* s, p.shapes)
    {
        Shape* sib = s;
        while (sib)
        {
            bool sibling = sib->sibling();
            out << sib->name();
            out << sib->color();
            out << sib->points();
            out << sibling;
            
            sib = sib->sibling();
        }
    }
    
    return out;
}

QDataStream& operator >> (QDataStream& in, Page& p)
{
    in >> p.dpi;
    in >> p.scale;
    
    int shapeCount = 0;
    in >> shapeCount;
    
    Shape* prev = 0;
    for (int i=0 ; i<shapeCount ; ++i)
    {
        char* name;
        QColor color;
        QVector<QPointF> points;
        bool sibling = false;
        
        in >> name;
        in >> color;
        in >> points;
        in >> sibling;
        
        Shape* shape = 0;
        if (QString(name) == "Area")
            shape = new Area();
        else
            shape = new Line();
        
        if (prev)
        {
            prev->linkShape(shape);
            prev = 0;
        }
        else
        {
            p.shapes.append(shape);
        }
        
        shape->setFinished(true);
        shape->color(color);
        shape->points(points);
        
        if (sibling)
        {
            prev = shape;
            i--;
        }
    }
    
    return in;
}