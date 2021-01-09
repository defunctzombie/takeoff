#pragma once

#include <QVector>
#include <QImage>
#include <QDomElement>

#include <poppler/qt5/poppler-qt5.h>

class Shape;

class Page
{
public:
    Page(Poppler::Page *ppage = 0);

    Poppler::Page *ppage;

    QVector<Shape *> shapes;

    float scale;
    int dpi;

    Poppler::Page::Rotation rotation;

    void rotateCW();
    void rotateCCW();

    /** write self into the parent element */
    void write(QDomElement &parent) const;

    /** populate self from this element */
    void read(const QDomElement &element);

    friend QDataStream &operator<<(QDataStream &out, const Page &p);
    friend QDataStream &operator>>(QDataStream &in, Page &p);

private:
};
