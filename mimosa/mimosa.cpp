#include "mimosa.h"

#include <QMessageBox>
#include <QHeaderView>
#include <QTableView>
#include <QProgressBar>
#include <QPainter>
#include <QFileDialog>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QSettings>
#include <QDir>


Mimosa::Mimosa(QWidget *parent)
    : QTableView(parent){
    downloadTable = new DownloadTable;
    downloadPainter = new DownloadItemDelegate;
    setup();
    loadSettings();
    loadSession();
}

Mimosa::~Mimosa(){
    saveSettings();
    saveSession();
}

int Mimosa::numberOfDownloadProcessRunning() const{
    return downloads.size();
}

bool Mimosa::tableIsEmpty() const{
    return downloadTable->rowCount(QModelIndex()) == 0;
}

void Mimosa::setup(){
    proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(downloadTable);
    proxy->setFilterKeyColumn(0);

    setModel(proxy);
    setSortingEnabled(true);
    resizeColumnsToContents();
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setColumnWidth(0, 350);
    setColumnWidth(1, 150);
    setColumnWidth(2, 100);
    setColumnWidth(3, 200);
    horizontalHeader()->setStretchLastSection(true);
    setItemDelegate(downloadPainter);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showDownloadedFileLocation(QModelIndex)));
    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(checkIfProcessExist(QModelIndex)));
}

QPair<int, QString> Mimosa::currentSelectedRowWithFilename(){
    QSortFilterProxyModel *proxySelected = static_cast<QSortFilterProxyModel*>(this->model());
    QItemSelectionModel *selectionModel = this->selectionModel();

    QModelIndexList indexies = selectionModel->selectedRows();

    QModelIndex indexToRemove = indexies[0];
    int row = proxySelected->mapToSource(indexToRemove).row();

    QModelIndex indexWithItemName = downloadTable->index(row, 0, QModelIndex());
    QString filename = downloadTable->data(indexWithItemName, Qt::DisplayRole).toString();
    return QPair<int, QString>(row, filename);
}

void Mimosa::checkIfProcessExist(QModelIndex index){
    Q_UNUSED(index);

    if(downloads.size() == 0){
        emit processExist(false, false);
        return;
    }

    auto selected = currentSelectedRowWithFilename();
    QString filename = selected.second;

    bool downloadsContainsFilename = downloads.count() != 0 ?  downloads.contains(filename) : false;
    auto processIsRunning = downloadsContainsFilename ? downloads[filename]->isRunning() : false;
    emit processExist(downloadsContainsFilename, processIsRunning);
}

