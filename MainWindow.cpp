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

MainWindow::MainWindow(QString file, QWidget* parent) :
    QMainWindow(parent), _activePage(0)
{
    _ui.setupUi(this);
    setupUi();
    
    //scales
    
    _scales.append(Scale(1/32.0, 1));
    _scales.append(Scale(1/16.0, 1));
    _scales.append(Scale(3/32.0, 1));
    _scales.append(Scale(1/8.0, 1));
    _scales.append(Scale(3/16.0, 1));
    _scales.append(Scale(1/4.0, 1));
    _scales.append(Scale(3/8.0, 1));
    _scales.append(Scale(1/2.0, 1));
    _scales.append(Scale(3/4.0, 1));
    _scales.append(Scale(1, 1));
    _scales.append(Scale(1.5, 1));
    _scales.append(Scale(3, 1));
    _scales.append(Scale(1, 10));
    _scales.append(Scale(1, 20));
    _scales.append(Scale(1, 30));
    _scales.append(Scale(1, 40));
    _scales.append(Scale(1, 50));
    _scales.append(Scale(1, 60));
    _scales.append(Scale(1, 70));
    _scales.append(Scale(1, 80));
    _scales.append(Scale(1, 90));
    _scales.append(Scale(1, 100));
    //scales.append(Scale(0, 0));
    
    Q_FOREACH(const Scale& s, _scales)
    {
        _scalesCombo->addItem(s.toString());
    }
    
    if (!file.isNull())
        openProject(file);
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

void MainWindow::scaleSelected(int index)
{
    _scale->setText(QString::number(_scales[index].scale(), 'f', 2));
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
		else if (action == _ui.actionZoom_Window)
		{
			_ui.viewer->changeTool(Viewer::ZoomTool);
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

void MainWindow::openProject(const QString& filename)
{
	_openProject = filename;
	_ui.viewer->reset();
	
	setWindowTitle("Takeoff - " + QFileInfo(filename).baseName());
	
	//TODO reset properly
    _pages.clear();
    _files.clear();
    
    _drawingsCombo->clear();
	
    QFile inFile(filename);
	
    inFile.open(QIODevice::ReadOnly);
#if 1
    QDomDocument doc;
    doc.setContent(&inFile);
    
    inFile.close();
    
    QDomElement takeoff = doc.documentElement();
    
    QDomNodeList documentsList = takeoff.elementsByTagName("documents");
    if (documentsList.count() != 1)
        return;
    
    QDomNodeList documents =documentsList.item(0).toElement().elementsByTagName("document");
    for (int i=0 ; i<documents.count() ; ++i)
    {
        QDomElement document = documents.item(i).toElement();
        _files.append(document.attribute("ref"));
    }
    
    QDomNodeList pagesList = takeoff.elementsByTagName("pages");
    if (pagesList.count() != 1)
        return;
    
    QDomNodeList pages = pagesList.item(0).toElement().elementsByTagName("page");
    for (int i=0 ; i<pages.count() ; ++i)
    {
        Page p;
        p.read(pages.item(i).toElement());
        _pages.append(p);
    }
    
#else
    QDataStream in(&inFile);
    
    in >> _files;
    in >> _pages;
    
    inFile.close();
    
#endif
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

void MainWindow::on_actionOpenProject_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open File", "", "*.tdf");
    
    if (filename.isNull())
        return;
	
	openProject(filename);
}

void MainWindow::on_actionSaveProject_triggered()
{
	QString filename = _openProject;
	if (filename.isNull())
	{
		filename = QFileDialog::getSaveFileName(this, "Save File", "", "*.tdf");
	}
    
    if (filename.isNull())
        return;
	
	if (!filename.endsWith(".tdf"))
	{
		filename.append(".tdf");
	}
	
	_openProject = filename;
    
#if 1
    QDomDocument doc;
    
    QDomElement takeoff = doc.createElement("takeoff");
    takeoff.setAttribute("version", "1.0");
    doc.appendChild(takeoff);
    
    QDomElement documents = doc.createElement("documents");
    takeoff.appendChild(documents);
    
    Q_FOREACH (const QString& s, _files)
    {
        QDomElement document = doc.createElement("document");
        document.setAttribute("ref", s);
        documents.appendChild(document);
    }
    
    QDomElement pages = doc.createElement("pages");
    takeoff.appendChild(pages);
    
    Q_FOREACH (const Page& p, _pages)
    {
        p.write(pages);
    }
    
    QFile file(_openProject);
    if (!file.open(QIODevice::WriteOnly))
        return;
    
    file.write(doc.toByteArray(4));
    file.close();
    
#else
    QFile outFile(_openProject);
    outFile.open(QIODevice::WriteOnly);
    QDataStream out(&outFile);
    
    out << _files;
    out << _pages;
    
    outFile.close();
#endif
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
    
    //_mainToolBar->addAction(_ui.actionOpen);
    //_mainToolBar->addSeparator();
    
    /// zoom
    _mainToolBar->addAction(_ui.actionZoom_In);
    _mainToolBar->addAction(_ui.actionZoom_Out);
    _mainToolBar->addAction(_ui.actionZoom_Fit);
    
    connect(_ui.actionZoom_Fit, SIGNAL(triggered()), _ui.viewer, SLOT(zoomFit()));
    
    _mainToolBar->addSeparator();
    
    /// tools
	_mainToolBar->addAction(_ui.actionZoom_Window);
    _mainToolBar->addAction(_ui.actionSelect);
    _mainToolBar->addAction(_ui.actionArea);
    _mainToolBar->addAction(_ui.actionLine);
    
    QActionGroup* toolsGroup = new QActionGroup(_mainToolBar);
	toolsGroup->addAction(_ui.actionZoom_Window);
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
    
    _scalesCombo = new QComboBox(_scaleToolBar);
    _scaleToolBar->addWidget(_scalesCombo);
    
    _scaleToolBar->addWidget(new QLabel("Scale: 1\" = ", _scaleToolBar));
    _scale = new QLineEdit(_scaleToolBar);
    _scaleToolBar->addWidget(_scale);
    _scale->setMaximumWidth(50);
    
    connect(_scalesCombo, SIGNAL(currentIndexChanged(int)), SLOT(scaleSelected(int)));
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
    this->addToolBar(_infoToolBar);
}
