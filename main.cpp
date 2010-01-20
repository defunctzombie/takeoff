#include <QApplication>
#include <QFileOpenEvent>
#include <QDebug>

#include "MainWindow.hpp"

MainWindow* mw;

class TakeoffApplication : public QApplication
{
	public:
		TakeoffApplication(int& argc, char** argv) :
            QApplication(argc, argv) {}
	
	protected:
		bool event(QEvent *);

};

bool TakeoffApplication::event(QEvent *event)
{
	switch (event->type())
	{
        case QEvent::FileOpen:
            mw->openProject(static_cast<QFileOpenEvent *>(event)->file());        
            return true;
        default:
            return QApplication::event(event);
	}
}

int main(int argc, char* argv[])
{
    TakeoffApplication app(argc, argv);
    
	QString file;
	if (argc > 1)
	{
		file = QString(argv[1]);
	}
	
    mw = new MainWindow(file);
    mw->setMinimumSize(500,500);
    mw->showMaximized();
    
    if (argc == 2)
    {
        mw->openProject(QString(argv[1]));
    }
    
    int ret = app.exec();
    
	delete mw;
	
	return ret;
}
