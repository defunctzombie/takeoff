#pragma once

#include "ui_MainWindow.h"

#include <QMainWindow>
#include <QWidget>
#include <QToolBar>
#include <QString>
#include <QListWidget>
#include <QSplitter>
#include <QLineEdit>
#include <poppler-qt4.h>

#include "Page.hpp"
#include "ColorSwatch.hpp"

class QComboBox;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    private:
        Ui::MainWindow _ui;
        
        /** documents we have opened */
        QVector< Poppler::Document* > _documents;
        
        /** all the pages from all documents */
        QVector<Page> _pages;
        
        /** list of the filenames */
        QStringList _files;
        
        Page* _activePage;
        
        QLineEdit* _scale;
        ColorSwatch* _colorSwatch;
        QComboBox* _drawingsCombo;
        
        QLabel* _infoLabel;
        
    public:
        MainWindow(QWidget* parent = 0);
        ~MainWindow();
        
        //open a document for viewing
        void open(const QString& filename);
        
    private Q_SLOTS:
        void scaleChanged(const QString& text);
        void openPage(int index);
        
        void infoChanged(float length, float area);
        
        void toolAction(QAction* action);
        void on_actionOpen_triggered();
        
        void on_actionAdd_Drawing_triggered();
        void on_actionAdd_Drawings_triggered();
        
        void on_actionSaveProject_triggered();
        void on_actionOpenProject_triggered();
        
    protected:    
        void setupUi();
};