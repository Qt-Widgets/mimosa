#include "mainwindow.h"
#include "downloaddialog.h"

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDesktopWidget>
#include <QApplication>
#include <QRect>
#include <QStatusBar>
#include <QMessageBox>
#include <QSettings>


MainWindow::MainWindow(){
    mimosa = new Mimosa;
    setWindowTitle("Mimosa");
    setCentralWidget(mimosa);
    setup();
    loadSettings();

    connect(mimosa, SIGNAL(processExist(bool, bool)),
            this, SLOT(updateDownloadAction(bool, bool)));
}

MainWindow::~MainWindow(){
    saveSettings();
}

void MainWindow::setup(){
    QMenu *fileMenu = menuBar()->addMenu("File");

    add = new QAction("New", this);
    fileMenu->addAction(add);
    add->setShortcut(QKeySequence::New);
    connect(add, SIGNAL(triggered(bool)), this, SLOT(newDownload()));

    fileMenu->addSeparator();

    auto exitFromProgram = new QAction("Exit", this);
    fileMenu->addAction(exitFromProgram);
    exitFromProgram->setShortcut(QKeySequence::Quit);
    connect(exitFromProgram, SIGNAL(triggered(bool)), this, SLOT(close()));

    QMenu *downloadMenu = menuBar()->addMenu("Download");

    resume = new QAction("Resume", this);
    resume->setEnabled(false);
    downloadMenu->addAction(resume);
    resume->setShortcut(QKeySequence("CTRL+R"));
    connect(resume, SIGNAL(triggered(bool)), mimosa, SLOT(resume()));

    abort = new QAction("Abort", this);
    abort->setEnabled(false);
    downloadMenu->addAction(abort);
    abort->setShortcut(QKeySequence("CTRL+A"));
    connect(abort, SIGNAL(triggered(bool)), mimosa, SLOT(abort()));

    downloadMenu->addSeparator();

    remove = new QAction("Remove", this);
    remove->setShortcut(QKeySequence::Delete);
    remove->setEnabled(!mimosa->tableIsEmpty());
    downloadMenu->addAction(remove);
    connect(remove, SIGNAL(triggered(bool)), mimosa, SLOT(remove()));

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/resources/mimosa.png"));
    trayIcon->setToolTip("Click to show Mimosa");
    trayIcon->setContextMenu(fileMenu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    QMenu *helpMenu = menuBar()->addMenu("Help");

    auto aboutAction = new QAction("About", this);
    helpMenu->addAction(aboutAction);

    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));
}

void MainWindow::newDownload(){
    if(!this->isVisible()) this->show();

    DownloadDialog downloadDialog;
    QString url;

    downloadDialog.exec();
    url = downloadDialog.downloadUrl->text();

    if(!url.isEmpty()) mimosa->start(url);
}

void MainWindow::updateDownloadAction(bool processExist, bool processRunning){
    resume->setEnabled(processExist and !processRunning);
    abort->setEnabled(processExist and processRunning);
    remove->setEnabled(!mimosa->tableIsEmpty());
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(this->isVisible()){
        event->ignore();
        this->hide();

        trayIcon->showMessage("Tray program",
                              "The Mimosa will be working in the background, "
                              "click to show window, add a new download "
                              "or exit from the program.",
                              QSystemTrayIcon::MessageIcon::Information,
                              2000);
    }
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason){
    switch(reason){
        case QSystemTrayIcon::Trigger:
            this->isVisible() ? this->hide() : this->show();
            break;
        case QSystemTrayIcon::MiddleClick:{
            int numberOfProcess = mimosa->numberOfDownloadProcessRunning();
            QString statusMessage = numberOfProcess == 0 ? "No download process currently working"
                                                         : tr("%1 process currently working").arg(numberOfProcess);
            trayIcon->showMessage("Download status",
                                  statusMessage,
                                  QSystemTrayIcon::MessageIcon::Information,
                                  2000);
            break;
        }
        default:
            break;
    }
}

void MainWindow::about(){
    QMessageBox::about(this,
                       tr("About Mimosa - 1.1.2"),
                       tr("<p style='text-align: center;'>Advantageous download manager to keep all the downloads.<br><br>"
                          "This software is written in C++ and Qt.<br><br>"
                          "<b>Author: </b>blooser<br>"
                          "<b>E-mail: </b>Blooser@protonmail.com<br>"
                          "<b>License: </b>MIT<br>"
                          "<b>Source code: </b><a href='https://github.com/blooser/mimosa'>https://github.com/blooser/mimosa</a>")
                       );
}

void MainWindow::saveSettings(){
    QSettings settings("Mimosa", "Dimensions");

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::loadSettings(){
    QSettings settings("Mimosa", "Dimensions");

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(960, 640)).toSize());
    move(settings.value("pos", QPoint(50, 50)).toPoint());
    settings.endGroup();
}
