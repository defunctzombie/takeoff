#include "MainWindow.hpp"

#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QAction>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QStatusBar>
#include <QFileDialog>
#include <QCheckBox>
#include <QComboBox>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), _activePage(0)
{
    _ui.setupUi(this);
    setupUi();
}

MainWindow::~MainWindow()
{
    for (int i=0 ; i<_documents.size() ; ++i)
        delete _documents[i];
}

void MainWindow::open(const QString& filename)
{
    if (filename.isNull())
        return;
    
    Poppler::Document* document = Poppler::Document::load(filename);
    if (!document)
    {
        QMessageBox::critical(this, "Document Error", "Unable to open the document: " + filename);
        return;
    }
    else if (document->isLocked())
    {
        QMessageBox::information(this, "Document Error", "The document is password protected.");
        return;
    }
    
    _files.append(filename);
    
    _documents.append(document);
    _ui.viewer->reset();
    
    //bool showPageList = false;
    //if (_document->numPages() > 1)
    //    showPageList = true;
    
    //_ui.pageList->clear();
    //_ui.leftSide->setVisible(showPageList);
    
    QFileInfo fInfo(filename);
    QString name(fInfo.baseName());
    //populate the list box
    for (int i=0 ; i<document->numPages() ; ++i)
    {
        _pages.append(Page(document->page(i)));
        
        _activePage = &_pages.last();
        
        if (document->numPages() > 1)
            name += QString("-%1").arg(i+1);
        _drawingsCombo->addItem(name);
    }
    
    _drawingsCombo->setCurrentIndex(0);
    openPage(0);
}

void MainWindow::openPage(int index)
{
    if (index < 0)
        return;
    
    _activePage = &_pages[index];
    
    //send the page to the viewer
    _ui.viewer->setPage(_activePage);
    _scale->setText(QString::number(_activePage->scale));
}

void MainWindow::scaleChanged(const QString& text)
{
    const float scale = text.toFloat();
    if (_activePage)
        _activePage->scale = scale;
    
    _ui.viewer->update();
}

void MainWindow::toolAction(QAction* action)
{
    if (action->isChecked())
    {
        if (action == _ui.actionSelect)
        {
            _ui.viewer->changeTool(Viewer::SelectTool);
        }
        else if (action == _ui.actionLine)
        {
            _ui.viewer->changeTool(Viewer::LineTool);
        }
        else if (action == _ui.actionArea)
        {
            _ui.viewer->changeTool(Viewer::AreaTool);
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "File Open", QString(), "PDF Files (*.pdf)");
    
    if (!filename.isNull())
        open(filename);
}

void MainWindow::on_actionAdd_Drawing_triggered()
{
    on_actionOpen_triggered();
}

void MainWindow::on_actionAdd_Drawings_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory");
    //QString dir = "/home/shtylman/projects/takeoff/testing/test";
    
    if (dir.isNull())
        return;
    
    QDir drawingDir(dir);
    
    QStringList filters;
    filters << "*.pdf" << "*.PDF";
    drawingDir.setNameFilters(filters);
    
    QStringList files = drawingDir.entryList();
    
    for (int i=0 ; i<files.size() ; ++i)
        open(drawingDir.absoluteFilePath(files.at(i)));
}

#include <QDebug>
void MainWindow::on_actionOpenProject_triggered()
{
    //TODO reset properly
    _pages.clear();
    _files.clear();
    
    QString filename = QFileDialog::getOpenFileName(this, "Open File", "", "*.tdf");
    
    if (filename.isNull())
        return;
        
    QFile inFile(filename);
    inFile.open(QIODevice::ReadOnly);
    QDataStream in(&inFile);
    
    in >> _files;
    in >> _pages;
    
    inFile.close();
    
    _ui.viewer->reset();
    
    int pageIndex = 0;
    //go through files and load them
    Q_FOREACH(const QString& filename, _files)
    {
        Poppler::Document* document = Poppler::Document::load(filename);
        if (!document)
        {
            QMessageBox::critical(this, "Document Error", "Unable to open the document: " + filename);
            return;
        }
        else if (document->isLocked())
        {
            QMessageBox::information(this, "Document Error", "The document is password protected.");
            return;
        }
        
        _documents.append(document);
        
        QFileInfo fInfo(filename);
        QString name(fInfo.baseName());
        
        //populate the list box
        for (int i=0 ; i<document->numPages() ; ++i)
        {
            _pages[pageIndex].ppage = document->page(i);
            
            _activePage = &_pages[pageIndex];
            
            if (document->numPages() > 1)
                name += QString("-%1").arg(i+1);
            
            _drawingsCombo->addItem(name);
            
            pageIndex++;
        }
    }
        
    _drawingsCombo->setCurrentIndex(0);
    openPage(0);
    _ui.viewer->update();
}