void Mimosa::download(QUrl &url){
    DownloadProcess *downloadProcess;
    downloadProcess = new DownloadProcess(url, downloadTable);
    downloadProcess->start();
    downloads[url.fileName()] = downloadProcess;

    connect(downloadProcess, SIGNAL(downloadFinished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
}

void Mimosa::resume(){
    auto selected = currentSelectedRowWithFilename();
    QString filename = selected.second;

    if(downloads.contains(filename)){
        auto currentDownloadProcess = downloads[filename];
        if(!currentDownloadProcess->isRunning()){
            currentDownloadProcess->start();
            emit processExist(true, true);
        }
    }
}

void Mimosa::abort(){
    auto selected = currentSelectedRowWithFilename();
    QString filename = selected.second;

    if(downloads.contains(filename)){
        auto currentDownloadProcess = downloads[filename];
        if(currentDownloadProcess->isRunning()){
            currentDownloadProcess->abort();
            emit processExist(true, false);
        }
    }
}

QString Mimosa::saveFileName(QUrl & url){
    QString path = url.path();
    QString filename = QFileInfo(path).fileName();
    if(filename.isEmpty()) filename = "download";

    int i = 0;
    if(QFile::exists(filename + QString::number(i))){
        while(QFile::exists(filename + QString::number(i)))
            ++i;

        filename += QString::number(i);
    }

    return filename;
}

void Mimosa::showDownloadedFileLocation(QModelIndex index){
    Q_UNUSED(index);

    auto selected = currentSelectedRowWithFilename();
    QString filename = selected.second;

    QFileDialog fileDialog(this, tr("Open file"), QDir::homePath());
    fileDialog.selectFile(filename);
    auto fileDialogCode = fileDialog.exec();

    if(fileDialogCode != QFileDialog::AcceptOpen) QDesktopServices::openUrl(filename);
}

void Mimosa::remove(){
    auto selected = currentSelectedRowWithFilename();
    int row = selected.first;
    QString filename = selected.second;

    QMessageBox removeFileQuestionBox;

    removeFileQuestionBox.setWindowTitle("Remove");
    removeFileQuestionBox.setText(tr("Remove %1?").arg(filename));
    removeFileQuestionBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    removeFileQuestionBox.resize(550, 300);

    auto removeFilesCheckBox = new QCheckBox("Remove file");
    removeFileQuestionBox.setCheckBox(removeFilesCheckBox);

    auto reply = removeFileQuestionBox.exec();
    if(reply == QMessageBox::Yes){
        if(downloads.contains(filename)){
            if(downloads[filename]->isRunning()) downloads[filename]->abort();
            downloads.remove(filename);
        }

        downloadTable->removeRows(row, 1, QModelIndex());
        checkIfProcessExist(QModelIndex());

        if(removeFileQuestionBox.checkBox()->checkState() == Qt::Checked){
            QDir fileRemover;
            fileRemover.remove(fileRemover.filePath(filename));
        }
    }
}

bool Mimosa::saveToDisk(const QString & filename, QIODevice *data){
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) return false;

    file.write(data->readAll());
    file.close();
    return true;
}

void Mimosa::insertDownloadingFilenameInTable(const QString filename){
    downloadTable->insertRows(0, 1, QModelIndex());
    QModelIndex index = downloadTable->index(0, 0, QModelIndex());
    downloadTable->setData(index, filename, Qt::EditRole);
}

void Mimosa::start(const QString downloadUrl){
    QUrl url = QUrl::fromEncoded(downloadUrl.toLocal8Bit());

    if(downloadTable->filenameExist(url.fileName())){
        QMessageBox::information(this, "Duplicate data!",
                                 tr("There is %1 already in downloads!").arg(url.fileName()));
        return;
    }

    if(!downloadUrl.isEmpty()){
        QRegularExpression urlRegex("^(?:http|ftp)s?://");
        QRegularExpressionMatch match = urlRegex.match(downloadUrl);

        if(!match.hasMatch()){
            QMessageBox::critical(this, "Error", "Incorrect URL", QMessageBox::Ok);
            return;
        }

        insertDownloadingFilenameInTable(url.fileName());
        download(url);
    }
}

bool Mimosa::isHttpRedricted(QNetworkReply *reply){
    int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return httpCode == 301 || httpCode == 302 || httpCode == 303
            || httpCode == 305 || httpCode == 307 || httpCode == 308;
}

void Mimosa::downloadFinished(QNetworkReply *reply){
    QUrl url = reply->url();
    downloads.remove(url.fileName());
    emit processExist(false, false);

    if(!reply->error()){
        if(isHttpRedricted(reply)) QMessageBox::information(this, "Redricted", "HTTP Redricted");
        else{
            QString filename = saveFileName(url);
            if(saveToDisk(filename, reply))
                QMessageBox::information(this, "Download successed!", tr("Download of %1 succesed!").arg(filename));
        }
    } else QMessageBox::warning(this, "Error", tr("Download error %1").arg(reply->errorString()));
}

void Mimosa::saveSession(){
    QFile file(tr("%1/%2").arg(QDir::homePath()).arg(".mimosa_session"));

    if(file.open(QIODevice::WriteOnly)){
        QDataStream stream(&file);
        int row = downloadTable->rowCount(QModelIndex());
        int column = downloadTable->columnCount(QModelIndex());
        stream<<row<<column;

        QModelIndex index;
        REP(i, row){
                REP(j, column){
                    index = downloadTable->index(i, j, QModelIndex());
                    stream << downloadTable->data(index, Qt::DisplayRole);
            }
        }

        foreach(QString key, downloads.keys())
            stream<<downloads.value(key)->getUrl();
    }

    file.close();
}

void Mimosa::loadSession(){
    QFile file(tr("%1/%2").arg(QDir::homePath()).arg(".mimosa_session"));

    if(file.open(QIODevice::ReadOnly)){
        QDataStream stream(&file);
        int row, column;

        stream >> row >> column;
        downloadTable->insertRows(0, row, QModelIndex());

        QModelIndex index;
        REP(i, row){
                REP(j, column){
                    index = downloadTable->index(i, j);
                    downloadTable->setData(index, stream, Qt::EditRole);
            }
        }

        while(!stream.atEnd()){
            QUrl url;
            stream >> url;

            DownloadProcess *downloadProcess;
            downloadProcess = new DownloadProcess(url, downloadTable);
            downloads[url.fileName()] = downloadProcess;

            connect(downloadProcess, SIGNAL(downloadFinished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
        }
    }

    file.close();
}

void Mimosa::loadSettings(){
    QSettings settings;

    settings.beginGroup("Mimosa");
    this->horizontalHeader()->restoreState(settings.value("tableSize").toByteArray());
    settings.endGroup();
}

void Mimosa::saveSettings(){
    QSettings settings;

    settings.beginGroup("Mimosa");
    settings.setValue("tableSize", this->horizontalHeader()->saveState());
    settings.endGroup();
}
