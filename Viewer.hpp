#pragma once

#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QVector>
#include <QMenu>
#include <QColor>

#include "Shape.hpp"

class Page;

class Viewer : public QWidget
{
    Q_OBJECT
    
    class ImageThread : public QThread
    {
        Viewer* _viewer;
        
        public:
            ImageThread(Viewer* v) : _viewer(v) {}
            void run();
    };
    
    public:
        typedef enum
        {
            SelectTool,
			ZoomTool,
            LineTool,
            AreaTool
        } Tool;
    
    private:
        /** the active page */
        Page* _page;
        
        QImage _pageImage;
        QMutex _imageMutex;
        
        QPoint _dragStart;
        QPoint _offset;
        
        Viewer::ImageThread _thread;
        
        //current shape
        Shape* _shape;
        
        //selected Shapes
        QVector<Shape*> _selected;
        
        //tool currently active
        Tool _tool;
        
        //true if we are panning around the drawing
        bool _panning;
	    bool _mouseDown;
        
        QMenu* _popupMenu;
        //remove a point
        QAction* _actionRemoveNode;
        QAction* _actionRemoveShape;
        QAction* _actionChangeColor;
        
        //if set will be removed by the remove action
        unsigned _removalNodeIndex;
        Shape* _removalShape;
        
        //point we can drag
        QPointF* _dragPoint;
		QPoint _mousePoint;
        
        QColor _color;
        
        bool _group;
    
    public:
        Viewer(QWidget* parent = 0);
        ~Viewer();
        
        //set the page to render
        void setPage(Page* page);
        
        void setScale(float scale);
        void changeTool(Viewer::Tool t);
        
        //reset for new document
        void reset();
        
    Q_SIGNALS:
        void infoChanged(float length, float area);
        
    public Q_SLOTS:
        void zoomIn();
        void zoomOut();
        void zoomFit();
        
        //removes the chosen node if there is one
        void removeNode();
        void removeShape();
        
        void changeColor(QColor);
        void changeColor();
        
        /** group new shapes with the previous */
        void group(bool);
        
    protected:
        void paintEvent(QPaintEvent* pe);
        void mouseDoubleClickEvent(QMouseEvent* me);
        void mouseMoveEvent(QMouseEvent* me);
        void mousePressEvent(QMouseEvent* me);
        void mouseReleaseEvent(QMouseEvent* me);
        void wheelEvent(QWheelEvent* we);
	
	private:
		void regenImage();
        
};