void MainWindow::on_actionSaveProject_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save File", "", "*.tdf");
    
    if (filename.isNull())
        return;
    
    QFile outFile(filename);
    outFile.open(QIODevice::WriteOnly);
    QDataStream out(&outFile);
    
    out << _files;
    out << _pages;
    
    outFile.close();
}

void MainWindow::infoChanged(float length, float area)
{
    QString text = QString("<strong>Lf: </strong>%1'").arg(length, 0, 'f', 3);
    
    if (area != 0)
        text += QString("  <strong>Sf: </strong>%1").arg(area, 0, 'f', 3);
    
    _infoLabel->setText(text);
}

void MainWindow::setupUi()
{
    /// setup toolbars ///
    QToolBar* _mainToolBar = new QToolBar("Main", this);
    
    _mainToolBar->addAction(_ui.actionOpen);
    
    _mainToolBar->addSeparator();
    
    /// zoom
    _mainToolBar->addAction(_ui.actionZoom_In);
    _mainToolBar->addAction(_ui.actionZoom_Out);
    _mainToolBar->addAction(_ui.actionZoom_Fit);
    
    connect(_ui.actionZoom_Fit, SIGNAL(triggered()), _ui.viewer, SLOT(zoomFit()));
    
    _mainToolBar->addSeparator();
    
    /// tools
    _mainToolBar->addAction(_ui.actionSelect);
    _mainToolBar->addAction(_ui.actionArea);
    _mainToolBar->addAction(_ui.actionLine);
    
    QActionGroup* toolsGroup = new QActionGroup(_mainToolBar);
    toolsGroup->addAction(_ui.actionSelect);
    toolsGroup->addAction(_ui.actionArea);
    toolsGroup->addAction(_ui.actionLine);
    
    _mainToolBar->addSeparator();
    
    QCheckBox* group = new QCheckBox("Append", _mainToolBar);
    connect(group, SIGNAL(toggled(bool)), _ui.viewer, SLOT(group(bool)));
    _mainToolBar->addWidget(group);
    
    _mainToolBar->addSeparator();
    
    _colorSwatch = new ColorSwatch(_mainToolBar);
    _ui.viewer->changeColor(_colorSwatch->color());
    connect (_colorSwatch, SIGNAL(colorChanged(QColor)), 
             _ui.viewer, SLOT(changeColor(QColor)));
    _mainToolBar->addWidget(_colorSwatch);
    
    /// scale
    QToolBar* _scaleToolBar = new QToolBar("Scale", this);
    
    _scaleToolBar->addWidget(new QLabel("Scale: 1\" = ", _scaleToolBar));
    _scale = new QLineEdit(_scaleToolBar);
    _scaleToolBar->addWidget(_scale);
    _scale->setMaximumWidth(50);
    
    connect(_scale, SIGNAL(textChanged(QString)), SLOT(scaleChanged(QString)));
    
    //TODO read from ... something
    _scale->setText("1.0");
    
    _scaleToolBar->addWidget(new QLabel("feet", _scaleToolBar));
    
    QDoubleValidator* dv = new QDoubleValidator(_scale);
    dv->setBottom(1.0);
    _scale->setValidator(dv);
    
    //connect actions for toolbars
    QObject::connect(_ui.actionZoom_In, SIGNAL(triggered(bool)), _ui.viewer, SLOT(zoomIn()));
    QObject::connect(_ui.actionZoom_Out, SIGNAL(triggered(bool)), _ui.viewer, SLOT(zoomOut()));
    QObject::connect(toolsGroup, SIGNAL(triggered(QAction*)), SLOT(toolAction(QAction*)));
    
    QToolBar* _infoToolBar = new QToolBar("Info", this);
    
    //drawings
    _infoToolBar->addWidget(new QLabel("Drawing:"));
    _drawingsCombo = new QComboBox(_infoToolBar);
    _infoToolBar->addWidget(_drawingsCombo);
    _drawingsCombo->setEditable(false);
    _drawingsCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(_drawingsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(openPage(int)));
    
    _infoToolBar->addSeparator();
    
    //basic info
    _infoLabel = new QLabel(_infoToolBar);
    _infoToolBar->addWidget(_infoLabel);
    connect(_ui.viewer, SIGNAL(infoChanged(float, float)), SLOT(infoChanged(float, float)));
    
    this->addToolBar(_mainToolBar);
    this->addToolBar(_scaleToolBar);
    this->addToolBar(Qt::BottomToolBarArea, _infoToolBar);
}
