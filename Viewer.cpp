#include "Viewer.hpp"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QPalette>
#include <QPainter>
#include <QDebug>
#include <QColorDialog>
#include <QStyleOptionFrame>
#include <QKeyEvent>

#include <poppler-qt4.h>

#include "Page.hpp"

#include "shapes/Area.hpp"
#include "shapes/Line.hpp"
#include "shapes/Count.hpp"

Viewer::Viewer(QWidget* parent) :
    QWidget(parent), _thread(this)
{
    _page = 0;
    _shape = 0;
    _panning = false;
    _tool = SelectTool;
    _removalNodeIndex = 0;
    _removalShape = 0;
    _dragPoint = 0;
	_mouseDown = false;
    
    this->setBackgroundRole(QPalette::Mid);
    this->setCursor(Qt::CrossCursor);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::ClickFocus);
    
    _popupMenu = new QMenu(this);
    _actionRemoveNode = _popupMenu->addAction("remove node");
    _actionRemoveShape = _popupMenu->addAction("remove shape");
    _popupMenu->addSeparator();
    _actionChangeColor = _popupMenu->addAction("change color");
    
    connect(_actionRemoveNode, SIGNAL(triggered(bool)), SLOT(removeNode()));
    connect(_actionRemoveShape, SIGNAL(triggered(bool)), SLOT(removeShape()));
    connect(_actionChangeColor, SIGNAL(triggered(bool)), SLOT(changeColor()));
}

Viewer::~Viewer()
{
    _thread.wait();
}

void Viewer::reset()
{
    _page = 0;
    _shape = 0;
    _panning = false;
    _tool = SelectTool;
    _removalNodeIndex = 0;
    _removalShape = 0;
    _dragPoint = 0;
    _pageImage = QImage();
    _group = false;
    
    update();
}

void Viewer::setPage(Page* page)
{
    _page = page;
    if (_page == 0)
        return;
    
	_pageImage = QImage();
	zoomFit();
}

#include <QDebug>

void Viewer::paintEvent(QPaintEvent* pe)
{
    if (!_page)
    {
        QPainter p(this);
        return;
    }
    
    QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);
    
    QSizeF pageSize = _page->ppage->pageSizeF();
    QSizeF imageSize = pageSize / 72.0 * _page->dpi;
    
    if (_page->ppage->orientation() == Poppler::Page::Portrait)
        imageSize.transpose();
    
	_imageMutex.lock();
    
    const QRect imageRect(_offset, imageSize.toSize());
    const QRect drawRect = rect().intersected(imageRect);
    
    QPoint source = drawRect.topLeft() - imageRect.topLeft();
    
    const float ratio = _pageImage.width()/imageSize.width();
    const QRect sourceRect(source * ratio, drawRect.size() * ratio);
    
    p.drawImage(drawRect, _pageImage, sourceRect);
    
    _imageMutex.unlock();
    
    p.save();
    p.translate(_offset);
    
    Q_FOREACH(const Shape* shape, _page->shapes)
    {
        shape->draw(p, _page->dpi);
    }
    
    p.restore();
    p.setClipping(false);
	
	if (_tool == ZoomTool && _mouseDown)
	{
		QPen pen;
		pen.setWidth(2);
		pen.setColor(Qt::green);
		
		QColor c(Qt::green);
		c.setAlphaF(.3);
		
		p.setPen(pen);
		QRect zoomRect = QRect(_dragStart, _mousePoint);
		p.drawRect(zoomRect);
		
		p.fillRect(zoomRect, c);
	}
}

void Viewer::mouseMoveEvent(QMouseEvent* me)
{
    if (_panning)
    {
        _offset += me->pos() - _dragStart;
        _dragStart = me->pos();
        repaint();
    }
    else if (_dragPoint)
    {
        //update the location
        QPoint d = me->pos() - _offset;
        _dragPoint->setX(d.x()/(float)_page->dpi);
        _dragPoint->setY(d.y()/(float)_page->dpi);
        repaint();
    }
    else if (_shape && !_shape->isFinished())
    {
        QPoint d = me->pos() - _offset;
        QPointF p(d.x()/(float)_page->dpi, d.y()/(float)_page->dpi);
        _shape->setMousePoint(p);
        repaint();
    }
	else if (_tool == ZoomTool && _mouseDown)
	{
		_mousePoint = me->pos();
		repaint();
	}
}

