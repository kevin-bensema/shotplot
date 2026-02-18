#include <QApplication>
#include <QX/Services.h>
#include <QX/QStylingService.h>
#include "App/MainWindow.h"
#include "Services/GraphicsSettingsService.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("ShotPlot");
    QApplication::setApplicationVersion("0.1.0");
    QApplication::setOrganizationName("ShotPlot");
    QApplication::setOrganizationDomain("shotplot.app");
    
    // Initialize service locator and register services
    qx::InitServiceLocator();
    qx::RegisterService(new GraphicsSettingsService());
    qx::RegisterService(new QStylingService());
    
    MainWindow mainWindow;
    mainWindow.show();
    
    int result = app.exec();
    
    qx::ShutdownServiceLocator();
    return result;
}
