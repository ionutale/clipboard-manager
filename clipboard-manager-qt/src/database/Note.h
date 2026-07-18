#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QDateTime>
#include <QUuid>

struct Note {
    QString id;
    QString title;
    QString content;
    qint64 createdAt;
    qint64 updatedAt;

    static Note create(const QString &title, const QString &content)
    {
        auto now = QDateTime::currentSecsSinceEpoch();
        return {
            QUuid::createUuid().toString(QUuid::WithoutBraces),
            title, content, now, now
        };
    }
};

#endif
