#include "ColorSwatch.hpp"

#include <QPainter>
#include <QColorDialog>

ColorSwatch::ColorSwatch(QWidget* parent) :
    QWidget(parent), _color(Qt::red)
{
    setFixedWidth(25);
}

void ColorSwatch::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    
    p.setRenderHint(QPainter::Antialiasing);
    
    p.setPen(Qt::NoPen);
    p.setBrush(_color);
    p.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 4, 4);
}

void ColorSwatch::mouseReleaseEvent(QMouseEvent*)
{
    QColor newColor = QColorDialog::getColor(_color);
        
    if (newColor.isValid())
    {
        _color = newColor;
        colorChanged(_color);
        repaint();
    }
}