void Viewer::mousePressEvent(QMouseEvent* me)
{
    if (!_page)
        return;
    
    if (me->button() == Qt::MidButton)
    {
        _dragStart = me->pos();
        _panning = true;
    }
    else if (me->button() == Qt::LeftButton)
    {
        if (_tool == SelectTool)
        {
            Q_FOREACH(Shape* shape, _selected)
            {
                shape->setSelected(false);
            }
            
            if (!(me->modifiers() & Qt::ShiftModifier))
            {
                _selected.clear();
            }
            
            QPointF* drag = 0;
            Q_FOREACH(Shape* shape, _page->shapes)
            {
                shape->setSelected(false);
                drag = shape->selected(me->pos()-_offset, _page->dpi);
                if (drag)
                {
					_shape = shape;
                    shape->setSelected(true);
                    _selected.append(shape);
                    _dragPoint = drag;
                    break;
                }
            }
            
            if (!drag)
            {
                _dragStart = me->pos();
                _panning = true;
            }
            
            recalcInfo();
            repaint();
        }
        else if (_tool == AreaTool || _tool == LineTool || _tool == CountTool)
        {
            Shape* newShape = 0;
            if (_shape == 0 || _shape->isFinished())
            {
                if (_tool == AreaTool)
                    newShape = new Area();
                else if (_tool == CountTool)
                    newShape = new Count();
                else
                    newShape = new Line();
                
                newShape->color(_color);
                newShape->setSelected(true);
            }
            
            if (newShape)
            {
                if (_shape && _group)
                {
                    _shape->linkShape(newShape);
                }
                else
                {
                    Q_FOREACH(Shape* shape, _selected)
                    {
                        shape->setSelected(false);
                    }
                    
                    _selected.clear();
                    _selected.append(newShape);
                    _page->shapes.append(newShape);
                }
                
                _shape = newShape;
            }
            
            QPoint d = me->pos() - _offset;
            QPointF p(d.x()/(float)_page->dpi, d.y()/(float)_page->dpi);
            _shape->addPoint(p);
            
            recalcInfo();
            repaint();
        }
		else if (_tool == ZoomTool)
		{
			_mouseDown = true;
			_dragStart = me->pos();
		}
    }
}

void Viewer::mouseReleaseEvent(QMouseEvent* me)
{
    _dragPoint = 0;
    
    if (!_page)
        return;
    
    _panning = false;
     
    if (me->button() == Qt::RightButton && _tool == SelectTool)
    {
        Q_FOREACH(Shape* shape, _page->shapes)
        {
            QPointF* drag = shape->selected(me->pos()-_offset, _page->dpi);
            if (drag)
            {
                //_removalNodeIndex = i;
                _removalShape = shape;
                _selected.clear();
                _selected.append(_removalShape);
                _popupMenu->popup(this->mapToGlobal(me->pos()), _actionRemoveNode);
                repaint();
                recalcInfo();
                return;
            }
        }
    }
	else if (_tool == ZoomTool && _mouseDown)
	{
		QPoint delta = _mousePoint - _dragStart;
		int dx = qAbs(delta.x());
		int dy = qAbs(delta.y());
		_page->dpi *= _pageImage.width()/dx;
		
		QPoint loc = _dragStart - _offset;
		_offset -= loc * _pageImage.width()/dx;
		
        _mouseDown = false;
		repaint();
		regenImage();
	}
}

void Viewer::mouseDoubleClickEvent(QMouseEvent* me)
{
    _shape->setFinished(true);
    repaint();
}

void Viewer::keyReleaseEvent(QKeyEvent* ke)
{
    if (Qt::Key_Escape == ke->key())
    {
        _shape->setFinished(true);
        repaint();
    }
}

void Viewer::wheelEvent(QWheelEvent* we)
{
}

