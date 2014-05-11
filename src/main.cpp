#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // create context
    QCLContext ctx;
    if (!ctx.create(QCLDevice::GPU))
    {
      std::cerr << "Failed to create OpenCL context" << std::endl;
      return 1;
    }

    w.setCLContext(&ctx);

    return a.exec();
}
