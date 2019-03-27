#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mimosa.h"

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QCloseEvent>


class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();
protected:
    void saveSettings();
    void loadSettings();
    void closeEvent(QCloseEvent*);
private slots:
    void newDownload();
    void updateDownloadAction(bool, bool);
    void trayIconActivated(QSystemTrayIcon::ActivationReason);
private:
    Mimosa *mimosa = nullptr;
    void setup();
    QAction *add = nullptr;
    QAction *remove = nullptr;
    QAction *abort = nullptr;
    QAction *resume = nullptr;
    QSystemTrayIcon *trayIcon = nullptr;

};

#endif // MAINWINDOW_H
