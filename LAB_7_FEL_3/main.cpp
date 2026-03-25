#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Alapértelmezett font beállítása az egész alkalmazásra
    QFont defaultFont("Segoe UI", 10);
    app.setFont(defaultFont);

    MainWindow w;
    w.show();

    return app.exec();
}
