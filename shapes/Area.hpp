#pragma once 

#include <QPointF>

#include "../Shape.hpp"

/** area shape */
class Area : public Shape
{
    public:
        virtual float length() const;
        virtual float area() const;
        virtual void draw(QPainter& painter, qreal scale) const;
        virtual const char* name() const { return "Area"; }
};