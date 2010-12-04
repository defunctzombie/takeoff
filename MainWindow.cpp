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
#include <QDebug>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), _activePage(0), _unsavedChanges(false)
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
    
    //use complete base name because some files can be named: A2.1.0.pdf
    QString name(fInfo.completeBaseName());
    
    for (int i=0 ; i<document->numPages() ; ++i)
    {
        _pages.append(Page(document->page(i)));
        
        _activePage = &_pages.last();
        
        Poppler::Page::Rotation rotate = Poppler::Page::Rotate0;
        if (_activePage->ppage->orientation() == Poppler::Page::Portrait)
            _activePage->rotation = Poppler::Page::Rotate90;
        
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
		else if (action == _ui.actionCount)
        {
            _ui.viewer->changeTool(Viewer::CountTool);
        }
        else if (action == _ui.actionCheck)
        {
            _ui.viewer->changeTool(Viewer::CheckTool);
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
    
    _ui.viewer->reset();
    
    QDir projectDir(QFileInfo(filename).absoluteDir());
    
    int pageIndex = 0;
    //go through files and load them
    Q_FOREACH(const QString& pdfFile, _files)
    {
        // backwards compat for when we used to save absolute paths
        QString refFilename = pdfFile;
        QFileInfo refFile(pdfFile);
        if (refFile.isRelative())
            refFilename = projectDir.absoluteFilePath(refFilename);
        
        Poppler::Document* document = Poppler::Document::load(refFilename);
        if (!document)
        {
            QMessageBox::critical(this, "Document Error", "Unable to open the document: " + refFilename);
            return;
        }
        else if (document->isLocked())
        {
            QMessageBox::information(this, "Document Error", "The document is password protected.");
            return;
        }
        
        _documents.append(document);
        
        QFileInfo fInfo(refFilename);
        QString name(fInfo.completeBaseName());
        
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
    QFileInfo projectFile(_openProject);
    QDir projectDir(projectFile.absoluteDir());
    
    setWindowTitle("Takeoff - " + QFileInfo(_openProject).baseName());
    
    QDomDocument doc;
    
    QDomElement takeoff = doc.createElement("takeoff");
    takeoff.setAttribute("version", "1.0");
    doc.appendChild(takeoff);
    
    QDomElement documents = doc.createElement("documents");
    takeoff.appendChild(documents);
    
    Q_FOREACH (const QString& s, _files)
    {
        QString docPath = projectDir.relativeFilePath(s);
        QDomElement document = doc.createElement("document");
        document.setAttribute("ref", docPath);
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
    
    _unsavedChanges = false;
}

void MainWindow::on_actionRotateLeft_triggered()
{
    _unsavedChanges = true;
    _ui.viewer->rotateCCW();
}

void MainWindow::on_actionRotateRight_triggered()
{
    _unsavedChanges = true;
    _ui.viewer->rotateCW();
}

void MainWindow::infoChanged(float length, float area)
{
    QString text = QString("<strong>Lf: </strong>%1'").arg(length, 0, 'f', 3);
    
    if (area != 0)
        text += QString("  <strong>Sf: </strong>%1").arg(area, 0, 'f', 3);
    
    _infoLabel->setText(text);
    
    if (!_unsavedChanges)
        setWindowTitle(windowTitle() + "*");
    
    _unsavedChanges = true;
}

void MainWindow::closeEvent(QCloseEvent* ce)
{
    if (_unsavedChanges)
    {
        const QMessageBox::StandardButton save = 
            QMessageBox::question(this, "Save?", "Save changes before closing?", 
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                                
        if (save == QMessageBox::Yes)
        {
            //save changes
            on_actionSaveProject_triggered();
            
            //there was an error saving changes
            if (_unsavedChanges)
                ce->ignore();
        }
    }
    ce->accept();
}

void MainWindow::setupUi()
{
    /// setup toolbars ///
    QToolBar* _mainToolBar = new QToolBar("Main", this);
    
    /// zoom
    _mainToolBar->addAction(_ui.actionZoom_In);
    _mainToolBar->addAction(_ui.actionZoom_Out);
    _mainToolBar->addAction(_ui.actionZoom_Fit);
    
    /// rotates
    _mainToolBar->addAction(_ui.actionRotateLeft);    
    _mainToolBar->addAction(_ui.actionRotateRight);
    
    connect(_ui.actionZoom_Fit, SIGNAL(triggered()), _ui.viewer, SLOT(zoomFit()));
    
    _mainToolBar->addSeparator();
    
    /// tools
	_mainToolBar->addAction(_ui.actionZoom_Window);
    _mainToolBar->addAction(_ui.actionSelect);
    _mainToolBar->addAction(_ui.actionArea);
    _mainToolBar->addAction(_ui.actionLine);
    _mainToolBar->addAction(_ui.actionCount);
    _mainToolBar->addAction(_ui.actionCheck);
    
    QActionGroup* toolsGroup = new QActionGroup(_mainToolBar);
	toolsGroup->addAction(_ui.actionZoom_Window);
    toolsGroup->addAction(_ui.actionSelect);
    toolsGroup->addAction(_ui.actionArea);
    toolsGroup->addAction(_ui.actionLine);
    toolsGroup->addAction(_ui.actionCount);
    toolsGroup->addAction(_ui.actionCheck);
    
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
