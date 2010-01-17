#include <QApplication>

#include "MainWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
	QString file;
	if (argc > 1)
	{
		file = QString(argv[1]);
	}
	
    MainWindow window(file);
    window.setMinimumSize(500,500);
    window.showMaximized();
    
    if (argc == 2)
    {
        window.open(QString(argv[1]));
    }
    
    return app.exec();
}
