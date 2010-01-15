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