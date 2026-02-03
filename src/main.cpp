#include <QApplication>
#include "App/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("ShotPlot");
    QApplication::setApplicationVersion("0.1.0");
    QApplication::setOrganizationName("ShotPlot");
    QApplication::setOrganizationDomain("shotplot.app");
    
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
