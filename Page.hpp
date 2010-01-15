#pragma once

#include <QVector>
#include <QImage>

namespace Poppler
{
    class Page;
}

class Shape;

class Page
{
    public:
        Page(Poppler::Page* ppage = 0);
        
        Poppler::Page* ppage;
        
        QVector<Shape*> shapes;
        
        float scale;
        int dpi;
        
        friend QDataStream& operator << (QDataStream& out, const Page& p);
        friend QDataStream& operator >> (QDataStream& in, Page& p);
                
    private:
};
