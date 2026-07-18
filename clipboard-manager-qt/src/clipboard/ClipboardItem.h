#ifndef CLIPBOARDITEM_H
#define CLIPBOARDITEM_H

#include <QString>
#include <QDateTime>

struct ClipboardItem {
    QString id;
    QString content;
    QString sourceApp;
    qint64 createdAt;
};

#endif
