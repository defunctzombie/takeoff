#pragma once

#include <QPainter>
#include <QVector>
#include <QColor>

//either a line shape or an area shape
class Shape
{
private:
    QColor _color;

protected:
    Shape *_sibling;
    bool _finished;
    bool _selected;

    QVector<QPointF> _points;

    QPointF _mousePoint;

public:
    Shape();
    virtual ~Shape(){};

    //all shapes can report a length (sum of lines)
    //(the circumference for area shapes)
    //unscaled
    virtual float length() const = 0;

    /** line shapes return an area of 0, unscaled */
    virtual float area() const = 0;

    virtual void draw(QPainter &painter, qreal scale) const = 0;

    void setMousePoint(const QPointF &p) { _mousePoint = p; }

    void addPoint(QPointF p);

    void linkShape(Shape *s);
    Shape *sibling() const { return _sibling; }

    QPointF *selected(QPoint p, int dpi);

    void setSelected(bool s);

    void setFinished(bool f) { _finished = f; }
    bool isFinished() { return _finished; }

    void color(const QColor &color);
    QColor color() const { return _color; }

    virtual const char *name() const = 0;

    QVector<QPointF> points() const { return _points; }
    void points(QVector<QPointF> points) { _points = points; }
};
