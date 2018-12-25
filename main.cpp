#include "mainwindow.h"

#include <QApplication>


int main(int argc, char ** argv){
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(icons);

    app.setWindowIcon(QIcon(":/resources/icons/mimosa.png"));

    MainWindow main_window;
    main_window.show();

    return app.exec();
}
