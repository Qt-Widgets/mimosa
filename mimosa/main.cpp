#include "mainwindow.h"

#include <QApplication>


int main(int argc, char ** argv){
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(mimosa);

    app.setWindowIcon(QIcon(":/resources/mimosa.png"));

    MainWindow main_window;
    main_window.show();

    return app.exec();
}
