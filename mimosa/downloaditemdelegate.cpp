#include "downloaditemdelegate.h"

#include <QTextCharFormat>
#include <QTextCursor>
#include <QApplication>
#include <QPainter>

DownloadItemDelegate::DownloadItemDelegate(QObject * parent)
    : QStyledItemDelegate(parent){

}

void DownloadItemDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const{
    if(index.column() == 1){
        int progress = index.data().toInt();
        QStyleOptionProgressBar progressbar;

        painter->setRenderHint(QPainter::Antialiasing);
        progressbar.rect = option.rect;
        progressbar.textVisible = true;

        if(progress != -1){
            progressbar.invertedAppearance = false;
            progressbar.minimum = 0;
            progressbar.maximum = 100;
            progressbar.progress = progress;
            progressbar.text = tr("%1%").arg(progress);
            if(progress == 100) progressbar.palette.setBrush(QPalette::Highlight, QColor(37, 142, 37));
        } else{
            progressbar.invertedAppearance = true;
            progressbar.text = "Aborted";
            progressbar.palette.setBrush(QPalette::Highlight, QColor(142, 37, 37));
        }

        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressbar, painter);
     } else if(index.column() != 1 && index.isValid()){
        QStyleOptionViewItem customText(option);

        QTextCharFormat format;
        format.setTextOutline(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        QTextCursor cursor;
        cursor.select(QTextCursor::Document);
        cursor.mergeCharFormat(format);

        customText.font.setWeight(600);
        customText.palette.setColor(QPalette::HighlightedText, "#ffd24d");
        customText.palette.setColor(QPalette::Highlight, "#0995DB");

        QStyledItemDelegate::paint(painter, customText, index);
     } else
        QStyledItemDelegate::paint(painter, option, index);
}

QSize DownloadItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const{
    Q_UNUSED(index);
    return option.widget->size();
}
