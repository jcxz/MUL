#include "mainwindow.h"
#include <QApplication>
#include "Filter.h"

int main(int argc, char *argv[])
{
  //std::cout << "CLSOURCEPATH: " << CLSOURCEPATH << std::endl;
  //std::cout << "CLSOURCE: " << CLSOURCE("grayscale.cl") << std::endl;
  std::cout << "CLSOURCE: " << CLSRCFILE("grayscale.cl") << std::endl;

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
