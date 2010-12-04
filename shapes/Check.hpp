#pragma once

#include "../Shape.hpp"

class Check : public Shape
{
    public:
        virtual float length() const;
        virtual float area() const;
        virtual void draw(QPainter& painter, qreal scale) const;
		virtual const char* name() const { return "Check"; }
};
