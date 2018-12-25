#ifndef DOWNLOADERWIDGET_H
#define DOWNLOADERWIDGET_H

#include <QTableView>
#include <QSortFilterProxyModel>
#include <QHash>
#include <QPair>

#include "downloadtable.h"
#include "downloaditemdelegate.h"
#include "downloadprocess.h"


class Mimosa : public QTableView{
    Q_OBJECT
public:
    Mimosa(QWidget *parent = nullptr);
    ~Mimosa();
    static QString saveFileName(QUrl&);
    static bool isHttpRedricted(QNetworkReply*);
    void start(const QString);
    bool saveToDisk(const QString&, QIODevice*);
    void download(QUrl &);
    int numberOfDownloadProcessRunning() const;
signals:
    void processExist(bool, bool);
public slots:
    void checkIfProcessExist(QModelIndex);
    void downloadFinished(QNetworkReply*);
    void showDownloadedFileLocation(QModelIndex);
    void remove();
    void abort();
    void resume();
protected:
    QPair<int, QString> currentSelectedRowWithFilename();
    void insertDownloadingFilenameInTable(const QString);
    void saveSession();
    void loadSession();
    void saveSettings();
    void loadSettings();
private:
    void setup();
    DownloadTable *downloadTable = nullptr;
    DownloadItemDelegate *downloadPainter = nullptr;
    QSortFilterProxyModel *proxy = nullptr;
    QHash<QString, DownloadProcess*> downloads;
};

#endif // DOWNLOADERWIDGET_H
