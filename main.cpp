#include <QApplication>

#include "MainWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    MainWindow window;
    window.setMinimumSize(500,500);
    window.showMaximized();
    
    if (argc == 2)
    {
        window.open(QString(argv[1]));
    }
    
    return app.exec();
}