void Viewer::zoomFit()
{
    if (!_page || !_page->ppage)
        return;
    
    //setup the initial page size
    QSizeF pageSize = _page->ppage->pageSizeF(); //in points (1/72th of an inch)
    int dpix = width()/(pageSize.width()/72);
    int dpiy = height()/(pageSize.height()/72);
    
    if (_page->ppage->orientation() == Poppler::Page::Portrait)
    {
        dpix = width()/(pageSize.height()/72);
        dpiy = height()/(pageSize.width()/72);
    }
    
    _page->dpi = qMin(dpix, dpiy);
    
    QSizeF imageSize = pageSize / 72.0 * _page->dpi;
    
    if (_page->ppage->orientation() == Poppler::Page::Portrait)
        imageSize.transpose();
    
    _offset = QPoint((width() - imageSize.width())/2, 
                     (height() - imageSize.height())/2);
	
    repaint();
    regenImage();
}

void Viewer::zoomIn()
{
    if (!_page)
        return;
    
	const int oldDpi = _page->dpi;
    _page->dpi += int(_page->dpi * .5);
	
    if (_offset.x() < 0 || _offset.y() < 0)
        _offset *= _page->dpi/(float)oldDpi;
	
    repaint();
    regenImage();
}

void Viewer::zoomOut()
{
    if (!_page)
        return;
    
	const int oldDpi = _page->dpi;
    _page->dpi -= int(_page->dpi * .5);
	
    if (_offset.x() < 0 || _offset.y() < 0)
        _offset *= _page->dpi/(float)oldDpi;
	
    repaint();
    regenImage();
}

void Viewer::setScale(float scale)
{
    if (!_page)
        return;
    
    _page->scale = scale;
    
    recalcInfo();
    update();
}

void Viewer::regenImage()
{
	_thread.start();
}

void Viewer::recalcInfo()
{
    const float scale = _page->scale;
    
    float area = 0;
    float length = 0;
    Q_FOREACH(const Shape* shape, _selected)
    {
        area += scale * scale * shape->area();
        length += scale * shape->length();
    }
    infoChanged(length, area);
}

void Viewer::changeTool(Viewer::Tool t)
{
    if (_shape)
    {
        _shape->setFinished(true);
        repaint();
    }
    
    //_shape = 0;
    _tool = t;
}

void Viewer::removeNode()
{
    //TODO
    #if 0
    if (_removalShape)
    {
        _removalShape->remove(_removalNodeIndex);
        _removalShape = 0;
        repaint();
    }
    #endif
}

void Viewer::removeShape()
{
    if (_removalShape && _page)
    {
        _selected.clear();
        
        if (_shape == _removalShape)
            _shape = 0;
        
        const int index = _page->shapes.indexOf(_removalShape);
        
        if (index >= 0)
            _page->shapes.remove(index);
        
        delete _removalShape;
        _removalShape = 0;        
        repaint();
    }
}

void Viewer::group(bool b)
{
    _group = b;
}

void Viewer::changeColor(QColor c)
{
    _color = c;
	
	if (_shape && !_shape->isFinished())
	{
		_shape->color(_color);
		repaint();
	}
}

void Viewer::changeColor()
{
    if (_removalShape)
    {
        QColor newColor = QColorDialog::getColor(_removalShape->color());
        
        if (newColor.isValid())
        {
            _removalShape->color(newColor);
        }
        
        _removalShape = 0;
        repaint();
    }
}

void Viewer::ImageThread::run()
{
    Poppler::Page::Rotation rotate = Poppler::Page::Rotate0;
    if (_viewer->_page->ppage->orientation() == Poppler::Page::Portrait)
        rotate = Poppler::Page::Rotate90;
	
	const int dpi = _viewer->_page->dpi;
	
	if (dpi > 250)
		return;
	
    QImage newImage = _viewer->_page->ppage->renderToImage(dpi, dpi,
                                                           -1, -1, -1, -1, rotate);
    if (newImage.isNull())
        return;
	
    _viewer->_imageMutex.lock();
    
    //shallow copy
    _viewer->_pageImage = QImage(newImage);
        
    _viewer->_imageMutex.unlock();
    
    _viewer->update(); //update not repaint to use queue
}
