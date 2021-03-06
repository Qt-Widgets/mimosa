#include "downloaddialog.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>


DownloadDialog::DownloadDialog(){
    setup();
}

void DownloadDialog::setup(){
    QGridLayout *dataLayout;
    dataLayout = new QGridLayout;

    downloadUrl = new QLineEdit;
    downloadUrl->setPlaceholderText(tr("Download URL"));
    downloadUrl->setToolTip("Insert URL for data files to download");
    dataLayout->addWidget(downloadUrl, 0, 1);

    QHBoxLayout *buttonsLayout;
    buttonsLayout = new QHBoxLayout;

    auto okButton = new QPushButton("Ok");
    auto cancelButton = new QPushButton("Cancel");
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    dataLayout->addLayout(buttonsLayout, 1, 1, Qt::AlignCenter);

    QVBoxLayout  *mainLayout;
    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(dataLayout);

    setWindowTitle(tr("New Download"));
    resize(450, 100);
    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));
}
