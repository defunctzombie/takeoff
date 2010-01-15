#pragma once

#include <QWidget>
#include <QColor>

class QPaintEvent;

class ColorSwatch : public QWidget
{
    Q_OBJECT
    
    private:
        QColor _color;
    
    public:
        ColorSwatch(QWidget* parent = 0);
        
        QColor color() const { return _color; }
        
    Q_SIGNALS:
        void colorChanged(QColor);
        
    protected:
        void paintEvent(QPaintEvent*);
        void mouseReleaseEvent(QMouseEvent*);
};